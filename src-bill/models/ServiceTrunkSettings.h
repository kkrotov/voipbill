#pragma once

struct ServiceTrunkSettings {
    int id;
    char type[13];
    int order;
    int src_number_id;
    int dst_number_id;
    int pricelist_id;
};