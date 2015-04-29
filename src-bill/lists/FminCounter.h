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
    map<int, map<int, map<time_t, int>>> counter;

    void load(BDb * db) {

        counter.clear();

        loadtime = time(NULL);

        time_t tMonth = time(nullptr);
        string sMonth = string_time(tMonth);
        {
            BDbResult res = db->query(
                    "   select date_trunc('month', connect_time) from calls_raw.calls_raw c " \
            "	order by c.id desc " \
            "	limit 1 "
            );
            while (res.next()) {
                tMonth = parseDateTime(res.get(0));
                sMonth = res.get_s(0);
            }
        }

        BDbResult res = db->query(
                "   select c.number_service_id, c.service_package_id, sum(package_time) from calls_raw.calls_raw c " \
            "   where " \
            "		c.connect_time >= '" + sMonth + "'::timestamp and " \
            "		c.connect_time <  '" + sMonth + "'::timestamp + interval '1 month' and " \
            "		c.service_package_id > 0 " \
            "		group by c.number_service_id, c.service_package_id "
        );
        while (res.next()) {
            set(res.get_i(0), res.get_i(1), tMonth, res.get_i(2));
        }

    }

    int & get(int usage_id, int group_id, time_t tmonth) {
        map<int, map<time_t, int>> &counterUsage = counter[usage_id];
        map<time_t, int> &counterGroup = counterUsage[group_id];
        int &counterMonth = counterGroup[tmonth];
        return counterMonth;
    }

    void set(int usage_id, int group_id, time_t tmonth, int new_value) {
        map<int, map<time_t, int>> &counterUsage = counter[usage_id];
        map<time_t, int> &counterGroup = counterUsage[group_id];
        int &counterMonth = counterGroup[tmonth];
        counterMonth = new_value;
    }

    void add(int usage_id, int group_id, time_t tmonth, int add_value) {
        map<int, map<time_t, int>> &counterUsage = counter[usage_id];
        map<time_t, int> &counterGroup = counterUsage[group_id];
        int &counterMonth = counterGroup[tmonth];
        counterMonth += add_value;
    }

    size_t size() {
        return counter.size();
    }
};


