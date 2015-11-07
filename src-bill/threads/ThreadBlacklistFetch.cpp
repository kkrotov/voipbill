#include "ThreadBlacklistFetch.h"

ThreadBlacklistFetch::ThreadBlacklistFetch() {
    id = idName();
    name = "Blacklist Fetch";

    blacklist_global = BlackListGlobal::instance();
    blacklist_local = BlackListLocal::instance();
    blacklist_trunk = BlackListTrunk::instance();

    threadSleepSeconds = 3600;
}

void ThreadBlacklistFetch::run() {

    blacklist_local->fetch();

    blacklist_global->fetch();

    blacklist_trunk->fetch();

}
