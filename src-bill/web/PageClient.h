#pragma once

#include "BasePage.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"
#include "../classes/BlackListTrunk.h"
#include "../threads/ThreadCurrentCalls.h"
#include "../threads/ThreadRemoteLoader.h"
#include "../classes/Repository.h"

class PageClient : public BasePage {

    Repository repository;

public:
    bool canHandle(std::string &path) {
        return path == "/client";
    }

    void render(std::stringstream &html, map<string, string> &parameters);

    void render_client_locks(std::stringstream &html, Client *client);

    void render_client_current_calls(std::stringstream &html, Client *client);

    void render_client_balance_indicators(std::stringstream &html, Client *client);

    void render_client_packeges_info(std::stringstream &html, Client *client);

};
