#!/bin/python
# -*- coding: utf-8 -*-

import psycopg2

centralDb    = psycopg2.connect(host='localhost', database='nispd_test', user='postgres')
centralDbCur = centralDb.cursor()

centralDbCur.execute('''
  DROP SCHEMA tests CASCADE;
''')
centralDb.commit()

centralDb.close()
