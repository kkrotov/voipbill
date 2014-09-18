#include "ThreadSyncCalls.h"
#include "../classes/AppBill.h"

ThreadSyncCalls::ThreadSyncCalls() {
    id = "sync_calls";
    name = "Sync Calls to central db";

    db_main.setCS(app().conf.db_main);
    db_calls.setCS(app().conf.db_calls);
}

void ThreadSyncCalls::run() {

    try {
        BDbResult res = db_main.query("select id, time from calls.calls_" + app().conf.str_instance_id + " order by id desc limit 1");
        if (res.next()) {
            main_last_id = res.get_s(0);
            main_last_time = res.get_s(1);
        } else {
            main_last_id = "0";
            main_last_time = "";
        }
    } catch (Exception &e) {
        e.addTrace("ThreadSyncCalls::run::get_main_last_id");
        throw e;
    }

    string local_sync_month;
    string local_prev_sync_month;
    try {
        BDbResult res = db_calls.query("select month, month - interval '1 month' from calls.calls where id>" + main_last_id + " order by id limit 1");
        if (!res.next()) {
            // nothing to sync
            return;
        }
        local_sync_month += res.get_s(0).substr(0, 4);
        local_sync_month += res.get_s(0).substr(5, 2);
        local_prev_sync_month += res.get_s(1).substr(0, 4);
        local_prev_sync_month += res.get_s(1).substr(5, 2);
    } catch (Exception &e) {
        e.addTrace("ThreadSyncCalls::run::get_local_sync_month");
        throw e;
    }

    bool transaction = false;
    try {
        db_calls.exec("BEGIN");
        transaction = true;

        copyCallsPart(local_prev_sync_month);

        copyCallsPart(local_sync_month);

        db_calls.exec("COMMIT");
        transaction = false;

    } catch (Exception &e) {

        if (transaction) {
            try {
                db_calls.exec("ROLLBACK");
            } catch (...) { }
        }

        e.addTrace("ThreadSyncCalls::run::copy(main_last_id:" + main_last_id + ")");
        throw e;
    }
}

void ThreadSyncCalls::copyCallsPart(string month) {
    BDb::copy("calls.calls_" + app().conf.str_instance_id + "_" + month,
        "",
        "       id, time, direction_out, usage_num, phone_num, redirect_num, len, usage_id, pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, month, day, amount, amount_op, client_id, region, geo_id, geo_operator_id, pricelist_op_id, price, price_op, len_mcn, len_op, prefix_geo, prefix_mcn, prefix_op, srv_region_id",
        "select id, time, direction_out, usage_num, phone_num, redirect_num, len, usage_id, pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, month, day, amount, amount_op, client_id, region, geo_id, geo_operator_id, pricelist_op_id, price, price_op, len_mcn, len_op, prefix_geo, prefix_mcn, prefix_op, " + app().conf.str_instance_id + "::smallint " \
        "   from calls.calls_" + month + 
        "   where id>" + main_last_id +
        "   order by id limit 100000",
        &db_calls, &db_main);
}

void ThreadSyncCalls::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";
    html << "Last Id: <b>" << main_last_id << "</b><br/>\n";
    html << "Last Time: <b>" << main_last_time << "</b><br/>\n";

}

