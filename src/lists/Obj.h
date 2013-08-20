#pragma once

#include "../common.h"

typedef struct _DestObj {
    char prefix[21];
    int dest;
    int geo_id;
    bool mob;
} DestObj, *pDestObj;

typedef struct _MobObj {
    char prefix[21];
} MobObj, *pMobObj;

typedef struct _UsageObj {
    long long int phone_num;
    int id;
    int client_id;
    int region;
    int freemin;
    bool paid_redirect;
    int pl_local_id;
    int pl_local_mob_id;
    int pl_russia_id;
    int pl_intern_id;
    int pl_sng_id;
} UsageObj, *pUsageObj;

typedef struct _Operator {
    int id;
    int pricelist_id;
    int term_in_cost; // x4
    int term_out_cost; // x4;
    int term_out_local_cost; // x4;
} Operator, *pOperator;

typedef struct _UsageRaw {
    int id;
    int client_id;
    long long int phone_num;
    time_t actual_from;
    time_t actual_to;
    int region;
} UsageRaw, *pUsageRaw;

typedef struct _PriceObj {
    int pricelist_id;
    time_t date_to;
    time_t date_from;
    char prefix[21];
    unsigned int price; // x4
} PriceObj, *pPriceObj;

typedef struct _ClientObj {
    int id;
    int limit_m;
    int limit_d;
    int credit;
    double balance;
    bool disabled;
    time_t amount_date;
    time_t last_payed_month;
} ClientObj, *pClientObj;

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
        if (amount_day == get_tday()) {
            return ((int) (sum_day * 1.18 + 0.5)) / 100.0;
        } else {
            return 0;
        }
    }

    double sumMonth() {
        if (amount_month == get_tmonth()) {
            return ((int) (sum_month * 1.18 + 0.5)) / 100.0;
        } else {
            return 0;
        }
    }

    double sumBalance() {
        return ((int) (sum * 1.18 + 0.5)) / 100.0;
    }
} ClientCounterObj, *pClientCounterObj;

typedef struct _CallObj {
    char id[20];
    char time[25];
    bool out;
    char usage[20];
    char phone[20];

    long long int id_num;

    bool mob;
    bool redirect;
    int dest;
    int usage_id;
    int client_id;
    int region;
    int len;
    int price; // x4
    int price_op; // x4
    int amount; // x2
    int amount_op; // x2
    int pricelist_id;
    int operator_id;
    int freemin_group_id;
    int geo_id;
    int pricelist_op_id;

    int kill_call_reason;

    DT dt;
    void make_dt();
} CallObj, *pCallObj;

typedef struct _AsteriskNumberObj {
    long long int usr_num;
    char srv_ip[20];
    char usr_ip[20];
} AsteriskNumberObj, *pAsteriskNumberObj;
