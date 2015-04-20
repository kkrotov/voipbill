#include "ThreadRemoteLoader.h"
#include "../classes/AppBill.h"

void ThreadRemoteLoader::run() {

    data->globalCounters.load(&db_main);

}

void ThreadRemoteLoader::htmlfull(stringstream &html) {
    this->html(html);

    html << "<table border=1 width=100%>";
    html << "<tr><th></th><th>Updated at</th><th>Size</th><th>Rows</th><th>Last time</th><th>Total time</th></tr>";
    {
        auto dl = &data->globalCounters;
        html << "<tr><th>server</th>";
        html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
        dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        html << "</tr>\n";
    }
    html << "</table>\n";
}

ThreadRemoteLoader::ThreadRemoteLoader() {
    id = idName();
    name = "Remote Loader";
    db_main.setCS(app().conf.db_main);

    data = DataContainer::instance();

    threadSleepSeconds = app().conf.global_counters_select_interval;
}


