#include "../classes/ConfBill.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

bool ConfBill::parse_config_variables(boost::property_tree::ptree &pt) {

    web_port = pt.get<unsigned short>("main.web_port", 8032);
    api_port = pt.get<unsigned short>("main.api_port", 8032);

    test_mode = pt.get<bool>("main.test_mode", false);
    
    string test_threads_string = pt.get<string>("main.test_threads", "");
    boost::split(test_threads, test_threads_string, boost::is_any_of(";, \t\n"),
            boost::algorithm::token_compress_on);

    db_main = pt.get<string>("db.main");
    db_rad = pt.get<string>("db.rad");
    db_calls = pt.get<string>("db.calls");

    instance_id = pt.get<unsigned short>("geo.instance_id");
    str_instance_id = boost::lexical_cast<string>(instance_id);

    openca_udp_host = pt.get<string>("udp.host", "");
    openca_udp_port = pt.get<unsigned short>("udp.port", 0);
    udp_openca_select_interval = pt.get<unsigned short>("udp.openca_select_interval", 10);
    udp_force_finish_call_interval = pt.get<unsigned short>("udp.force_finish_call_interval", 3);


    billing_dc_break = pt.get<unsigned short>("billing.dc_break", 126);
    billing_free_seconds = pt.get<unsigned short>("billing.free_seconds", 5);

    global_counters_select_interval = pt.get<unsigned short>("billing.global_counters_select_interval", 10);
    
}
