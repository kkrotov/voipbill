#include "CdrManager.h"

CdrManager::CdrManager() {
    lastId = -1;
    lastTime = 0;
    counter = 0;
}

bool CdrManager::ready() {
    lock_guard<Spinlock> guard(lock);

    return lastId >= 0;
}

void CdrManager::add(const Cdr &cdr) {
    lock_guard<Spinlock> guard(lock);

    queue.push_back(cdr);
    lastId = cdr.id;
    lastTime = cdr.connect_time;
    counter += 1;
}


bool CdrManager::get(Cdr &cdr) {
    lock_guard<Spinlock> guard(lock);

    if (queue.size() == 0) {
        return false;
    }

    cdr = queue.front();
    queue.pop_front();

    return true;
}

void CdrManager::revert(const Cdr &cdr) {
    lock_guard<Spinlock> guard(lock);

    queue.push_front(cdr);
}

size_t CdrManager::size() {
    lock_guard<Spinlock> guard(lock);

    return queue.size();
}

long long int CdrManager::getLastId() {
    lock_guard<Spinlock> guard(lock);

    return lastId;
}

time_t CdrManager::getLastTime() {
    lock_guard<Spinlock> guard(lock);

    return lastTime;
}

size_t CdrManager::getCounter() {
    lock_guard<Spinlock> guard(lock);

    return counter;
}

void CdrManager::setLastId(long long int lastId) {
    this->lastId = lastId;
}

void CdrManager::setLastTime(time_t lastTime) {
    this->lastTime = lastTime;
}


