#include "ThreadUpdateActiveClients.h"

ThreadUpdateActiveClients::ThreadUpdateActiveClients() {
    id = idName();
    name = "Update active clients";

    threadSleepSeconds = 60;
}

void ThreadUpdateActiveClients::run() {

    if (repository.prepare()) {
        repository.data->activeCounter.get()->load(repository.data->serviceNumber.get(), repository.data->serviceTrunk.get());
    }

}
