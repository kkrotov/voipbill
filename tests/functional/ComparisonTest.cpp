#include "ComparisonTest.h"
#include <time.h>
#include <cxxtest/TestSuite.h>
#include <stdio.h>
#include <utility>

bool ComparisonTest :: testAllResults () {

    timespec setTime   = {0, 0};
    timespec checkTime = {0, 0};
    const int MESSAGE_SIZE = 1024;
    char message[MESSAGE_SIZE] = {};

    for (int i = 0; i <= this->getResultsNumber(); i++) {

        timespec currentStart = {0, 0};
        timespec currentStop  = {0, 0};

        clock_gettime (CLOCK_MONOTONIC, &currentStart);
        if (!this->setResult (i)) {
            sprintf (message, "%s : ERROR SET RESULT\nIndex:%d\nDesc:\n%s", this->getTestName(),
                                                                            i, this->getLastError().c_str());
            TS_FAIL(message);
            return false;
        }
        clock_gettime (CLOCK_MONOTONIC, &currentStop);
        setTime.tv_sec  += currentStop.tv_sec  - currentStart.tv_sec;
        setTime.tv_nsec += currentStop.tv_nsec - currentStart.tv_nsec;

        clock_gettime (CLOCK_MONOTONIC, &currentStart);
        if (!this->checkResult(i)) {
            sprintf (message, "%s : ERROR CHECK RESULT\nIndex:%d\nDesc:\n%s", this->getTestName(),
                                                                              i, this->getLastError().c_str());
            TS_FAIL(message);
            return false;
        }
        clock_gettime (CLOCK_MONOTONIC, &currentStop);
        checkTime.tv_sec  += currentStop.tv_sec  - currentStart.tv_sec;
        checkTime.tv_nsec += currentStop.tv_nsec - currentStart.tv_nsec;
    }
    sprintf (message, "%s : SUCCESS (set:%lfs)(check:%lfs)", this->getTestName(),
             (double)setTime.  tv_sec + (double)setTime.  tv_nsec / 1000000000.0,
             (double)checkTime.tv_sec + (double)checkTime.tv_nsec / 1000000000.0);
    TS_TRACE (message);

    return true;
}

