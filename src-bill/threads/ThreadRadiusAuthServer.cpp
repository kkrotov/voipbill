#include "ThreadRadiusAuthServer.h"
#include "../classes/AppBill.h"

void ThreadRadiusAuthServer::run() {

    Repository repository;

    if(!repository.prepare() && !app().conf.isApiHostMode()) return;

    server.run(app().conf.radius_secret, app().conf.radius_auth_port);
}

bool ThreadRadiusAuthServer::hasFullHtml() {
    return true;
}

void ThreadRadiusAuthServer::htmlfull(stringstream & html) {
    this->html(html);

    html << "Request count: <b>" << server.request_count << "</b><br/>\n";
    html << "Response count: <b>" << server.response_count << "</b><br/>\n";
    html << "\n";
    html << "<b>last request</b><br/>\n";
    html << "<pre>" + server.last_request + "</pre>\n";
    html << "\n";
    html << "<b>last response</b><br/>\n";
    html << "<pre>" + server.last_response + "</pre>\n";
    if (server.last_error.size() > 0) {
        html << "\n";
        html << "<b>last error</b><br/>\n";
        html << "<pre>" + server.last_error + "</pre>\n";
    }
}

ThreadRadiusAuthServer::ThreadRadiusAuthServer() {
    id = idName();
    name = "Radius Auth Server";
    threadSleepSeconds = 1;
}
