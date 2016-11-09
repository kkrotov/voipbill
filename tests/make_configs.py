#!/bin/python
# -*- coding: utf-8 -*-

import os
import imp

configTemplate = """
[main]
web_port = 80%(region_id)s
skip_threads = checkstarttable cdr_parser client_lock blacklist_fetch blacklist_calc limitcontrol checkstarttable radius_auth_server
mode = SINGLEREGIONAL

[db]
main =  host=127.0.0.1 dbname=nispd_test user=bill_daemon_remote password=qRPCcAvrS4qWvGAhgzrXWuGJ2nwYEf8C
calls = host=127.0.0.1 dbname=nispd%(region_id)s_test user=bill_daemon_local password=gwvIg7OPTfoJE5vxWVKpmvAxBPMNP7t1
bandwidth_limit_mbits = 100

[geo]
instance_id = %(region_id)s

[udp]
host = 127.0.0.1
port = 588%(region_id)s

[billing]
dc_break = 126
free_seconds = 5

[log]
#syslog_ident=voipbill
#syslog_min_level = 1
file_filename=/tmp/x%(region_id)s.log

[cdr_parcer]
cdr_ftp_host = localhost
cdr_ftp_user = igor
cdr_ftp_password = 10159
cdr_nasip = 127.0.0.1

[radius]
auth_port = 18%(region_id)s
acct_port = 19%(region_id)s
secret = 427a7663dbd035c5f7611c9545e9d0
"""

__location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__)))
regionsList = imp.load_source('regions-list', os.path.join(__location__, 'regions-list')).REGIONS_LIST.split()

for region_id in regionsList :
  configFile = open('app_bill%s.conf' % region_id, 'w+')
  configFile.write(configTemplate % {'region_id': region_id})
  configFile.close()

