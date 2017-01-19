#include "PageConfig.h"

void PageConfig::render(std::stringstream &html, map<string, string> &parameters) {
    renderHeader("config", html);

    html << "<h2>Config</h2>\n";
    html << "build date: " << build_date() << " " << build_time() << " (UTC)<br/>\n";
    html << "built on commit: <b><a href=\"https://github.com/welltime/billing_voip/commit/";
    html << build_commit() << "\">" << build_commit() << "</a></b><br/><br/>\n";

    html << "start time: " << string_time(app().getStartTime()) << " (UTC)<br/>\n";
    html << "&nbsp;&nbsp;run time: " << seconds_to_uptime((app().getRuntime())) << "<br/>\n";
    html << "<hr>\n";
    html << "mode: " << app().conf.getBillerMode() << "<br/>\n";

    if (app().conf.hub_id > 0) {

        Repository repository;

        if (repository.prepare()) {
            int hub_id = app().conf.hub_id;
            Hub *iHub = repository.getHub(hub_id);

            if (iHub != nullptr) {
                html << "geo.hub_id=" << hub_id << " table public.server (" << iHub->name << ")\n";
                vector<Server> servers;
                repository.getServersByHubId(servers, hub_id);
                html << " (";
                for (auto i:servers) {
                    html << i.id << ",";
                }
                html << ") <br>\n";
            }
        }
    }
    html << "geo.instance_id: " << app().conf.instance_id << "<br/>\n";
    html << "geo.sql_regions_list: " << app().conf.get_sql_regions_list() << "<br/>\n";

    html << "<hr>\n";

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
    if (app().conf.active_threads.size() > 0) {

        html << "main.run_threads: ";
        for (auto active_thread: app().conf.active_threads)
            html << active_thread << " ";

        html << "<br/>\n";
    }
    if (app().conf.skip_threads.size() > 0) {

        html << "main.skip_threads: ";
        for (auto skipped_thread: app().conf.skip_threads)
            html << skipped_thread << " ";

        html << "<br/>\n";
    }
    html << "main.calls_raw_sync_delay: " << app().conf.calls_raw_sync_delay << "<br/>\n";
    html << "<br/>\n";
    html << "log.grouping_interval: " << app().conf.log_grouping_interval << "<br/>\n";
    html << "<br/>\n";
    html << "<br/>\n";
    html << "udp.host: " << app().conf.openca_udp_host << "<br/>\n";
    html << "udp.port: " << app().conf.openca_udp_port << "<br/>\n";
    html << "<br/>\n";
    html << "billing.free_seconds: " << app().conf.billing_free_seconds << "<br/>\n";
    html << "billing.global_counters_select_interval: " << app().conf.global_counters_select_interval << "<br/>\n";

    html << "<br/>\n";
    html << "cdr_parcer.cdr_ftp_host: " << app().conf.cdr_ftp_host << "<br/>\n";
    html << "cdr_parcer.cdr_ftp_dir: " << app().conf.cdr_ftp_dir << "<br/>\n";
    html << "cdr_parcer.cdr_nasip: " << app().conf.cdr_nasip << "<br/>\n";
    html << "cdr_parcer.interval: " << app().conf.cdr_parcer_interval << "<br/>\n";

}
