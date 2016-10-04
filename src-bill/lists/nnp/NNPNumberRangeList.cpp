#include "NNPNumberRangeList.h"

struct NNPNumberRangeList::key_full_number_from {
    bool operator()(const NNPNumberRange &left, long long int num) {
        return left.full_number_from <= num;
    }

    bool operator()(long long int num, const NNPNumberRange &right) {
        return num <= right.full_number_to;
    }
};

NNPNumberRange *NNPNumberRangeList::getNNPNumberRange(long long int num, stringstream *trace) {
    if (num > 0 && this->data.size() > 0) {

        auto begin_orig = this->data.begin();
        auto end_orig = this->data.end();

        auto begin = begin_orig;
        auto end = end_orig;

        {
            int i = 0;

            do {
                auto p = equal_range(begin, end, num, key_full_number_from());
                begin = p.first;
                end = p.second;

                if ((begin != begin_orig) && (begin != end_orig)) {

                    auto item = begin - 1;

                    if (item->full_number_from <= num && num <= item->full_number_to) {

                        *trace << "FOUND|NNPNumberRange|BY NUM '" << num << "'" << "\n";
                        *trace << "||";
                        item->dump(*trace);
                        *trace << "\n";
                        return &*item;
                    }
                }

                i++;
            } while (begin != end && i < 5);


        }
    }
    if (trace != nullptr) {
        *trace << "NOT FOUND|NNPNumberRange|BY NUM '" << num << "'" << "\n";
        *trace << "\n";
    }
    return nullptr;
}