#include "BasePull.h"

#include "../common.h"
#include "../classes/Exception.h"
#include "../classes/Log.h"

BasePull::BasePull() {
    pull_count_full = 0;
    pull_count_partial = 0;
    pull_count_errors = 0;
}

void BasePull::setManager(ManagerPull * manager) {
    this->manager = manager;
}

void BasePull::clear() {
    need_pull = false;
    full_pull = key == "";
    ids_to_pull.clear();
}

void BasePull::addId(string &id) {
    need_pull = true;
    if (!full_pull) {
        if (id == "0") {
            full_pull = true;
        } else {
            ids_to_pull.insert(id);
        }
    }
}

void BasePull::pull() {

    if (!need_pull) {
        return;
    }

    try {
        timer.start();
        if (full_pull) {
            pullFull();
        } else {
            pullPartial();
        }
        timer.stop();
    } catch (Exception &e) {
        timer.stop();
        e.addTrace("BasePull::pull: " + event);
        throw  e;
    }
}

void BasePull::pullFull() {
    BDbTransaction trans(&manager->db_calls);

    string del = "delete from " + dst_table;
    manager->db_calls.exec(del);

    BDb::copy(dst_table, src_table, getQueryFields(), "", &manager->db_main, &manager->db_calls, manager->bandwidth_limit_mbits);

    trans.commit();

    pull_count_full += 1;
}

void BasePull::pullPartial() {

    string query_fields = getQueryFields();

    string sel =
            "   select " + query_fields +
            "   from " + src_table +
            "   where \"" + key + "\" in (" + getFilterIds() + ")";

    string del = "delete from " + dst_table + " where \"" + key + "\" in (" + getFilterIds() + ")";

    {
        BDbTransaction trans(&manager->db_calls);
        manager->db_calls.exec(del);
        BDb::copy(dst_table, src_table, getQueryFields(), sel, &manager->db_main, &manager->db_calls, manager->bandwidth_limit_mbits);
        trans.commit();
    }

    auto it = ids_to_pull.find(key);
    if (it != ids_to_pull.end()) {
        ids_to_pull.erase(it);
    }

    pull_count_partial += 1;

}

string BasePull::getQueryFields() {
    return "\"" + join(fields, "\",\"") + "\"";
}

string BasePull::getFilterIds() {
    return "'" + join(ids_to_pull, "','") + "'";
}

