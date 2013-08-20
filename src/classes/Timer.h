#pragma once

#include <time.h>
#include <string>

using namespace std;

class Timer {
    unsigned long long time;
    unsigned long long full_time;


    timespec ts_start;
    timespec ts_stop;

public:
    unsigned long long count;

    Timer() {
        this->time = 0;
        this->full_time = 0;
        this->count = 0;
    }

    void start() {
        clock_gettime(CLOCK_MONOTONIC, &ts_start);
    }

    void stop() {
        clock_gettime(CLOCK_MONOTONIC, &ts_stop);

        this->time = (ts_stop.tv_sec * 1e9 + ts_stop.tv_nsec) - (ts_start.tv_sec * 1e9 + ts_start.tv_nsec);
        this->full_time += this->time;
        this->count++;
    }

    double tloop() {
        return this->time / 1.0e9;
    }

    double tfull() {
        return this->full_time / 1.0e9;
    }

    string sloop() {
        char buf[50];
        sprintf(buf, "%f", this->time / 1.0e9);
        return string(buf);
    }

    string sfull() {
        char buf[50];
        sprintf(buf, "%f", this->full_time / 1.0e9);
        return string(buf);
    }
};
