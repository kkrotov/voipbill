#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPNumberRange.h"
#include "../../models/nnp/NNPNumberRangePrefix.h"
#include "../../classes/AppBill.h"

class NNPNumberRangePrefixList : public ObjList<NNPNumberRangePrefix> {
protected:

    string sql(BDb *db) {
        return "select prefix_id,number_range_id, extract(epoch from insert_time) " \
            "   from nnp.number_range_prefix  order by number_range_id";
    }

    inline void parse_item(BDbResult &row, NNPNumberRangePrefix *item) {
        item->nnp_prefix_id = row.get_i(0);
        item->nnp_number_range_id = row.get_i(1);
        item->insert_time = row.get_ll(2);
    }

    struct key_nnp_number_range;

public:

    bool getNNPPrefixsByNumberRange(vector<NNPNumberRangePrefix *> &nnpNumberRangePrefixList,
                                    int nnpNumberRangeId, stringstream *trace);
};
