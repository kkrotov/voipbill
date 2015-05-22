#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Cdr.h"

class CurrentCdrList : public ObjList<Cdr> {
protected:

    string sql(BDb * db) {
        return "	select " \
            "       connect_time, " \
            "       src_number, " \
            "       dst_number," \
            "       redirect_number," \
            "       src_route, " \
            "       dst_route," \
            "       src_noa, " \
            "       dst_noa," \
            "       call_id " \
            "	from calls_cdr.start " \
            "   order by call_id ";
    }

    inline void parse_item(BDbResult &row, Cdr * item) {
        time_t now = time(nullptr);

        item->id = 0;
        item->connect_time = parseDateTime(row.get(0));
        item->session_time = (int)(now - item->connect_time);
        strcpy((char*) &item->src_number, row.get(1));
        strcpy((char*) &item->dst_number, row.get(2));
        strcpy((char*) &item->redirect_number, row.get(3));
        strcpy((char*) &item->src_route, row.get(4));
        strcpy((char*) &item->dst_route, row.get(5));
        item->src_noa = row.get_i(6);
        item->dst_noa = row.get_i(7);
        item->call_id = row.get_ll(8);
    }
};
