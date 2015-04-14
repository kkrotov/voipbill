#pragma once

#include "CalcBase.h"

class CalcFull : public CalcBase {
protected:
    void calc_item(pCallObj call);
    UsageObj * spawnUsageVoip(pCallObj call);
    int getCallLength(int len, bool byMinutes, bool fullFirstMinute, bool freeFirstMinutes);
    void calculateOperator(pCallObj call);
    void calculateMcn(pCallObj call);
    void calculateMcnOut(pCallObj call, UsageObj * usage);
    void calculateMcnIn(pCallObj call, UsageObj * usage);
    void updateFreeMinsCounters(pCallObj call, UsageObj * usage);
    void updateClientCounters(pCallObj call);
    bool isCallFromAnotherInstance(pCallObj call);
    bool isUsage7800(pCallObj call);
public:
    void html(std::stringstream &html);
};

