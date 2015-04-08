#include "CdrObjList.h"
#include "../classes/AppBill.h"

CdrObjList::CdrObjList() {
    last_id = 0;
    new_last_id = 0;
}

size_t CdrObjList::item_size() {
    return sizeof (CdrObj);
}

bool CdrObjList::loaddata(BDb *db) {
    ObjList::load(db, 0);
    if (count > 0) {
        pCdrObj item = (pCdrObj) _get(count - 1);
        new_last_id = boost::lexical_cast<long long int>(item->id);
        return true;
    }
    return false;
}

bool CdrObjList::next() {
    if (count > 0) {
        last_id = new_last_id;
        new_last_id = 0;
        return true;
    } else {
        return false;
    }
}

pCdrObj CdrObjList::get(size_t i) {
    return (pCdrObj) _get(i);
}

void CdrObjList::refresh_current_id() {
    last_id = 0;
    new_last_id = 0;
}

string CdrObjList::sql(BDb * db) {
    if (last_id == 0) {
        BDbResult res = db->query("select max(id) from calls.calls");
        if (res.next()) {
            last_id = res.get_ll(0);
        } else {
            last_id = 100000000;
        }
    }
    return "	select " \
            "       id, " \
            "       connect_time, " \
            "       session_time, " \
            "       src_number, " \
            "       dst_number," \
            "       redirect_number," \
            "       src_route, " \
            "       dst_route," \
            "       src_noa, " \
            "       dst_noa" \
            "	from cdr.cdr " \
            "	where " \
            "       id > '" + lexical_cast<string>(last_id) + "' " \
            "	order by id " \
            "	limit " + lexical_cast<string>(nrows);
}

inline void CdrObjList::parse_item(BDbResult &row, void * obj) {
    pCdrObj item = (pCdrObj) obj;
    row.fill_cs(0, item->id, sizeof(item->id));
    item->id_num = row.get_ll(0);
    row.fill_cs(1, item->connect_time, sizeof(item->connect_time));
    item->make_dt();
    item->session_time = row.get_i(2);
    strcpy((char*) &item->src_number, row.get(3));
    strcpy((char*) &item->dst_number, row.get(4));
    strcpy((char*) &item->redirect_number, row.get(5));
    strcpy((char*) &item->src_route, row.get(6));
    strcpy((char*) &item->dst_route, row.get(7));
    item->src_noa = row.get_i(8);
    item->dst_noa = row.get_i(9);
}

