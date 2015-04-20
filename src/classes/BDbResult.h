#pragma once
#include <string>
#include <libpq-fe.h>

using namespace std;

class BDbResult {
protected:
    PGresult * res;
    int count;
public:
    int index;
    BDbResult(PGresult * res);
    ~BDbResult();
    int size();
    char * get(int r, int f);
    bool is_null(int f);
    char * get(int f);
    int get_i(int f);
    double get_d(int f);
    long long int get_ll(int f);
    bool get_b(int f);
    string get_s(int f);
    void fill_cs(int f, char * str, int size);
    bool next();
    bool previous();
    bool first();
    bool last();
    void force_free();
};