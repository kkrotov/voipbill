#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPNumberRange.h"
#include "../../classes/AppBill.h"

class NNPNumberRangeList : public ObjList<NNPNumberRange> {

protected:

    string sql(BDb *db) {
        return "select id, country_prefix, ndc, number_from, number_to, is_mob, " \
            "   is_active, operator_id, region_id, extract(epoch from insert_time), extract(epoch from update_time), city_id, " \
            "   full_number_from, full_number_to " \
            "   from nnp.number_range  " \
            "   where is_active and ( full_number_from <= full_number_to ) and  ( full_number_from >= 0 ) and " \
            "   length(full_number_from::varchar) = length(full_number_to::varchar) " \
            "   order by full_number_from, full_number_to";
    }

    inline void parse_item(BDbResult &row, NNPNumberRange *item) {
        item->id = row.get_i(0);
        item->country_prefix = row.get_i(1);
        item->ndc = row.get_i(2);
        item->number_from = row.get_ll(3);
        item->number_to = row.get_ll(4);
        item->is_mob = row.get_b(5);
        item->is_active = row.get_b(6);
        item->nnp_operator_id = row.get_i(7);
        item->nnp_region_id = row.get_i(8);
        item->insert_time = row.get_i(9);
        item->update_time = row.get_i(10);
        item->nnp_city_id = row.get_i(11);
        item->full_number_from = row.get_ll(12);
        item->full_number_to = row.get_ll(13);

    }

    struct key_full_number_from;

public:


    NNPNumberRange *getNNPNumberRange(long long int num, stringstream *trace);
};