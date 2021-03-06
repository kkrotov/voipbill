#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPAccountTariffLight.h"
#include "../../classes/AppBill.h"

class NNPAccountTariffLightList : public ObjList<NNPAccountTariffLight> {

protected:

    string sql(BDb *db) {
        return "select id, account_tariff_id, account_client_id, " \
            "   tariff_id, extract(epoch from activate_from), extract(epoch from deactivate_from), coefficient,  " \
            "   price, service_type_id " \
            "   from nnp.account_tariff_light order by id asc";

    }

    inline void parse_item(BDbResult &row, NNPAccountTariffLight *item) {
        item->id = row.get_i(0);
        item->account_client_id = row.get_i(2);
        item->nnp_tariff_id = row.get_i(3);
        item->activate_from = row.get_i(4);
        item->deactivate_from = row.get_i(5);
        item->coefficient = row.get_d(6);
        item->price = row.get_d(7);
        item->service_type_id = row.get_i(8);
        if(item->service_type_id == ACCOUNT_TARIFF_LIGHT_SERVICE_TYPE_SIMPLE_NUMBER) {
            item->service_number_id = row.get_i(1); // Внимание! в БД поле называется account_tariff_id
        } else {
            item->service_trunk_id  = row.get_i(1); // Внимание! в БД поле называется account_tariff_id
        }
    }

    struct key_id {
        bool operator()(const NNPAccountTariffLight &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const NNPAccountTariffLight &right) {
            return id < right.id;
        }
    };

public:
    NNPAccountTariffLight *find(int id, stringstream *trace = nullptr);

    void findAllActiveByClientID(vector<NNPAccountTariffLight> &resultNNPAccountTariffLight, int client_id,
                                 time_t connect_time, int service_number_id, stringstream *trace);
};
