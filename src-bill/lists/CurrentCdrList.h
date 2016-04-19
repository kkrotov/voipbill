#pragma once

#include "../classes/ObjList.h"
#include "../models/Cdr.h"

class CurrentCdrList : public ObjList<Cdr> {
protected:

    string sql(BDb * db) {
        return "	select " \
            "       connect_time, " \
            "       src_number, " \
            "       dst_number, " \
            "       dst_replace, " \
            "       redirect_number, " \
            "       src_route, " \
            "       dst_route, " \
            "       src_noa, " \
            "       dst_noa, " \
            "       call_id, " \
            "       insert_time, " \
            "       now() at time zone 'utc' " \
            "	from calls_cdr.start " \
            "   order by call_id ";
    }

    inline void parse_item(BDbResult &row, Cdr * item) {

        item->id = 0;
        item->connect_time = parseDateTime(row.get(0));
        strcpy((char*) &item->src_number, row.get(1));
        strcpy((char*) &item->dst_number, row.get(2));
        strcpy((char*) &item->dst_replace, row.get(3));
        strcpy((char*) &item->redirect_number, row.get(4));
        strcpy((char*) &item->src_route, row.get(5));
        strcpy((char*) &item->dst_route, row.get(6));
        item->src_noa = row.get_i(7);
        item->dst_noa = row.get_i(8);
        item->call_id = row.get_ll(9);

        time_t insert_time = parseDateTime(row.get(10));
        time_t db_now = parseDateTime(row.get(11));

        // Старый код, дававший расхождения с фактической продолжительностью:
        // time_t now = time(nullptr);
        // item->session_time = (int)(now - item->connect_time);

        // Вычисления полностью в БД, запас в две секунды.
        item->session_time = (int)(db_now - insert_time);
        if (item->session_time > 1) {
            item->session_time -= 2;
        }
    }
};
