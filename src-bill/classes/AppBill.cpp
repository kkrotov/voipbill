#include "../classes/AppBill.h"

#include "../threads/ThreadWeb.h"
#include "../threads/ThreadLog.h"
#include "../threads/ThreadSync.h"
#include "../threads/ThreadLoader.h"
#include "../threads/ThreadAccountBalanceRecalc.h"
#include "../threads/ThreadRemoteLoader.h"
#include "../threads/ThreadCurrentCalls.h"
#include "../threads/ThreadFetchCdr.h"
#include "../threads/ThreadBillRuntime.h"
#include "../threads/ThreadRadiusAuthServer.h"
#include "../threads/ThreadSave.h"
#include "../threads/ThreadSyncCalls.h"
#include "../threads/ThreadSyncCounters.h"
#include "../threads/ThreadSyncLocks.h"
#include "../threads/ThreadClientLock.h"
#include "../threads/ThreadBlacklistFetch.h"
#include "../threads/ThreadBlacklistCalc.h"
#include "../threads/ThreadLimitControl.h"
#include "../threads/ThreadUpdateActiveClients.h"
#include "../threads/ThreadCheckStartTable.h"
#include "../threads/ThreadTasks.h"
#include "../threads/ThreadCdrParser.h"
#include "../threads/ThreadSyncCdrs.h"
#include "../healthcheck/DbConnectStatus.h"
#include "../healthcheck/FtpConnectStatus.h"
#include "../healthcheck/CallSyncStatus.h"
#include "../healthcheck/CdrSyncStatus.h"
#include "../healthcheck/CallSaveStatus.h"
#include "../healthcheck/CdrWaitProcessing.h"
#include "../healthcheck/CallsWaitSaving.h"
#include "../healthcheck/RadiusAuthServerStatus.h"
#include "../healthcheck/ThreadErrorStatus.h"
#include "../healthcheck/TrunkLoadStatus.h"
#include "../threads/ThreadHealthManager.h"
#include "../healthcheck/EventManagementStatus.h"

AppBill &app() {
    static AppBill appVar;
    return appVar;
}

void AppBill::setHealthCheck() {

    healthCheckController.add(std::shared_ptr<DbCallsConnectStatus>(new DbCallsConnectStatus()));
    healthCheckController.add(std::shared_ptr<DbMainConnectStatus>(new DbMainConnectStatus()));
    healthCheckController.add(std::shared_ptr<FtpConnectStatus>(new FtpConnectStatus()));
    healthCheckController.add(std::shared_ptr<CallSyncStatus>(new CallSyncStatus()));
    healthCheckController.add(std::shared_ptr<CdrSyncStatus>(new CdrSyncStatus()));
    healthCheckController.add(std::shared_ptr<CallSaveStatus>(new CallSaveStatus()));
    healthCheckController.add(std::shared_ptr<CdrWaitProcessing>(new CdrWaitProcessing()));
    healthCheckController.add(std::shared_ptr<CallsWaitSaving>(new CallsWaitSaving()));
    healthCheckController.add(std::shared_ptr<ThreadErrorStatus>(new ThreadErrorStatus()));
    healthCheckController.add(std::shared_ptr<TrunkLoadStatus>(new TrunkLoadStatus()));
    healthCheckController.add(std::shared_ptr<RadiusAuthServerStatus>(new RadiusAuthServerStatus()));
    healthCheckController.add(std::shared_ptr<EventManagementStatus>(new EventManagementStatus()));
}

void AppBill::runApp() {

    registerAllThreads();

    // Модуль веб интерфейса
    ThreadWeb web;

    std::thread web_thread(std::ref(web));

    setHealthCheck();

    if (conf.active_threads.size() == 0)
        runAppInSingleMode();
    else
        runActiveThreads();

    threads.joinAll();

    web.stop();
    web_thread.join();
}

template<class T>
void AppBill::registerThread() {
    threadConstructorsMap[T::idName()] = []() -> Thread * { return new T(); };
}

Thread *AppBill::newThreadObject(std::string id) {
    auto fn = threadConstructorsMap[id];
    if (fn) {
        return fn();
    } else {
        return 0;
    }
}

void AppBill::registerAllThreads() {

    registerThread<ThreadLog>();
    registerThread<ThreadSync>();
    registerThread<ThreadLoader>();
    registerThread<ThreadAccountBalanceRecalc>();
    registerThread<ThreadRemoteLoader>();
    registerThread<ThreadCurrentCalls>();
    registerThread<ThreadFetchCdr>();
    registerThread<ThreadBillRuntime>();
    registerThread<ThreadSave>();
    registerThread<ThreadSyncCalls>();
    registerThread<ThreadSyncCdrs>();
    registerThread<ThreadSyncCounters>();
    registerThread<ThreadSyncLocks>();
    registerThread<ThreadClientLock>();
    registerThread<ThreadBlacklistFetch>();
    registerThread<ThreadBlacklistCalc>();
    registerThread<ThreadLimitControl>();
    registerThread<ThreadUpdateActiveClients>();
    registerThread<ThreadCheckStartTable>();
    registerThread<ThreadTasks>();
    registerThread<ThreadCdrParser>();
    registerThread<ThreadRadiusAuthServer>();
    registerThread<ThreadHealthManager>();
}

void AppBill::runAppInSingleMode() {
    std::vector<std::string> standardThreads{
            // Логирование
            "log",
            // Перемещение данных из таблиц центрального сервера на региональные
            "sync",
            // Блок считывания данных в оперативную память
            "loader",
            // синхронизация данных по балансу таблиц billing.clients и billing.stats_accounts
            "account_balance_recalc",
            // синхронизация таблиц billing.stats_account на центральном и региональных серверах
            "remote_loader",
            // Контроль текущих звонков
            "current_calls",
            // Загрузка вновь полеченных cdr из таблицы calls_cdr.cdr
            "fetch_cdr",
            // Расчет себестоимости звонка, Расчет цены MCN для звонка
            "runtime",
            // Обновление центральной БД (копирование данных из региональной БД в центральную)
            "save",
            // Передача данных по локальным вызовам calls_raw.calls_raw на центральный сервер
            "sync_calls",
            // Синхронизация таблиц локальных вызовов calls_cdr.cdr и calls_cdr.cdr_unfinished на центральный сервер
            "sync_cdrs",
            // Обновление счётчиков по клиентским счетам, использованных бесплатных минут и пакетов на центральной БД
            "sync_counters",
            // обновление таблицы клиентских блокировок billing.clients_locks на центральном сервере
            "sync_locks",
            // Извлечение чёрных списков из openca
            "client_lock",
            // Извлечение чёрных списков из openca
            "blacklist_fetch",
            // модификация чёрных списков в репозитории и в openca
            "blacklist_calc",
            // Сброс звонка при недостаточном балансе
            "limitcontrol",
            // ведение списка активных номеров (billing.service_number) и транков (billing.service_trunk)
            "update_active_clients",
            // извлечение новых записей из таблицы calls_cdr.start
            "checkstarttable",
            // отслеживание команды запуска процедуры пересчёта начислений за текущий или предыдущий месяцы
            "tasks",
            // извлечение xml файлов с записями cdr, их разбор и занесение в таблицу calls_cdr.cdr
            "cdr_parser",
            // сервер обработки запросов по протоколу radius
            "radius_auth_server",
            // контроль загрузки транков
            "health_manager",
    };

    if (app().conf.isApiHostMode()) {
        standardThreads.clear();
        standardThreads = {"log", "sync", "loader", "remote_loader"};
    };

    for (auto thread: standardThreads) {

        if (std::find(conf.skip_threads.begin(), conf.skip_threads.end(), thread) != conf.skip_threads.end()) {

            cout << "Skipping thread " + thread + "...\n";
            continue;
        }
        cout << "Running thread " + thread + "...\n";
        threads.run(newThreadObject(thread));
    }
}

void AppBill::runActiveThreads() {
    for (auto thread_name: conf.active_threads) {

        Thread *t = newThreadObject(thread_name);
        if (t == nullptr) {

            cout << "ERROR: Unable to run thread " + thread_name + "\n";
            continue;
        }
        cout << "Running thread " + thread_name + "...\n";
        threads.run(t);
    }
}
