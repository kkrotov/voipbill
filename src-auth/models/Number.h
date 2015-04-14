#pragma once

#include <vector>
#include <boost/algorithm/string.hpp>

using boost::algorithm::split;
using boost::algorithm::is_any_of;

using namespace std;

struct Number {
    int id;
    char name[51];
    char prefixlist_ids[101];

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

typedef Number * pNumber;