#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPPrefixDestination.h"
#include "../../classes/AppBill.h"

class NNPPrefixDestinationList : public ObjList<NNPPrefixDestination> {
protected:

    string sql(BDb *db) {
        return "select prefix_id,destination_id, is_addition, extract(epoch from insert_time) " \
            "   from nnp.prefix_destination ";
    }

    inline void parse_item(BDbResult &row, NNPPrefixDestination *item) {
        item->nnp_prefix_id = row.get_i(0);
        item->nnp_destination_id = row.get_i(1);
        item->is_addition = row.get_i(2);
        item->insert_time = row.get_i(3);
    }

public:

};
