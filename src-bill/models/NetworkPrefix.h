#pragma once

struct NetworkPrefix {
    int operator_id;
    long long int prefix;
    time_t date_from;
    time_t date_to;
    int network_type_id;
};
