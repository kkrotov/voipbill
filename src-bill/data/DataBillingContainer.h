#pragma once

#include "../../src/common.h"
#include "../../src/classes/BDb.h"
#include "../../src/classes/Spinlock.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "ClientCounterData.h"
#include "FminCounterData.h"

class DataBillingContainer {
public:
    mutex calcLock;
    mutex saveLock;

    Spinlock cdrsWaitProcessingLock;
    Spinlock callsWaitSavingLock;

    deque<Cdr> cdrsWaitProcessing;
    deque<Call> callsWaitSaving;

    ClientCounterData clientCounter;
    FminCounterData fminCounter;

    long long int lastCallId = -1;
    time_t lastTime = 0;
    size_t calcedCdrsCount = 0;
    size_t savedCallsCount = 0;

    static DataBillingContainer * instance() {
        static DataBillingContainer inst;
        return &inst;
    }

    void loadAll(BDb * db) {

        clientCounter.load(db);
        fminCounter.load(db);

        loadLastCallIdAndTime(db);

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

        return true;
    }

    void html(stringstream &html) {
        html << "<table border=0>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>cdrsWaitProcessing queue length:</td>";
        html << "<td style='text-align: left' nowrap>" << cdrsWaitProcessing.size() << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>callsWaitSaving queue length:</td>";
        html << "<td style='text-align: left' nowrap>" << callsWaitSaving.size() << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>calced cdrs count:</td>";
        html << "<td style='text-align: left' nowrap>" << calcedCdrsCount << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>saved calls count:</td>";
        html << "<td style='text-align: left' nowrap>" << savedCallsCount << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last call id:</td>";
        html << "<td style='text-align: left' nowrap>" << lastCallId << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last connect time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(lastTime) << "<br/>";
        html << "</tr>\n";
        html << "</table>\n";
    }
private:
    void loadLastCallIdAndTime(BDb * db) {
        auto res = db->query("SELECT MAX(id), MAX(connect_time) FROM calls_raw.calls_raw");
        if (res.next()) {
            lastCallId = res.get_ll(0);
            lastTime = parseDateTime(res.get(1));
        } else {
            lastCallId = 0;
            lastTime = 0;
        }
    }
};