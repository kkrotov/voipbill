#pragma once

#include "../common.h"


// Коды причин завершения звонка Q.850/Q.931 https://ru.wikipedia.org/wiki/Q.931
// Cisco-специфичные http://www.cisco.com/c/en/us/td/docs/voice_ip_comm/cucm/service/8_5_1/cdrdef/cdradmin/cdrcodes.html
enum CauseCode {
    // Normal call clearing
    CAUSE_NORMAL_CLEARING = 16,
    // User busy
    CAUSE_BUSY = 17,
    // No user responding
    CAUSE_NO_REPONDING = 18,
    // T.301 expired: – User Alerted, No answer from user
    CAUSE_NO_ANSWER = 19,
    // Normal, unspecified
    CAUSE_NORMAL_UNSPECIFIED = 31
};

struct Cdr {
    long long int id;

    time_t connect_time;
    time_t setup_time;
    int session_time;

    char src_number[33];
    char dst_number[33];
    char dst_replace[33];
    char redirect_number[33];

    char src_route[33];
    char dst_route[33];

    int src_noa;
    int dst_noa;

    long long int call_id;

    int disconnect_cause;

    char call_finished[16];
    char releasing_party[32];

    Cdr() {
        id = 0;
        connect_time = 0;
        setup_time = 0;
        session_time = 0;
        src_number[0] = 0;
        dst_number[0] = 0;
        dst_replace[0] = 0;
        redirect_number[0] = 0;
        src_route[0] = 0;
        dst_route[0] = 0;
        src_noa = 0;
        dst_noa = 0;
        call_id = 0;
        disconnect_cause = 0;
    }

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id " << id << ", ";
        trace << "connect_time: " << string_time(connect_time) << ", ";
        trace << "session_time: " << session_time << ", ";
        trace << "src_number: " << src_number << ", ";
        trace << "dst_number: " << dst_number << ", ";
        trace << "dst_replace: " << dst_replace << ", ";
        trace << "redirect_number: " << redirect_number << ", ";
        trace << "src_route: " << src_route << ", ";
        trace << "dst_route: " << dst_route << ", ";
        trace << "src_noa: " << src_noa << ", ";
        trace << "dst_noa: " << dst_noa << ", ";
        trace << "call_id: " << call_id << ", ";
        trace << "disconnect_cause: " << disconnect_cause << ", ";
        trace << ")";
    }
};
