#pragma once

#include "../classes/Timer.h"
#include "../classes/Task.h"
#include "../classes/Repository.h"

class TaskRecalc : public Task {
protected:
    time_t date_from;
    Repository repository;
    DataBillingContainer newBillingData;
    long long int recalc_from_call_id;

    Timer t_load;
    Timer t_calc;
    Timer t_save;

    void html(stringstream &html);
public:
    TaskRecalc(time_t date_from, string server_id);

    void run();
};

