#pragma once

#include "CalcBase.h"

class CalcFull : public CalcBase {
protected:
    void calc_item(pCallObj call);
    void cleanupCalculatedFields(pCallObj call);
    pUsageObj spawnUsageVoip(pCallObj call);
    int getCallLength(int len, bool byMinutes, bool fullFirstMinute, bool freeFirstMinutes);
    void calculateOperator(pCallObj call);
    void calculateMcn(pCallObj call);
    void calculateMcnOut(pCallObj call, pUsageObj usage);
    void calculateMcnIn(pCallObj call, pUsageObj usage);
    void updateFreeMinsCounters(pCallObj call, pUsageObj usage);
    void updateClientCounters(pCallObj call);
    bool isCallFromAnotherInstance(pCallObj call);
    bool isUsage7800(pCallObj call);
public:
    void html(std::stringstream &html);
};

