# -*- coding: utf-8 -*-

from collections import namedtuple
import datetime

from sqlalchemy import *
from sqlalchemy.orm import sessionmaker

from psycopg2.extras import Inet


def sqlConnection(user, password, dbname='', autocommit=False):
    extArgs = {}
    if autocommit:
        extArgs = {'isolation_level': "AUTOCOMMIT"}
    engine = create_engine('postgresql+psycopg2://' + user + ':' + password + '@localhost/' + dbname,
                           client_encoding='utf8', echo=False, **extArgs)
    SessionClass = sessionmaker(bind=engine, autocommit=autocommit)
    return SessionClass()

'''
def sqlConnection(dbname, user, password):
    engine = create_engine('postgres://' + user + ':' + password + '@localhost/' + dbname)
    SessionClass = sessionmaker(bind=engine)
    return SessionClass()
'''

TableCleaner = namedtuple("TableCleaner", "connection tables")

def cleanTables(cleanTables):
    for cleanDb in cleanTables:
        cleanScript = ''
        for cleanTable in cleanDb.tables:
            cleanScript += 'DELETE FROM ' + cleanTable + ';'
        cleanDb.connection.execute(cleanScript)
        cleanDb.connection.commit()
        cleanDb.connection.flush()

def fixTime(timeValue):
    """ В PostgreSQL время в миллисекундах, в Python'е - в микросекундах. """
    return timeValue.replace(microsecond = int(timeValue.microsecond / 1000) * 1000)

def stripTime(timeValue):
    return datetime.date(timeValue.year, timeValue.month, timeValue.day)