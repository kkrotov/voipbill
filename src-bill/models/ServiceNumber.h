#pragma once

struct ServiceNumber {
    int id;
    int client_account_id;
    char did[20];
    long long activation_dt;
    long long expire_dt;
    int connection_point_id;
};