#include "ThreadWeb.h"

#include "../../src/http/server.hpp"
#include "../http/file_handler.hpp"

#include "../../src/common.h"
#include "../../src/threads/Thread.h"
#include "../classes/AppAuth.h"

void ThreadWeb::operator()() {
    if (app().conf.web_port == 0) return;

    http::server4::server(io_service, "0.0.0.0",
            string_fmt("%d", app().conf.web_port),
            http::server4::file_handler())();
    io_service.run();

}

void ThreadWeb::stop() {
    io_service.stop();
}

void ThreadWeb::handlerHeader(stringstream &html) {
    html << "<style>\n";
    html << "* {font-family:monospace !important;}\n";
    html << "td { text-align: center }\n";
    html << "</style>\n";

    html << " <a href='/'>home</a> ";
    html << " <a href='/config'>config</a> ";
    html << string_time(time(NULL)) << " ";

    AppStatus status = app().getStatus();
    AppStatus real_status = app().getRealStatus();
    html << "<b>" << AppStatusNames[real_status] << "</b>";
    if (status != real_status) {
        html << " (" << AppStatusNames[status] << ")";
    }

    html << " <hr/> ";
}

void ThreadWeb::handlerHome(stringstream &html) {
    handlerHeader(html);
    
    app().threads.forAllThreads([&](Thread* thread) {
        thread->html(html);
        html << "<hr>\n";
        return true;
    });
}

void ThreadWeb::handlerConfig(stringstream &html) {
    handlerHeader(html);

    html << "<h2>Config</h2>\n";
    html << "build date: <b>" << build_date() << " " << build_time() << "</b><br/>\n";
    html << "config_file: " << app().conf.config_file << "<br/>\n";
    html << "pid_file: " << app().conf.pid_file << "<br/>\n";
    html << "<hr>\n";
    html << "log_file_filename: " << app().conf.log_file_filename << "<br/>\n";
    html << "log_file_min_level: " << app().conf.log_file_min_level << "<br/>\n";
    html << "log_file_max_level: " << app().conf.log_file_max_level << "<br/>\n";
    html << "log_file2_filename: " << app().conf.log_file2_filename << "<br/>\n";
    html << "log_file2_min_level: " << app().conf.log_file2_min_level << "<br/>\n";
    html << "log_file2_max_level: " << app().conf.log_file2_max_level << "<br/>\n";
    html << "log_syslog_ident: " << app().conf.log_syslog_ident << "<br/>\n";
    html << "log_syslog_min_level: " << app().conf.log_syslog_min_level << "<br/>\n";
    html << "log_syslog_max_level: " << app().conf.log_syslog_max_level << "<br/>\n";

    html << "<br/>\n";
    html << "main.web_port: " << app().conf.web_port << "<br/>\n";
    html << "main.api_port: " << app().conf.api_port << "<br/>\n";
    html << "main.auth_api_host: " << app().conf.auth_api_host << "<br/>\n";
    html << "main.auth_api_port: " << app().conf.auth_api_port << "<br/>\n";
    html << "<br/>\n";
    html << "log.grouping_interval: " << app().conf.log_grouping_interval << "<br/>\n";
    html << "<br/>\n";
    html << "db.main: " << app().conf.db_main << "<br/>\n";
    html << "db.local: " << app().conf.db_local << "<br/>\n";
}

bool ThreadWeb::handlerTask(stringstream &html, map<string, string> &parameters) {
    string task_id;
    if (parameters.find("id") != parameters.end())
        task_id = parameters["id"];
    Thread * thread = 0;

    app().threads.forAllThreads([&](Thread* th) {
        if (th->id == task_id) {
            thread = th;
            return false;
        }
        return true;
    });

    if (thread == 0) {
        return false;
    }

    handlerHeader(html);

    thread->htmlfull(html);

    return true;
}