#pragma once

struct GeoPrefix {
    char prefix[20];
    int dest;
    int geo_id;
    int geo_operator_id;
    bool mob;
};