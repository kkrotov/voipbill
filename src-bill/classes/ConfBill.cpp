#include "../classes/ConfBill.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

bool ConfBill::parse_config_variables(boost::property_tree::ptree &pt) {

    web_port = pt.get<unsigned short>("main.web_port", 8032);

    db_main = pt.get<string>("db.main");
    db_calls = pt.get<string>("db.calls");

    instance_id = pt.get<unsigned short>("geo.instance_id");
    str_instance_id = boost::lexical_cast<string>(instance_id);

    openca_udp_host = pt.get<string>("udp.host", "");
    openca_udp_port = pt.get<unsigned short>("udp.port", 0);
    udp_openca_select_interval = pt.get<unsigned short>("udp.openca_select_interval", 10);
    udp_force_finish_call_interval = pt.get<unsigned short>("udp.force_finish_call_interval", 5);

    billing_free_seconds = pt.get<unsigned short>("billing.free_seconds", 5);

    global_counters_select_interval = pt.get<unsigned short>("billing.global_counters_select_interval", 10);

    cdr_ftp_host = pt.get<string>("cdr_parcer.cdr_ftp_host", "");
    cdr_ftp_user = pt.get<string>("cdr_parcer.cdr_ftp_user", "");
    cdr_ftp_password = pt.get<string>("cdr_parcer.cdr_ftp_password", "");
    cdr_ftp_dir = pt.get<string>("cdr_parcer.cdr_ftp_dir", "");
    cdr_nasip = pt.get<string>("cdr_parcer.cdr_nasip", "127.0.0.1");
    cdr_parcer_timeout = pt.get<long>("cdr_parcer.timeout", 1800);
}
