#include "NNPPrefixImport.h"
#include "../DbException.h"
#include "../../models/nnp/NNPDestination.h"

bool NNPPrefixImport::read(int prefixlist_id) {

    try {

        errormessage = "";
        string sql = "select id, name, type_id, "
                             "nnp_filter_json ->> 'nnp_destination_id' as nnp_destination_id, "
                             "nnp_filter_json ->> 'country_code' as country_code, "
                             "nnp_filter_json ->> 'is_exclude_operators' as exclude_operators, "
                             "nnp_filter_json ->> 'operator_id' as operator_list, "
                             "nnp_filter_json ->> 'region_id' as region_id, "
                             "nnp_filter_json ->> 'city_id' as city_id, "
                             "nnp_filter_json ->> 'ndc_type_id' as ndc_type_id, "
                             "nnp_filter_json ->> 'token' as ndc_token "
                             "from auth.prefixlist "
                             "where id="+std::to_string(prefixlist_id);

        BDbResult row = db_main.query(sql);
        if (!row.next()) {

            errormessage = "Invalid prefixlist_id";
            return false;
        }
        prefixlist.id = row.get_i(0);
        row.fill_cs(1, prefixlist.name, sizeof(prefixlist.name));
        prefixlist.type_id = row.get_i(2);
        prefixlist.nnp_destination_id = row.get_i(3);
        prefixlist.country_code = row.get_i(4);
        prefixlist.exclude_oper = row.get_b(5);
        string operator_list = row.get_s(6);
        prefixlist.region_id = row.get_i(7);
        prefixlist.city_id = row.get_i(8);
        prefixlist.ndc_type_id = row.get_i(9);
        prefixlist.ndc_token = row.get_s(10);

        if (operator_list.size()>0) {

            sql = "select replace((json_array_elements(nnp_filter_json -> 'operator_id'))::text,'\"', '')::int as operator_id "
                          "from auth.prefixlist "
                          "where id="+std::to_string(prefixlist_id);

            BDbResult row = db_main.query(sql);
            while (row.next()) {

                int operator_id = row.get_i(0);
                prefixlist.operator_id.push_back(operator_id);
            }
        }
        return true;
    }
    catch (DbException &e) {

        errormessage = e.what();
        Log::error(e.what());
        return false;
    }
}

bool NNPPrefixImport::save (int prefixlist_id, std::vector<PhoneNumber> phoneList) {

    errormessage = "";
    string sql = "DELETE FROM auth.prefixlist_prefix WHERE prefixlist_id="+std::to_string(prefixlist_id)+"; ";
    if (phoneList.size()>0) {

        sql += "INSERT INTO auth.prefixlist_prefix(prefixlist_id, prefix) VALUES ";
        string phonePrefixList;
        for (auto it = phoneList.begin(); it != phoneList.end(); ++it) {

            if (phonePrefixList.size() > 0)
                phonePrefixList += ",";

            string recPrefix = "(" + std::to_string(prefixlist_id) + ",'" + std::to_string(*it) + "')";
            phonePrefixList += recPrefix;
        }
        sql += phonePrefixList;
    }
    try {

        db_main.exec(sql);
        return true;
    }
    catch (DbException &e) {

        errormessage = e.what();
        Log::error(e.what());
        return false;
    }
}

bool NNPPrefixImport::save (int prefixlist_id) {

    if (!repository.prepare()) {

        return false;
    }
    if (prefixlist.nnp_destination_id>0) {

        NNPDestination *nnpDestination = repository.getNNPDestination (prefixlist.nnp_destination_id);
        if (nnpDestination== nullptr) {

            errormessage = "Invalid nnp_destination_id";
            return false;
        }
        repository.getPrefixByNNPDestination (phoneList, prefixlist.nnp_destination_id);
    }
    else {

        repository.getPrefixByFilter(phoneList, prefixlist.country_code, prefixlist.operator_id, prefixlist.exclude_oper,
                                     prefixlist.region_id, prefixlist.city_id, prefixlist.ndc_type_id);
    }
    return save(prefixlist_id, phoneList);
}