#pragma once

struct NetworkPrefix {
    int operator_id;
    char prefix[20];
    time_t date_from;
    time_t date_to;
    int network_type_id;
};
