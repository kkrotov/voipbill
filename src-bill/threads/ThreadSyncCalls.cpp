#include "ThreadSyncCalls.h"
#include "../classes/AppBill.h"

ThreadSyncCalls::ThreadSyncCalls() {
    id = idName();
    name = "Sync Calls to central db";

    db_main.setCS(app().conf.db_main);
    db_calls.setCS(app().conf.db_calls);

    last_central_id = 0;
    last_local_id = 0;
}

void ThreadSyncCalls::run() {

    unique_lock<mutex> lock(repository.billingData->syncCallsCentralLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }
    repository.billingData->prepareSyncCallsCentral(&db_main);

    string local_prev_sync_month;
    string local_curr_sync_month;
    string local_next_sync_month;
    try {
        BDbResult res = db_calls.query("select date_trunc('month', connect_time) - interval '1 month', "
                                               "date_trunc('month', connect_time), date_trunc('month', connect_time) + interval '1 month' "
                                               "from calls_raw.calls_raw "
                                               "where id>" + lexical_cast<string>(repository.billingData->lastSyncCentralCallId) +
                                       " order by id limit 1");
        if (!res.next()) {
            // nothing to sync
            return;
        }
        local_prev_sync_month += res.get_s(0).substr(0, 4);
        local_prev_sync_month += res.get_s(0).substr(5, 2);
        local_curr_sync_month += res.get_s(1).substr(0, 4);
        local_curr_sync_month += res.get_s(1).substr(5, 2);
        local_next_sync_month += res.get_s(2).substr(0, 4);
        local_next_sync_month += res.get_s(2).substr(5, 2);
    } catch (Exception &e) {
        e.addTrace("ThreadSyncCalls::run::get_local_sync_month");
        throw e;
    }

    syncCallsRaw(local_prev_sync_month,local_curr_sync_month,local_next_sync_month);
    syncCallsCdr(local_curr_sync_month);
}

bool ThreadSyncCalls::syncCallsRaw(string local_prev_sync_month,string local_curr_sync_month,string local_next_sync_month) {

    try {

        while (copyCallsPart(local_prev_sync_month)) { }

        while (copyCallsPart(local_curr_sync_month)) { }

        while (copyCallsPart(local_next_sync_month)) { }

    } catch (Exception &e) {
        e.addTrace("ThreadSyncCalls::run::copy(main_last_id:" + lexical_cast<string>(repository.billingData->lastSyncCentralCallId) + ")");
        throw e;
    }
    return true;
}

bool ThreadSyncCalls::copyCallsPart(string month) {
    long long int central_id, local_id;
    string central_time, local_time;

    auto res1 = db_calls.query("select id, connect_time from calls_raw.calls_raw_" + month + " order by id desc limit 1");
    if (res1.next()) {
        local_id = res1.get_ll(0);
        local_time = res1.get(1);
    } else {
        return false;
    }

    auto res2 = db_main.query("select id, connect_time from calls_raw.calls_raw_" + month + " where server_id = " + app().conf.str_instance_id + " order by id desc limit 1");
    if (res2.next()) {
        central_id = res2.get_ll(0);
        central_time = res2.get(1);
    } else {
        central_id = 0;
        central_time = "";
    }

    last_central_month = month;
    last_central_id = central_id;
    last_central_time = central_time;

    last_local_month = month;
    last_local_id = local_id;
    last_local_time = local_time;

    if (local_id <= central_id) {
        return false;
    }


    BDb::copy("calls_raw.calls_raw_" + month,
        "",
        "       id, orig, our, peer_id, cdr_id, connect_time, trunk_id, account_id, trunk_service_id, number_service_id, src_number, dst_number, billed_time, rate, cost, tax_cost, interconnect_rate, interconnect_cost, service_package_id, service_package_stats_id, package_time, package_credit, trunk_settings_stats_id, destination_id, pricelist_id, prefix, geo_id, geo_operator_id, mob, geo_mob, server_id, disconnect_cause",
        "select id, orig, our, peer_id, cdr_id, connect_time, trunk_id, account_id, trunk_service_id, number_service_id, src_number, dst_number, billed_time, rate, cost, tax_cost, interconnect_rate, interconnect_cost, service_package_id, service_package_stats_id, package_time, package_credit, trunk_settings_stats_id, destination_id, pricelist_id, prefix, geo_id, geo_operator_id, mob, geo_mob, " + app().conf.str_instance_id + ", disconnect_cause  " \
        "   from calls_raw.calls_raw_" + month +
        "   where id>" + lexical_cast<string>(central_id) +
        "   order by id limit 100000",
        &db_calls, &db_main);

    return true;
}

bool ThreadSyncCalls::syncCallsCdr(string month) {

    string field_names = "server_id,id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,"
            "dst_route,src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party";
    string field_types ="server_id integer,"
                        "  id bigint,"
                        "  call_id bigint,"
                        "  nas_ip inet,"
                        "  src_number character varying(32),"
                        "  dst_number character varying(32),"
                        "  redirect_number character varying(32),"
                        "  setup_time timestamp without time zone,"
                        "  connect_time timestamp without time zone,"
                        "  disconnect_time timestamp without time zone,"
                        "  session_time bigint,"
                        "  disconnect_cause smallint,"
                        "  src_route character varying(32),"
                        "  dst_route character varying(32),"
                        "  src_noa smallint,"
                        "  dst_noa smallint,"
                        "  hash uuid,"
                        "  dst_replace character varying(32),"
                        "  call_finished character varying,"
                        "  releasing_party character varying";
    string select = "select "+ app().conf.str_instance_id +",id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,"
                            "dst_route,src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party "
                    "from calls_cdr.cdr_"+month+" where id>"+ lexical_cast<string>(repository.billingData->lastSyncCentralCdrId);

    try {

        BDb::copy_dblink("calls_cdr.cdr_"+month, field_names, field_types, select, &db_calls, &db_main);
        return true;
    }
    catch (Exception e) {

        std::string message = "Error syncronizing cdr tables: "+e.message;
        Log::error(message);
        return false;
    }
}

void ThreadSyncCalls::htmlfull(stringstream &html) {
    this->html(html);

    html << "Local Table: <b>calls_raw.calls_raw_" << last_local_month << "</b><br/>\n";
    html << "Local Call Id: <b>" << lexical_cast<string>(last_local_id) << "</b><br/>\n";
    html << "Local Call Time: <b>" << last_local_time << "</b><br/>\n";
    html << "<br/>\n";

    html << "Central Table: <b>calls_raw.calls_raw_" << last_central_month << "</b><br/>\n";
    html << "Central Call Id: <b>" << lexical_cast<string>(last_central_id) << "</b><br/>\n";
    html << "Central Call Time: <b>" << last_central_time << "</b><br/>\n";
    html << "<br/>\n";

    html << "Table Queue: <b>" << lexical_cast<string>(last_local_id - last_central_id) << "</b><br/>\n";
    html << "<br/>\n";
}

bool ThreadSyncCalls::hasFullHtml() {
    return true;
}


