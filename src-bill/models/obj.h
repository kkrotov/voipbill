#pragma once

#include "../../src/common.h"
#include "Cdr.h"

#define KILL_REASON_UNKNOWN_NUMBER      1000
#define KILL_REASON_UNKNOWN_CLIENT      1010
#define KILL_REASON_UNKNOWN_MCN_PRICE   1030
#define KILL_REASON_VOIP_DISABLED       2000
#define KILL_REASON_CREDIT_LIMIT        3000
#define KILL_REASON_DAILY_LIMIT         3010
#define KILL_REASON_MONTHLY_LIMIT       3020

typedef struct _DestObj {
    char prefix[20];
    int dest;
    int geo_id;
    int geo_operator_id;
    bool mob;
} DestObj, *pDestObj;

typedef struct _MobObj {
    char prefix[20];
} MobObj, *pMobObj;


typedef struct _ClientCounterObj {
    int client_id;
    int sum;
    int sum_day;
    int sum_month;
    time_t amount_month;
    time_t amount_day;
    bool disabled_local;
    bool disabled_global;
    unsigned char updated;

    double sumDay() {
        if (abs(amount_day - get_tday()) < 43200) {
            return ((int) (sum_day * 1.18 + 0.5)) / 100.0;
        } else {
            return 0;
        }
    }

    double sumMonth() {
        if (abs(amount_month - get_tmonth()) < 43200) {
            return ((int) (sum_month * 1.18 + 0.5)) / 100.0;
        } else {
            return 0;
        }
    }

    double sumBalance() {
        return ((int) (sum * 1.18 + 0.5)) / 100.0;
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

} ClientCounterObj, *pClientCounterObj;

typedef struct _CallObj {
    char id[20];
    char cdr_id[20];
    char call_time[28];
    char call_origin[10];

    int client_account_id;
    int service_id;

    char src_number[33];
    char dst_number[33];
    char redirect_number[33];

    int billed_time;
    double rate;
    double cost;
    double tax_cost;

    int service_package_id;
    int service_package_limit_id;
    int package_time;
    double package_credit;

    int pricelist_id;
    char prefix[20];

    int destination_id;
    int geo_id;

    int kill_call_reason;

    DT dt;
    void initByCdr(Cdr * cdr);
    void initOrigByCdr(Cdr * cdr);
    void initTermByCdr(Cdr * cdr);
    void make_dt();
} CallObj, *pCallObj;
