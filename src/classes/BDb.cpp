#include "BDb.h"
#include "DbException.h"
#include <stdlib.h>
#include <errno.h>
#include "Log.h"
#include "../common.h"

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

BDb::BDb() {
    this->conn = 0;
}

BDb::BDb(const string &connstr) {
    this->conn = 0;
    this->connstr = connstr;
}

BDb::~BDb() {
    disconnect();
}

void BDb::setCS(const string &connstr) {
    this->connstr = connstr;
}

PGconn * BDb::getConn() {
    return conn;
}

bool BDb::connect() {
    if (connected()) return true;
    conn = PQconnectdb(connstr.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        DbException e(conn);
        disconnect();
        throw e;
    }

    for (map < string, bool>::iterator i = listener.begin(); i != listener.end(); ++i) {
        PGresult *res = PQexec(conn, i->first.c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            DbException e(conn, "BDb::connect:listen");
            PQclear(res);
            disconnect();
            throw e;
        }
        PQclear(res);
    }
    return true;
}

bool BDb::connected() {
    return conn != 0;
}

void BDb::disconnect() {
    if (conn != 0) {
        PQfinish(conn);
        conn = 0;
    }
}

bool BDb::reconnect() {
    disconnect();
    return connect();
}

void BDb::listen(const string name) {
    const string qlisten = "LISTEN " + name;

    if (listener.find(qlisten) != listener.end()) return;

    listener[qlisten] = true;

    exec(qlisten);
}

list<string> BDb::notifies() {

    connect();

    int sock = PQsocket(conn);
    if (sock < 0) {
        throw DbException("Could not create PQsocket", "BDb::notifies");
    }

    map<string, bool> m;
    list<string> list;
    PGnotify *notify;

    PQconsumeInput(conn);
    while ((notify = PQnotifies(conn)) != NULL) {
        string n = string(notify->relname);
        if (*notify->extra != 0) n += "|" + string(notify->extra);

        map<string, bool>::iterator it = m.find(n);
        if (it == m.end()) {
            m[n] = true;
            list.push_back(n);
        }
        PQfreemem(notify);
    }
    if (list.size() > 0) return list;

    fd_set input_mask;
    FD_ZERO(&input_mask);
    FD_SET(sock, &input_mask);

    if (select(sock + 1, &input_mask, NULL, NULL, NULL) < 0) {
        disconnect();
        throw DbException(strerror(errno), "BDb::notifies: select()");
    }

    PQconsumeInput(conn);
    while ((notify = PQnotifies(conn)) != NULL) {
        string n = string(notify->relname);
        if (*notify->extra != 0) n += "|" + string(notify->extra);

        map<string, bool>::iterator it = m.find(n);
        if (it == m.end()) {
            m[n] = true;
            list.push_back(n);
        }
        PQfreemem(notify);
    }

    return list;
}

void BDb::exec(const char * squery) {

    int attemps_count = connected() ? 2 : 1;

    string errors;

    while (attemps_count-- > 0) {
        try {
            connect();

            PGresult *res = PQexec(conn, squery);
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                DbException e(conn);
                PQclear(res);
                res = 0;
                disconnect();
                throw e;
            }

            if (res != 0) {
                PQclear(res);
            }
            return;
        } catch (DbException &e) {
            errors += e.what() + "\n";
        }
    }


    char short_query[256];
    strncpy(short_query, squery, sizeof (short_query) - 1);
    errors += "SQL: ";
    errors += short_query;
    throw DbException(errors, "BDb::exec");
}

void BDb::exec(const string &squery) {
    exec(squery.c_str());
}

BDbResult BDb::query(const char * squery) {

    int attemps_count = connected() ? 2 : 1;

    string errors;

    while (attemps_count-- > 0) {
        try {
            connect();

            PGresult *res = PQexec(conn, squery);
            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                DbException e(conn);
                PQclear(res);
                res = 0;
                disconnect();
                throw e;
            }

            if (res != 0) {
                return BDbResult(res);
            } else {
                disconnect();
            }
        } catch (DbException &e) {
            errors += e.what() + "\n";
        }
    }

    char short_query[256];
    strncpy(short_query, squery, sizeof (short_query) - 1);
    errors += "SQL: ";
    errors += short_query;
    throw DbException(errors, "BDb::query");
}

BDbResult BDb::query(const string &squery) {
    return query(squery.c_str());
}

void BDb::copy(string table_to, string table_from, string columns, string query, BDb *db_from, BDb *db_to) {
    char error[1024];

    string query_from("COPY ");
    if (query != "") {
        query_from.append("(" + query + ") ");
    } else {
        query_from.append(table_from + " ");
        if (columns != "") query_from.append("(" + columns + ") ");
    }
    query_from.append("TO STDOUT WITH BINARY");

    string query_to("COPY ");
    query_to.append(table_to + " ");
    if (columns != "") query_to.append("(" + columns + ") ");
    query_to.append("FROM STDIN WITH BINARY");

    if (!db_from->connect()) {
        throw Exception("Database error");
    }
    if (!db_to->connect()) {
        throw Exception("Database error");
    }

    PGresult *res;

    res = PQexec(db_from->getConn(), query_from.c_str());
    if (PQresultStatus(res) != PGRES_COPY_OUT) {
        DbException e(db_from->getConn(), "BDb::copy::from");
        PQclear(res);
        db_from->disconnect();
        throw e;
    }
    PQclear(res);

    res = PQexec(db_to->getConn(), query_to.c_str());
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        DbException e(db_to->getConn(), "BDb::copy::to");
        PQclear(res);
        db_from->disconnect();
        db_to->disconnect();
        throw e;
    }
    PQclear(res);

    char* pBuffer = 0;
    int bytesRead = 0;
    while ((bytesRead = PQgetCopyData(db_from->getConn(), &pBuffer, 0)) > -1) {
        if (pBuffer == NULL) continue;
        if (PQputCopyData(db_to->getConn(), pBuffer, bytesRead) != 1) {
            PQfreemem(pBuffer);
            db_from->disconnect();
            db_to->disconnect();
            throw DbException("error", "BDb::copy::transfer");
        }
        PQfreemem(pBuffer);
    }

    res = PQgetResult(db_from->getConn());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        DbException e(db_from->getConn(), "BDb::copy::from::result");
        PQclear(res);
        db_from->disconnect();
        db_to->disconnect();
        throw e;
    }
    PQclear(res);

    if (PQputCopyEnd(db_to->getConn(), NULL) != 1) {
        DbException e(db_to->getConn(), "BDb::copy::to::end");
        db_to->disconnect();
        throw e;

    }

    res = PQgetResult(db_to->getConn());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        DbException e(db_to->getConn(), "BDb::copy::to::result");
        PQclear(res);
        db_to->disconnect();
        throw e;
    }
    PQclear(res);
}

bool BDb::ping() {
    try {
        BDbResult res = query("SELECT 9");
        return (res.size() == 1) && (atoi(res.get(0, 0)) == 9);
    } catch (Exception &e) {
        Log::error(e.what());
    }
    return false;
}

bool BDb::test(const string &connstr) {
    BDb db(connstr);
    return db.ping();
}
