#include "CdrManager.h"

CdrManager::CdrManager() {
    lastId = -1;
    lastTime = 0;
    counter = 0;
    cdrsParts.push_back(vector<Cdr>());
}

bool CdrManager::ready() {
    return lastId >= 0;
}

void CdrManager::add(const Cdr &cdr) {
    size_t parts = cdrsParts.size();

    vector<Cdr> &cdrs = cdrsParts.at(parts - 1);

    cdrs.push_back(cdr);
    lastId = cdr.id;
    lastTime = cdr.connect_time;
    counter += 1;

    if (cdrs.size() >= CDRS_PARTITION_SIZE) {
        cdrsParts.push_back(vector<Cdr>());
    }
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

void CdrManager::setLastId(long long int lastId) {
    this->lastId = lastId;
}

void CdrManager::setLastTime(time_t lastTime) {
    this->lastTime = lastTime;
}

size_t CdrManager::getLastPartSize() {
    size_t parts = cdrsParts.size();
    return cdrsParts.at(parts - 1).size();
}
