#pragma once

#include "../../src/common.h"

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
            intPrefixlistIds.push_back(atoi((*it).c_str()));
        }

        return intPrefixlistIds;
    }

};
