#pragma once

#include "../common.h"

struct Number {
    int id;
    char name[51];
    char prefixlist_ids[100];

    vector<int> getPrefixlistIds() {
        string str(prefixlist_ids);
        str = str.substr(1, str.size() - 2);

        vector<string> strPrefixlistIds;
        split(strPrefixlistIds, str, is_any_of(","));

        vector<int> intPrefixlistIds;
        intPrefixlistIds.reserve(strPrefixlistIds.size());

        for (auto it = strPrefixlistIds.begin(); it != strPrefixlistIds.end(); ++it) {
            int prefixlistId = atoi((*it).c_str());
            if (prefixlistId > 0) {
                intPrefixlistIds.push_back(prefixlistId);
            }
        }

        return intPrefixlistIds;
    }

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "name: " << name << ", ";
        trace << ")";
    }
};
