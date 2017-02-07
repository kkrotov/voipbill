#!/bin/python
# -*- coding: utf-8 -*-

import re
import psycopg2
import pgpubsub
import time
import datetime
import os
import sys
import imp
import urllib2
import httplib
import ConfigParser
import StringIO
import requests

ts = time.time()
timestamp = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')

print 'Call caller - запускает тест из auth.test_auth'
print 'Результат записывает в tests.call_tests'

centralDb    = psycopg2.connect(database='nispd_test', user='postgres')
centralDbCur = centralDb.cursor()

# Создаём таблицы для хранения тестовых данных
centralDbCur.execute('''
  CREATE SCHEMA IF NOT EXISTS tests;
  CREATE TABLE IF NOT EXISTS tests.call_tests (
  id      SERIAL,          -- id теста
  date TIMESTAMP NOT NULL, -- timestamp времени запуска Auth caller
  answer TEXT              -- Ответ биллера
  )
''')
centralDb.commit()

centralDbCur.execute('''
    SELECT id, server_id, trunk_name, src_number, dst_number,
    src_noa, dst_noa, redirect_number, session_time, connect_time, orig
    FROM auth.test_call;
''')

testRows = centralDbCur.fetchall()


def getRegions():
    parser = ConfigParser.RawConfigParser()

    data = '[REGIONS]\n' + open ("tests/regions-list", "r").read()
    readable = StringIO.StringIO (data)

    parser.readfp (readable)
    get = parser.get ("REGIONS", "REGIONS_LIST")

    return re.findall ('\d+', get)

def testRegion(region):
    for id, server_id, trunk_name, src_number, dst_number, src_noa, dst_noa, redirect_number,\
        session_time, connect_time, orig in testRows:
        if int(server_id) != int (region):
            continue
        testUrl = 'http://localhost:80'
        testUrl += str(region)
        testUrl += '/test/calc?trunk_name='
        testUrl += str(trunk_name)
        testUrl += '&src_number='
        testUrl += str(src_number)
        testUrl += '&dst_number='
        testUrl += str(dst_number)
        if redirect_number:
            testUrl += '&redirect_number='
            testUrl += str(redirect_number)
        testUrl += '&src_noa='
        testUrl += str(src_noa)
        testUrl += '&dst_noa='
        testUrl += str(dst_noa)
        testUrl += '&session_time='
        testUrl += str(session_time)
        testUrl += '&connect_time='
        testUrl += str(connect_time)
        testUrl += '&orig='
        testUrl += str(orig)

        resultPage   = requests.get (testUrl)
        resultString = re.findall ('RESULT\|.*', resultPage.text)

        resultQuery = 'INSERT INTO tests.call_tests (id, date, answer) VALUES ('
        resultQuery += str(id)
        resultQuery += ', \''
        resultQuery += timestamp
        resultQuery += '\', \''
        resultQuery += resultString[0]
        resultQuery += '\');'

        centralDbCur.execute(resultQuery)
        centralDb.commit()


regions = getRegions()
for region in regions:
    testRegion(region)

centralDb.close()
