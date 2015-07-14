#pragma once

#include "../common.h"

struct TrunkRule {
    int trunk_id;
    bool outgoing;
    int order;
    int prefixlist_id;
};
