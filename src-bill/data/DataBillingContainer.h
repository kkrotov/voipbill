#pragma once

#include "../../src/common.h"
#include "../../src/classes/BDb.h"
#include "../../src/classes/Spinlock.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "ClientCounterData.h"
#include "FminCounterData.h"
#include "../classes/AppBill.h"

class DataBillingContainer {
public:
    mutex fetchCdrLock;
    mutex calcCallsLock;
    mutex saveCallsLock;
    mutex syncCallsCentralLock;
    mutex syncCountersCentralLock;

    Spinlock cdrsWaitProcessingLock;
    Spinlock callsWaitSavingLock;

    deque<Cdr> cdrsWaitProcessing;
    deque<Call> callsWaitSaving;

    ClientCounterData clientCounter;
    FminCounterData fminCounter;

    long long int lastCdrId = -1;
    long long int lastCalcCallId = -1;
    long long int lastSaveCallId = -1;
    long long int lastSyncCentralCallId = -1;

    time_t lastCdrTime = 0;
    time_t lastCalcCallTime = 0;
    time_t lastSaveCallTime = 0;
    time_t lastSyncCentralCallTime = 0;

    size_t calcedCdrsCount = 0;
    size_t savedCallsCount = 0;

    static DataBillingContainer * instance() {
        static DataBillingContainer inst;
        return &inst;
    }

    void loadAll(BDb * db) {

        clientCounter.load(db);
        fminCounter.load(db);

        loadLastCallIdAndCdrIdAndTime(db);

        cdrsWaitProcessing.clear();
        callsWaitSaving.clear();
        calcedCdrsCount = 0;
        savedCallsCount = 0;
    }

    bool ready() {

        if (!clientCounter.ready()) {
            return false;
        }

        if (!fminCounter.ready()) {
            return false;
        }

        if (lastCallId < 0) {
            return false;
        }

        if (lastCdrId < 0) {
            return false;
        }

        return true;
    }

    void prepareSyncCallsCentral(BDb * db_main) {
        if (lastSyncCentralCallId < 0) {
            loadSyncCentralCallIdAndTime(db_main);
        }
    }

    void html(stringstream &html) {
        html << "<table border=0>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>cdrsWaitProcessing queue length:</td>";
        html << "<td style='text-align: left' nowrap>" << cdrsWaitProcessing.size() << "<br/>";
        html << "<td style='text-align: left' nowrap>callsWaitSaving queue length:</td>";
        html << "<td style='text-align: left' nowrap>" << callsWaitSaving.size() << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>calced cdrs count:</td>";
        html << "<td style='text-align: left' nowrap>" << calcedCdrsCount << "<br/>";
        html << "<td style='text-align: left' nowrap>saved calls count:</td>";
        html << "<td style='text-align: left' nowrap>" << savedCallsCount << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last cdr id:</td>";
        html << "<td style='text-align: left' nowrap>" << lastCdrId << "<br/>";
        html << "<td style='text-align: left' nowrap>last cdr time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(lastCdrTime) << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last calc call id:</td>";
        html << "<td style='text-align: left' nowrap>" << lastCalcCallId << "<br/>";
        html << "<td style='text-align: left' nowrap>last calc call time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(lastCalcCallTime) << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last save call id:</td>";
        html << "<td style='text-align: left' nowrap>" << lastSaveCallId << "<br/>";
        html << "<td style='text-align: left' nowrap>last save call time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(lastSaveCallTime) << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last sync central call id:</td>";
        html << "<td style='text-align: left' nowrap>" << lastSyncCentralCallId << "<br/>";
        html << "<td style='text-align: left' nowrap>last sync central call time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(lastSyncCentralCallTime) << "<br/>";
        html << "</tr>\n";
        html << "</table>\n";
    }
private:
    void loadLastCallIdAndCdrIdAndTime(BDb * db_calls) {
        auto res = db_calls->query("SELECT MAX(cdr_id), MAX(id), MAX(connect_time) FROM calls_raw.calls_raw");
        if (res.next()) {
            lastCdrId           = res.get_ll(0);
            lastSaveCallId      = res.get_ll(1);
            lastSaveCallTime    = parseDateTime(res.get(2));
        } else {
            lastCdrId           = 0;
            lastSaveCallId      = 0;
            lastSaveCallTime    = 0;
        }

        lastCalcCallId      = lastSaveCallId;
        lastCalcCallTime    = lastSaveCallTime;
        lastCdrTime         = lastSaveCallTime;
    }

    void loadSyncCentralCallIdAndTime(BDb * db_main) {
        auto res = db_main->query("select id, connect_time from calls_raw.calls_raw where server_id = " + app().conf.str_instance_id + " order by id desc limit 1");
        if (res.next()) {
            lastSyncCentralCallId   = res.get_ll(0);
            lastSyncCentralCallTime = parseDateTime(res.get(1));
        } else {
            lastSyncCentralCallId   = 0;
            lastSyncCentralCallTime = 0;
        }
    }
};