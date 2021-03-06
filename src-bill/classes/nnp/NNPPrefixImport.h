#pragma once

#include "../AppBill.h"
#include "../../models/Prefixlist.h"
#include "../BDb.h"
#include "../Repository.h"

class NNPPrefixImport {

    Repository repository;
    BDb db_main;
    std::string errormessage;
    Prefixlist prefixlist = {0};
    std::vector<PhoneNumber> phoneList;

public:
    NNPPrefixImport() {
        db_main.setCS(app().conf.db_main);
    }
    bool read(int prefixlist_id);
    bool save (int prefixlist_id, std::vector<PhoneNumber> phoneList);
    bool save (int prefixlist_id);
    std::string err_message() { return errormessage; }
    int destination_id() { return prefixlist.nnp_destination_id; };
    int type_id() { return prefixlist.type_id; };
    int country_code() { return prefixlist.country_code; };
    string operator_ids() {
        std::ostringstream oss;
        std::copy(prefixlist.operator_id.begin(), prefixlist.operator_id.end(), std::ostream_iterator<int>(oss, " "));
        return oss.str();
    };
    bool operator_excluded() { return prefixlist.exclude_oper; }
    int region_id() { return prefixlist.region_id; };
    int city_id() { return prefixlist.city_id;};
    int ndc_type_id() { return prefixlist.ndc_type_id; };
    bool is_valid() {
        return (prefixlist.nnp_destination_id || prefixlist.country_code || prefixlist.operator_id.size()>0 ||
                prefixlist.region_id || prefixlist.city_id || prefixlist.ndc_type_id);
    };
    int prefix_list_size() { return phoneList.size(); };
    bool valid_token (string token) { return prefixlist.ndc_token==token; };
};
