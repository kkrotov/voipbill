#pragma once

#include "../../classes/BDb.h"
#include "../../models/nnp/NNPCountry.h"
#include "../../classes/ObjList.h"

class NNPCountryCodeList : public ObjList<NNPCountry> {

    vector<pair<int,int>> prefixList;
    bool sorted=false;
protected:
    string sql(BDb *db) {

        return "select code, name, prefixes from nnp.country order by code ";
    }

    inline void parse_item(BDbResult &row, NNPCountry *item) {

        item->code = row.get_i(0);
        item->name = row.get_s(1);
        string prefixes = row.get_s(2);
        item->prefixes = parse_int_array(prefixes);

        std::set<int>::iterator it;
        for (it = item->prefixes.begin(); it != item->prefixes.end(); ++it) {

            prefixList.push_back(pair<int,int>(*it,item->code));
        }
        sorted = false;
    }

    std::set<int> parse_int_array(string prefixes) {

        unsigned first = prefixes.find("{");
        unsigned last = prefixes.find("}");
        if (first<last)
            prefixes = prefixes.substr (first+1,last-first-1);

        std::set<int> res;
        std::stringstream ss(prefixes);
        int prefix;
        while (ss >> prefix) {

            res.insert(prefix);
            if (ss.peek() == ',')
                ss.ignore();
        }
        return res;
    }

    struct key_code {

        bool operator()(const NNPCountry &left, int code) {
            return left.code < code;
        }

        bool operator()(int code, const NNPCountry &right) {
            return code < right.code;
        }
    };
    struct compare_prefix {

        bool operator()(const pair<int,int> &left, const pair<int,int> &right) {
            return left.first < right.first;
        }

        bool operator()(const pair<int,int> &left, int prefix) {
            return left.first < prefix;
        }

        bool operator()(int prefix, const pair<int,int> &right) {
            return prefix < right.first;
        }
    };

public:
    NNPCountry *find(int code, stringstream *trace = nullptr) {

        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, code, key_code());
            begin = p.first;
            end = p.second;
        }
        NNPCountry *result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {

            if (result != nullptr) {

                *trace << "FOUND|NNPCountry|BY CODE '" << code << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            }
            else {
                *trace << "NOT FOUND|NNPCountry|BY CODE '" << code << "'" << "\n";
            }
        }
        return result;
    }
    pair<int,int> get_code_by_prefix (int prefix) {

        sort_prefix_list();
        vector<pair<int,int>> ::iterator it = lower_bound(prefixList.begin(), prefixList.end(), prefix, compare_prefix());
        if (it!=prefixList.end()) {

            pair<int,int> p = *it;
            if (p.first==prefix)
                return p;
        }
        return make_pair(0,0);
    }
    pair<int,int> get_prefix_by_number (long long int number) {

        pair<int,int> valid_prefix = make_pair(0,0);
        for (int i=1; i<5; i++) {

            int prefix = phone_prefix(number, i);
            //vector<pair<int,int>> ::iterator it = lower_bound(prefixList.begin(), prefixList.end(), prefix, compare_prefix());
            pair<int,int> p = get_code_by_prefix(prefix);
            if (p.first!=0 && p.first==prefix)
                valid_prefix = p;
        }
        return valid_prefix;
    }
    int phone_prefix (long long phonenumber, int len) {

        string pref = std::to_string(phonenumber).substr(0,len);
        return atoi(pref.c_str());
    }
    void sort_prefix_list() {

        if (!sorted) {

            std::sort(prefixList.begin(), prefixList.end(), compare_prefix());
            sorted = true;
        }
    }
};
