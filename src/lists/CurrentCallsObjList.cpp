#include "CurrentCallsObjList.h"
#include "../classes/App.h"

size_t CurrentCallsObjList::item_size() {
    return sizeof (CallObj);
}

string CurrentCallsObjList::sql(BDb * db) {
    return "	select acctsessionid, connecttime, direction, callingstationid, calledstationid, in_oper, out_oper, prefix_des between 21 and 24 " \
            "	from radacct_voip_start " \
            "   where direction in ('in','out') " \
            "       and not (in_oper = " + app.conf.str_instance_id + " and out_oper = 80) " \
            "       and not (in_oper = 80 and out_oper = " + app.conf.str_instance_id + ") " \
            "   order by acctsessionid";
}

inline void CurrentCallsObjList::parse_item(BDbResult &row, void * obj) {
    pCallObj item = (pCallObj) obj;
    strncpy((char*) item->id, row.get(0), 20);
    item->id_num = row.get_ll(0);
    strncpy((char*) item->time, row.get(1), 25);
    item->make_dt();
    item->len = 0;
    item->len_mcn = 0;
    item->out = (strcmp(row.get(2), "out") == 0);
    if (item->out) {
        strcpy((char*) &item->usage, row.get(3));
        strcpy((char*) &item->phone, row.get(4));
        item->instance_id = row.get_i(5);
        item->operator_id = row.get_i(6);
    } else {
        strcpy((char*) &item->phone, row.get(3));
        strcpy((char*) &item->usage, row.get(4));
        item->instance_id = row.get_i(6);
        item->operator_id = row.get_i(5);
    }
    item->redirect = row.get_b(7);

    if (item->instance_id == 80) item->instance_id = app.conf.instance_id;
    if (item->instance_id == 0) item->instance_id = app.conf.instance_id;
    if (item->operator_id == 80) item->operator_id = app.conf.instance_id;

    item->kill_call_reason = 0;
    item->prefix_geo[0] = 0;
    item->prefix_mcn[0] = 0;
    item->prefix_op[0] = 0;
}

inline long long int CurrentCallsObjList::key(const void *obj) {
    return ((pCallObj) obj)->id_num;
}

pCallObj CurrentCallsObjList::get(size_t i) {
    return (pCallObj) _get(i);
}

inline pCallObj CurrentCallsObjList::find(const long long int value) {
    return (pCallObj) _find(value);
}

