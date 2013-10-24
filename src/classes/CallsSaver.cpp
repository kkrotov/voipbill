#include "CallsSaver.h"
#include "../common.h"

string ins_str(CallsObjList *list) {
    string q;
    q.reserve(300 + list->count * 300);

    char num[50];

    q.append("INSERT INTO billing.calls(" \
                "time,id,direction_out,len,len_mcn,client_id,usage_id,usage_num, phone_num," \
                "amount,pricelist_mcn_id,amount_op,operator_id,free_min_groups_id,mob,redirect," \
                "dest,month,day,region,geo_id,pricelist_op_id,price,price_op,prefix_geo,prefix_mcn,prefix_op" \
             ")VALUES\n");

    for (int i = 0; i < list->count; i++) {
        pCallObj call = list->get(i);
        if (i > 0) {
            q.append(",");
        }

        q.append("(");

        q.append("'");
        q.append(call->time);
        q.append("',");
        q.append("'");
        q.append(call->id);
        q.append("',");
        q.append(call->out ? "true" : "false");
        q.append(",");
        sprintf(num, "%d", call->len);
        q.append(num);
        q.append(",");
        sprintf(num, "%d", call->len_mcn);
        q.append(num);
        q.append(",");
        sprintf(num, "%d", call->client_id);
        q.append(call->client_id != 0 ? num : "NULL");
        q.append(",");
        sprintf(num, "%d", call->usage_id);
        q.append(call->usage_id != 0 ? num : "NULL");
        q.append(",");
        q.append("'0");
        q.append(call->usage);
        q.append("',");
        q.append("'0");
        q.append(call->phone);
        q.append("',");

        sprintf(num, "%d", call->amount);
        q.append(num);
        q.append(",");
        sprintf(num, "%d", call->pricelist_id);
        q.append(call->pricelist_id != 0 ? num : "NULL");
        q.append(",");
        sprintf(num, "%d", call->amount_op);
        q.append(num);
        q.append(",");
        sprintf(num, "%d", call->operator_id);
        q.append(num);
        q.append(",");

        sprintf(num, "%d", call->freemin_group_id);
        q.append(num);
        q.append(",");
        q.append(call->mob ? "true" : "false");
        q.append(",");
        q.append(call->redirect ? "true" : "false");
        q.append(",");

        sprintf(num, "%d", call->dest);
        q.append(num);
        q.append(",");
        q.append("'");
        q.append(call->time);
        q.append("',");
        q.append("'");
        q.append(call->time);
        q.append("',");
        sprintf(num, "%d", call->region);
        q.append(num);
        q.append(",");
        sprintf(num, "%d", call->geo_id);
        q.append(call->geo_id != 0 ? num : "NULL");
        q.append(",");
        sprintf(num, "%d", call->pricelist_op_id);
        q.append(call->pricelist_op_id != 0 ? num : "NULL");
        q.append(",");
        sprintf(num, "%d", call->price);
        q.append(call->pricelist_id != 0 ? num : "NULL");
        q.append(",");
        sprintf(num, "%d", call->price_op);
        q.append(call->pricelist_op_id != 0 ? num : "NULL");
        q.append(",");
        q.append(call->prefix_geo[0] != 0 ? call->prefix_geo : "NULL");
        q.append(",");
        q.append(call->prefix_mcn[0] != 0 ? call->prefix_mcn : "NULL");
        q.append(",");
        q.append(call->prefix_op[0] != 0 ? call->prefix_op : "NULL");

        q.append(")\n");
    }
    return q;
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
        db->exec(ins_str(list));
    } catch (Exception &e) {
        throw e;
    }
}



