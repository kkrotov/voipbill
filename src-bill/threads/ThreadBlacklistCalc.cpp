#include "ThreadBlacklistCalc.h"

ThreadBlacklistCalc::ThreadBlacklistCalc() {
    id = idName();
    name = "Blacklist Calc";

    blacklist_global = BlackListGlobal::instance();
    blacklist_local = BlackListLocal::instance();
    blacklist_trunk = BlackListTrunk::instance();

}

bool ThreadBlacklistCalc::ready() {

    if (!repository.billingData->ready()) {
        return false;
    }

    if (!repository.prepare()) {
        return false;
    }

    return true;
}

void ThreadBlacklistCalc::run() {

    return;

    if (!repository.billingData->ready()) {
        return;
    }

    if (!repository.prepare()) {
        return;
    }

    auto clientLock = repository.billingData->clientLock.get();
    if (!clientLock->isReady) {
        return;
    }

    set<string> wanted_blacklist_local;
    set<string> wanted_blacklist_global;
    set<string> wanted_blacklist_trunk;

    vector<ClientLockObj> locks;
    clientLock->getLocks(locks);

    for (ClientLockObj & lock : locks) {

        if (lock.disabled_local || lock.disabled_global) {
            map<int, ServiceNumber> &numbers = repository.activeCounter->clientNumbers[lock.client_id];
            for (auto number : numbers) {
                if (lock.disabled_local) {
                    wanted_blacklist_local.insert(number.second.did);
                }
                if (lock.disabled_global) {
                    wanted_blacklist_global.insert(number.second.did);
                }
            }
        }

        if (lock.disabled_global) {
            map<int, ServiceTrunk> &trunks = repository.activeCounter->clientTrunks[lock.client_id];
            for (auto pairTrunk : trunks) {
                auto trunk = repository.getTrunk(pairTrunk.second.trunk_id);
                if (trunk != nullptr) {
                    wanted_blacklist_trunk.insert(trunk->trunk_name);
                }
            }
        }
    }

    blacklist_local->push(wanted_blacklist_local);
    blacklist_global->push(wanted_blacklist_global);
    blacklist_trunk->push(wanted_blacklist_trunk);

}
