#pragma once

struct Cdr {
    long long int id;

    char connect_time[28];
    int session_time;

    char src_number[33];
    char dst_number[33];
    char redirect_number[33];

    char src_route[33];
    char dst_route[33];

    short src_noa;
    short dst_noa;

    long long int call_id;
};