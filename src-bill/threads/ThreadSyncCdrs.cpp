#include "ThreadSyncCdrs.h"
#include "../classes/AppBill.h"

ThreadSyncCdrs::ThreadSyncCdrs() {
    id = idName();
    name = "Sync Cdrs to central db";
    threadSleepSeconds = app().conf.cdr_parcer_interval;

    db_main.setCS(app().conf.db_main);
    db_calls.setCS(app().conf.db_calls);

    last_cdr_central_id = 0;
    last_cdr_local_id = 0;

    last_cdr_unfin_central_id = 0;
    last_cdr_unfin_local_id = 0;

}

void ThreadSyncCdrs::run() {

    unique_lock<mutex> lock(repository.billingData->syncCdrsCentralLock, try_to_lock);
    if (!lock.owns_lock()) {

        Log::error("ThreadSyncCdrs: mutex is locked");
        return;
    }
    repository.billingData->prepareSyncCallsCentral(&db_main);

    while (syncCallsCdr());
    while (syncCallsCdrUnfinished());

    repository.billingData->prepareSyncCallsCentral(&db_main);
}

bool ThreadSyncCdrs::getCurrentMonths (string &local_prev_sync_month, string &local_curr_sync_month, string &local_next_sync_month) {

    string select;
    try {

        select = "select date_trunc('month', CURRENT_TIMESTAMP) - interval '1 month',date_trunc('month', CURRENT_TIMESTAMP),date_trunc('month', CURRENT_TIMESTAMP) + interval '1 month'";
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

        std::string message = "ThreadSyncCdrs::getCurrentMonths ERROR: "+select+" "+e.message;
        Log::error(message);
    }
    return true;
}

bool ThreadSyncCdrs::getCurrentMonths (string relname, string fieldname, long long int id, string &local_prev_sync_month, string &local_curr_sync_month, string &local_next_sync_month) {

    string select;
    try {

        select = "select date_trunc('month', "+fieldname+") - interval '1 month', date_trunc('month', "+fieldname+"), date_trunc('month', "+fieldname+") + interval '1 month' "
                                    "from "+relname+
                                    " where id>" + lexical_cast<string>(id) + " order by id limit 1";
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

        std::string message = "ThreadSyncCdrs::getCurrentMonths ERROR: "+select+" "+e.message;
        Log::error(message);
    }
    return true;
}

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

    string suffix = month.substr(0, 4)+month.substr(5, 2);
    string relname = "calls_cdr.cdr_" + suffix;
    if (!db_main.rel_exists(relname))
        db_main.query("select calls_cdr.create_calls_cdr_partition('"+month+"'::timestamp without time zone)");

    long long int central_id=0, local_id=0;
    string central_time, local_time;
    auto res_main = db_main.query("select id, setup_time from "+relname+" where server_id="+app().conf.str_instance_id+" order by id desc limit 1");
    if (res_main.next()) {

        central_id = res_main.get_ll(0);
        central_time = res_main.get(1);
    }

    auto res_calls = db_calls.query("select id, setup_time from " + relname + " order by id desc limit 1");
    if (res_calls.next()) {

        local_id = res_calls.get_ll(0);
        local_time = res_calls.get(1);
    }
    last_cdr_central_month = suffix;
    last_cdr_central_id = central_id;
    last_cdr_central_time = central_time;

    last_cdr_local_month = suffix;
    last_cdr_local_id = local_id;
    last_cdr_local_time = local_time;

    if (local_id <= central_id)
        return false;

    BDb::copy(relname, "",
              "server_id,id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,dst_route,"
                      "src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party",
              "select "+app().conf.str_instance_id +",id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,dst_route,"
                       "src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party "
              "from "+relname+"  where id>"+lexical_cast<string>(central_id)+" order by id limit "+lexical_cast<string>(limit),
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

    string suffix = month.substr(0, 4)+month.substr(5, 2);
    string relname = "calls_cdr.cdr_unfinished_" + suffix;

    if (!db_calls.rel_exists(relname))
        return true;
    
    if (!db_main.rel_exists(relname))
        db_main.query("select calls_cdr.create_calls_cdr_unfinished_partition('"+month+"'::timestamp without time zone)");

    long long int central_id=0,local_id=0;
    string central_time, local_time;
    auto res_main = db_main.query("select id,setup_time from "+relname+" where server_id = " + app().conf.str_instance_id + " order by id desc limit 1");
    if (res_main.next()) {

        central_id = res_main.get_ll(0);
        central_time = res_main.get(1);
    }
    auto res_calls = db_calls.query("select id,setup_time from "+relname+" order by id desc limit 1");
    if (res_calls.next()) {

        local_id = res_calls.get_ll(0);
        local_time = res_calls.get(1);
    }
    last_cdr_unfin_central_month = suffix;
    last_cdr_unfin_central_time = central_time;
    last_cdr_unfin_central_id = central_id;

    last_cdr_unfin_local_month = suffix;
    last_cdr_unfin_local_time = local_time;
    last_cdr_unfin_local_id = local_id;

    if (local_id <= central_id)
        return false;

    BDb::copy(relname, "",
              "server_id,id,call_id,setup_time,hash,dst_route,releasing_party,release_timestamp,disconnect_cause,src_number,dst_number,src_route",
              "select "+ app().conf.str_instance_id +",id::bigint,call_id,setup_time,hash,dst_route,releasing_party,release_timestamp,disconnect_cause,src_number,dst_number,src_route "
              "from "+relname+"  where id>"+lexical_cast<string>(central_id)+" order by id limit "+lexical_cast<string>(limit),
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

void ThreadSyncCdrs::htmlfull(stringstream &html) {

    this->html(html);

    html << "Local Cdr Table: <b>calls_cdr.cdr_" << last_cdr_local_month << "</b><br/>\n";
    html << "Local Cdr Id: <b>" << lexical_cast<string>(last_cdr_local_id) << "</b><br/>\n";
    html << "Local Cdr Time: <b>" << last_cdr_local_time << "</b><br/>\n";
    html << "Local Cdr Unfinished Table: <b>calls_cdr.cdr_" << last_cdr_unfin_local_month << "</b><br/>\n";
    html << "Local Cdr Unfinished Id: <b>" << lexical_cast<string>(last_cdr_unfin_local_id) << "</b><br/>\n";
    html << "Local Cdr Unfinished Time: <b>" << last_cdr_unfin_local_time << "</b><br/>\n";
    html << "<br/>\n";

    html << "Central Cdr Table: <b>calls_cdr.cdr_" << last_cdr_central_month << "</b><br/>\n";
    html << "Central Cdr Id: <b>" << lexical_cast<string>(last_cdr_central_id) << "</b><br/>\n";
    html << "Central Cdr Time: <b>" << last_cdr_central_time << "</b><br/>\n";
    html << "Central Cdr Unfinished Table: <b>calls_cdr.cdr_" << last_cdr_unfin_central_month << "</b><br/>\n";
    html << "Central Cdr Unfinished Id: <b>" << lexical_cast<string>(last_cdr_unfin_central_id) << "</b><br/>\n";
    html << "Central Cdr Unfinished Time: <b>" << last_cdr_unfin_central_time << "</b><br/>\n";
    html << "<br/>\n";

    html << "Table Queue: <b>" << lexical_cast<string>(last_cdr_local_id - last_cdr_central_id) << "</b><br/>\n";
    html << "<br/>\n";
}

bool ThreadSyncCdrs::hasFullHtml() {
    return true;
}