#include "RuntimeCallsObjList.h"
#include <boost/lexical_cast.hpp>
#include "../classes/App.h"

RuntimeCallsObjList::RuntimeCallsObjList() {
    last_id = 0;
    new_last_id = 0;
}

void RuntimeCallsObjList::refresh_current_id() {
    last_id = 0;
    new_last_id = 0;
}

string RuntimeCallsObjList::sql(BDb * db) {
    if (last_id == 0) {
        BDb db_calls(app.conf.db_calls);
        BDbResult res = db_calls.query("select max(id) from billing.calls");
        if (res.next()) {
            last_id = res.get_ll(0);
        } else {
            last_id = 100000000;
        }
    }
    return "	select radacctid, connecttime, acctsessiontime, direction, " \
            "       substring(callingstationid from '^\\d{1,18}')::bigint, " \
            "       substring(calledstationid from '^\\d{1,18}')::bigint, " \
            "       in_oper, out_oper, prefix_des between 21 and 24 " \
            "	from radacct_voip_stop " \
            "	where radacctid>'" + lexical_cast<string>(last_id) + "' " \
            "       and direction in ('in','out') " \
            "		and disconnectcause != " + lexical_cast<string>(app.conf.billing_dc_break) +
            "	order by radacctid " \
            "	limit " + lexical_cast<string>(nrows);
}

void RuntimeCallsObjList::parse_item(BDbResult &row, void * obj) {
    pCallObj item = (pCallObj) obj;
    strncpy((char*) item->id, row.get(0), 20);
    item->id_num = row.get_ll(0);
    strncpy((char*) item->time, row.get(1), 25);
    item->make_dt();
    item->len = row.get_i(2);
    item->len_mcn = item->len;
    item->out = (strcmp(row.get(3), "out") == 0);
    if (item->out) {
        strcpy((char*) &item->usage, row.get(4));
        strcpy((char*) &item->phone, row.get(5));
        item->region = row.get_i(6);
        item->operator_id = row.get_i(7);
    } else {
        strcpy((char*) &item->phone, row.get(4));
        strcpy((char*) &item->usage, row.get(5));
        item->region = row.get_i(7);
        item->operator_id = row.get_i(6);
    }
    item->redirect = row.get_b(8);
    if (item->region == 0) item->region = app.conf.region_id;
    item->kill_call_reason = 0;
    item->prefix_geo[0] = 0;
    item->prefix_mcn[0] = 0;
    item->prefix_op[0] = 0;
}

