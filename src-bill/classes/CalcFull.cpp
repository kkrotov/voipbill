#include "CalcFull.h"
#include "AppBill.h"

void CalcFull::calc_item(pCallObj call) {

    cleanupCalculatedFields(call);

    pDestObj dest = data.dest->find(call->phone_num);

    if (dest != 0) {
        call->mob = dest->mob;
        call->dest = dest->dest;
        call->geo_id = dest->geo_id;
        call->geo_operator_id = dest->geo_operator_id;
        strcpy(call->prefix_geo, dest->prefix);
    }

    calculateOperator(call);

    if (call->isCallFromAnotherInstance()) return;

    calculateMcn(call);
}

void CalcFull::cleanupCalculatedFields(pCallObj call) {
    call->usage_id = 0;
    call->client_id = 0;

    call->price_mcn = 0;
    call->price_op = 0;

    call->amount_mcn = 0;
    call->amount_op = 0;

    call->freemin_group_id = 0;

    call->pricelist_mcn_id = 0;
    call->pricelist_op_id = 0;

    call->prefix_geo[0] = 0;
    call->prefix_mcn[0] = 0;
    call->prefix_op[0] = 0;

    call->mob = false;
    call->dest = 2;
    call->geo_id = 0;
    call->geo_operator_id = 0;

    call->len_mcn = call->len_op = call->len;
}

void CalcFull::calculateOperator(pCallObj call) {
    pOperator oper = data.oper->find(call->operator_id);
    if (oper == 0) return;

    pPriceObj price_op = 0;

    if (call->out) {
        char * phone = &call->phone_num[0];

        if (call->isLocal()) {
            if (oper->local_network_pricelist_id) {
                call->pricelist_op_id = oper->local_network_pricelist_id;

                pNetworkPrefixObj networkItem = data.network_prefix->find(call->operator_id, call->phone_num);
                if (networkItem != 0) {
                    price_op = data.price->find(call->pricelist_op_id, networkItem->network_type_id);
                    if (price_op != 0) {
                        call->price_op = price_op->price;
                        strcpy(call->prefix_op, price_op->prefix);
                    }
                }

                if (price_op == 0) {
                    price_op = data.price->find(call->pricelist_op_id, "900");
                    if (price_op != 0) {
                        call->price_op = price_op->price;
                        strcpy(call->prefix_op, price_op->prefix);
                    }
                }

            }
        } else {
            call->pricelist_op_id = oper->pricelist_id;
            if (call->pricelist_op_id == 0) return;

            price_op = data.price->find(call->pricelist_op_id, phone);
            if (price_op != 0) {
                call->price_op = price_op->price;
                strcpy(call->prefix_op, price_op->prefix);
            }
        }

    } else {
        if (isUsage7800(call)) {
            if (oper->operator_7800_pricelist_id == 0) return;

            call->pricelist_op_id = oper->operator_7800_pricelist_id;

            price_op = data.price->find(call->pricelist_op_id, call->phone_num);
            if (price_op != 0) {
                call->price_op = price_op->price;
                strcpy(call->prefix_op, price_op->prefix);
            } else {
                price_op = data.price->find(call->pricelist_op_id, "7");
                if (price_op != 0) {
                    call->price_op = price_op->price;
                    strcpy(call->prefix_op, price_op->prefix);
                }
            }
        } else {
            call->price_op = -oper->term_in_cost;
        }
    }

    pPricelist pricelist = data.pricelist->find(call->pricelist_op_id);
    if (pricelist != 0) {
        call->len_op = getCallLength(
                call->len,
                pricelist->tariffication_by_minutes,
                pricelist->tariffication_full_first_minute,
                false
                );
    }

    call->amount_op = floor((float) (call->len_op * call->price_op) / (60 * 100) + 0.5);
}

pUsageObj CalcFull::spawnUsageVoip(pCallObj call) {

    if (call->isTrank()) {
        return data.usage->find(call->instance_id);
    } else {
        return data.usage->find(call->usage_num);
    }

}

void CalcFull::calculateMcn(pCallObj call) {

    pUsageObj usage = spawnUsageVoip(call);
    if (usage == 0) return;

    call->usage_id = usage->id;
    call->client_id = usage->client_id;

    if (call->out) {
        calculateMcnOut(call, usage);
    } else {
        calculateMcnIn(call, usage);
    }

}

void CalcFull::calculateMcnOut(pCallObj call, pUsageObj usage) {

    if (call->isLocal())
        call->pricelist_mcn_id = usage->pl_local_id;
    else if (call->isZonaMob())
        call->pricelist_mcn_id = usage->pl_local_mob_id;
    else if (call->isZonaStd())
        call->pricelist_mcn_id = usage->pl_russia_id;
    else if (call->isRussianStd())
        call->pricelist_mcn_id = usage->pl_russia_id;
    else if (call->isRussianMob())
        call->pricelist_mcn_id = usage->pl_russia_mob_id;
    else if (call->isInternational())
        call->pricelist_mcn_id = usage->pl_intern_id;
    else if (call->isSNG())
        call->pricelist_mcn_id = usage->pl_sng_id;

    call->len_mcn = getCallLength(
            call->len,
            usage->tariffication_by_minutes,
            usage->tariffication_full_first_minute,
            usage->tariffication_free_first_seconds
            );

    pPriceObj price_mcn = data.price->find(call->pricelist_mcn_id, call->phone_num);
    if (price_mcn != 0) {
        call->price_mcn = price_mcn->price;
        call->amount_mcn = floor((float) (call->len_mcn * call->price_mcn) / (60 * 100) + 0.5);
        strcpy(call->prefix_mcn, price_mcn->prefix);
    }


    updateFreeMinsCounters(call, usage);

    updateClientCounters(call);

}

void CalcFull::calculateMcnIn(pCallObj call, pUsageObj usage) {
    if (isUsage7800(call)) {
        pOperator oper = data.oper->find(call->operator_id);
        if (oper != 0 && oper->client_7800_pricelist_id != 0) {
            call->pricelist_mcn_id = oper->client_7800_pricelist_id;

            call->len_mcn = getCallLength(
                    call->len,
                    usage->tariffication_by_minutes,
                    usage->tariffication_full_first_minute,
                    usage->tariffication_free_first_seconds
                    );

            pPriceObj price_mcn = data.price->find(call->pricelist_mcn_id, call->phone_num);
            if (price_mcn != 0) {
                call->price_mcn = price_mcn->price;
                call->amount_mcn = floor((float) (call->len_mcn * call->price_mcn) / (60 * 100) + 0.5);
                strcpy(call->prefix_mcn, price_mcn->prefix);
            } else {
                price_mcn = data.price->find(call->pricelist_mcn_id, "7");
                if (price_mcn != 0) {
                    call->price_mcn = price_mcn->price;
                    call->amount_mcn = floor((float) (call->len_mcn * call->price_mcn) / (60 * 100) + 0.5);
                    strcpy(call->prefix_mcn, price_mcn->prefix);
                }
            }

            updateClientCounters(call);
        }
    } else {
        call->len_mcn = getCallLength(
                call->len,
                usage->tariffication_by_minutes,
                usage->tariffication_full_first_minute,
                usage->tariffication_free_first_seconds
        );
    }
}

int CalcFull::getCallLength(int len, bool byMinutes, bool fullFirstMinute, bool freeFirstMinutes) {

    if (freeFirstMinutes && len <= app().conf.billing_free_seconds)
        return 0;

    if (len <= 0)
        return 0;

    if (byMinutes) {

        int s = len % 60;
        if (s == 0)
            return len;

        return len + 60 - s;

    } else {

        if (fullFirstMinute && len < 60)
            return 60;

        return len;

    }
}

void CalcFull::updateFreeMinsCounters(pCallObj call, pUsageObj usage) {
    if (call->isLocal() &&
            usage->free_seconds > 0 &&
            (call->redirect_num[0] == 0 || usage->paid_redirect == false)
            ) {
        int used_free_seconds = data.counter_fmin->get(call->usage_id, 1);
        int used_free_seconds_local = fmin_counter2->get(call->usage_id, 1);
        if (used_free_seconds + used_free_seconds_local + call->len_mcn <= usage->free_seconds) {
            fmin_counter2->set(call->usage_id, 1, used_free_seconds_local + call->len_mcn);
            call->freemin_group_id = 1;
            call->amount_mcn = 0;
        }
    }
}

void CalcFull::updateClientCounters(pCallObj call) {
    if (call->amount_mcn <= 0) return;

    ClientCounterObj &cc = client_counter2->get(call->client_id);

    if (call->dt.month == cc.amount_month) {
        cc.sum_month += call->amount_mcn;
    } else if (call->dt.month > cc.amount_month) {
        cc.amount_month = call->dt.month;
        cc.sum_month = call->amount_mcn;
    }

    if (call->dt.day == cc.amount_day) {
        cc.sum_day += call->amount_mcn;
    } else if (call->dt.day > cc.amount_day) {
        cc.amount_day = call->dt.day;
        cc.sum_day = call->amount_mcn;
    }

    pClientObj client = data.client->find(call->client_id);
    if (client != 0 && call->dt.time >= client->amount_date) {
        cc.sum += call->amount_mcn;
    }

    if (cc.client_id == 0) cc.client_id = call->client_id;
}

bool CalcFull::isCallFromAnotherInstance(pCallObj call) {
    return call->instance_id >= 80
            && call->instance_id < 100
            && call->instance_id != app().conf.instance_id;
}

bool CalcFull::isUsage7800(pCallObj call) {
    return call->usage_num[0] == '7' && call->usage_num[1] == '8' && call->usage_num[2] == '0' && call->usage_num[3] == '0';
}

void CalcFull::html(std::stringstream &html) {

    html << "Time: <b>" << string_time(last_call_time) << "</b><br/>\n";

    if (false) {
        lock_guard<mutex> lock(data_global->rwlock);

        html << string_fmt(
                "usage:%d, price:%d, network_prefix:%d, c_fmins:%d <br/>\n",
                data_global->usage.datamap.size(),
                data_global->price.datamap.size(),
                data_global->network_prefix.datamap.size(),
                data_global->counter_fmin.datamap.size()
                );
    }
}



