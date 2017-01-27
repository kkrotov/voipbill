#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPNumberRange.h"
#include "../../models/nnp/NNPNumberRangePrefix.h"
#include "../../classes/AppBill.h"

#include <boost/multi_index_container.hpp>

using namespace boost::multi_index;

class NNPNumberRangePrefixList : public ObjList<NNPNumberRangePrefix> {
protected:


  typedef multi_index_container<
            NNPNumberRangePrefix,
            indexed_by<
                    ordered_non_unique<
                            tag<NNPNumberRangePrefix::byPrefixId>, member<
                                    NNPNumberRangePrefix,int,&NNPNumberRangePrefix::nnp_prefix_id
                            >
                    >,
                    ordered_non_unique<
                            tag<NNPNumberRangePrefix::byNumberRangeId>, member<
                                    NNPNumberRangePrefix,int,&NNPNumberRangePrefix::nnp_number_range_id
                            >
                    >

            >
    > MultiIndexData;
    MultiIndexData numberRangePrefixData;
    string sql(BDb *db) {
        return "select prefix_id,number_range_id, extract(epoch from insert_time) " \
            "   from nnp.number_range_prefix  order by number_range_id";
    }

    inline void parse_item(BDbResult &row, NNPNumberRangePrefix *item) {
        item->nnp_prefix_id = row.get_i(0);
        item->nnp_number_range_id = row.get_i(1);
        item->insert_time = row.get_ll(2);

        numberRangePrefixData.insert (*item);
    }

    struct key_nnp_number_range;

public:
    virtual void load(BDb * db) {
        BDbResult res = db->query(sql(db));

        loadtime = time(NULL);

        numberRangePrefixData.get<NNPNumberRangePrefix::byPrefixId>().clear();
        NNPNumberRangePrefix i;

        while (res.next()) {
            parse_item(res, &i);
        }
    }

    virtual size_t dataSize() {
        return sizeof(NNPNumberRangePrefix) * this->numberRangePrefixData.get<NNPNumberRangePrefix::byPrefixId>().size();
    }

    virtual size_t size() {
        return this->numberRangePrefixData.get<NNPNumberRangePrefix::byPrefixId>().size();
    }
    bool getNNPPrefixsByNumberRange(vector<int> &nnpPrefixIds, int nnpNumberRangeId, stringstream *trace);
    bool getNumberRangesByNNPPrefix(vector<int> &numberRanges, int prefixId);
};
