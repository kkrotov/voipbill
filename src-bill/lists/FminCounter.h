#pragma once

#include "../../src/lists/ObjList.h"
#include <map>
using namespace std;

class FminCounter : public ObjList<int> {
protected:

    string sql(BDb * db) {
        return "";
    }

    inline void parse_item(BDbResult &row, int * item) {
        *item = 0;
    }

public:
    map<int, int> counter;

    void load(BDb * db, time_t dt) {

        t.start();

        this->dt = dt;
        counter.clear();
        string stime = string_time(dt);

        BDbResult res = db->query(
                "   select c.usage_id, c.free_min_groups_id, sum(len_mcn) from calls.calls c " \
            "   where " \
            "		c.time >= date_trunc('month', '" + stime + "'::timestamp) and " \
            "		c.time <  date_trunc('month', '" + stime + "'::timestamp)+interval '1 month' and " \
            "		c.free_min_groups_id <> 0 " \
            "		group by c.usage_id, c.free_min_groups_id "
        );
        loadtime = time(NULL);
        last_use = loadtime;
        while (res.next()) {
            int key = (res.get_i(0) << 10) + res.get_i(1);
            counter[key] = res.get_i(2);
        }

        loaded = true;

        t.stop();
    }


    int & get(int usage_id, int group_id) {
        int key = (usage_id << 10) + group_id;
        return counter[key];
    }

    int & get(const int key) {
        return counter[key];
    }

    void set(const int usage_id, const int group_id, const int new_value) {
        const int key = (usage_id << 10) + group_id;
        counter[key] = new_value;
    }

    void add(const int usage_id, const int group_id, const int add_value) {
        const int key = (usage_id << 10) + group_id;
        int & value = counter[key];
        value += add_value;
    }

    void clear() {
        counter.clear();
    }

    void append(FminCounter *newfmin) {
        map<int, int>::iterator i;
        for (i = newfmin->counter.begin(); i != newfmin->counter.end(); ++i) {
            int key = i->first;
            int add_value = i->second;
            int &value = counter[key];
            value += add_value;
        }
    }
};
