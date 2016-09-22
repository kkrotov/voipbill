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

// Возващаем только действующие прямо сейчас пакеты минут

void NNPAccountTariffLightList::findAllActiveByClientID(vector<NNPAccountTariffLight> &resultNNPAccountTariffLight,
                                                        int client_id,
                                                        stringstream *trace) {
    auto begin = this->data.begin();
    auto end = this->data.end();

    bool f = false;

    for (auto it = begin; it != end; ++it) {
        NNPAccountTariffLight nnpTariff = *it;
        time_t rawtime;
        time(&rawtime); // Текущие время в UTC

        if (nnpTariff.account_client_id == client_id &&
            nnpTariff.activate_from <= rawtime &&
            nnpTariff.deactivate_from >= rawtime) {
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
