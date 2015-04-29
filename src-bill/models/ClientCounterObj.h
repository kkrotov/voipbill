#pragma once

#include "../../src/common.h"

struct ClientCounterObj {
    int client_id;
    double sum;
    double sum_day;
    double sum_month;
    time_t amount_month;
    time_t amount_day;
    bool disabled_local;
    bool disabled_global;
    unsigned char updated;

    double sumDay() {
        if (abs(amount_day - get_tday()) < 43200) {
            return sum_day * 1.18;
        } else {
            return 0;
        }
    }

    double sumMonth() {
        if (abs(amount_month - get_tmonth()) < 43200) {
            return sum_month * 1.18;
        } else {
            return 0;
        }
    }

    double sumBalance() {
        return sum * 1.18;
    }

    void updateVoipDisabledGlobal(bool lock) {
        updateVoipDisabled(true, lock);
    }

    void updateVoipDisabledLocal(bool lock) {
        updateVoipDisabled(false, lock);
    }

private:
    void updateVoipDisabled(bool global, bool lock) {
        bool & voipDisabled = global ? disabled_global : disabled_local;

        if (voipDisabled != lock) {
            voipDisabled = lock;
            updated = 1;
        }
    }

};