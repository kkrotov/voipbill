#pragma once

#include "BasePage.h"

class PageConfig : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/config";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        html << "<h2>Config</h2>\n";
        html << "build date: <b>" << build_date() << " " << build_time() << "</b><br/>\n";
        html << "config_file: " << app().conf.config_file << "<br/>\n";
        html << "pid_file: " << app().conf.pid_file << "<br/>\n";
        html << "<hr>\n";
        html << "log_file_filename: " << app().conf.log_file_filename << "<br/>\n";
        html << "log_file_min_level: " << app().conf.log_file_min_level << "<br/>\n";
        html << "log_file_max_level: " << app().conf.log_file_max_level << "<br/>\n";
        html << "log_syslog_ident: " << app().conf.log_syslog_ident << "<br/>\n";
        html << "log_syslog_min_level: " << app().conf.log_syslog_min_level << "<br/>\n";
        html << "log_syslog_max_level: " << app().conf.log_syslog_max_level << "<br/>\n";

        html << "<br/>\n";
        html << "main.web_port: " << app().conf.web_port << "<br/>\n";
        html << "main.test_mode: " << app().conf.test_mode << "<br/>\n";
        html << "<br/>\n";
        html << "log.grouping_interval: " << app().conf.log_grouping_interval << "<br/>\n";
        html << "<br/>\n";
        //html << "db.main: " << app().conf.db_main << "<br/>\n";
        //html << "db.calls: " << app().conf.db_calls << "<br/>\n";
        html << "<br/>\n";
        html << "udp.host: " << app().conf.openca_udp_host << "<br/>\n";
        html << "udp.port: " << app().conf.openca_udp_port << "<br/>\n";
        html << "<br/>\n";
        html << "billing.free_seconds: " << app().conf.billing_free_seconds << "<br/>\n";
        html << "billing.global_counters_select_interval: " << app().conf.global_counters_select_interval << "<br/>\n";
    }
};