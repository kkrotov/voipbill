#pragma once

struct ConfigVersion {
    int id;
    char name[201];
    int low_balance_outcome_id;
    int blocked_outcome_id;
    char calling_station_id_for_line_without_number[101];
};

typedef ConfigVersion * pConfigVersion;