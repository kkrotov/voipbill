#include "Conf.h"
#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

bool Conf::parse_cmd_line(int argc, char* argv[]) {
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("config-file,c", po::value<string>(), "path to config file")
                ("pid-file,p", po::value<string>(), "path to pid file")
                ("test,t", "run tests")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);


        if (vm.count("test")) {
            test_mode = true;
        } else
            if (vm.count("help")) {
            cout << desc << "\n";
            return false;
        }

        if (vm.count("config-file"))
            config_file = vm["config-file"].as<string>();

        if (vm.count("pid-file"))
            pid_file = vm["pid-file"].as<string>();


    } catch (exception& e) {
        cout << "ERROR: Parse cmd line: " << e.what() << endl;
        return false;
    }
    return true;
}

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>

bool Conf::parse_config_file() {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(config_file, pt);

        test_mode = pt.get<bool>("main.test_mode", test_mode);
        web_port = pt.get<unsigned short>("main.web_port", web_port);

        unsigned short level;
        log_file_filename = pt.get<string>("log.file_filename", "");
        log_file_min_level = static_cast<LogLevel> (pt.get<unsigned short>("log.file_min_level", 0));
        log_file_max_level = static_cast<LogLevel> (pt.get<unsigned short>("log.file_max_level", 5));

        log_file2_filename = pt.get<string>("log.file2_filename", "");
        log_file2_min_level = static_cast<LogLevel> (pt.get<unsigned short>("log.file2_min_level", 0));
        log_file2_max_level = static_cast<LogLevel> (pt.get<unsigned short>("log.file2_max_level", 5));

        log_syslog_ident = pt.get<string>("log.syslog_ident", "");
        log_syslog_min_level = static_cast<LogLevel> (pt.get<unsigned short>("log.syslog_min_level", 0));
        log_syslog_max_level = static_cast<LogLevel> (pt.get<unsigned short>("log.syslog_max_level", 5));

        log_grouping_interval = pt.get<unsigned short>("log.grouping_interval", 60);

        db_main = pt.get<string>("db.main");
        db_rad = pt.get<string>("db.rad");
        db_calls = pt.get<string>("db.calls");

        region_id = pt.get<unsigned short>("geo.region");
        str_region_id = boost::lexical_cast<string>(region_id);

        udp_host = pt.get<string>("udp.host", "");
        udp_port = pt.get<unsigned short>("udp.port", 0);

        billing_dc_break = pt.get<unsigned short>("billing.dc_break", billing_dc_break);
        billing_free_seconds = pt.get<unsigned short>("billing.free_seconds", billing_free_seconds);

    } catch (exception& e) {
        cout << "ERROR: Parse config file: " << e.what() << endl;
        return false;
    }
    return true;
}

#include <boost/algorithm/string.hpp>

bool Conf::prepare() {
    return true;
}

bool Conf::init(int argc, char* argv[]) {
    string path(argv[0]);
    vector<string> mpath;
    boost::algorithm::split(mpath, path, boost::algorithm::is_any_of("/\\"));

    app_name = mpath[mpath.size() - 1];
    mpath.erase(mpath.end() - 1);
    app_directory = boost::algorithm::join(mpath, "/");

    config_file = app_directory + "/" + app_name + ".conf";
    pid_file = "/var/run/" + app_name + ".pid";

    test_mode = false;
    web_port = 8032;

    billing_dc_break = 126;
    billing_free_seconds = 5;

    if (!parse_cmd_line(argc, argv)) return false;

    if (!parse_config_file()) return false;

    if (!prepare()) return false;

    return true;
}
