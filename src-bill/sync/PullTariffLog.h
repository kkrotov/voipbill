#pragma once

#include "BasePull.h"

class PullLogTariff : public BasePull {
public:
    void init() {
        event = "tariff_change_log";
        src_table = "billing.tariff_change_log";
        dst_table = "billing.tariff_change_log";

        key = "id";

        fields.push_back("id");
        fields.push_back("usage_id");
        fields.push_back("ts");
        fields.push_back("date_activation");
        fields.push_back("tarif_id_local");
        fields.push_back("tarif_id_local_mob");
        fields.push_back("tarif_id_russia");
        fields.push_back("tarif_id_russia_mob");
        fields.push_back("tarif_id_intern");
        fields.push_back("tarif_id_sng");
    }
};
