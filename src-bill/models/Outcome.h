#pragma once

#include "../common.h"

struct Outcome {
    int id;
    char name[51];
    int type_id;
    int route_case_id;
    int release_reason_id;
    int airp_id;
    char calling_station_id[21];
    char called_station_id[21];

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

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "type_id: " << type_id << ", ";
        trace << "route_case_id: " << route_case_id << ", ";
        trace << "release_reason_id: " << release_reason_id << ", ";
        trace << "airp_id: " << airp_id << ", ";
        trace << "calling_station_id: " << calling_station_id << ", ";
        trace << "called_station_id: " << called_station_id << ", ";
        trace << ")";
    }

};
