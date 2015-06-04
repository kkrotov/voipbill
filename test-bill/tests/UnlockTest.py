# -*- coding: utf-8 -*-
import time

from classes.BaseTestCase import *
from classes.Radius import Radius
from models.nispd.Server import *
from models.nispd.OperatorGroup import *
from models.nispd.Operator import *
from models.nispd.GeoPrefix import *
from models.nispd.Client import *
from models.nispd.Pricelist import *
from models.nispd.Tariff import *
from models.nispd.Usage import *
from models.nispd.UsageTariff import *
from models.nispd.Sync import *
from models.nispd.Call import Call as NispdCall
from models.nispdRegion.Call import Call as NispdRegionCall
from models.avbilling.StopCall import *
from models.avbilling.StartCall import *

class UnlockTest(BaseTestCase):

    def before(self):
        self.cleanup([
            StartCall,
            StopCall,
            NispdCall,
            NispdRegionCall,
            Sync
        ])

        self.fixture([
            Server(id = 99, region_id = [7495, 7495496], city_prefix = [7495, 74951234567], city_geo_id = 7495),
            Server(id = 97, region_id = [7861], city_prefix = [78612, 786194], city_geo_id = 78612),

            GeoPrefix(prefix = 74951, geo_id = 7495, mob = False, dest = 1, region = 7495, operator_id = 5000012),

            OperatorGroup(),

            Operator(id = 2, region = 99),
            Operator(id = 4, region = 99),
            Operator(id = 6, region = 97),
            Operator(id = 8, region = 97),

            Pricelist(id = 2, type = 'operator', operator_id = 2, region = 99),
            Pricelist(id = 4, type = 'operator', operator_id = 4, region = 99),
            Pricelist(id = 6, type = 'operator', operator_id = 6, region = 97),
            Pricelist(id = 8, type = 'operator', operator_id = 8, region = 97),

            LocalNetwork(network_type_id = 101, prefix='7495', instance_id = 99, operator_id=2),
            Price(ndef = 101, pricelist_id = 2, price = 0.1555),

            Price(ndef = 79, pricelist_id = 2, price=0.1555),


            Client(id = 10, balance = 10),
            Client(id = 11, balance = -10, credit = 20),
            Client(id = 12, balance = -10, credit = -1),
            Client(id = 13, balance = 10, voip_disabled = True),
            Client(id = 14, balance = 1000, voip_limit_day = 100),
            Client(id = 15, balance = 1000, voip_limit_month = 100),

            Pricelist(id = 10, type = 'client', operator_id = 999, region = 99),

            Price(ndef=7495123, pricelist_id = 10, price=5.00),

            Tariff(id = 10, pricelist_id = 11),

            Usage(region = 99, client_id = 10, id = 10, phone_num = '74951005555'),
            Usage(region = 99, client_id = 11, id = 11, phone_num = '74952006666'),

            UsageTariff(id = 10, usage_id = 10, tariff_id = 10),
            UsageTariff(id = 11, usage_id = 11, tariff_id = 10),
        ])

    def testUnlockUnused(self):
        """ Переход из действующего usage'а в недействующий (тот, который не загружается в память приложения),
        должен приводить к удалению лишних номеров из чёрного списка.

        Переход должен осуществляться во время работы приложения.
        Услуга должна быть заблокирована - поле disabled/disabled_global должно стоять до запуска приложения.

        Эмулятор должен получить команду блокировки - это момент, когда допустимо начинать тест.

        Тест: поле usage.actual_to = now(). Эмулятор OpenCA должен получить команды разблокировки.
        (текущая реализация синхронизирует списки с OpenCA один раз в сутки).

        Для корректного теста нужна бредовая управляющая последовательность:
        1. Запустить loader
        2. Запустить blacklist.prepare и .run
        3. Поменять дату actual_to.
        4. Запустить blacklist.run
        5. После остановки blacklist.run остановить loader, выгрузиться.
        6. Запись должна исчезнуть
        """

        self.startOpenCA()

        assert(OpenCA.readBlacklistGlobal() == '')

        self.execBillingApp('sync, loader, runtime, blacklist')
        assert(OpenCA.readBlacklistGlobal() == '74952006666,74951005555')

        conn = Usage.connection()
        conn.query(Usage).filter_by(id=10).update({"actual_to": '2014-02-02'})
        conn.commit()

        self.execBillingApp('sync, loader, runtime, blacklist:1:m')
        assert(OpenCA.readBlacklistGlobal() == '74952006666')
        self.stopOpenCA()




if __name__ == "__main__":
    sys.path.insert(0, os.path.dirname(os.path.dirname(__file__)))
    unittest.main()
