#include "TaskRecalc.h"

#include "../common.h"
#include "../classes/AppBill.h"
#include "../threads/ThreadLoader.h"
#include "../threads/ThreadBillRuntime.h"
#include "../web/PageDataBilling.h"

TaskRecalc::TaskRecalc(time_t date_from, string server_id) : Task(server_id) {
    this->date_from = date_from;
    name = "Recalc from " + string_time(date_from);
    recalc_from_call_id = 0;
}

void TaskRecalc::run() {

    BDb db_calls(app().conf.db_calls);

    setStatus("1. waiting save calls lock");
    unique_lock<mutex> lock_save_calls(repository.billingData->saveLock);

    setStatus("2. waiting sync calls central lock");
    unique_lock<mutex> lock_sync_calls_central(repository.billingData->syncCallsCentralLock);


    setStatus("3. getting min id");
    BDbResult res = db_calls.query(
            "select min(id) from calls_raw.calls_raw where connect_time>='" + string_time(date_from) + "'");
    recalc_from_call_id = res.next() ? res.get_ll(0) : 0;

    if (recalc_from_call_id == 0) {
        Log::error("Nothing to recalc");
        return;
    }

    Log::notice("Recalc calls: date " + string_time(date_from) +", id >= " + lexical_cast<string>(recalc_from_call_id));



    setStatus("4. delete calls_raw from id " + lexical_cast<string>(recalc_from_call_id));
    db_calls.exec("delete from calls_raw.calls_raw where id>=" + lexical_cast<string>(recalc_from_call_id));



    setStatus("5. recalc temp data");
    DataContainer data;
    data.loadAll(&db_calls);



    setStatus("6. recalc temp counters");
    newBillingData.loadAll(&db_calls, true);
    long long int oldStoredLastId = newBillingData.getCallsStoredLastId();



    setStatus("7. calc");

    Billing billing;
    billing.setData(&data);
    billing.setBillingData(&newBillingData);

    while (true) {
        {
            TimerScope ts(t_load);
            newBillingData.cdrsLoadPart(&db_calls);
        }

        if (newBillingData.cdrsQueueSize() == 0) {
            break;
        }


        {
            TimerScope ts(t_calc);
            billing.calc();
        }

        {
            TimerScope ts(t_save);
            newBillingData.save(&db_calls);
        }


        setStatus("7. calc " + lexical_cast<string>(newBillingData.getCallsStoredCounter()));
    }



    setStatus("8. waiting fetch cdr lock");
    unique_lock<mutex> lock_fetch_cdr(repository.billingData->fetchCdrLock);

    setStatus("9. waiting calc calls lock");
    unique_lock<mutex> lock_calc_calls(repository.billingData->calcCallsLock);

    setStatus("10. waiting sync counters central lock");
    unique_lock<mutex> lock_sync_counters_central(repository.billingData->syncCountersCentralLock);

    setStatus("11. waiting sync locks central lock");
    unique_lock<mutex> lock_sync_locks_central(repository.billingData->syncLocksCentralLock);

    setStatus("12. recalc counters");
    repository.billingData->statsAccount.recalc(&db_calls);
    repository.billingData->loadAll(&db_calls);

    lock_fetch_cdr.unlock();
    lock_calc_calls.unlock();
    lock_save_calls.unlock();



    setStatus("13. delete calls_raw from main");
    db_main->exec("DELETE FROM calls_raw.calls_raw WHERE server_id = " + app().conf.str_instance_id + " and id >= " + lexical_cast<string>(recalc_from_call_id));

    setStatus("14. sync accounts");
    {
        BDbTransaction trans(db_main);
        db_main->exec("DELETE FROM billing.stats_account WHERE server_id = " + app().conf.str_instance_id);
        repository.billingData->statsAccount.sync(db_main, repository.billingData);
        trans.commit();
    }

    setStatus("15. sync freemin");
    {
        BDbTransaction trans(db_main);
        db_main->exec("DELETE FROM billing.stats_freemin WHERE server_id = " + app().conf.str_instance_id + " and max_call_id > " + lexical_cast<string>(oldStoredLastId));
        repository.billingData->statsFreemin.sync(db_main, &db_calls);
        trans.commit();
    }

    setStatus("16. sync package");
    {
        BDbTransaction trans(db_main);
        db_main->exec("DELETE FROM billing.stats_package WHERE server_id = " + app().conf.str_instance_id + " and max_call_id > " + lexical_cast<string>(oldStoredLastId));
        repository.billingData->statsPackage.sync(db_main, &db_calls);
        trans.commit();
    }
}

void TaskRecalc::html(stringstream &html) {
    html << "Recalc from call id: " << recalc_from_call_id << "<br/>\n";
    html << "<br/>\n";
    PageDataBilling::renderBillingData(html, &newBillingData);
    html << "<br/>\n";
    html << "Time load: " << t_load.sloop() << " / " << t_load.sfull() << "<br/>\n";
    html << "Time calc: " << t_calc.sloop() << " / " << t_calc.sfull() << "<br/>\n";
    html << "Time save: " << t_save.sloop() << " / " << t_save.sfull() << "<br/>\n";
}
