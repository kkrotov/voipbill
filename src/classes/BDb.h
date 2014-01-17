#pragma once

#include <libpq-fe.h>

#include <map>
#include <list>
#include <string>
using namespace std;

#include "BDbResult.h"

class BDb {
protected:
    PGconn * conn;
    string connstr;
    int need_advisory_lock;
public:
    BDb();
    BDb(const string &connstr);
    ~BDb();

    void setCS(const string &connstr);

    PGconn * getConn();
    bool connect();

    bool connected();

    void disconnect();

    void needAdvisoryLock(int key);

    void exec(const char * squery);
    void exec(const string &squery);

    BDbResult query(const char * squery);
    BDbResult query(const string &squery);

    static void copy(string table_to, string table_from, string columns, string query, BDb *db_from, BDb *db_to);

    bool ping();
    static bool test(const string &connstr);
};

