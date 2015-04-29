#pragma once

struct Cdr {
    long long int id;

    time_t connect_time;
    int session_time;

    char src_number[33];
    char dst_number[33];
    char redirect_number[33];

    char src_route[33];
    char dst_route[33];

    int src_noa;
    int dst_noa;

    long long int call_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id " << id << ", ";
        trace << "connect_time: " << string_time(connect_time) << ", ";
        trace << "session_time: " << session_time << ", ";
        trace << "src_number: " << src_number << ", ";
        trace << "dst_number: " << dst_number << ", ";
        trace << "redirect_number: " << redirect_number << ", ";
        trace << "src_route: " << src_route << ", ";
        trace << "dst_route: " << dst_route << ", ";
        trace << "src_noa: " << src_noa << ", ";
        trace << "dst_noa: " << dst_noa << ", ";
        trace << "call_id: " << call_id << ", ";
        trace << ")";
    }
};