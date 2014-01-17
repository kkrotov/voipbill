#include "CallsSaver.h"
#include "../common.h"

void insert_row(pCallObj call, string *q) {
    char num[50];

    q->append("(");

    q->append("'");
    q->append(call->time);
    q->append("',");
    q->append("'");
    q->append(call->id);
    q->append("',");
    q->append(call->out ? "true" : "false");
    q->append(",");
    sprintf(num, "%d", call->len);
    q->append(num);
    q->append(",");
    sprintf(num, "%d", call->len_mcn);
    q->append(num);
    q->append(",");
    sprintf(num, "%d", call->len_op);
    q->append(num);
    q->append(",");
    sprintf(num, "%d", call->client_id);
    q->append(call->client_id != 0 ? num : "NULL");
    q->append(",");
    sprintf(num, "%d", call->usage_id);
    q->append(call->usage_id != 0 ? num : "NULL");
    q->append(",");
    q->append("'0");
    q->append(call->usage);
    q->append("',");
    q->append("'0");
    q->append(call->phone);
    q->append("',");

    sprintf(num, "%d", call->amount_mcn);
    q->append(num);
    q->append(",");
    sprintf(num, "%d", call->pricelist_mcn_id);
    q->append(call->pricelist_mcn_id != 0 ? num : "NULL");
    q->append(",");
    sprintf(num, "%d", call->amount_op);
    q->append(num);
    q->append(",");
    sprintf(num, "%d", call->operator_id);
    q->append(num);
    q->append(",");

    sprintf(num, "%d", call->freemin_group_id);
    q->append(num);
    q->append(",");
    q->append(call->mob ? "true" : "false");
    q->append(",");
    q->append(call->redirect ? "true" : "false");
    q->append(",");

    sprintf(num, "%d", call->dest);
    q->append(num);
    q->append(",");
    q->append("'");
    q->append(call->time);
    q->append("',");
    q->append("'");
    q->append(call->time);
    q->append("',");
    sprintf(num, "%d", call->instance_id);
    q->append(num);
    q->append(",");
    sprintf(num, "%d", call->geo_id);
    q->append(call->geo_id != 0 ? num : "NULL");
    q->append(",");
    sprintf(num, "%d", call->pricelist_op_id);
    q->append(call->pricelist_op_id != 0 ? num : "NULL");
    q->append(",");
    sprintf(num, "%d", call->price_mcn);
    q->append(call->pricelist_mcn_id != 0 ? num : "NULL");
    q->append(",");
    sprintf(num, "%d", call->price_op);
    q->append(call->pricelist_op_id != 0 ? num : "NULL");
    q->append(",");
    q->append(call->prefix_geo[0] != 0 ? call->prefix_geo : "NULL");
    q->append(",");
    q->append(call->prefix_mcn[0] != 0 ? call->prefix_mcn : "NULL");
    q->append(",");
    q->append(call->prefix_op[0] != 0 ? call->prefix_op : "NULL");

    q->append(")\n");
}

void make_insert_queries(map<time_t, string> &queryPerMonth, CallsObjList *list) {

    string *q = 0;

    for (int i = 0; i < list->count; i++) {
        pCallObj call = list->get(i);

        if (queryPerMonth.find(call->dt.month) == queryPerMonth.end()) {
            char buff[20];
            struct tm * timeinfo = localtime(&call->dt.month);
            strftime(buff, 20, "%Y%m", timeinfo);

            q = &queryPerMonth[call->dt.month];
            q->reserve(300 + list->count * 300);
            q->append("INSERT INTO calls.calls_" + string(buff) + "(" \
                        "time,id,direction_out,len,len_mcn,len_op,client_id,usage_id,usage_num, phone_num," \
                        "amount,pricelist_mcn_id,amount_op,operator_id,free_min_groups_id,mob,redirect," \
                        "dest,month,day,region,geo_id,pricelist_op_id,price,price_op,prefix_geo,prefix_mcn,prefix_op" \
                     ")VALUES\n");
        } else {
            q = &queryPerMonth[call->dt.month];
            q->append(",");
        }

        insert_row(call, q);

    }
}

CallsSaver::CallsSaver() {
    this->db = 0;
}

CallsSaver::CallsSaver(BDb *db) {
    this->db = db;
}

void CallsSaver::setDb(BDb *db) {
    this->db = db;
}

void CallsSaver::save(CallsObjList *list) {
    try {
        map<time_t, string> queryPerMonth;

        make_insert_queries(queryPerMonth, list);

        typename map<time_t, string>::iterator i, e;
        for (i = queryPerMonth.begin(), e = queryPerMonth.end(); i != e;) {
            db->exec(i->second);
            i++;
        }
    } catch (Exception &e) {
        throw e;
    }
}



