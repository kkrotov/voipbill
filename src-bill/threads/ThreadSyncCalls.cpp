#include "ThreadSyncCalls.h"
#include "../classes/AppBill.h"

ThreadSyncCalls::ThreadSyncCalls() {
    id = "sync_calls";
    name = "Sync Calls";

    db_main.setCS(app().conf.db_main);
    db_calls.setCS(app().conf.db_calls);
}

void ThreadSyncCalls::run() {


    long long int main_last_id;
    try {
        BDbResult res = db_main.query("select max(id) from calls.calls_" + app().conf.str_instance_id);
        main_last_id = res.next() ? res.get_ll(0) : 0;
    } catch (Exception &e) {
        e.addTrace("ThreadSaveCounters::save_calls::get_main_last_id");
        throw e;
    }

    string local_sync_month;
    try {
        BDbResult res = db_calls.query("select month from calls.calls where id>" + lexical_cast<string>(main_last_id) + " order by id");
        if (!res.next()) {
            // nothing to sync
            return;
        }
        local_sync_month += res.get_s(0).substr(0, 4);
        local_sync_month += res.get_s(0).substr(5, 2);
    } catch (Exception &e) {
        e.addTrace("ThreadSaveCounters::save_calls::get_local_sync_month");
        throw e;
    }

    try {
        BDb::copy("calls.calls_" + app().conf.str_instance_id + "_" + local_sync_month,
                "",
                "       id, time, direction_out, usage_num, phone_num, redirect_num, len, usage_id, pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, month, day, amount, amount_op, client_id, region, geo_id, geo_operator_id, pricelist_op_id, price, price_op, len_mcn, len_op, prefix_geo, prefix_mcn, prefix_op, srv_region_id",
                "select id, time, direction_out, usage_num, phone_num, redirect_num, len, usage_id, pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, month, day, amount, amount_op, client_id, region, geo_id, geo_operator_id, pricelist_op_id, price, price_op, len_mcn, len_op, prefix_geo, prefix_mcn, prefix_op, " + app().conf.str_instance_id + "::smallint from calls.calls_" + local_sync_month + " where id>" + lexical_cast<string>(main_last_id) + " order by id limit 100000",
                &db_calls, &db_main);

    } catch (Exception &e) {
        e.addTrace("ThreadSaveCounters::save_calls::copy(main_last_id:" + lexical_cast<string>(main_last_id) + ")");
        throw e;
    }
}

void ThreadSyncCalls::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";

}

