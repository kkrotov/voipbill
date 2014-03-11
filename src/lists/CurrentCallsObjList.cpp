#include "CurrentCallsObjList.h"
#include "../classes/App.h"

size_t CurrentCallsObjList::item_size() {
    return sizeof (CallObj);
}

string CurrentCallsObjList::sql(BDb * db) {
    return "	select " \
            "       acctsessionid, " \
            "       connecttime, " \
            "       direction, " \
            "       substring(callingstationid from '^\\d{1,18}')::bigint, " \
            "       substring(calledstationid from '^\\d{1,18}')::bigint, " \
            "       in_oper, " \
            "       out_oper, " \
            "       case prefix_des between 21 and 24 when true"
    "           then substring(redirectnum from '^\\d{1,18}')::bigint "
    "           else null"
    "       end " \
            "	from radacct_voip_start " \
            "   where " \
            "       direction in ('in','out') " \
            "       and not (in_oper = " + app().conf.str_instance_id + " and out_oper = 80) " \
            "       and not (in_oper = 80 and out_oper = " + app().conf.str_instance_id + ") " \
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
        strcpy((char*) &item->usage_num, row.get(3));
        strcpy((char*) &item->phone_num, row.get(4));
        item->instance_id = row.get_i(5);
        item->operator_id = row.get_i(6);
    } else {
        strcpy((char*) &item->phone_num, row.get(3));
        strcpy((char*) &item->usage_num, row.get(4));
        item->operator_id = row.get_i(5);
        item->instance_id = row.get_i(6);
    }

    strcpy((char*) &item->redirect_num, row.get(7));

    if (item->instance_id == 80) item->instance_id = app().conf.instance_id;
    if (item->instance_id == 0) item->instance_id = app().conf.instance_id;
    if (item->operator_id == 80) item->operator_id = app().conf.instance_id;

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

