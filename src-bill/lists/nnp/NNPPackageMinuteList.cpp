#include "NNPPackageMinuteList.h"

// ************* Эту функцию нужно обязательно оптимизировать для быстрого поиска.

void NNPPackageMinuteList::findAllByTariffID(vector<NNPPackageMinute> &resultNNPPackageMinute, int nnp_tariff_id,
                                             stringstream *trace) {
    auto begin = this->data.begin();
    auto end = this->data.end();

    bool f = false;

    for (auto it = begin; it != end; ++it) {
        NNPPackageMinute nnpPackageMinute = *it;
        if (nnpPackageMinute.nnp_tariff_id == nnp_tariff_id) {
            f = true;
            resultNNPPackageMinute.push_back(nnpPackageMinute);
            if (trace != nullptr) {
                *trace << "FOUND|NNPPackageMinute|BY NNP_TARIFF_ID '" << nnp_tariff_id << "'\n";
                *trace << "||";
                nnpPackageMinute.dump(*trace);
                *trace << "\n";
            }
        }
    }
    if (!f) {
        if (trace != nullptr) {
            *trace << "NOT FOUND|NNPPackageMinute|BY NNP_TARIFF_ID '" << nnp_tariff_id << "'\n";
        }
    }
}