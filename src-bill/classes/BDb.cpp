#include "BDb.h"
#include "DbException.h"
#include <stdlib.h>
#include <errno.h>
#include "Log.h"
#include "../common.h"

BDb::BDb() {
    this->conn = 0;
    this->need_advisory_lock = 0;
}

BDb::BDb(const string &connstr) {
    this->conn = 0;
    this->connstr = connstr;
    this->need_advisory_lock = 0;
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



    if (need_advisory_lock != 0) {
        string query = "select pg_try_advisory_lock(" + lexical_cast<string>(need_advisory_lock) + ") as locked";
        PGresult *res = PQexec(conn, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            DbException e(conn, "BDb::try_advisory_lock");
            PQclear(res);
            disconnect();
            throw e;
        }
        if ('t' != *PQgetvalue(res, 0, 0)) {
            PQclear(res);
            disconnect();
            throw Exception("Can not lock key" + lexical_cast<string>(need_advisory_lock), "BDb::try_advisory_lock");
        }
        if (res != nullptr) {
            PQclear(res);
        }
    }

    {
        PGresult *res = PQexec(conn, "set session time zone '+00'");
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            DbException e(conn, "BDb::connect set timezone");
            PQclear(res);
            disconnect();
            throw e;
        }
        if (res != nullptr) {
            PQclear(res);
        }
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

void BDb::needAdvisoryLock(int key) {
    need_advisory_lock = key;
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


    //char short_query[256];
    //strncpy(short_query, squery, sizeof (short_query) - 1);
    //errors += "SQL: ";
    //errors += short_query;
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

    //char short_query[256];
    //strncpy(short_query, squery, sizeof (short_query) - 1);
    //errors += "SQL: ";
    //errors += short_query;
    throw DbException(errors, "BDb::query");
}

BDbResult BDb::query(const string &squery) {
    return query(squery.c_str());
}

void BDb::copy(string dst_table, string src_table, string columns, string query, BDb *db_from, BDb *db_to) {

    string query_from("COPY ");
    if (query != "") {
        query_from.append("(" + query + ") ");
    } else {
        query_from.append(src_table + " ");
        if (columns != "") query_from.append("(" + columns + ") ");
    }
    query_from.append("TO STDOUT WITH BINARY");

    string query_to("COPY ");
    query_to.append(dst_table + " ");
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
