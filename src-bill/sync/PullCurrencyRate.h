#pragma once

#include "BasePull.h"

class PullCurrencyRate : public BasePull {
public:
    void init() {
        event = "currency_rate";
        src_table = "billing.currency_rate";
        dst_table = "billing.currency_rate";

        fields.push_back("id");
        fields.push_back("date");
        fields.push_back("currency");
        fields.push_back("rate");
    }
};
