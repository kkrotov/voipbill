#pragma once

#include "../../src/lists/ObjList.h"
#include "../../src/classes/Spinlock.h"
#include "../models/ClientLockObj.h"
#include <mutex>
#include <map>

using namespace std;

class ClientLockCounter : public ObjList<ClientLockObj> {
protected:
    string sql(BDb * db) {
        return "";
    }
    inline void parse_item(BDbResult &row, ClientLockObj * item) {
        item->client_id = 0;
    }

public:
    Spinlock lock;

    unsigned long long int marker = 0;

    bool needTotalSync = true;
    bool isReady = false;

    map<int, ClientLockObj> counter;
    map<int, unsigned long long int> changes;

    ClientLockObj get(int client_id) {
        lock_guard<Spinlock> guard(lock);
        return counter[client_id];
    }

    void push(map<int, ClientLockObj> &client_locks) {
        lock_guard<Spinlock> guard(lock);

        for (auto pair : client_locks) {
            ClientLockObj &cc = counter[pair.first];
            if (cc.client_id == 0 || cc.disabled_local != pair.second.disabled_local || cc.disabled_global != pair.second.disabled_global) {
                cc.client_id = pair.first;
                cc.disabled_local = pair.second.disabled_local;
                cc.disabled_global = pair.second.disabled_global;
                marker++;
                changes[pair.first] = marker;
            }
        }

        map<int, ClientLockObj>::iterator it = counter.begin();
        while (it != counter.end()) {
            map<int, ClientLockObj>::iterator it2 = client_locks.find(it->first);
            if (it2 == client_locks.end()) {
                counter.erase(it++);
                marker++;
                changes[it->first] = marker;
                needTotalSync = true;
            } else {
                ++it;
            }
        }

        isReady = true;
    }

    void getLocks(vector<ClientLockObj> &destLocks) {
        lock_guard<Spinlock> guard(lock);

        destLocks.reserve(counter.size());
        for (auto pair : counter) {
            destLocks.push_back(pair.second);
        }
    }

    void getChanges(unsigned long long int &destMarker, vector<int> &destChanges, bool &destNeedTotalSync) {
        lock_guard<Spinlock> guard(lock);

        destMarker = marker;
        destNeedTotalSync = needTotalSync;

        if (needTotalSync) {
            destChanges.reserve(counter.size());
            for (auto pair : counter) {
                destChanges.push_back(pair.first);
            }
        } else {
            destChanges.reserve(changes.size());
            for (auto pair : changes) {
                destChanges.push_back(pair.first);
            }
        }
    }

    void fixChanges(unsigned long long int old_marker) {
        lock_guard<Spinlock> guard(lock);

        auto it = changes.begin();
        while (it != changes.end()) {
            if (it->second <= old_marker) {
                changes.erase(it++);
            } else {
                ++it;
            }
        }
    }

    void load(BDb * db) {
        loadtime = time(nullptr);
    }

    size_t size() {
        return counter.size();
    }
};


