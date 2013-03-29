#include "Conf.h"
#include "Log.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

bool Conf::parse_cmd_line(int argc, char* argv[])
{
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("config-file,c", po::value<string>(), "path to config file")
            ("err-log-file,l", po::value<string>(), "path to error log file")
            ("log-file,L", po::value<string>(), "path to log file")
            ("pid-file,p", po::value<string>(), "path to pid file")
            ("test,t", "run tests")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);


        if (vm.count("test")) {
            test_mode = true;
        }else
        if (vm.count("help")) {
            cout << desc << "\n";
            return false;
        }

        if (vm.count("config-file"))
            config_file = vm["config-file"].as<string>();

        if (vm.count("err-log-file"))
            err_log_file = vm["err-log-file"].as<string>();

        if (vm.count("log-file"))
            log_file = vm["log-file"].as<string>();

        if (vm.count("pid-file"))
            pid_file = vm["pid-file"].as<string>();


    }
    catch(exception& e) {
        Log::er(string("Parse cmd line: ").append(e.what()));
        return false;
    }
    return true;
}

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>

bool Conf::parse_config_file()
{
    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(config_file, pt);

        test_mode = pt.get<bool>("main.test_mode", test_mode);
        web_port = pt.get<unsigned short>("main.web_port", web_port);

        db_main = pt.get<string>("db.main");
        db_rad = pt.get<string>("db.rad");
        db_calls = pt.get<string>("db.calls");
        db_ast = pt.get<string>("db.ast", "");

        region_id = pt.get<unsigned short>("geo.region");
        str_region_id = boost::lexical_cast<string>(region_id);

        udp_host = pt.get<string>("udp.host","");
        udp_port = pt.get<unsigned short>("udp.port", 0);

        billing_dc_break = pt.get<unsigned short>("billing.dc_break", billing_dc_break);
        billing_free_seconds = pt.get<unsigned short>("billing.free_seconds", billing_free_seconds);

    }catch(exception& e) {
        Log::er(string("Parse config file: ").append(e.what()));
        return false;
    }
    return true;
}

#include <boost/algorithm/string.hpp>

bool Conf::prepare()
{
    return true;
}

#include <boost/filesystem.hpp>

bool Conf::init(int argc, char* argv[]) {
    boost::filesystem::path path(argv[0]);
    app_directory = path.parent_path().string();
    app_name = path.filename().string();

    err_log_file =  app_directory + "/" + app_name + ".err.log";
    log_file =      app_directory + "/" + app_name + ".log";
    config_file =   app_directory + "/" + app_name + ".conf";
    pid_file =      "/var/run/" + app_name + ".pid";

    test_mode = false;
    web_port = 8032;

    billing_dc_break = 126;
    billing_free_seconds = 5;

    if (!parse_cmd_line(argc, argv)) return false;

    if (!parse_config_file()) return false;

    if (!prepare()) return false;

    return true;
}
