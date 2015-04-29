#include "BasePull.h"

#include "../../src/common.h"
#include "../../src/classes/Exception.h"
#include "../../src/classes/Log.h"

BasePull::BasePull() {
    pull_count_full = 0;
    pull_count_updated = 0;
    pull_count_deleted = 0;
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

    BDb::copy(dst_table, src_table, getQueryFields(), "", &manager->db_main, &manager->db_calls);

    trans.commit();

    pull_count_full += 1;
}

void BasePull::pullPartial() {


    size_t originalCount = ids_to_pull.size();

    string query_fields = getQueryFields();

    string sel =
            "   select " + query_fields +
            "   from " + src_table +
            "   where \"" + key + "\" in (" + getFilterIds() + ")";

    string del = "delete from " + dst_table + " where \"" + key + "\" in (" + getFilterIds() + ")";


    BDbResult res = manager->db_main.query(sel);
    if (res.size() > 0) {

        string ins = "insert into " + dst_table + "(" + query_fields + ") values ";

        while (res.next()) {

            if (res.index > 0) ins.append(",");
            ins.append("(");

            for (size_t col = 0; col < fields.size(); col++) {
                if (col > 0) ins.append(",");
                if (res.is_null(col)) {
                    ins.append("NULL");
                } else {
                    ins.append("'").append(res.get(col)).append("'");
                }
            }

            ins.append(")");

            auto it = ids_to_pull.find(res.get_s(0));
            if (it != ids_to_pull.end()) {
                ids_to_pull.erase(it);
            }
        }

        {
            BDbTransaction trans(&manager->db_calls);
            manager->db_calls.exec(del);
            manager->db_calls.exec(ins);
            trans.commit();
        }

        pull_count_updated += res.size();
        pull_count_deleted += originalCount - res.size();
    } else {
        manager->db_calls.exec(del);

        pull_count_deleted += originalCount;
    }

}

string BasePull::getQueryFields() {
    return "\"" + join(fields, "\",\"") + "\"";
}

string BasePull::getFilterIds() {
    return "'" + join(ids_to_pull, "','") + "'";
}

