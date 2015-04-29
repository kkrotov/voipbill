#pragma once

#include "../../src/classes/Timer.h"
#include "../../src/tasks/Task.h"
#include "../data/DataBillingContainer.h"

class TaskRecalc : public Task {
protected:
    time_t date_from;
    DataBillingContainer billingData;
    long long int recalc_from_call_id;

    Timer t_load;
    Timer t_calc;
    Timer t_save;

    void html(stringstream &html);
public:
    TaskRecalc(time_t date_from, string server_id);

    void run();
};

