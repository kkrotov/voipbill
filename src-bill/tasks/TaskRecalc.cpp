#include "TaskRecalc.h"

#include "../../src/common.h"
#include "../classes/AppBill.h"
#include "../classes/CallsSaver.h"
#include "../threads/ThreadLoader.h"
#include "../threads/ThreadBillRuntime.h"
#include "../classes/CdrLoader.h"

TaskRecalc::TaskRecalc(time_t date_from, string server_id) : Task(server_id) {
    this->date_from = date_from;
    name = "Recalc from " + string_time(date_from);
    recalc_from_call_id = 0;
}

void TaskRecalc::run() {

    BDb db_calls(app().conf.db_calls);

    setStatus("1. waiting save calls lock");
    unique_lock<mutex> lock_save_calls(DataBillingContainer::instance()->saveCallsLock);

    setStatus("2. waiting sync calls central lock");
    unique_lock<mutex> lock_sync_calls_central(DataBillingContainer::instance()->syncCallsCentralLock);


    setStatus("3. getting min id");
    BDbResult res = db_calls.query(
            "select min(id) from calls_raw.calls_raw where connect_time>='" + string_time(date_from) + "'");
    recalc_from_call_id = res.next() ? res.get_ll(0) : 0;

    if (recalc_from_call_id == 0) {
        Log::error("Nothing to recalc");
        return;
    }

    Log::notice("Recalc calls: date " + string_time(date_from) +", id >= " + lexical_cast<string>(recalc_from_call_id));

    boost::this_thread::interruption_point();

    setStatus("4. delete calls_raw from id " + lexical_cast<string>(recalc_from_call_id));
    db_calls.exec("delete from calls_raw.calls_raw where id>=" + lexical_cast<string>(recalc_from_call_id));

    boost::this_thread::interruption_point();

    setStatus("5. recalc temp data");
    DataContainer data;
    data.loadAll(&db_calls);

    boost::this_thread::interruption_point();

    setStatus("6. recalc temp counters");
    billingData.loadAll(&db_calls);

    boost::this_thread::interruption_point();

    setStatus("7. calc");

    CdrLoader cdrLoader;
    cdrLoader.setDb(&db_calls);
    cdrLoader.setBillingData(&billingData);

    Billing billing;
    billing.setData(&data);
    billing.setBillingData(&billingData);

    CallsSaver saver;
    saver.setDb(&db_calls);
    saver.setBillingData(&billingData);


    while (true) {
        const size_t rows_per_request = 25000;
        const size_t save_part_count = 50000;

        {
            TimerScope ts(t_load);
            cdrLoader.load(rows_per_request);
        }

        if (billingData.cdrsWaitProcessing.size() == 0) {
            break;
        }


        {
            TimerScope ts(t_calc);
            billing.calc();
        }

        {
            TimerScope ts(t_save);
            saver.save(save_part_count);
        }

        boost::this_thread::interruption_point();

        setStatus("7. calc " + lexical_cast<string>(billingData.savedCallsCount));
    }

    boost::this_thread::interruption_point();

    setStatus("8. waiting fetch cdr lock");
    unique_lock<mutex> lock_fetch_cdr(DataBillingContainer::instance()->fetchCdrLock);

    setStatus("9. waiting calc calls lock");
    unique_lock<mutex> lock_calc_calls(DataBillingContainer::instance()->calcCallsLock);

    setStatus("10. waiting sync counters central lock");
    unique_lock<mutex> lock_sync_counters_central(DataBillingContainer::instance()->syncCountersCentralLock);

    setStatus("11. waiting sync locks central lock");
    unique_lock<mutex> lock_sync_locks_central(DataBillingContainer::instance()->syncLocksCentralLock);

    setStatus("12. recalc counters");
    DataBillingContainer::instance()->loadAll(&db_calls);

    lock_fetch_cdr.unlock();
    lock_calc_calls.unlock();
    lock_save_calls.unlock();

    boost::this_thread::interruption_point();

    setStatus("13. delete calls_raw from main");
    db_main->exec("delete from calls_raw.calls_raw where server_id = " + app().conf.str_instance_id + " and id>=" + lexical_cast<string>(recalc_from_call_id));

}

void TaskRecalc::html(stringstream &html) {
    html << "Recalc from call id: " << recalc_from_call_id << "<br/>\n";
    html << "<br/>\n";
    billingData.html(html);
    html << "<br/>\n";
    html << "Time load: " << t_load.sloop() << " / " << t_load.sfull() << "<br/>\n";
    html << "Time calc: " << t_calc.sloop() << " / " << t_calc.sfull() << "<br/>\n";
    html << "Time save: " << t_save.sloop() << " / " << t_save.sfull() << "<br/>\n";
}
