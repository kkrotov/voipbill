#include "App.h"

#include "Daemon.h"

App::App() {
    threads.setApp(this);
    
    onStatusChanged.connect(boost::bind(&ThreadPool::app_status_changed, &threads));
    onRealStatusChanged.connect(boost::bind(&ThreadPool::app_real_status_changed, &threads));

    status = AppStatus::APP_RUNNING;
    setRealStatus(AppStatus::APP_STARTED);
}

bool App::init(int argc, char* argv[]) {

    srand(time(0));
    
    setRealStatus(AppStatus::APP_INITIALIZING);

    return true;
}

void App::run() {

    initLogger();

    Daemoin::initSignalHandler(this);

    runApp();
}

void App::setStatus(AppStatus status) {
    this->status = status;
    onStatusChanged();
}

void App::setRealStatus(AppStatus real_status) {
    this->real_status = real_status;
    onRealStatusChanged();
}

AppStatus App::getStatus() {
    return status;
}

AppStatus App::getRealStatus() {
    return real_status;
}