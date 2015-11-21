#pragma once

#include "../common.h"

struct StatDestinationPrefixlists
{
    int destination_id;
    int prefixlist_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "destination_id: " << destination_id << ", ";
        trace << "prefixlist_id: " << prefixlist_id << ", ";
        trace << ")";
    }
};
