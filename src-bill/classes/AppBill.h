#pragma once

#include "App.h"
#include "ConfBill.h"
#include "../healthcheck/HealthCheckController.h"

class AppBill : public App {
public:
    ConfBill conf;
    HealthCheckController healthCheckController;

    Thread * newThreadObject(std::string id);

protected:
    void runApp();
    void runAppInSingleMode();
    void runActiveThreads();
    void setHealthCheck();

    void registerAllThreads();    
    template<class T> void registerThread();

    std::map<std::string, Thread * (*)()> threadConstructorsMap;
};

AppBill & app();


