#include "ThreadRadiusAuthServer.h"
#include "../classes/AppBill.h"

void ThreadRadiusAuthServer::run() {
    server.run(app().conf.radius_secret, app().conf.radius_auth_port);
}

ThreadRadiusAuthServer::ThreadRadiusAuthServer() {
    id = idName();
    name = "Radius Auth Server";
}
