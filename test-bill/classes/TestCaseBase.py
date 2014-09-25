# -*- coding: utf-8 -*-

import unittest

from classes.model import *
from classes.openca import *
from classes.app_bill import *
from datetime import datetime

class TestConfiguration:
    pass

'''
TestConfiguration = namedtuple("TestConfiguration", "cleanTables instances geoprefixes pricelists operatorGroups\
 localNetworks operators prices clients usages tarifs usageTarifs")
'''

'''
class TestCall:
    def __init__(self, abonent, other, time=60, dir_out=True):
        pass
    def expectResult(self, len, op_len, amount, amount_op, free_mins):
        pass
'''

class TestCaseBase(unittest.TestCase):
    def basicConfig(self):
        '''
        Test Fixture:
            Определяем базовую конфигурацию: задаём параметры для всех объектов (в упрощённом автоматизируемом виде).
        Test:
            1. Определяем тестируемые компоненты и исходные параметры для них.
            2. Запускаем, ожидаем, читаем/загружаем/получаем события.
            3. Проверяем, что полученные/прочитанные/загруженные результаты соответствуют ожидаемым.
        '''
        self.testConfiguration = TestConfiguration()
        self.testConfiguration.cleanTables = [
                TableCleaner(connection=avbilling, tables=[
                    'public.radacct_voip_start',
                    'public.radacct_voip_stop']),
                TableCleaner(connection=nispd99, tables=[
                    'calls.calls',
                    'billing.clients',
                    'billing.defs',
                    'billing.events',
                    'billing.tarif',
                    'billing.tarif_change_log',
                    'billing.usage',
                    'billing.network_prefix',
                    'billing.operator',
                    'billing.pricelist',
                    'geo.prefix',
                    'billing.instance_settings']),
                TableCleaner(connection=nispd, tables=[
                    'calls.calls',
                    'billing.sync',
                    'billing.tasks',
                    'billing.defs',
                    'billing.network_prefix',
                    'billing.clients',
                    'billing.tarif',
                    'billing.tarif_change_log',
                    'billing.usage',
                    'geo.prefix',
                    'voip.operator',
                    'voip.operator_group',
                    'voip.pricelist',
                    'billing.instance_settings'])
            ]

        self.testConfiguration.instances=[Instance(id=99, region_id=[7495, 7495496], city_prefix=[7495, 74951234567],
                                                   active=True, city_geo_id=7495)]

        self.testConfiguration.geoprefixes=[GeoPrefix(prefix=74951, geo_id=7495, mob=False, dest=1, region=7495, operator_id=5000012)]

        self.testConfiguration.pricelists=[]

        # Pricelist(id=1, region=99, operator_id=2, name='Местные Билайн операторский',
        #                      tariffication_by_minutes=False, tariffication_full_first_minute=False,
        #                      type='operator', initiate_mgmn_cost=0.71, initiate_zona_cost=0.12)]

        self.testConfiguration.operatorGroups=[OperatorGroup(id=1, name='prime')]

        self.testConfiguration.localNetworks=[]  # LocalNetwork(prefix='7495123', instance_id=99, operator_id=2, network_type_id=102,
                           #         date_from=datetime.now(), deleted=False, date_to='4000-01-01')],

        self.testConfiguration.operators=[]  #Operator(region=99, id=2, term_in_cost=0.23, name='Beeline',
                       #pricelist_id=1, local_network_pricelist_id=1,
                       #client_7800_pricelist_id=None, operator_7800_pricelist_id=None)],

        self.testConfiguration.prices=[]  #Price(ndef=7495100, pricelist_id=1, price=0.1555,
                    #  date_from=datetime.now(), deleted=False, date_to='4000-01-01'),

                # Price(ndef=102, pricelist_id=1, price=2.5000,
                #      date_from=datetime.now(), deleted=False, date_to='4000-01-01')],

        self.testConfiguration.clients=[]  # Client(id=1, voip_limit_month=5000, voip_limit_day=1000, voip_disabled=False, credit=500,
                    #  balance=1.50, amount_date=None, last_payed_month=None)],

        self.testConfiguration.usages=[]  # Usage(region=99, client_id=1, id=1, phone_num='74951005555', no_of_lines=1, actual_from='2014-01-01',
                    #   actual_to='4000-01-01')],

        self.testConfiguration.tarifs=[]  # Tarif(id=1, name='Tarif1', currency_id=1, status='public', freemin=1000, freemin_for_number=True,
                    #  pricelist_id=1, paid_redirect=True, tariffication_by_minutes=False,
                    #  tariffication_full_first_minute=False, tariffication_free_first_seconds=True)],

        self.testConfiguration.usageTarifs=[]  # UsageTarif(id=1, usage_id=1, tarif_id_local=1, tarif_id_local_mob=1, tarif_id_russia=1,
                        #        tarif_id_russia_mob=1, tarif_id_intern=1, tarif_id_sng=1,
                        #        date_activation=datetime.now(), ts=fixTime(datetime.now()))]


        '''
        self.abonentMskAlone = 74951111111
        self.abonentSpbAlone = 78121111111
        self.abonentMskAlone800 = 78001111111
        self.abonentSpbAlone800 = 78002222222
        self.abonentMskWithSpb = 74952222222
        self.abonentSpbWithMsk = 78122222222
        self.abonentMsk800WithSpb = 78003333333
        self.abonentSpb800WithMsk = 78004444444

        # Присоединённые операторы:
        # Клиент только в Москве
        self.abonentConnectedMskAlone = 74953333333
        # У клиента есть другой номер в Спб.
        self.abonentConnectedMskWithSpb = 74954444444
        # Клиент только в Спб.
        self.abonentConnectedSpbAlone = 78123333333
        # У клиента есть другой номер в Москве
        self.abonentConnectedSpbWithMsk = 78124444444
        '''

        self.bill = None
        self.openCA = None

    def cleanTables(self):
        cleanTables(self.testConfiguration.cleanTables)

    def insertRecords(self, db, recordsList):
        for record in recordsList:
            db.add(record)
        db.commit()

    def fixSyncUpdate(self):
        connection = nispd.bind.raw_connection()
        try:
            cursor = connection.cursor()
            cursor.execute("""SELECT billing.mark_sync('geo_prefix',0);
                       SELECT billing.mark_sync('network_prefix',0);
                       SELECT billing.mark_sync('defs',0);""")
            cursor.close()
            connection.commit()
        finally:
            connection.close()

    def insertCommonTestData(self):
        for records in [self.testConfiguration.instances,
                        self.testConfiguration.geoprefixes,
                        self.testConfiguration.pricelists,
                        self.testConfiguration.operatorGroups,
                        self.testConfiguration.localNetworks,
                        self.testConfiguration.operators,
                        self.testConfiguration.prices,
                        self.testConfiguration.clients,
                        self.testConfiguration.usages,
                        self.testConfiguration.tarifs,
                        self.testConfiguration.usageTarifs]:
            self.insertRecords(nispd, records)

        # Признаки sync выставляются при парсинге данных, а не вставке записей. Пока не будет сделано иначе,
        # придётся вручную выставлять обновления:
        self.fixSyncUpdate()

        self.insertRecords(avbilling, self.finishedCalls)

    def matchCall(self, expectedCdr):
        calls = nispd.query(CalculatedCall).filter_by(id=expectedCdr.id).all()
        calculatedCall = calls[0]

        for column in CalculatedCall.__table__.columns:
            expected = getattr(expectedCdr, column.name, None)
            calculated = getattr(calculatedCall, column.name, None)
            self.assertEqual(expected, calculated, "calls." + column.name
                             + " should be '" + str(expected) + "', but calculated as '" + str(calculated) + "'")

    def verifyCalls(self):
        for expectedCall in self.expectedCalls:
            self.matchCall(expectedCall)

    def startOpenCA(self):
        self.openCA = OpenCARealtimeEmulator()

    def startBillingApp(self):
        self.bill = AppBill()

    def stopOpenCA(self):
        if self.openCA: self.openCA.stop()

    def stopBillingApp(self):
        if self.bill: self.bill.stop()

    def configure(self, configuration):
        pass

    def setUp(self):
        self.basicConfig()

        self.configure(self.testConfiguration)

        self.cleanTables()
        self.insertCommonTestData()

        self.startOpenCA()
        self.startBillingApp()

    def tearDown(self):
        self.stopBillingApp()
        self.stopOpenCA()

        self.testConfiguration = None
        self.finishedCalls = None
        self.expectedCalls = None

    '''
    def forCall(self, abonent, other, operator, time=60, dir_out=True):

        return TestCall(abonent, other, time, dir_out)
    '''

    def operatorPrice(self, region, operator, prefix, pricelist, price,
                      by_minutes, full_first, tollFree=None, networkType=None):

        if networkType:
            self.testConfiguration.localNetworks.append(
                LocalNetwork(prefix=str(prefix), instance_id=region, operator_id=operator, network_type_id=networkType,
                             date_from=datetime.now(), deleted=False, date_to='4000-01-01'))
            self.testConfiguration.prices.append(Price(ndef=networkType, pricelist_id=pricelist, price=price,
                  date_from=datetime.now(), deleted=False, date_to='4000-01-01'))
        else:
            self.testConfiguration.prices.append(Price(ndef=prefix, pricelist_id=pricelist, price=price,
                  date_from=datetime.now(), deleted=False, date_to='4000-01-01'))

        for i, pl in enumerate(self.testConfiguration.pricelists):
            if pl.id == pricelist and pl.region == region:
                self.testConfiguration.pricelists[i].tariffication_by_minutes = by_minutes
                self.testConfiguration.pricelists[i].tariffication_full_first_minute = full_first

        for k, op in enumerate(self.testConfiguration.operators):
            if op.id == operator and op.region == region:
                if tollFree:
                    self.testConfiguration.operators[k].operator_7800_pricelist_id = pricelist
                else:
                    if networkType:
                        self.testConfiguration.operators[k].local_network_pricelist_id = pricelist
                    else:
                        self.testConfiguration.operators[k].pricelist_id = pricelist

