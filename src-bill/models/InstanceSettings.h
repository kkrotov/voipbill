#pragma once

struct InstanceSettings {
    int id;
    char region_id[100];
    int city_geo_id;
    int country_id;
    int city_id;

    vector<int> getRegionIds() {
        string temp = string(region_id);
        temp = temp.substr(1, temp.size() - 2);

        vector<string> strRegionIds;
        vector<int> regionIds;

        split(strRegionIds, temp, boost::algorithm::is_any_of(","));

        for (auto it = strRegionIds.begin(); it != strRegionIds.end(); ++it) {
            regionIds.push_back(atoi((*it).c_str()));
        }

        return regionIds;
    }

};