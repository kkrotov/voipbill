#pragma once

struct OperatorRule {
    int key;
    int operator_id;
    bool outgoing;
    int order;
    int trunk_group_id;
    int prefixlist_id;
};

typedef OperatorRule * pOperatorRule;