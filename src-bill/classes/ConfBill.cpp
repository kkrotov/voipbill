#include "../classes/ConfBill.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

bool ConfBill::parse_config_variables(boost::property_tree::ptree &pt) {

    web_port = pt.get<uint16_t>("main.web_port", 8032);


    db_main = pt.get<string>("db.main");
    db_calls = pt.get<string>("db.calls");
    db_bandwidth_limit_mbits = pt.get<double>("db.bandwidth_limit_mbits");

    string threads = pt.get<string>("main.run_threads", "");
    if (threads.length()>0) {

        std::stringstream ss(threads);
        string tmp;

        while (ss >> tmp) {
            active_threads.push_back(tmp);
        }
    }

    threads = pt.get<string>("main.skip_threads", "");
    if (threads.length()>0) {

        std::stringstream ss(threads);
        string tmp;

        while (ss >> tmp) {
            skip_threads.push_back(tmp);
        }
    }
    calls_raw_sync_delay = pt.get<int>("main.calls_raw_sync_delay", 5);
    instance_id = pt.get<uint16_t>("geo.instance_id");

    str_instance_id = boost::lexical_cast<string>(instance_id);

    sql_regions_list = pt.get<string>("main.sql_regions_list", "(" + str_instance_id + ")");

    hub_id = pt.get<uint16_t>("main.hub_id", 0);


    openca_udp_host = pt.get<string>("udp.host", "");
    openca_udp_port = pt.get<uint16_t>("udp.port", 0);
    udp_openca_select_interval = pt.get<uint16_t>("udp.openca_select_interval", 10);
    udp_force_finish_call_interval = pt.get<uint16_t>("udp.force_finish_call_interval", 5);

    billing_free_seconds = pt.get<uint16_t>("billing.free_seconds", 5);

    global_counters_select_interval = pt.get<uint16_t>("billing.global_counters_select_interval", 10);

    radius_auth_port = pt.get<uint16_t>("radius.auth_port", 1812);
    radius_acct_port = pt.get<uint16_t>("radius.acct_port", 1813);
    radius_secret = pt.get<string>("radius.secret", "");

    cdr_ftp_host = pt.get<string>("cdr_parcer.cdr_ftp_host", "");
    cdr_ftp_user = pt.get<string>("cdr_parcer.cdr_ftp_user", "");
    cdr_ftp_password = pt.get<string>("cdr_parcer.cdr_ftp_password", "");
    cdr_ftp_dir = pt.get<string>("cdr_parcer.cdr_ftp_dir", "");
    cdr_nasip = pt.get<string>("cdr_parcer.cdr_nasip", "127.0.0.1");
    cdr_parcer_interval = pt.get<int>("cdr_parcer.interval", 1800);
}
