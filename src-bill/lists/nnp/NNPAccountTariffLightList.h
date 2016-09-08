#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPAccountTariffLight.h"
#include "../../classes/AppBill.h"

class NNPAccountTariffLightList : public ObjList<NNPAccountTariffLight> {
protected:

    string sql(BDb * db) {
        return "select id, \"number\", account_client_id, " \
            "   tariff_id, extract(epoch from activate_from), extract(epoch from deactivate_from), coefficient " \
            "   from nnp.account_tariff_light ";
    }

    inline void parse_item(BDbResult &row, NNPAccountTariffLight * item) {
        item->id = row.get_i(0);
        item->number = row.get_i(1);
        item->account_client_id = row.get_i(2);
        item->nnp_tariff_id = row.get_i(3);
        item->activate_from = row.get_i(4);
        item->deactivate_from = row.get_i(5);
        item->coefficient = row.get_d(6);

    }

    struct key_id {
        bool operator() (const NNPAccountTariffLight & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const NNPAccountTariffLight & right) {
            return id < right.id;
        }
    };

public:
    NNPAccountTariffLight * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        NNPAccountTariffLight * result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|NNPAccountTariffLight|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|NNPAccountTariffLight|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
