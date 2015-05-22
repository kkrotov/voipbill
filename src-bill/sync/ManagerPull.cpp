#include "ManagerPull.h"
#include "../classes/AppBill.h"

ManagerPull::ManagerPull() {
    part_size = 100;
    db_main.setCS(app().conf.db_main);
    db_calls.setCS(app().conf.db_calls);
    errors_count = 0;
}

void ManagerPull::add(BasePull * pull) {
    pull->setManager(this);
    pull->init();
    pulls.insert(make_pair(pull->event, pull));
}

void ManagerPull::pull() {
    string select_events_query =
            "select event, param, version from event.queue where server_id = " + app().conf.str_instance_id + " order by server_id, version limit " + lexical_cast<string>(part_size);

    while (true) {
        BDbResult res = db_main.query(select_events_query);
        if (res.size() == 0) {
            break;
        }

        clearPulls();

        while (res.next()) {
            string event = res.get_s(0);
            string id = res.get_s(1);


            map<string, BasePull *>::iterator it = pulls.find(event);
            if (it != pulls.end()) {
                it->second->addId(id);
            } else {
                db_main.exec("delete from event.queue where server_id = " + app().conf.str_instance_id + " and event = '" + event + "'");
            }
        }

        if (res.last()) {
            string version = res.get_s(2);

            for (auto it : pulls) {
                try {
                    it.second->pull();
                    db_main.exec("delete from event.queue where server_id = " + app().conf.str_instance_id + " and event = '" + it.second->event  + "' and version <= " + version);
                } catch (Exception &e) {
                    e.addTrace("ManagerPull:pull");
                    Log::exception(e);
                    errors_count += 1;
                }
            }
        }

        boost::this_thread::interruption_point();
    }
}

void ManagerPull::clearPulls() {
    for (auto it = pulls.cbegin(); it != pulls.cend(); ++it) {
        it->second->clear();
    }
}

void ManagerPull::runPulls() {
    for (auto it = pulls.cbegin(); it != pulls.cend(); ++it) {
        it->second->pull();
    }
}