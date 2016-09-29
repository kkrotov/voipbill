#include "NNPPrefixDestinationList.h"

struct NNPPrefixDestinationList::key_prefix_id_range {
    bool operator()(const NNPPrefixDestination &left, int nnp_prefix_id) {
        return left.nnp_prefix_id < nnp_prefix_id;
    }

    bool operator()(int nnp_prefix_id, const NNPPrefixDestination &right) {
        return nnp_prefix_id < right.nnp_prefix_id;
    }
};

bool NNPPrefixDestinationList::getNNPDestinationsByPrefix(set<int> &nnpDestinationIds,
                                                          vector<int> &nnpNumberRangePrefixIds,
                                                          stringstream *trace) {

    set<int> idsIns, idsDel;
    bool fResult = false;

    if (this->data.size() == 0) return false;
    if (nnpNumberRangePrefixIds.size() == 0)return false;

    for (auto itNumberRangePrefix = nnpNumberRangePrefixIds.begin();
         itNumberRangePrefix != nnpNumberRangePrefixIds.end();
         itNumberRangePrefix++) {

        int id = *itNumberRangePrefix;

        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_prefix_id_range());
            auto begin2 = p.first;
            auto end2 = p.second;

            for (auto itPrefixDestination = begin2; itPrefixDestination != end2; itPrefixDestination++) {
                if (itPrefixDestination->is_addition) idsIns.insert(itPrefixDestination->nnp_destination_id);
                else idsDel.insert(itPrefixDestination->nnp_destination_id);
            }
        }
    }

    for (auto it = idsIns.begin(); it != idsIns.end(); it++) {
        if (idsDel.count(*it) == 0) {
            nnpDestinationIds.insert(*it);
            fResult = true;
        }
    }

    return fResult;

}