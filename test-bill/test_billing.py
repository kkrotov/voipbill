#!/usr/bin/python
# -*- coding: utf-8 -*-

import unittest

from config import *
from sql_access import *
from model import *
from openca import *
from app_bill import *
from time import sleep

class CheckBillingAppWork(unittest.TestCase):
    def setUp(self):
        self.bill = None

        # insert instance settings to billing daemon work:
        self.nispd = NispdDatabase(cfg['dbmanager_conn_str'] + " dbname=" + cfg['dbname_nispd']) ## cfg['nispd_conn_str'])
        self.nispd.insertInstanceSettings() ## stat access denied

    def testRun(self):
        self.bill = AppBill()
        sleep(3)

    def tearDown(self):
        if self.bill: self.bill.stop()
        self.nispd.clear()

class CheckOpenCAEmulatorWork(unittest.TestCase):
    def setUp(self):
        self.openCA = None

    def testCreate(self):
        self.openCA = OpenCAEmulator()
        sleep(3)

    def tearDown(self):
        if self.openCA: self.openCA.stop()

class CheckBillingAppToOpenCABasicWork(unittest.TestCase):
    def setUp(self):
        self.openCA = None
        self.bill = None

        # insert instance settings to billing daemon work:
        self.nispd = NispdDatabase(cfg['dbmanager_conn_str'] + " dbname=" + cfg['dbname_nispd']) ## cfg['nispd_conn_str'])
        self.nispd.insertInstanceSettings() ## stat access denied

    def testBillingAppQueriesBlacklists(self):
        self.openCA = OpenCAEmulator()
        self.bill = AppBill()

        sleep(15)

        assert self.openCA.localBlacklistQueried
        assert self.openCA.globalBlacklistQueried

    def tearDown(self):
        if self.bill: self.bill.stop()
        if self.openCA: self.openCA.stop()

        self.nispd.clear()

'''
Тесты расчёта стоимости звонка:

Виды расположений абонентов относительно точки биллинга (13):
- Наши абоненты:
* Наш местный абонент (обычный номер)
* Наш местный абонент (800-й номер)
* Наш местный абонент (услуга без номера, короткий номер)
* Наш абонент из другого региона (обычный номер)
* Наш абонент из другого региона (800-й номер)
* Наш абонент из другого региона (услуга без номера, короткий номер)

- Присоединённые операторы:
* Абонент местного присоединённого оператора (его номера мы не проверяем)
* Абонент присоединённого в другом регионе оператора (его номера мы не проверяем)

- Прочие:
* Внешний оператор местный регион
* Внешний оператор внутризоновый абонент
* Внешний оператор междугородка по России
* Внешний оператор СНГ
* Внешний оператор международный номер

Исключительные ситуации (5):
* Всё ок
* Для наших абонентов: пара номер-регион не найдена в БД
* Для наших абонентов: для пары номер-регион истёк срок действия
* Поле "оператор" абонента отсутствует в наших записях:
    ** По классу "внешний оператор"
    ** По классу нашего региона не надо, т.к. все забиты
    ** По классу присоединённого оператора

Режимы (2):
* Обычный звонок
* С включённой прослушкой СОРМом

Направления (2):
* Входящий звонок
* Исходящий звонок

Видов звонков у нас 13х5 x 13х5 x 2

'''

class OperatorPriceLocalCase(unittest.TestCase):
    def setUp(self):
        self.openCA = None
        self.bill = None

        self.nispd = NispdDatabase(cfg['dbmanager_conn_str'] + " dbname=" + cfg['dbname_nispd']) ## cfg['nispd_conn_str'])

        self.nispd.insertInstanceSettings() ## stat access denied
        self.nispd.insertVoipOperatorGroup()

        self.nispd.insertGeoPrefix(prefix='74951', areaCodePrefix=7495, mob=False, dest=1, geo_id=7495, geo_operator_id=5000012)

        pricelist_id = self.nispd.insertVoipPricelist(99, 2, 'Местные Билайн операторский', False, False, 'operator', 0.71, 0.12)

        self.nispd.insertVoipOperator(99, 2, 0.23, pricelist_id, pricelist_id, None, None)

        ## access from STAT PHP code with parsing a file in stat/modules/voipnew/network.php
        ## we write that at creation stage for a while:
        self.nispd.insertNetworkPrefix('7495100', 99, 2, 102) ## A/D

        # Local operator call price
        self.nispd.insertPrice('102', pricelist_id, 0.1555)

        self.nispd.insertPrice('74951005555', pricelist_id, 0.6667)

        # client objects:

        clt_pricelist_id = self.nispd.insertVoipPricelist(99, 2, 'Местные Билайн', False, False, 'client', 1.71, 1.12)
        self.nispd.insertPrice('74951005555', clt_pricelist_id, 1.6667) ## A/D

        self.nispd.insertTarif(tarif_id=1, freemin=1000, freemin_for_number=True, pricelist_id=clt_pricelist_id, 
                    paid_redirect=True, tariffication_by_minutes=False, tariffication_full_first_minute=False, 
                    tariffication_free_first_seconds=True)

        self.nispd.insertClient(client_id=1, voip_limit_month=5000, voip_limit_day=1000, voip_disabled=False, credit=500, balance=1.50, amount_date=None, last_payed_month=None)

        self.nispd.insertClientPhoneWithTarif(change_id=1, branchCityCode=99, client_id=1, usage_id=1, phone_num='79296091327', no_of_lines=1,
                tarif_id_local=1, tarif_id_local_mob=1, tarif_id_russia=1, tarif_id_russia_mob=1, tarif_id_intern=1, tarif_id_sng=1)

        self.openCA = OpenCAEmulator()
        self.bill = AppBill()

    def testB(self):
        #callid = openCA.emulateCallStart('79296091326', '79296091320', 2, 2, '', datetime.now())
        callid = self.openCA.emulateCallStart('79296091327', '74951005555', 99, 2, '', datetime.now())
        callrecords = self.openCA.emulateCallEnd(callid, 16, 10)

        #openCA.generateCall(datetime.now(), 15, '79296091328', '79296091320', 2, 2)

        # wait for calc to complete
        sleep(15)

        for callrecord in callrecords:
            record = self.nispd.getCall(callrecord)
            #print(record)

        #callid = openCA.emulateCallStart('79296091329', '79296091320', 2, 2, '', datetime.now())
        #openCA.emulateCallEnd(callid, 16, 10)

    def tearDown(self):
        if self.bill: self.bill.stop()
        if self.openCA: self.openCA.stop()
        self.nispd.clear()

if __name__ == "__main__":
    unittest.main()

