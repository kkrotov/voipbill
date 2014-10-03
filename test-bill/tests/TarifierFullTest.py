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

class TarifierFullTest(BaseTestCase):

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

            UsageTariff(id = 10, usage_id = 10, tariff_id = 10),
        ])


    def testCallMatch(self):
        Radius.stopCall(caller = '74951005555', callee = '74951234567', inOper = 99, outOper = 2, duration = 15 )

        cfg.appbill_threads = 'loader, runtime'
        self.startBillingApp()
        time.sleep(5)
        self.stopBillingApp()




if __name__ == "__main__":
    sys.path.insert(0, os.path.dirname(os.path.dirname(__file__)))
    unittest.main()
