#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPPrefixDestination.h"
#include "../../models/nnp/NNPDestination.h"
#include "../../models/nnp/NNPNumberRangePrefix.h"
#include "../../classes/AppBill.h"
#include <boost/multi_index_container.hpp>

using namespace boost::multi_index;

class NNPPrefixDestinationList : public ObjList<NNPPrefixDestination> {
protected:


    typedef multi_index_container<
            NNPPrefixDestination,
            indexed_by<
                    ordered_non_unique<
                            tag<NNPPrefixDestination::byPrefixId>, member<
                                    NNPPrefixDestination,int,&NNPPrefixDestination::nnp_prefix_id
                            >
                    >,
                    ordered_non_unique<
                            tag<NNPPrefixDestination::byDestinationId>, member<
                                    NNPPrefixDestination,int,&NNPPrefixDestination::nnp_destination_id
                            >
                    >

            >
    > MultiIndexData;
    MultiIndexData prefixDestinationData;

    string sql(BDb *db) {
        return "select prefix_id,destination_id, is_addition, extract(epoch from insert_time) " \
            "   from nnp.prefix_destination order by prefix_id";
    }

    inline void parse_item(BDbResult &row, NNPPrefixDestination *item) {
        item->nnp_prefix_id = row.get_i(0);
        item->nnp_destination_id = row.get_i(1);
        item->is_addition = row.get_b(2);
        item->insert_time = row.get_i(3);

        prefixDestinationData.insert(*item);
    }

    struct key_prefix_id_range;

public:

    virtual void load(BDb * db) {
        BDbResult res = db->query(sql(db));

        loadtime = time(NULL);

        prefixDestinationData.get<NNPPrefixDestination::byDestinationId>().clear();
        NNPPrefixDestination i;

        while (res.next()) {
            parse_item(res, &i);
        }
    }

    virtual size_t dataSize() {
        return sizeof(NNPPrefixDestination) * this->prefixDestinationData.get<NNPPrefixDestination::byDestinationId>().size();
    }

    virtual size_t size() {
        return this->prefixDestinationData.get<NNPPrefixDestination::byDestinationId>().size();
    }

    bool getNNPDestinationsByPrefix(set<int> &nnpDestinationIds,
                                    vector<int> &nnpNumberRangePrefixIds, stringstream *trace);
    bool getPrefixsByNNPDestination(int nnpDestinationId, vector<int> &nnpPrefixesPlus,
                                    vector<int> &prefixesMinus);

};

/* TODO: add prefix by destination */