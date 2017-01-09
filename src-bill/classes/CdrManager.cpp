#include "CdrManager.h"

CdrManager::CdrManager() {
    clear();
}

void CdrManager::clear() {
    lock_guard<Spinlock> guard(lock);

    lastId = -1;
    lastTime = 0;
    counter = 0;
    queue.clear();
    cdrsParts.clear();
    cdrsParts.push_back(vector<Cdr>());
}

bool CdrManager::ready() {
    lock_guard<Spinlock> guard(lock);

    return lastId >= 0;
}

bool CdrManager::loadPart(BDb * db_calls) {

    if (!ready() || getQueueSize() >= CDRS_MAX_QUEUE_SIZE) {
        return true;
    }

    string query =
            "	select " \
            "       id, " \
            "       connect_time, " \
            "       session_time, " \
            "       src_number, " \
            "       dst_number, " \
            "       dst_replace, " \
            "       redirect_number, " \
            "       src_route, " \
            "       dst_route, " \
            "       src_noa, " \
            "       dst_noa, " \
            "       call_id, " \
            "       disconnect_cause, " \
            "       call_finished, " \
            "       releasing_party, " \
            "       in_sig_call_id, " \
            "       out_sig_call_id, " \
            "       hash " \
            "	from calls_cdr.cdr " \
            "	where " \
            "       id > '" + lexical_cast<string>(getLastId()) + "' " \
            "	order by id " \
            "	limit " + lexical_cast<string>(CDRS_PARTITION_SIZE);

    BDbResult res = db_calls->query(query);
    if (res.size() > 0) {

        lock_guard<Spinlock> guard(lock);

        while (res.next()) {

            Cdr cdr;
            cdr.id = res.get_ll(0);
            cdr.connect_time = parseDateTime(res.get(1));
            cdr.session_time = res.get_i(2);
            strcpy((char *) &cdr.src_number, res.get(3));
            strcpy((char *) &cdr.dst_number, res.get(4));
            strcpy((char *) &cdr.dst_replace, res.get(5));
            strcpy((char *) &cdr.redirect_number, res.get(6));
            strcpy((char *) &cdr.src_route, res.get(7));
            strcpy((char *) &cdr.dst_route, res.get(8));
            cdr.src_noa = res.get_i(9);
            cdr.dst_noa = res.get_i(10);
            cdr.call_id = res.get_ll(11);
            cdr.disconnect_cause = res.get_i(12);
            strncpy((char*) &cdr.call_finished, res.get(13), sizeof(cdr.call_finished));
            strncpy((char*) &cdr.releasing_party, res.get(14), sizeof(cdr.releasing_party));

            strncpy((char *) &cdr.in_sig_call_id, res.get(15), sizeof(cdr.in_sig_call_id));
            strncpy((char *) &cdr.out_sig_call_id, res.get(16), sizeof(cdr.out_sig_call_id));
            strncpy((char *) &cdr.hash, res.get(17), sizeof(cdr.hash));

            queue.push_back(cdr);

            lastId = cdr.id;
            lastTime = cdr.connect_time;
            counter += 1;
        }
    }
    return res.size() < CDRS_PARTITION_SIZE;
}

size_t CdrManager::getQueueSize() {
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

Cdr * CdrManager::getFirstCdr() {
    lock_guard<Spinlock> guard(lock);

    if (queue.size() > 0) {
        return &queue.at(0);
    } else {
        return nullptr;
    }
}

void CdrManager::removeFirstCdr() {
    lock_guard<Spinlock> guard(lock);

    queue.pop_front();
}

void CdrManager::setLastId(long long int lastId) {
    lock_guard<Spinlock> guard(lock);

    this->lastId = lastId;
}

void CdrManager::setLastTime(time_t lastTime) {
    lock_guard<Spinlock> guard(lock);

    this->lastTime = lastTime;
}
