#pragma once

#include "../../classes/BDb.h"
#include "../../models/nnp/NNPCountryCode.h"
#include "../../classes/ObjList.h"

class NNPCounryCodeList : public ObjList<NNPCountryCode> {

protected:
    string sql(BDb *db) {

        return "select code, name, prefix from nnp.country order by code ";
    }

    inline void parse_item(BDbResult &row, NNPCountryCode *item) {

        item->code = row.get_i(0);
        item->name = row.get_s(1);
        item->prefix = row.get_i(2);
    }

    struct key_code {

        bool operator()(const NNPCountryCode &left, int code) {
            return left.code < code;
        }
        bool operator()(int code, const NNPCountryCode &right) {
            return code < right.code;
        }
    };

public:
    NNPCountryCode *find(int code, stringstream *trace = nullptr) {

        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, code, key_code());
            begin = p.first;
            end = p.second;
        }
        NNPCountryCode *result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {

            if (result != nullptr) {

                *trace << "FOUND|NNPCountryCode|BY CODE '" << code << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            }
            else {
                *trace << "NOT FOUND|NNPCountryCode|BY CODE '" << code << "'" << "\n";
            }
        }
        return result;
    }
    int get_code_by_prefix (int prefix) {

        for (NNPCountryCode &i: this->data) {

            if (i.prefix == prefix)
                return i.code;
        }
        return 0;
    }
};
