#include "NNPNumberRangeList.h"

struct NNPNumberRangeList::key_full_number_from {
    bool operator()(const NNPNumberRange &left, long long int num) {
        return left.full_number_from <= num;
    }

    bool operator()(long long int num, const NNPNumberRange &right) {
        return num < right.full_number_from;
    }
};


NNPNumberRange *NNPNumberRangeList::getNNPNumberRange(long long int num, stringstream *trace) {
    if (num > 0 && this->data.size() > 0) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = lower_bound(begin, end, num, key_full_number_from());
            if (p != begin) {
                p--;
                do {
                    bool isMatch = p->full_number_from <= num && num <= p->full_number_to && p->is_active;
                    if (isMatch) {
                        NNPNumberRange *result = p.base();
                        if (trace != nullptr && result != nullptr) {
                            *trace << "FOUND|NNPNumberRange|BY NUM '" << num << "'" << "\n";
                            *trace << "||";
                            result->dump(*trace);
                            *trace << "\n";
                        }
                        return p.base();
                    }
                    p++;
                    if (p == end) break;
                } while (p->full_number_to < num);
            }
        }
    }
    if (trace != nullptr) {
        *trace << "NOT FOUND|NNPNumberRange|BY NUM '" << num << "'" << "\n";
        *trace << "\n";
    }
    return nullptr;
}

