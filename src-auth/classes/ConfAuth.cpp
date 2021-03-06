#include "../classes/ConfAuth.h"

#include <boost/lexical_cast.hpp>

bool ConfAuth::parse_config_variables(boost::property_tree::ptree &pt) {

    web_port = pt.get<unsigned short>("main.web_port", 8031);
    api_port = pt.get<unsigned short>("main.api_port", 8031);
    auth_api_host = pt.get<string>("main.auth_api_host", "127.0.0.1");
    auth_api_port = pt.get<unsigned short>("main.auth_api_port", 8032);

    db_main = pt.get<string>("db.main");
    db_local = pt.get<string>("db.local");

    server_id = pt.get<unsigned short>("main.server_id");
    str_server_id = boost::lexical_cast<string>(server_id);

}
