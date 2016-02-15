#!/bin/python
# -*- coding: utf-8 -*-

'''
Пример реализации миграции конфиг-файла, добавляюшей новый параметр:

import os
import imp
import configparser

__location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__)))
regionsList = imp.load_source('regions-list', os.path.join(__location__, 'regions-list')).REGIONS_LIST.split()

for region_id in regionsList :
  config = configparser.ConfigParser()
  config.read('app_bill%s.conf' % region_id)
  config['db']['bandwidth_limit_mbits'] = '100'

  with open('app_bill%s.conf' % region_id, 'w') as configfile :
    config.write(configfile)
'''

