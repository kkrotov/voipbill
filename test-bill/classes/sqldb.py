# -*- coding: utf-8 -*-

from collections import namedtuple
import datetime


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