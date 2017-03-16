#!/bin/python
# -*- coding: utf-8 -*-
import re
import psycopg2
import pgpubsub
import time
import datetime
import os
import imp
import urllib2
import httplib
import ConfigParser
import StringIO
import requests
import json
import sys
from pprint import pprint
reload(sys)
sys.setdefaultencoding('utf8')

ts = time.time()
timestamp = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')

with open('config_auth.json') as data_file:
    data = json.load(data_file)

reserveUrl = 'http://'
reserveUrl += data["reserve"] + ':8032'


db  = psycopg2.connect(data["db"])
cur = db.cursor()

cur.execute('''
    SELECT id, hostname FROM public.server;
    ''')
hostnames = cur.fetchall()

cur.execute('''
    SELECT id, server_id, name, trunk_name, src_number, dst_number,
    src_noa, dst_noa, redirect_number FROM auth.test_auth;
''')
testRows = cur.fetchall()


hostnames[0][0]
def hostnameLookup(num):
    for pair in hostnames:
        if  int(pair[0]) == int(num):
            return pair[1]
    return None

success = 0
fail = 0
skip = 0
for id, server_id, name, trunk_name, src_number, dst_number, src_noa, dst_noa, redirect_number in testRows:
    host = hostnameLookup(server_id)
    if host == None:
        skip = skip + 1
        continue

    host = 'http://' + host + ':8032'

    testUrl = '/test/auth?trunk_name='
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

    queryRegion = host
    queryRegion += testUrl
    queryRegion += '&region='
    queryRegion += str(server_id)

    queryReserve = reserveUrl
    queryReserve += testUrl

    resultPage   = requests.get (queryReserve)
    resultString = re.findall ('RESULT\|.*', resultPage.text)
    resultReserve = resultString

    resultPage   = requests.get (queryRegion)
    resultString = re.findall ('RESULT\|.*', resultPage.text)
    resultRegion = resultString

    if str(resultRegion) != str(resultReserve):
        fail = fail + 1

        print ('For test: ' + str(name) + '(' + str(id) + ')')
        print ('Region{' + str(server_id) + '} response:' + str(resultRegion))
        print ('Reserve    response:' + str(resultReserve))
        print ('-----------')
    else:
        success = success + 1

print ('')
print ('Successful:')
print (success)
print ('\nFailure:')
print (fail)
print ('\nSkipped:')
print (skip)
print ('')
