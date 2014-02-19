#pragma once

#include "../common.h"

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

typedef struct _UsageObj {
    long long int phone_num;
    int id;
    int client_id;
    int region;
    int free_seconds;
    bool paid_redirect;
    bool tariffication_by_minutes;
    bool tariffication_full_first_minute;
    bool tariffication_free_first_seconds;
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
    int local_network_pricelist_id;
    int client_7800_pricelist_id;
    int operator_7800_pricelist_id;
} Operator, *pOperator;

typedef struct _Pricelist {
    int id;
    int region;
    int operator_id;
    bool tariffication_by_minutes;
    bool tariffication_full_first_minute;
} Pricelist, *pPricelist;

typedef struct _PriceObj {
    int pricelist_id;
    char prefix[20];
    time_t date_from;
    time_t date_to;
    unsigned int price; // x4
} PriceObj, *pPriceObj;

typedef struct _NetworkPrefixObj {
    int operator_id;
    char prefix[20];
    time_t date_from;
    time_t date_to;
    short int network_type_id;
} NetworkPrefixObj, *pNetworkPrefixObj;

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

    char usage_num[20];
    char phone_num[20];
    char redirect_num[20];

    char prefix_geo[20];
    char prefix_mcn[20];
    char prefix_op[20];

    long long int id_num;

    bool mob;
    int dest;
    int usage_id;
    int client_id;
    int instance_id;
    int len;
    int len_mcn;
    int len_op;
    int price_mcn; // x4
    int price_op; // x4
    int amount_mcn; // x2
    int amount_op; // x2
    int pricelist_mcn_id;
    int pricelist_op_id;
    int operator_id;
    int freemin_group_id;
    int geo_id;
    int geo_operator_id;

    int kill_call_reason;

    DT dt;
    void make_dt();
    bool isLocal();
    bool isLocalOrZona();
    bool isZona();
    bool isRussian();
    bool isInternational();
    bool isSNG();
} CallObj, *pCallObj;
