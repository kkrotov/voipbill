#include "ThreadRegionsCounters.h"
#include "../classes/AppBill.h"
#include "../classes/DataLoader.h"

shared_ptr<RegionsCountersObjList> ThreadRegionsCounters::list(new RegionsCountersObjList());
Spinlock ThreadRegionsCounters::sync_mutex;

shared_ptr<RegionsCountersObjList> ThreadRegionsCounters::getList() {
    lock_guard<Spinlock> guard(sync_mutex);
    shared_ptr<RegionsCountersObjList> regionsCounters = list;
    return regionsCounters;
}

void ThreadRegionsCounters::setList(shared_ptr<RegionsCountersObjList> regionsCounters) {
    lock_guard<Spinlock> guard(sync_mutex);
    list = regionsCounters;
}

void ThreadRegionsCounters::run() {

    shared_ptr<RegionsCountersObjList> regionsCounters(new RegionsCountersObjList());

    regionsCounters->load(&db_main, 0);

    this->setList(regionsCounters);
}

void ThreadRegionsCounters::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() + "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() + "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";


    shared_ptr<RegionsCountersObjList> regionsCounters = getList();
    ObjList * ol = regionsCounters.get();

    if (ol != 0) html << "Regions Shared Counters: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

    html << "<br/>\n";
}

ThreadRegionsCounters::ThreadRegionsCounters() {
    id = "regionscounters";
    name = "Regions Counters";
    db_main.setCS(app().conf.db_main);
    
    threadSleepSeconds = app().conf.regions_counters_select_interval;
}


