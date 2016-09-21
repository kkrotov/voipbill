#include "ThreadSyncCdrs.h"
#include "../classes/AppBill.h"

ThreadSyncCdrs::ThreadSyncCdrs() {
    id = idName();
    name = "Sync Cdrs to central db";

    db_main.setCS(app().conf.db_main);
    db_calls.setCS(app().conf.db_calls);
}

void ThreadSyncCdrs::run() {

    unique_lock<mutex> lock(repository.billingData->syncCallsCentralLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }
    repository.billingData->prepareSyncCallsCentral(&db_main);

    while (syncCallsCdr());
    while (syncCallsCdrUnfinished());
}

bool ThreadSyncCdrs::getCurrentMonths (string &local_prev_sync_month, string &local_curr_sync_month, string &local_next_sync_month) {

    try {

        string select = "select to_char(CURRENT_TIMESTAMP - interval '1 month', 'YYYYMM'),to_char(CURRENT_TIMESTAMP, 'YYYYMM'),to_char(CURRENT_TIMESTAMP + interval '1 month', 'YYYYMM')";
        BDbResult res = db_calls.query(select);
        if (!res.next()) {
            // nothing to sync
            return false;
        }
        local_prev_sync_month = res.get_s(0);
        local_curr_sync_month = res.get_s(1);
        local_next_sync_month = res.get_s(2);
    }
    catch (Exception &e) {

        e.addTrace("ThreadSyncCdrs::getCurrentMonths");
        throw e;
    }
    return true;
}

bool ThreadSyncCdrs::getCurrentMonths (string relname, string fieldname, long long int id, string &local_prev_sync_month, string &local_curr_sync_month, string &local_next_sync_month) {

    try {

        string select = "select date_trunc('month', "+fieldname+") - interval '1 month', date_trunc('month', "+fieldname+"), date_trunc('month', "+fieldname+") + interval '1 month' "
                                    "from "+relname+
                                    " where id>" + lexical_cast<string>(id) + " order by id limit 1";
        BDbResult res = db_calls.query(select);
        if (!res.next()) {
            // nothing to sync
            return false;
        }
        local_prev_sync_month += res.get_s(0).substr(0, 4);
        local_prev_sync_month += res.get_s(0).substr(5, 2);
        local_curr_sync_month += res.get_s(1).substr(0, 4);
        local_curr_sync_month += res.get_s(1).substr(5, 2);
        local_next_sync_month += res.get_s(2).substr(0, 4);
        local_next_sync_month += res.get_s(2).substr(5, 2);
    }
    catch (Exception &e) {
        e.addTrace("ThreadSyncCdrs::getCurrentMonths");
        throw e;
    }
    return true;
}

/*bool ThreadSyncCdrs::syncCallsRaw() {

    string local_prev_sync_month;
    string local_curr_sync_month;
    string local_next_sync_month;
    if (!getCurrentMonths ("calls_raw.calls_raw", "connect_time", repository.billingData->lastSyncCentralCallId, local_prev_sync_month, local_curr_sync_month, local_next_sync_month))
        return false;

    try {

        while (copyCallsPart(local_prev_sync_month)) { }

        while (copyCallsPart(local_curr_sync_month)) { }

        while (copyCallsPart(local_next_sync_month)) { }

    } catch (Exception &e) {
        e.addTrace("ThreadSyncCdrs::run::copy(main_last_id:" + lexical_cast<string>(repository.billingData->lastSyncCentralCallId) + ")");
        throw e;
    }
    return true;
}

bool ThreadSyncCdrs::copyCallsPart(string month) {

    long long int central_id, local_id;
    string central_time, local_time;

    if (!db_calls.rel_exists("calls_raw.calls_raw_" + month ))
        return false;

    auto res1 = db_calls.query("select id, connect_time from calls_raw.calls_raw_" + month + " order by id desc limit 1");
    if (!res1.next())
        return false;

    local_id = res1.get_ll(0);
    local_time = res1.get(1);

    central_id = 0;
    central_time = "";
    if (db_main.rel_exists("calls_raw.calls_raw_" + month )) {

        auto res2 = db_main.query("select id, connect_time from calls_raw.calls_raw_" + month + " where server_id = " + app().conf.str_instance_id + " order by id desc limit 1");
        if (res2.next()) {
            central_id = res2.get_ll(0);
            central_time = res2.get(1);
        }
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


    try {

        string field_names ="server_id,id,orig,peer_id,cdr_id,connect_time,trunk_id,account_id,trunk_service_id,number_service_id,src_number,dst_number,billed_time,rate,"
                "cost,tax_cost,interconnect_rate,interconnect_cost,service_package_id,service_package_stats_id,package_time,package_credit,destination_id,pricelist_id,prefix,"
                "geo_id,geo_operator_id,mob,operator_id,geo_mob,our,trunk_settings_stats_id,disconnect_cause,account_version,stats_nnp_package_minute_id";

        string field_types = "server_id integer,"
                "  id bigint,"
                "  orig boolean,"
                "  peer_id bigint,"
                "  cdr_id bigint,"
                "  connect_time timestamp,"
                "  trunk_id integer,"
                "  account_id integer,"
                "  trunk_service_id integer,"
                "  number_service_id integer,"
                "  src_number bigint,"
                "  dst_number bigint,"
                "  billed_time integer,"
                "  rate double precision,"
                "  cost double precision,"
                "  tax_cost double precision,"
                "  interconnect_rate double precision,"
                "  interconnect_cost double precision,"
                "  service_package_id integer,"
                "  service_package_stats_id integer,"
                "  package_time integer,"
                "  package_credit double precision,"
                "  destination_id integer,"
                "  pricelist_id integer,"
                "  prefix bigint,"
                "  geo_id integer,"
                "  geo_operator_id integer,"
                "  mob boolean,"
                "  operator_id integer,"
                "  geo_mob boolean,"
                "  our boolean,"
                "  trunk_settings_stats_id integer,"
                "  disconnect_cause smallint,"
                "  account_version integer,"
                "  stats_nnp_package_minute_id integer";

        string select = "select "+ app().conf.str_instance_id + ", id,orig,peer_id,cdr_id,connect_time,trunk_id,account_id,trunk_service_id,number_service_id,src_number,dst_number,billed_time,rate,"
                                "cost,tax_cost,interconnect_rate,interconnect_cost,service_package_id,service_package_stats_id,package_time,package_credit,destination_id,pricelist_id,prefix,"
                                "geo_id,geo_operator_id,mob,operator_id,geo_mob,our,trunk_settings_stats_id,disconnect_cause,account_version,stats_nnp_package_minute_id "
                        "from calls_raw.calls_raw_" + month +
                        " where id>" + lexical_cast<string>(central_id) +
                        " order by id limit 100000";
        BDb::copy_dblink("calls_raw.calls_raw", field_names, field_types, select, &db_calls, &db_main);
        return true;
    }
    catch (Exception e) {

        std::string message = "Error syncronizing cdr tables: "+e.message;
        Log::error(message);
        return false;
    }
}
*/
bool ThreadSyncCdrs::syncCallsCdr() {

    string local_prev_sync_month;
    string local_curr_sync_month;
    string local_next_sync_month;
    if (!getCurrentMonths("calls_cdr.cdr", "setup_time", repository.billingData->lastSyncCentralCdrId,
                          local_prev_sync_month, local_curr_sync_month, local_next_sync_month))
        return false;

    return copyCallsCdr(local_curr_sync_month, 100000);
}

bool ThreadSyncCdrs::copyCallsCdr(string month, int limit) {

    long long int central_id=repository.billingData->lastSyncCentralCdrId,
            local_id=0;
    auto res = db_calls.query("select id from calls_cdr.cdr_" + month + " order by id desc limit 1");
    if (res.next())
        local_id = res.get_ll(0);

    if (local_id <= central_id)
        return false;

    BDb::copy("calls_cdr.cdr_" + month,
              "",
              "server_id,id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,dst_route,"
                      "src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party",
              "select "+app().conf.str_instance_id +",id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,dst_route,"
                       "src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party "
              "from calls_cdr.cdr_" + month +
              "  where id>" + lexical_cast<string>(central_id) +
              "  order by id limit "+lexical_cast<string>(limit),
              &db_calls, &db_main);

    return true;
}

bool ThreadSyncCdrs::copyDblinkCallsCdr(string month) {

    try {

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
                "from calls_cdr.cdr_"+month+" where id>"+ lexical_cast<string>(repository.billingData->lastSyncCentralCdrId)+" order by id limit 100000";

        BDb::copy_dblink("calls_cdr.cdr", field_names, field_types, select, &db_calls, &db_main);
        return true;
    }
    catch (Exception e) {

        std::string message = "Error syncronizing cdr tables: "+e.message;
        Log::error(message);
        return false;
    }
}

bool ThreadSyncCdrs::syncCallsCdrUnfinished() {

    string local_prev_sync_month;
    string local_curr_sync_month;
    string local_next_sync_month;
    if (!getCurrentMonths("calls_cdr.cdr_unfinished", "setup_time",repository.billingData->lastSyncCentralCdrUnfinishedId, local_prev_sync_month, local_curr_sync_month, local_next_sync_month) &&
        !getCurrentMonths(local_prev_sync_month, local_curr_sync_month, local_next_sync_month))
        return false;

    return copyCallsCdrUnfinished(local_curr_sync_month, 100000);
}

bool ThreadSyncCdrs::copyCallsCdrUnfinished(string month, int limit) {

    long long int central_id=repository.billingData->lastSyncCentralCdrUnfinishedId,local_id=0;
    auto res = db_calls.query("select id from calls_cdr.cdr_unfinished_"+month+" where server_id = " + app().conf.str_instance_id + " order by id desc limit 1");
    if (res.next())
        local_id = res.get_ll(0);

    if (local_id <= central_id)
        return false;

    BDb::copy("calls_cdr.cdr_unfinished_" + month,
              "",
              "server_id,id,call_id,setup_time,hash,dst_route,releasing_party,release_timestamp,disconnect_cause,src_number,dst_number,src_route",
              "select "+ app().conf.str_instance_id +",id,call_id,setup_time,hash,dst_route,releasing_party,release_timestamp,disconnect_cause,src_number,dst_number,src_route "
              "from calls_cdr.cdr_unfinished_" + month +
              "  where id>" + lexical_cast<string>(central_id) +
              "  order by id limit "+lexical_cast<string>(limit),
              &db_calls, &db_main);

    return true;
}

bool ThreadSyncCdrs::copyDblinkCallsCdrUnfinished(string local_curr_sync_month) {

    try {

        string field_names = "server_id,id,call_id,setup_time,hash,dst_route,releasing_party,release_timestamp,disconnect_cause,src_number,dst_number,src_route";
        string field_types ="server_id integer,"
                        "  id bigint,"
                        "  call_id bigint,"
                        "  setup_time timestamp,"
                        "  hash uuid,"
                        "  dst_route character varying(32),"
                        "  releasing_party character varying,"
                        "  release_timestamp timestamp,"
                        "  disconnect_cause smallint,"
                        "  src_number character varying(32),"
                        "  dst_number character varying(32),"
                        "  src_route character varying(32)";
        string select = "select "+ app().conf.str_instance_id +",id,call_id,setup_time,hash,dst_route,releasing_party,release_timestamp,disconnect_cause,src_number,dst_number,src_route "
                "from calls_cdr.cdr_unfinished_"+local_curr_sync_month+" where id>"+ lexical_cast<string>(repository.billingData->lastSyncCentralCdrUnfinishedId)+" order by id limit 100000";

        BDb::copy_dblink("calls_cdr.cdr_unfinished", field_names, field_types, select, &db_calls, &db_main);
        return true;
    }
    catch (Exception e) {

        std::string message = "Error syncronizing cdr_unfinished tables: "+e.message;
        Log::error(message);
        return false;
    }
}
/*
void ThreadSyncCdrs::htmlfull(stringstream &html) {
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

bool ThreadSyncCdrs::hasFullHtml() {
    return true;
}
*/