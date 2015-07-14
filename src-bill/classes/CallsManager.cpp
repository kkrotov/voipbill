#include "CallsManager.h"

CallsManager::CallsManager() {
    lastId = -1;
    lastTime = 0;
    counter = 0;
    storedLastId = -1;
    storedLastTime = 0;
    storedCounter = 0;
}

bool CallsManager::ready() {
    lock_guard<Spinlock> guard(lock);

    return lastId >= 0 && storedLastId >= 0;
}

void CallsManager::add(Call &origCall, Call &termCall) {
    lock_guard<Spinlock> guard(lock);

    queue.push_back(origCall);
    queue.push_back(termCall);

    lastId = termCall.id;
    lastTime = termCall.connect_time;
    counter += 2;
}

void CallsManager::get(vector<Call> &calls, size_t maxCount) {
    lock_guard<Spinlock> guard(lock);

    maxCount = queue.size() < maxCount ? queue.size() : maxCount;
    calls.reserve(maxCount);

    while (queue.size() > 0 && calls.size() < maxCount) {
        calls.push_back(queue.front());
        queue.pop_front();
    }
}

void CallsManager::revert(vector<Call> &calls) {
    lock_guard<Spinlock> guard(lock);

    for (auto it = calls.rbegin(); it!= calls.rend(); ++it) {
        queue.push_front(*it);
    }
}

size_t CallsManager::size() {
    lock_guard<Spinlock> guard(lock);

    return queue.size();
}

long long int CallsManager::getLastId() {
    lock_guard<Spinlock> guard(lock);

    return lastId;
}

time_t CallsManager::getLastTime() {
    lock_guard<Spinlock> guard(lock);

    return lastTime;
}

size_t CallsManager::getCounter() {
    lock_guard<Spinlock> guard(lock);

    return counter;
}

void CallsManager::setLastId(long long int lastId) {
    this->lastId = lastId;
}

void CallsManager::setLastTime(time_t lastTime) {
    this->lastTime = lastTime;
}

long long int CallsManager::getStoredLastId() {
    lock_guard<Spinlock> guard(lock);

    return storedLastId;
}

time_t CallsManager::getStoredLastTime() {
    lock_guard<Spinlock> guard(lock);

    return storedLastTime;
}

size_t CallsManager::getStoredCounter() {
    lock_guard<Spinlock> guard(lock);

    return storedCounter;
}

void CallsManager::setStoredLastId(long long int storedLastId) {
    this->storedLastId = storedLastId;
}

void CallsManager::setStoredLastTime(time_t storedLastTime) {
    this->storedLastTime = storedLastTime;
}

void CallsManager::incStoredCounter(size_t count) {
    storedCounter += count;
}