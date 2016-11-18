#!/bin/python
# -*- coding: utf-8 -*-

import xmlrunner
import unittest2
import psycopg2

import ConfigParser
import StringIO
import re
from datetime import date

import sys
reload(sys)

sys.setdefaultencoding('utf8')

class TestComparativeNewMonthTables(unittest2.TestCase):

    def getRegions(self):

        parser = ConfigParser.RawConfigParser()

        data = '[REGIONS]\n' + open ("tests/regions-list", "r").read()
        readable = StringIO.StringIO (data)

        parser.readfp (readable)
        get = parser.get ("REGIONS", "REGIONS_LIST")

        return re.findall ('\d+', get)

    def getQueryStr(self, schema, table):

        time  = str (date.today().year)
        time += '%02d' % date.today().month

        result  = 'SELECT EXISTS (\n'
        result += 'SELECT 1\n'
        result += 'FROM   pg_catalog.pg_class c\n'
        result += 'JOIN   pg_catalog.pg_namespace n ON n.oid = c.relnamespace\n'
        result += 'WHERE  n.nspname = \'' + schema + '\'\n'
        result += 'AND    c.relname = \'' + table + '_' + time + '\'\n'
        result += ');\n'

        return result;

    def checkWithQuery(self, query, cur):

        cur.execute (query)

        created = cur.fetchall()

        for exists in created :
            return exists[0]

    def checkCentral(self):

        conn = psycopg2.connect(database='nispd_test', user='postgres')
        cur = conn.cursor()

        exists = self.checkWithQuery (self.getQueryStr('calls_aggr', 'calls_aggr'), cur)
        self.assertTrue (exists, msg='nispd_test doesnt have calls_aggr.calls_aggr_DATE')

        exists = self.checkWithQuery (self.getQueryStr('calls_raw', 'calls_raw'), cur)
        self.assertTrue (exists, msg='nispd_test doesnt have calls_raw.calls_raw_DATE')

        conn.close ()

    def checkRegional(self):

        regions = self.getRegions()
        for region in regions:
            conn = psycopg2.connect(database='nispd' + region + '_test', user='postgres')
            cur = conn.cursor()

            exists = self.checkWithQuery (self.getQueryStr('calls_raw', 'calls_raw'), cur)
            self.assertTrue (exists, msg='nispd' + region + '_test doesnt have calls_raw.calls_raw_DATE')

            exists = self.checkWithQuery (self.getQueryStr('calls_cdr', 'cdr'), cur)
            self.assertTrue (exists, msg='nispd' + region + '_test doesnt have calls_cdr.cdr_DATE')

            exists = self.checkWithQuery (self.getQueryStr('calls_cdr', 'cdr_unfinished'), cur)
            self.assertTrue (exists, msg='nispd' + region + '_test doesnt have calls_cdr.cdr_unfinished_DATE')

            conn.close()


    def runTest(self):
        self.checkCentral()
        self.checkRegional()

if __name__ == '__main__':
    unittest2.main()
