#pragma once

#include "../common.h"

struct ClientLockObj {
    int client_id;
    bool disabled_local;
    bool disabled_global;
};