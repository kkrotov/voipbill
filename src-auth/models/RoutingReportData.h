#pragma once

#include "../../src/common.h"

using namespace std;

struct RoutingReportData {
    char prefix[21];
    char routes[31];

    vector<int> getOperators() {
        string temp = string(routes);
        temp = temp.substr(1, temp.size() - 2);

        vector<string> strOperators;
        vector<int> operators;

        split(strOperators, temp, boost::algorithm::is_any_of(","));

        for (auto it = strOperators.begin(); it != strOperators.end(); ++it) {
            operators.push_back(atoi((*it).c_str()));
        }

        return operators;
    }
};

typedef RoutingReportData * pRoutingReportData;