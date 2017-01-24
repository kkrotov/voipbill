#include "NNPNumberRangeList.h"
#include "NNPNumberRangePrefixList.h"

#include <tuple>

struct NNPNumberRangePrefixList::key_nnp_number_range {
    bool operator()(const NNPNumberRangePrefix &left, int nnp_number_range_id) {
        return left.nnp_number_range_id < nnp_number_range_id;
    }

    bool operator()(int nnp_number_range_id, const NNPNumberRangePrefix &right) {
        return nnp_number_range_id < right.nnp_number_range_id;
    }
};

bool NNPNumberRangePrefixList::getNNPPrefixsByNumberRange(vector<int> &nnpPrefixIds,
                                                          int nnpNumberRangeId, stringstream *trace) {
    if (nnpNumberRangeId < 1 || this->size() == 0) return false;
    {
        MultiIndexData::index<NNPNumberRangePrefix::byNumberRangeId>::type::iterator it, end;
        std::tie(it, end) = numberRangePrefixData.get<NNPNumberRangePrefix::byNumberRangeId>().equal_range(nnpNumberRangeId);

        if (it != end) {
            for (; it != end; ++it) {
                nnpPrefixIds.push_back(it->nnp_prefix_id);
                if (trace != nullptr) {
                    *trace << "FOUND|NNPPrefix|BY NNPNumberRange '" << nnpNumberRangeId << "'" << "\n";
                    *trace << "||";
                    it->dump(*trace);
                    *trace << "\n";
                }
            }
            return true;
        }

    }
    return false;
}

bool NNPNumberRangePrefixList::getNumberRangesByNNPPrefix(vector<int> &numberRanges, int prefixId) {
    if (prefixId < 1 || this->size() == 0) return false;
    {
        MultiIndexData::index<NNPNumberRangePrefix::byPrefixId>::type::iterator it, end;
        std::tie(it, end) = numberRangePrefixData.get<NNPNumberRangePrefix::byPrefixId>().equal_range(prefixId);

        if (it != end) {
            for (; it != end; ++it) {
                numberRanges.push_back(it->nnp_number_range_id);
            }
            return true;
        }

    }
    return false;
}
/* TODO: Do range_by_prefix */