#include "NNPPrefixDestinationList.h"
#include <tuple>

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

    if (this->size() == 0) return false;
    if (nnpNumberRangePrefixIds.size() == 0)return false;

    for (auto itNumberRangePrefix = nnpNumberRangePrefixIds.begin();
         itNumberRangePrefix != nnpNumberRangePrefixIds.end();
         itNumberRangePrefix++) {

        int id = *itNumberRangePrefix;

        {
            MultiIndexData::index<NNPPrefixDestination::byPrefixId>::type::iterator it, end;
            std::tie(it, end) = prefixDestinationData.get<NNPPrefixDestination::byPrefixId>().equal_range(id);

            for (; it != end; ++it) {
                if (it->is_addition) idsIns.insert(it->nnp_destination_id);
                else idsDel.insert(it->nnp_destination_id);
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

bool NNPPrefixDestinationList::getPrefixsByNNPDestination(int nnpDestinationId, vector<int> &nnpPrefixesPlus,
                                vector<int> &nnpPrefixesMinus) {
    bool fResult = false;

    if (this->size() == 0) return false;
    {
        MultiIndexData::index<NNPPrefixDestination::byDestinationId>::type::iterator it, end;
        std::tie(it, end) = prefixDestinationData.get<NNPPrefixDestination::byDestinationId>().equal_range(nnpDestinationId);

        for (; it != end; ++it) {
            if (it->is_addition) nnpPrefixesPlus.push_back(it->nnp_prefix_id);
            else nnpPrefixesMinus.push_back(it->nnp_prefix_id);
            fResult = true;
        }
    }

    return fResult;
}
