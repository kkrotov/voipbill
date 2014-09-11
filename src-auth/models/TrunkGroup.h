#pragma once

#include <vector>

#include <boost/algorithm/string.hpp>

using boost::algorithm::split;
using boost::algorithm::is_any_of;

using namespace std;

struct TrunkGroup {
    int id;
    char name[51];
    char trunk_numbers[101];

    vector<int> getTrunkNumbers() {
        string str(trunk_numbers);
        str = str.substr(1, str.size() - 2);

        vector<string> strTrunkNumbers;
        split(strTrunkNumbers, str, is_any_of(","));

        vector<int> intTrunkNumbers;
        intTrunkNumbers.reserve(strTrunkNumbers.size());

        for (auto it = strTrunkNumbers.begin(); it != strTrunkNumbers.end(); ++it) {
            intTrunkNumbers.push_back(atoi((*it).c_str()));
        }

        return intTrunkNumbers;
    }

};

typedef TrunkGroup * pTrunkGroup;