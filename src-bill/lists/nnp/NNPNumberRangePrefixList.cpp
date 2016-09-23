#include "NNPNumberRangeList.h"
#include "NNPNumberRangePrefixList.h"


struct NNPNumberRangePrefixList::key_nnp_number_range {
    bool operator()(const NNPNumberRangePrefix &left, int nnp_number_range_id) {
        return left.nnp_number_range_id < nnp_number_range_id;
    }

    bool operator()(int nnp_number_range_id, const NNPNumberRangePrefix &right) {
        return nnp_number_range_id < right.nnp_number_range_id;
    }
};

bool NNPNumberRangePrefixList::getNNPPrefixsByNumberRange(vector<NNPNumberRangePrefix *> &nnpNumberRangePrefixList,
                                                          int nnpNumberRangeId, stringstream *trace) {
    if (nnpNumberRangeId < 1 || this->data.size() == 0) return false;
    {
        auto begin = this->data.begin();
        auto end = this->data.end();
        auto p = equal_range(begin, end, nnpNumberRangeId, key_nnp_number_range());

        if (p.first != p.second) {
            for (auto it = p.first; it != p.second; it++) {
                nnpNumberRangePrefixList.push_back(it.base());
                if (trace != nullptr) {
                    *trace << "FOUND|NNPPrefix|BY NNPNumberRange '" << nnpNumberRangeId << "'" << "\n";
                    *trace << "||";
                    it.base()->dump(*trace);
                    *trace << "\n";
                }
            }
        }

    }
}

