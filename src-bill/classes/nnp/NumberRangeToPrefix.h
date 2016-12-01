#include <vector>
#include "../../models/nnp/NNPNumberRange.h"

/**
 * 1. Нельзя использовать нулевые префиксы
 * 2. Для любых операций изменяющих список префиксов можно использовать только функции
 *  из класса NumberRangeToPrefix
 */
class NumberRangeToPrefix {

protected:

    static const size_t BUFFER_SIZE = 256;

    void initial (char* from, char* to, char* buffer, int index, std::vector<PhoneNumber> &prefix);

    bool handleUp   (char* from, int index, std::vector<PhoneNumber> &prefix);
    bool handleDown (char* from, int index, std::vector<PhoneNumber> &prefix);

    bool addAnyUp   (char* from, char* pos, int start, int to, std::vector<PhoneNumber> &prefix);

    bool fixZeroing   (std::vector<PhoneNumber> &prefix);
    bool fixInclusion (std::vector<PhoneNumber> &prefix);

    bool isIncluded (std::vector<PhoneNumber> &prefix, PhoneNumber);


public:

    bool checkRangeToPrefix (std::vector<PhoneNumber> &prefix, NNPNumberRange range);

    void transfer (NNPNumberRange &range, std::vector<PhoneNumber> &prefix);
    void add (std::vector<PhoneNumber> &range, PhoneNumber addition);
    void sub (std::vector<PhoneNumber> &range, PhoneNumber del);

    void add (std::vector<PhoneNumber> &range, std::vector<PhoneNumber> &addition);
    void sub (std::vector<PhoneNumber> &range, std::vector<PhoneNumber> &del);

    bool checkAdd (std::vector<PhoneNumber> &a, std::vector<PhoneNumber> &b,std::vector<PhoneNumber> &c);
    bool checkSub (std::vector<PhoneNumber> &a, std::vector<PhoneNumber> &b,std::vector<PhoneNumber> &c);
};
