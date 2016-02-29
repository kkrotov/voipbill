#!/bin/python
# -*- coding: utf-8 -*-

import psycopg2
import time
import datetime
import os
import sys
import imp

nowTime = datetime.datetime.now()
nowTimeStr = datetime.datetime.strftime(nowTime, '%Y-%m-%d %H:%M:%S')
nowTimeDbPartition = datetime.datetime.strftime(nowTime, '%Y%m')

# This file location
__location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__)))


regionsList = imp.load_source('regions-list', os.path.join(__location__, 'regions-list')).REGIONS_LIST.split()
print regionsList


# Import migrations list
migrations = imp.load_source('migrations.py', os.path.join(__location__, '../install/migrations/migrations.py')).migrations_order
print migrations


def centralDb() :
  return psycopg2.connect(database='nispd_test', user='postgres')

def regionalDb(reg) :
  return psycopg2.connect(database='nispd%s_test' % reg, user='postgres')


suffix = sys.argv[1] if len(sys.argv) > 1 else ''

for migration_name in migrations :
  migration_file = migration_name + suffix + '.py'
  migrate = None

  if suffix == '' :
    migrate = imp.load_source(migration_file, os.path.join(__location__, '../install/migrations/' + migration_file)).migrate
    print 'Migrating ' + migration_name + '...'
  else :
    # Специальная обработка seed-данных для тестирования миграций
    try :
      migrate = imp.load_source(migration_file, os.path.join(__location__, 'seed_data/' + migration_file)).migrate
    except IOError :
      continue
    print 'Applying seed data for testing ' + migration_name + ' migration...'

  migrate(centralDb, regionalDb, regionsList)

