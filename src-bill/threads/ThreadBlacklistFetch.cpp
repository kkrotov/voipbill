#include "ThreadBlacklistFetch.h"

ThreadBlacklistFetch::ThreadBlacklistFetch() {
    id = idName();
    name = "Blacklist Fetch";

    blacklist_global = BlackListGlobal::instance();
    blacklist_local = BlackListLocal::instance();
    blacklist_trunk = BlackListTrunk::instance();

    threadSleepSeconds = 3600;
}

bool ThreadBlacklistFetch::prepare() {

    return true;

    if (!blacklist_local->fetch()) {
        return false;
    }

    if (!blacklist_global->fetch()) {
        return false;
    }

    if (!blacklist_trunk->fetch()) {
        return false;
    }

    return true;
}

void ThreadBlacklistFetch::run() {
    return;
    blacklist_local->fetch();

    blacklist_global->fetch();

    blacklist_trunk->fetch();

}
