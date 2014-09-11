#!/usr/bin/python
# -*- coding: utf-8 -*-

from sql_access import *
from config import *
from model import *

def dropTestDatabases():
    db = DbWrap(cfg['dbmanager_conn_str'])
    dst = db.cursor()
    for dbname in [cfg['dbname_avbilling'], cfg['dbname_nispd'], cfg['dbname_nispd99']]:
        try:
            dst.execute("DROP DATABASE " + dbname + ";")
        except psycopg2.Error as e:
            if psycopg2.errorcodes.OBJECT_IN_USE == e.pgcode:
                print('Please, close all DB connections to the test databases before running test')
                exit()
            elif psycopg2.errorcodes.INVALID_CATALOG_NAME == e.pgcode:
                continue

def createEmptyTestDatabases():
    db = DbWrap(cfg['dbmanager_conn_str'])
    dst = db.cursor()
    for dbname in [cfg['dbname_avbilling'], cfg['dbname_nispd'], cfg['dbname_nispd99']]:
        dst.execute("CREATE DATABASE " + dbname + ";")

def createSchema(connstr, filename):
    db = DbWrap(connstr)
    dst = db.cursor()
    # strange bug prevention
    dst.execute("DROP TYPE dblink_pkey_results;")
    dst.execute(open(filename, "r").read())

def recreateDb():
    dropTestDatabases()
    createEmptyTestDatabases()

    createSchema(cfg['dbmanager_conn_str'] + " dbname=" + cfg['dbname_avbilling'], './sql/avbilling.sql')
    createSchema(cfg['dbmanager_conn_str'] + " dbname=" + cfg['dbname_nispd'], './sql/nispd.sql')
    createSchema(cfg['dbmanager_conn_str'] + " dbname=" + cfg['dbname_nispd99'], './sql/nispd99.sql')

    # common config not accessed by stat (TODO: is this error?)
    # nispd = NispdDatabase(cfg['dbmanager_conn_str'] + " dbname=" + cfg['dbname_nispd'])
    # nispd.insertInstanceSettings()
    # nispd.insertNetworkPrefix('7495100', 99, 2, 102)

    # nispd.insertPrice('74951005555', clt_pricelist_id, 1.6667)

