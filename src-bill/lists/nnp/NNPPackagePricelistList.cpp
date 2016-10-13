#include "NNPPackagePricelistList.h"

void NNPPackagePricelistList::findPackagePricelistIds(set<NNPPackagePricelist *> &resultNNPPackagePricelist,
                                                      int tariff_id,
                                                      long long int num, stringstream *trace) {
    auto begin = this->data.begin();
    auto end = this->data.end();
    {
        auto p = equal_range(begin, end, tariff_id, key_tariff_id());
        begin = p.first;
        end = p.second;
    }

    if (begin < end) {
        for (auto it = begin; it != end; ++it) {
            NNPPackagePricelist *package = &*it;
            resultNNPPackagePricelist.insert(package);
        }
    }

    else {
        if (trace != nullptr) {
            *trace << "NOT FOUND|NNP PACKAGE PRICELIST|BY NNP_TARIFF_ID '" << tariff_id << "'" << "\n";
        }
    }
}