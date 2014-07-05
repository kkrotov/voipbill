#include "../classes/ConfAuth.h"

#include <boost/lexical_cast.hpp>

bool ConfAuth::parse_config_variables(boost::property_tree::ptree &pt) {

    web_port = pt.get<unsigned short>("main.web_port", 8030);

    db_main = pt.get<string>("db.main");
    db_local = pt.get<string>("db.local");

    server_id = pt.get<unsigned short>("geo.instance_id");
    str_server_id = boost::lexical_cast<string>(str_server_id);

}
