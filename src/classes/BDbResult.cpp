#include "BDbResult.h"

BDbResult::BDbResult(PGresult * res) {
    this->res = res;
    this->index = -1;
    this->count = PQntuples(res);
}

BDbResult::~BDbResult() {
    if (res != 0)
        PQclear(res);
}

int BDbResult::size() {
    return count;
}

char * BDbResult::get(int r, int f) {
    return PQgetvalue(res, r, f);
}

char * BDbResult::get(int f) {
    return PQgetvalue(res, index, f);
}

int BDbResult::get_i(int f) {
    return atoi(PQgetvalue(res, index, f));
}

double BDbResult::get_d(int f) {
    return atof(PQgetvalue(res, index, f));
}

long long int BDbResult::get_ll(int f) {
    return atoll(PQgetvalue(res, index, f));
}

bool BDbResult::get_b(int f) {
    return 't' == *PQgetvalue(res, index, f);
}

string BDbResult::get_s(int f) {
    return PQgetvalue(res, index, f);
}

bool BDbResult::next() {
    index++;
    if (count > 0 && index < count) {
        return true;
    } else
        return false;
}

void BDbResult::force_free() {
    if (res != 0) {
        PQclear(res);
        res = 0;
    }
}
