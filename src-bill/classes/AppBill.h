#pragma once

#include "../../src/classes/App.h"
#include "ConfBill.h"

class AppBill : public App {
public:
    ConfBill conf;

    virtual bool init(int argc, char* argv[]) override;

    Thread * newThreadObject(std::string id);

protected:
    void runApp();
    void initLogger();
    void runAppInSingleMode();
    void runAppInTestMode();

    void registerAllThreads();    
    template<class T> void registerThread();

    std::map<std::string, Thread * (*)()> threadConstructorsMap;
};

AppBill & app();


