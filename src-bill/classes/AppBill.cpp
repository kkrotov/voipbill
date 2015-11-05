#include "../classes/AppBill.h"

#include "../threads/ThreadWeb.h"
#include "../threads/ThreadLog.h"
#include "../threads/ThreadSync.h"
#include "../threads/ThreadLoader.h"
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
#include "../classes/LogWriterScreen.h"
#include "../classes/LogWriterFile.h"
#include "../classes/LogWriterSyslog.h"

AppBill & app() {
    static AppBill appVar;
    return appVar;
}

void AppBill::runApp() {

    registerAllThreads();

    ThreadWeb web;

    std::thread web_thread(std::ref(web));

    runAppInSingleMode();

    threads.joinAll();

    web.stop();
    web_thread.join();
}

template<class T>
void AppBill::registerThread() {
    threadConstructorsMap[T::idName()] = []() -> Thread * { return new T(); };
}

Thread * AppBill::newThreadObject(std::string id) {
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
    registerThread<ThreadRemoteLoader>();
    registerThread<ThreadCurrentCalls>();
    registerThread<ThreadFetchCdr>();
    registerThread<ThreadBillRuntime>();
    registerThread<ThreadSave>();
    registerThread<ThreadSyncCalls>();
    registerThread<ThreadSyncCounters>();
    registerThread<ThreadSyncLocks>();
    registerThread<ThreadClientLock>();
    registerThread<ThreadBlacklistFetch>();
    registerThread<ThreadBlacklistCalc>();
    registerThread<ThreadLimitControl>();
    registerThread<ThreadUpdateActiveClients>();
    registerThread<ThreadCheckStartTable>();
    registerThread<ThreadTasks>();
    registerThread<ThreadRadiusAuthServer>();
}

void AppBill::runAppInSingleMode()
{
    std::vector<std::string> standardThreads {
            "log",
            "sync",
            "loader",
            "remote_loader",
            "current_calls",
            "fetch_cdr",
            "runtime",
            "save",
            "sync_calls",
            "sync_counters",
            "sync_locks",
            "client_lock",
            "blacklist_fetch",
            "blacklist_calc",
            "limitcontrol",
            "update_active_clients",
            "checkstarttable",
            "tasks",
            "radius_auth_server",
    };

    for (auto thread: standardThreads) {
        threads.run(newThreadObject(thread));
    }
}
