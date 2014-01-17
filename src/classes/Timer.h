#pragma once

#include <time.h>
#include <string>

using namespace std;

class Timer {
    unsigned long long time;
    unsigned long long full_time;

    timespec ts_start;
    timespec ts_stop;

    bool started;

public:
    unsigned long long count;

    Timer() {
        this->time = 0;
        this->full_time = 0;
        this->count = 0;
        this->started = false;
    }

    void start() {
        if (started) return;

        clock_gettime(CLOCK_MONOTONIC, &ts_start);
        started = true;
    }

    void stop() {
        if (!started) return;

        clock_gettime(CLOCK_MONOTONIC, &ts_stop);

        this->time = (ts_stop.tv_sec * 1e9 + ts_stop.tv_nsec) - (ts_start.tv_sec * 1e9 + ts_start.tv_nsec);
        this->full_time += this->time;
        this->count++;

        started = false;
    }

    double tloop() {
        return this->time / 1.0e9;
    }

    double tfull() {
        return this->full_time / 1.0e9;
    }

    string sloop() {
        char buf[50];
        sprintf(buf, "%f", tloop());
        return string(buf);
    }

    string sfull() {
        char buf[50];
        sprintf(buf, "%f", tfull());
        return string(buf);
    }
};

class TimerScope {
    Timer *timer;

public:

    TimerScope(Timer &timer) {
        this->timer = &timer;
        start();
    }

    virtual ~TimerScope() {
        stop();
    }

    void start() {
        timer->start();
    }

    void stop() {
        timer->stop();
    }
};