#pragma once

#include "../common.h"

struct ClientLockObj {
    int client_id;
    bool disabled_local;   // Заблокированы звонки по МН и МГ
    bool disabled_global;  // Полностью заблокированна связь (вход и выход)
    bool is_finance_block; // В данный момент есть финансовая блокировка ( баланс + кредитный лимит  < 0 )
    bool is_overran; // В данный момент клиент выбрал лимит дневной или месячный
};