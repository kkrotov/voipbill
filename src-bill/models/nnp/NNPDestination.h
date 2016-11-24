#pragma once

#include "../../common.h"
#include <json/json.h>

struct NNPDestination {
    int id;
    string name;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "name: " << name << "";
        trace << ")";
    }

    Json::Value writeJsonValue() {
        Json::Value value;

        value["id"] = id;
        value["name"] = name;

        return value;
    }
};