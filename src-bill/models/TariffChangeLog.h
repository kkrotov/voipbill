#pragma once

#include <time.h>

struct TariffChangeLog {
    int usage_id;
    time_t activation_date;
    time_t expire_dt;
    int tariff_id_local;
    int tariff_id_local_mob;
    int tariff_id_russia;
    int tariff_id_russia_mob;
    int tariff_id_intern;
    int tariff_id_sng;
};
