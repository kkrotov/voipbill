#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPNumberRangePrefix.h"
#include "../../classes/AppBill.h"

class NNPNumberRangePrefixList : public ObjList<NNPNumberRangePrefix> {
protected:

    string sql(BDb *db) {
        return "select prefix_id,number_range_id, extract(epoch from insert_time) " \
            "   from nnp.number_range_prefix ";
    }

    inline void parse_item(BDbResult &row, NNPNumberRangePrefix *item) {
        item->nnp_prefix_id = row.get_i(0);
        item->nnp_number_range_id = row.get_i(1);
        item->insert_time = row.get_i(2);
    }

public:

};
