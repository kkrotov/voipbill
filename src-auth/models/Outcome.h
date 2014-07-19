#pragma once

struct Outcome {
    int id;
    char name[51];
    int type_id;
    char calling_station_id[21];
    char called_station_id[21];
    int route_case_id;
    int release_reason_id;
    int airp_id;

    bool isAuto() {
        return type_id == 1;
    }

    bool isRouteCase() {
        return type_id == 2;
    }

    bool isReleaseReason() {
        return type_id == 3;
    }

    bool isAirp() {
        return type_id == 4;
    }
};

typedef Outcome * pOutcome;