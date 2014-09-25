# -*- coding: utf-8 -*-

import os
import sys

sys.path.insert(0, os.path.dirname(__file__) + r"/../../test-bill")

from password import *
from classes.config_class import *


baseCfg = Config(
    # Код региона сервера, используемого для тестов по умолчанию.
    instance_id = 99,

    # Настраиваемые имена БД.
    dbname_avbilling = 'avbilling_test',
    dbname_nispd = 'nispd_test',
    dbname_nispd99 = 'nispd99_test',

    # Административная учётка с полными правами. Используется для создания баз и манипуляции тестовыми данными.
    dbmanageruser = 'postgres',
    dbmanagerpassword = my_dbmanagerpassword,

    # Учётная запись для эмулятора OpenCA. Желательно не использовать административную учётку.
    avbillinguser = 'postgres',  # 'radius',
    avbillingpassword = my_dbmanagerpassword,  # my_avbillingpassword,

    # statuser = 'stat',
    # statpassword = my_statpassword,

    # Учётная запись, которую использует приложение биллинга для подключения к его локальной БД.
    bill_daemon_local_username = 'postgres',  # 'bill_daemon_local',
    bill_daemon_local_password = my_dbmanagerpassword,  # my_bill_daemon_local_password,

    # Учётная запись, которую использует приложение биллинга для подключения к центральной БД.
    bill_daemon_remote_username = 'postgres',  # 'bill_daemon_remote',
    bill_daemon_remote_password = my_dbmanagerpassword,  # my_bill_daemon_remote_password
    )

substCfg = Config(
    openca_defaultConnectTime = 5,
    openca_nasipaddress = '85.94.32.233',
    openca_operatorRouteNamePrefix = 'TestOperator_%02d',
    openca_udpPort = 58888,
    openca_lastSessionId = 0,

    billing_app_path = os.path.abspath(os.path.dirname(__file__) + '/../../app_bill/dist/Debug/GNU-Linux-x86/app_bill'),
    billing_app_config_template = os.path.abspath(os.path.dirname(__file__) + '/../config/app_bill_template.conf'),
    billing_app_config = os.path.abspath(os.path.dirname(__file__) + '/../runtime/app_bill.conf')
    )

# merge baseCfg and substCfg
cfg = baseCfg.merge(substCfg)

