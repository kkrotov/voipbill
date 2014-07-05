#include "ThreadLoader.h"
#include "../classes/AppAuth.h"

bool ThreadLoader::ready() {
    return app().init_sync_done;
}

bool ThreadLoader::prepare() {

    if (!app().init_load_counters_done) {
        Log::info("Loading counters...");
        app().init_load_counters_done = true;
    }

    if (!app().init_load_data_done) {
        Log::info("Loading data...");
        app().init_load_data_done = true;
    }

    return true;
}

void ThreadLoader::run() {


}

void ThreadLoader::htmlfull(stringstream &html) {
    this->html(html);


}

ThreadLoader::ThreadLoader() {
    id = "loader";
    name = "Loader";

    errors = 0;
}
