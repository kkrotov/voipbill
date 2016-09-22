#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>


using namespace std;

#include "BaseTest.h"
#include "RemoveHtmlTagsTest.h"

bool startTests ();
bool testThis       (BaseTest* test);

BOOST_AUTO_TEST_CASE(first_test) {

    bool failed = startTests();

    if (!failed) {
        printf ("\nTESTING SUCCESS\n");
    }
    else {
        printf ("\nTESTING FAILED\n");
    }

    BOOST_CHECK (!failed);
}

/**
 * Запускает unit тесты.
 *
 * @return Если хоть один тест не был пройден false, если все прошли удачно true
 */
bool startTests () {
    const unsigned int TEST_NUM = 1;

    BaseTest* TESTS[TEST_NUM] = {
            static_cast<BaseTest*>(new RemoveHtmlTagsTest ())
    };

    printf ("\n");

    bool failed = false;
    for (auto test : TESTS) {
        printf ("---\nTesting:%s", test->getTestName());
            failed = testThis(test);

    }
    return !failed;
}

/**
 * Запускает setResult и checkResult со всеми индексами.
 * Записывает время потраченное на setResult b checkResult.
 *
 * @param test тест, который нужно запустить
 * @return true - если пройден удачно, false - если пройден неудачно.
 */
bool testThis  (BaseTest* test) {

    timespec setTime   = {0, 0};
    timespec checkTime = {0, 0};
    bool failedCurrent = false;

    for (int i = 0; i <= test->getMaxIndex(); i++) {

        timespec currentStart = {0, 0};
        timespec currentStop  = {0, 0};

        clock_gettime (CLOCK_MONOTONIC, &currentStart);
        if (!test->setResult (i)) {
            printf ("\nERROR SET RESULT\nIndex:%d\nDesc:\n%s\n", i, test->getLastError().c_str());
            test->resetError();
            failedCurrent = true;
        }
        clock_gettime (CLOCK_MONOTONIC, &currentStop);
        setTime.tv_sec  += currentStop.tv_sec  - currentStart.tv_sec;
        setTime.tv_nsec += currentStop.tv_nsec - currentStart.tv_nsec;

        clock_gettime (CLOCK_MONOTONIC, &currentStart);
        if (!test->checkResult(i)) {
            printf ("\nERROR CHECK RESULT\nIndex:%d\nDesc:\n%s\n", i, test->getLastError().c_str());
            test->resetError();
            failedCurrent = true;
        }
        clock_gettime (CLOCK_MONOTONIC, &currentStop);
        checkTime.tv_sec  += currentStop.tv_sec  - currentStart.tv_sec;
        checkTime.tv_nsec += currentStop.tv_nsec - currentStart.tv_nsec;
    }
    if (!failedCurrent)
        printf (" :: SUCCESS (set:%lfs)(check:%lfs)\n", (double)setTime.  tv_sec + (double)setTime.  tv_nsec / 1000000000.0,
                                                        (double)checkTime.tv_sec + (double)checkTime.tv_nsec / 1000000000.0);
    else
        printf ("\n%s :: FAILED\n", test->getTestName());

    return !failedCurrent;
}
