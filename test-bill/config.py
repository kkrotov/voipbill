#!/usr/bin/python
# -*- coding: utf-8 -*-

baseCfg = {
    'instance_id': 99,
    'dbname_avbilling': 'avbilling_test',
    'dbname_nispd': 'nispd_test',
    'dbname_nispd99': 'nispd99_test',

    'dbmanageruser': 'postgres',
    'dbmanagerpassword': 'Gomoku2000',

    'avbillinguser': 'radius',
    'avbillingpassword': 'Gomoku2000',

    'statuser': 'stat',
    'statpassword': 'Gomoku2000',

    'bill_daemon_local_username': 'bill_daemon_local',
    'bill_daemon_local_password': 'Gomoku2000',

    'bill_daemon_remote_username': 'bill_daemon_remote',
    'bill_daemon_remote_password': 'Gomoku2000',
}

substCfg = {
    'dbmanager_conn_str': "host='localhost' dbname='postgres' user='" + baseCfg['dbmanageruser'] + "' password='" + baseCfg['dbmanagerpassword'] + "'",
    'dbadmin_conn_str': "host='localhost' user='" + baseCfg['dbmanageruser'] + "' password='" + baseCfg['dbmanagerpassword'] + "'",

    'avbilling_conn_str': "host='localhost' dbname='" + baseCfg['dbname_avbilling'] + "' user='" + baseCfg['avbillinguser'] + "' password='" + baseCfg['avbillingpassword'] + "'",
    'nispd_conn_str': "host='localhost' dbname='" + baseCfg['dbname_nispd'] + "' user='" + baseCfg['statuser'] + "' password='" + baseCfg['statpassword'] + "'",
    'nispd99_conn_str': "host='localhost' dbname='" + baseCfg['dbname_nispd99'] + "' user='" + baseCfg['bill_daemon_local_username'] + "' password='" + baseCfg['bill_daemon_local_password'] + "'",

    'openca_defaultConnectTime': 5,
    'openca_nasipaddress': '85.94.32.233',
    'openca_operatorRouteNamePrefix': 'TestOperator_%02d',
    'openca_udpPort': 58888,
    'openca_lastSessionId': 0,

    'billing_app_path': '/home/avesus/dev/billing_voip/app_bill/dist/Debug/GNU-Linux-x86/app_bill',
    'billing_app_config_template': '/home/avesus/dev/billing_voip/test-bill/app_bill_template.conf',
    'billing_app_config': '/home/avesus/dev/billing_voip/test-bill/app_bill.conf',
}

# merge baseCfg and substCfg
cfg = dict(baseCfg.items() + substCfg.items())

