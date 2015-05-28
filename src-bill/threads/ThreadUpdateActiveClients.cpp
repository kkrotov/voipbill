#include "ThreadUpdateActiveClients.h"

ThreadUpdateActiveClients::ThreadUpdateActiveClients() {
    id = idName();
    name = "Update active clients";

    data = DataContainer::instance();
    threadSleepSeconds = 60;
}

bool ThreadUpdateActiveClients::ready() {
    return data->prepareData(preparedData, time(nullptr));
}

void ThreadUpdateActiveClients::run() {

    if (data->prepareData(preparedData, time(nullptr))) {
        data->activeCounter.load(preparedData.serviceNumber, preparedData.serviceTrunk);
    }

}
