#include "NNPAccountTariffLightList.h"

NNPAccountTariffLight *NNPAccountTariffLightList::find(int id, stringstream *trace) {
    auto begin = this->data.begin();
    auto end = this->data.end();
    {
        auto p = equal_range(begin, end, id, key_id());
        begin = p.first;
        end = p.second;
    }
    NNPAccountTariffLight *result = begin < end ? &*begin : nullptr;

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

// Возващаем только действующие для указанного времени пакеты минут

// *************** Требует оптимизации на скорость поиска !!!!!!!!!!!!!!!!!!!!

void NNPAccountTariffLightList::findAllActiveByClientID(vector<NNPAccountTariffLight> &resultNNPAccountTariffLight,
                                                        int client_id, time_t connect_time, int service_number_id,
                                                        stringstream *trace) {
    auto begin = this->data.begin();
    auto end = this->data.end();

    bool f = false;

    for (auto it = begin; it != end; ++it) {
        NNPAccountTariffLight nnpTariff = *it;

        //time(&connect_time); // Текущие время в UTC

        if (nnpTariff.account_client_id == client_id &&
                ( nnpTariff.service_number_id == service_number_id ||
                  nnpTariff.service_trunk_id  == service_number_id
                ) &&
            nnpTariff.activate_from <= connect_time &&
            nnpTariff.deactivate_from >= connect_time) {
            f = true;
            resultNNPAccountTariffLight.push_back(nnpTariff);
            if (trace != nullptr) {
                *trace << "FOUND|NNPAccountTariffLight|BY CLIENT_ID '" << client_id << "'\n";
                *trace << "||";
                nnpTariff.dump(*trace);
                *trace << "\n";
            }
        }
    }
    if (!f) {
        if (trace != nullptr) {
            *trace << "NOT FOUND|NNPAccountTariffLight|BY CLIENT_ID '" << client_id << "'\n";
        }
    }
}
