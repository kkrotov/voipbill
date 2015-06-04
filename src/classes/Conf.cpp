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

#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>

bool Conf::parse_config_file() {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(config_file, pt);

        unsigned short level;
        log_file_filename = pt.get<string>("log.file_filename", "");
        log_file_min_level = static_cast<LogLevel> (pt.get<unsigned short>("log.file_min_level", 0));
        log_file_max_level = static_cast<LogLevel> (pt.get<unsigned short>("log.file_max_level", 5));

        log_syslog_ident = pt.get<string>("log.syslog_ident", "");
        log_syslog_min_level = static_cast<LogLevel> (pt.get<unsigned short>("log.syslog_min_level", 0));
        log_syslog_max_level = static_cast<LogLevel> (pt.get<unsigned short>("log.syslog_max_level", 5));

        log_grouping_interval = pt.get<unsigned short>("log.grouping_interval", 60);

        parse_config_variables(pt);

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

    if (!parse_cmd_line(argc, argv)) return false;

    if (!parse_config_file()) return false;

    if (!prepare()) return false;

    return true;
}
