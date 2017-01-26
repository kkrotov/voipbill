//#include "BaseTest.h"
#include <string.h>
#include <cxxtest/TestSuite.h>
#include "../../src-bill/classes/nnp/PrefixTree.cpp"

/**
 * Класс проверяющий корректную работу класса PrefixTree.
 *
 */
class PrefixTreeTest : public CxxTest::TestSuite {

    public:

    void testRangeToPrefix (void) {
        constexpr int n = 3;
        constexpr int repeat = 1000;

        long long int min = 1;
        for (int i = 0; i < n - 1; i++)
            min *= 10;
        long long int max = min * 10;


        TS_TRACE ("Testing PrefixTree addRange");
        timespec start, stop;
        long long int full = 0;
        for (int i = 0; i < repeat; i++) {
            PrefixTree tree(100);
            long long int iFrom = min   + rand() % (max - min);
            long long int iTo   = iFrom + rand() % (max - iFrom);
            std::pair<long long int, long long int> pair = {iFrom, iTo};
            clock_gettime (CLOCK_REALTIME, &start);
            tree.addRange(pair);
            clock_gettime (CLOCK_REALTIME, &stop);
            long long int add = 1000000000 * (stop.tv_sec  - start.tv_sec)
                                +            (stop.tv_nsec - start.tv_nsec);
            full += add;

            for (long long int j = min; j < max; j++) {
                char input[32] = {};
                sprintf (input, "%lld", j);
                bool isIncluded = tree.checkInclusion (input, 0);
                bool toInclude  = (j >= iFrom) && (j <= iTo);
                if ( isIncluded != toInclude ) {
                    TS_ASSERT (isIncluded != toInclude);
                    break;
                }
            }
        }
        char microseconds[256];
        sprintf (microseconds, "Time(microseconds):%lld\nrepeats:%lld\nmax:%lld", full / 1000000,
                 repeat, max);
        TS_TRACE (microseconds);
        
        return ;
    }
};
