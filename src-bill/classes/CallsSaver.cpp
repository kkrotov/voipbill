#include <mutex>
#include "CallsSaver.h"


void insert_row(Call * call, stringstream &q) {

    q << "(";
    q << "'" << call->id << "',";
    q << (call->orig ? "true" : "false") << ",";
    q << "'" << call->peer_id << "',";
    q << "'" << call->cdr_id << "',";
    q << "'" << string_time(call->connect_time) << "',";
    if (call->trunk_id != 0) {
        q << "'" << call->trunk_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->account_id != 0) {
        q << "'" << call->account_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->trunk_service_id != 0) {
        q << "'" << call->trunk_service_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->number_service_id != 0) {
        q << "'" << call->number_service_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->src_number != 0) {
        q << "'" << call->src_number << "',";
    } else {
        q << "NULL,";
    }
    if (call->dst_number != 0) {
        q << "'" << call->dst_number << "',";
    } else {
        q << "NULL,";
    }
    q << "'" << call->billed_time << "',";
    q << "'" << call->rate << "',";
    q << "'" << call->cost << "',";
    q << "'" << call->tax_cost << "',";
    q << "'" << call->interconnect_rate << "',";
    q << "'" << call->interconnect_cost << "',";
    if (call->service_package_id != 0) {
        q << "'" << call->service_package_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->service_package_limit_id != 0) {
        q << "'" << call->service_package_limit_id << "',";
    } else {
        q << "NULL,";
    }
    q << "'" << call->package_time << "',";
    q << "'" << call->package_credit << "',";
    q << "'" << call->destination_id << "',";
    if (call->pricelist_id != 0) {
        q << "'" << call->pricelist_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->prefix != 0) {
        q << "'" << call->prefix << "',";
    } else {
        q << "NULL,";
    }
    if (call->geo_id != 0) {
        q << "'" << call->geo_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->geo_operator_id != 0) {
        q << "'" << call->geo_operator_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->operator_id != 0) {
        q << "'" << call->operator_id << "',";
    } else {
        q << "NULL,";
    }
    q << (call->mob ? "true" : "false") << ",";
    q << (call->geo_mob ? "true" : "false");
    q << ")\n";
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

void CallsSaver::setBillingData(DataBillingContainer *billingData) {
    repository.billingData = billingData;
}

size_t CallsSaver::save(const size_t save_part_count) {
    map<time_t, stringstream> queryPerMonth;

    vector<Call> callsForSave;

    try {
        repository.billingData->calls.get(callsForSave, save_part_count);

        for (Call &call : callsForSave) {

            if (queryPerMonth.find(call.dt.month) == queryPerMonth.end()) {
                char buff[20];
                struct tm timeinfo;
                gmtime_r(&call.dt.month, &timeinfo);
                strftime(buff, 20, "%Y%m", &timeinfo);

                stringstream &q = queryPerMonth[call.dt.month];
                q << "INSERT INTO calls_raw.calls_raw_" + string(buff) + "(" \
                        "id,orig,peer_id,cdr_id,connect_time,trunk_id,account_id,trunk_service_id,number_service_id," \
                        "src_number,dst_number,billed_time,rate,cost,tax_cost,interconnect_rate,interconnect_cost," \
                        "service_package_id,service_package_limit_id,package_time,package_credit," \
                        "destination_id,pricelist_id,prefix,geo_id,geo_operator_id,operator_id,mob,geo_mob" \
                     ")VALUES\n";

                insert_row(&call, q);

            } else {

                stringstream &q = queryPerMonth[call.dt.month];
                q << ",";

                insert_row(&call, q);
            }

        }


        {
            BDbTransaction trans(db);

            for (auto &it : queryPerMonth) {
                db->exec(it.second.str());
            }

            trans.commit();
        }

        if (callsForSave.size() > 0) {
            Call &call = callsForSave.at(callsForSave.size() - 1);
            repository.billingData->calls.setStoredLastId(call.id);
            repository.billingData->calls.setStoredLastTime(call.connect_time);
            repository.billingData->calls.incStoredCounter(callsForSave.size());
        }

        return callsForSave.size();

    } catch (exception &e) {

        repository.billingData->calls.revert(callsForSave);

        throw e;
    }
}



