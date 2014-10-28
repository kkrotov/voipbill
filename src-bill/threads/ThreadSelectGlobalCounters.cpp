#include "ThreadSelectGlobalCounters.h"
#include "../classes/AppBill.h"
#include "../classes/DataLoader.h"

shared_ptr<GlobalCountersObjList> ThreadSelectGlobalCounters::list(new GlobalCountersObjList());
Spinlock ThreadSelectGlobalCounters::sync_mutex;

shared_ptr<GlobalCountersObjList> ThreadSelectGlobalCounters::getList() {
    lock_guard<Spinlock> guard(sync_mutex);
    shared_ptr<GlobalCountersObjList> regionsCounters = list;
    return regionsCounters;
}

void ThreadSelectGlobalCounters::setList(shared_ptr<GlobalCountersObjList> regionsCounters) {
    lock_guard<Spinlock> guard(sync_mutex);
    list = regionsCounters;
}

void ThreadSelectGlobalCounters::run() {

    shared_ptr<GlobalCountersObjList> globalCounters(new GlobalCountersObjList());

    globalCounters->load(&db_main, 0);

    this->setList(globalCounters);
}

void ThreadSelectGlobalCounters::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() + "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() + "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";


    shared_ptr<GlobalCountersObjList> regionsCounters = getList();
    ObjList * ol = regionsCounters.get();

    if (ol != 0) html << "Regions Shared Counters: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

    html << "<br/>\n";
}

ThreadSelectGlobalCounters::ThreadSelectGlobalCounters() {
    id = idName();
    name = "Select Global counters";
    db_main.setCS(app().conf.db_main);
    
    threadSleepSeconds = app().conf.global_counters_select_interval;
}


