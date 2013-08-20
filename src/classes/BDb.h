#pragma once

#include <map>
#include <list>
#include <string>
#include <libpq-fe.h>

using namespace std;

class BDbResult {
protected:
    PGresult * res;
    int count;
    int index;
public:
    BDbResult(PGresult * res);
    ~BDbResult();
    int size();
    char * get(int r, int f);
    char * get(int f);
    int get_i(int f);
    double get_d(int f);
    long long int get_ll(int f);
    bool get_b(int f);
    string get_s(int f);
    bool next();
    void force_free();
};

class BDb {
protected:
    PGconn * conn;
    string connstr;
    map<string, bool> listener;
public:
    BDb();
    BDb(const string &connstr);
    ~BDb();

    void setCS(const string &connstr);

    PGconn * getConn();
    bool connect();

    bool connected();

    void disconnect();

    void listen(const string name);
    list<string> notifies();

    void exec(const char * squery);
    void exec(const string &squery);

    BDbResult query(const char * squery);
    BDbResult query(const string &squery);

    static void copy(string table_to, string table_from, string columns, string query, BDb *db_from, BDb *db_to);

    bool ping();
    static bool test(const string &connstr);
};

