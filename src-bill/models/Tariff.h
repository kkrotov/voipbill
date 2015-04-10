#pragma once

struct Tariff {
    int id;
    int freemin;
    bool freemin_for_number;
    int pricelist_id;
    bool paid_redirect;
    bool tariffication_by_minutes;
    bool tariffication_full_first_minute;
    bool tariffication_free_first_seconds;
};
