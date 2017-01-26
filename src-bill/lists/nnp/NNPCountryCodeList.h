#pragma once

#include "../../classes/BDb.h"
#include "../../models/nnp/NNPCountry.h"
#include "../../classes/ObjList.h"

class NNPCountryCodeList : public ObjList<NNPCountry> {

protected:
    string sql(BDb *db) {

        return "select code, name, prefix from nnp.country order by code ";
    }

    inline void parse_item(BDbResult &row, NNPCountry *item) {

        item->code = row.get_i(0);
        item->name = row.get_s(1);
        item->prefix = row.get_i(2);
    }

    struct key_code {

        bool operator()(const NNPCountry &left, int code) {
            return left.code < code;
        }

        bool operator()(int code, const NNPCountry &right) {
            return code < right.code;
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
    int get_code_by_prefix (int prefix) {

        if (prefix==7) // во избежание путаницы с Казахстаном
            return 643;

        for (NNPCountry &i: this->data) {

            if (i.prefix == prefix)
                return i.code;
        }
        return 0;
    }
};
