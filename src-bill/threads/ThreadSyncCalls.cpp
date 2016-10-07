#include "ThreadSyncCalls.h"

ThreadSyncCalls::ThreadSyncCalls() {
    id = idName();
    name = "Sync Calls to central db";
    threadSleepSeconds = 10;

    db_main.setCS(app().conf.db_main);
    db_calls.setCS(app().conf.db_calls);

    last_central_id = 0;
    last_local_id = 0;
}

bool ThreadSyncCalls::getCurrentMonths(string &local_prev_sync_month, string &local_curr_sync_month,
                                       string &local_next_sync_month) {

    try {

        BDbResult res = db_calls.query(
                "select date_trunc('month', connect_time) - interval '1 month', date_trunc('month', connect_time), date_trunc('month', connect_time) + interval '1 month' "
                        "from calls_raw.calls_raw where id>" +
                lexical_cast<string>(repository.billingData->lastSyncCentralCallId) + " order by id limit 1");
        if (!res.next()) {
            // nothing to sync
            return false;
        }
        local_prev_sync_month = res.get_s(0);
        local_curr_sync_month = res.get_s(1);
        local_next_sync_month = res.get_s(2);
    }
    catch (Exception &e) {

        e.addTrace("ThreadSyncCalls::run::get_local_sync_month");
        throw e;
    }
    return true;
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
    if (!getCurrentMonths(local_prev_sync_month, local_curr_sync_month, local_next_sync_month))
        return;

    try {

        while (copyCallsPart(local_prev_sync_month, 100000)) { }

        while (copyCallsPart(local_curr_sync_month, 100000)) { }

        while (copyCallsPart(local_next_sync_month, 100000)) { }

    }
    catch (Exception &e) {

        e.addTrace("ThreadSyncCalls::run::copy(main_last_id:" +
                   lexical_cast<string>(repository.billingData->lastSyncCentralCallId) + ")");
        throw e;
    }
}

bool ThreadSyncCalls::getLocalId(string month, long long &local_id, string &local_time) {

    auto res1 = db_calls.query(
            "select id, connect_time from calls_raw.calls_raw_" + month + " order by id desc limit 1");
    if (res1.next()) {

        local_id = res1.get_ll(0);
        local_time = res1.get(1);
        return true;
    }
    else {

        return false;
    }
}

void ThreadSyncCalls::getCentalId(string month, long long int &central_id, string &central_time) {

    auto res2 = db_main.query("select id, connect_time from calls_raw.calls_raw_" + month + " where server_id = " +
                              app().conf.str_instance_id + " order by id desc limit 1");
    if (res2.next()) {

        central_id = res2.get_ll(0);
        central_time = res2.get(1);
    }
    else {

        central_id = 0;
        central_time = "";
    }
}

bool ThreadSyncCalls::copyCallsPart(string month, unsigned long limit) {

    string suffix = month.substr(0, 4) + month.substr(5, 2);
    string relname = "calls_raw.calls_raw_" + suffix;
    if (!db_main.rel_exists(relname)) {

        string create_partition =
                "select calls_raw.create_calls_raw_partition('" + month + "'::timestamp without time zone)";
        db_main.query(create_partition);
    }
    if (!db_calls.rel_exists(relname)) {

        string create_partition =
                "select calls_raw.create_calls_raw_partition('" + month + "'::timestamp without time zone)";
        db_calls.query(create_partition);
    }

    long long int central_id, local_id;
    string central_time, local_time;

    if (!getLocalId(suffix, local_id, local_time))
        return false;

    getCentalId(suffix, central_id, central_time);

    last_central_month = suffix;
    last_central_id = central_id;
    last_central_time = central_time;

    last_local_month = suffix;
    last_local_id = local_id;
    last_local_time = local_time;

    if (local_id <= central_id) {
        return false;
    }

    try {

        BDb::copy("calls_raw.calls_raw_" + suffix,
                  "",
                  " id, orig, our, peer_id, cdr_id, connect_time, trunk_id, account_id, trunk_service_id, number_service_id, "\
                " src_number, dst_number, billed_time, rate, cost, tax_cost, interconnect_rate, interconnect_cost, service_package_id,"\
                " service_package_stats_id, package_time, package_credit, trunk_settings_stats_id, destination_id, pricelist_id, prefix,"\
                " nnp_operator_id, nnp_region_id, nnp_city_id, nnp_country_prefix, nnp_ndc, nnp_is_mob, "\
                " nnp_package_minute_id, nnp_package_price_id, nnp_package_pricelist_id,  "
                " geo_id, geo_operator_id, mob, geo_mob, server_id, disconnect_cause, account_version, stats_nnp_package_minute_id",

                  " select id, orig, our, peer_id, cdr_id, connect_time, trunk_id, account_id, trunk_service_id, number_service_id,"\
                " src_number, dst_number, billed_time, rate, cost, tax_cost, interconnect_rate, interconnect_cost, service_package_id,"\
                " service_package_stats_id, package_time, package_credit, trunk_settings_stats_id, destination_id, pricelist_id, prefix,"\
                " nnp_operator_id, nnp_region_id, nnp_city_id, nnp_country_prefix, nnp_ndc, nnp_is_mob, "\
                " nnp_package_minute_id, nnp_package_price_id, nnp_package_pricelist_id,  "
                " geo_id, geo_operator_id, mob, geo_mob, " + app().conf.str_instance_id +
                  ", disconnect_cause, account_version, stats_nnp_package_minute_id  " \

                          "   from calls_raw.calls_raw_" + suffix +
                  "   where id>" + lexical_cast<string>(central_id) +
                  "   order by id limit " + lexical_cast<string>(limit),
                  &db_calls, &db_main);

    }
    catch (Exception e) {

        std::string message = "Error syncronizing cdr tables: " + e.message;
        Log::error(message);
        return false;
    }
    return true;
}

void ThreadSyncCalls::htmlfull(stringstream &html) {
    this->html(html);

    html << "Local Call Table: <b>calls_raw.calls_raw_" << last_local_month << "</b><br/>\n";
    html << "Local Call Id: <b>" << lexical_cast<string>(last_local_id) << "</b><br/>\n";
    html << "Local Call Time: <b>" << last_local_time << "</b><br/>\n";
    html << "<br/>\n";

    html << "Central Call Table: <b>calls_raw.calls_raw_" << last_central_month << "</b><br/>\n";
    html << "Central Call Id: <b>" << lexical_cast<string>(last_central_id) << "</b><br/>\n";
    html << "Central Call Time: <b>" << last_central_time << "</b><br/>\n";
    html << "<br/>\n";

    html << "Table Queue: <b>" << lexical_cast<string>(last_local_id - last_central_id) << "</b><br/>\n";
    html << "<br/>\n";
}

bool ThreadSyncCalls::hasFullHtml() {
    return true;
}


