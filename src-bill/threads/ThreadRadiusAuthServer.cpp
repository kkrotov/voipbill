#include "ThreadRadiusAuthServer.h"
#include "../classes/AppBill.h"

void ThreadRadiusAuthServer::run() {
    server.run(app().conf.radius_secret, app().conf.radius_auth_port);
}

bool ThreadRadiusAuthServer::hasFullHtml() {
    return true;
}

void ThreadRadiusAuthServer::htmlfull(stringstream & html) {
    this->html(html);

    html << "Request count: <b>" << server.request_count << "</b><br/>\n";
    html << "Response count: <b>" << server.response_count << "</b><br/>\n";
    html << "<table>\n";
    html << "<tr><th>last request</th><th>last response</th></tr>";
    html << "<tr><td><pre>" + server.last_request + "</pre></td><td><pre>" + server.last_response + "</pre></td></tr>";
    html << "</table>\n";
}

ThreadRadiusAuthServer::ThreadRadiusAuthServer() {
    id = idName();
    name = "Radius Auth Server";
}
