#include "CalcFull.h"
#include "App.h"

void CalcFull::calc_item(pCallObj call) {

    call->cleanupCalculatedFields();

    pDestObj dest = data.dest->find(call->phone);
    pOperator oper = data.oper->find(call->operator_id);

    if (dest != 0) {
        call->mob = dest->mob;
        call->dest = dest->dest;
        call->geo_id = dest->geo_id;
        strcpy(call->prefix_geo, dest->prefix);
    }

    call->pricelist_op_id = oper != 0 ? oper->pricelist_id : 0;

    if (call->pricelist_op_id != 0) {
        if (call->out) {
            pPriceObj price_op = data.price->find(call->pricelist_op_id, call->phone);
            if (price_op != 0) {
                call->price_op = price_op->price;
                strcpy(call->prefix_op, price_op->prefix);
            }

        } else {
            call->price_op = -oper->term_in_cost;
        }
        call->amount_op = floor((float) (call->len * call->price_op) / (60 * 100) + 0.5);
    }

    // call from other regions is not calc by client price
    if (call->region >= 80 && call->region < 100 && call->region != app.conf.region_id) return;

    pUsageObj usage_item;
    if (call->region < 100) {
        usage_item = data.usage->find(call->usage);
    } else {
        char fake_a_number[10];
        sprintf(fake_a_number, "%d", call->region);
        usage_item = data.usage->find(fake_a_number);
    }

    if (usage_item == 0) return;

    call->usage_id = usage_item->id;
    call->client_id = usage_item->client_id;

    if (call->out == false) return;

    if (call->dest <= 0)
        call->pricelist_id = (call->mob ? usage_item->pl_local_mob_id : usage_item->pl_local_id);
    else if (call->dest == 1)
        call->pricelist_id = usage_item->pl_russia_id;
    else if (call->dest == 2)
        call->pricelist_id = usage_item->pl_intern_id;
    else if (call->dest == 3)
        call->pricelist_id = usage_item->pl_sng_id;

    if (call->len > 60) call->len_mcn = call->len;
    else if (call->len > app.conf.billing_free_seconds) call->len_mcn = 60;
    else call->len_mcn = 0;

    pPriceObj price_mcn = data.price->find(call->pricelist_id, call->phone);
    if (price_mcn != 0) {
        call->price = price_mcn->price;
        call->amount = floor((float) (call->len_mcn * call->price) / (60 * 100) + 0.5);
        strcpy(call->prefix_mcn, price_mcn->prefix);
    }


    if (call->dest <= 0 &&
            call->mob == false &&
            usage_item->freemin > 0 &&
            (call->redirect == false || usage_item->paid_redirect == false)
            ) {
        int fm_len = data.counter_fmin->get(call->usage_id, 1);
        int fm_len2 = fmin_counter2->get(call->usage_id, 1);
        if (fm_len + fm_len2 + call->len_mcn <= usage_item->freemin) {
            fmin_counter2->set(call->usage_id, 1, fm_len2 + call->len_mcn);
            call->freemin_group_id = 1;
            call->amount = 0;
        }
    }

    if (call->amount > 0) {
        ClientCounterObj &cc = client_counter2->get(call->client_id);

        if (call->dt.month == cc.amount_month) {
            cc.sum_month += call->amount;
        } else if (call->dt.month > cc.amount_month) {
            cc.amount_month = call->dt.month;
            cc.sum_month = call->amount;
        }

        if (call->dt.day == cc.amount_day) {
            cc.sum_day += call->amount;
        } else if (call->dt.day > cc.amount_day) {
            cc.amount_day = call->dt.day;
            cc.sum_day = call->amount;
        }

        pClientObj client = data.client->find(call->client_id);
        if (client != 0 && call->dt.time >= client->amount_date) {
            cc.sum += call->amount;
        }

        if (cc.client_id == 0) cc.client_id = call->client_id;
    }

}

void CalcFull::html(std::stringstream &html) {

    //QReadLocker locker(&data_global->rwlock);
    data_global->rwlock.lock();
    int usage = data_global->usage.datamap.size();
    int price = data_global->price.datamap.size();
    int c_fmins = data_global->counter_fmin.datamap.size();
    data_global->rwlock.unlock();
    //locker.unlock();

    html << "Time: <b>" << string_time(last_call_time) << "</b><br/>\n";
    html << string_fmt("usage:%d, price:%d, c_fmins:%d <br/>\n",
            usage, price, c_fmins);
}



