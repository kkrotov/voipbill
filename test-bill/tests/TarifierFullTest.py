#!/usr/bin/python
# -*- coding: utf-8 -*-

import unittest
import sys
import os
from time import sleep

sys.path.insert(0, os.path.dirname(__file__) + r"/../../test-bill")

from classes.TestCaseBase import *


class TarifierFullTest(TestCaseBase):
    def configure(self, configuration):

        configuration.operators = [Operator(region=99, id=2, term_in_cost=0.23, name='Beeline')]

        configuration.pricelists =\
            [Pricelist(id=1, region=99, operator_id=2, name='Местные Билайн операторский',
                       type='operator', initiate_mgmn_cost=0.71, initiate_zona_cost=0.12)]

        self.operatorPrice(region=99, operator=2, prefix=7495, pricelist=1, price=0.1555,
                           by_minutes=False, full_first=False, tollFree=False, networkType=101)

        self.operatorPrice(region=99, operator=2, prefix=79, pricelist=1, price=0.1555,
                           by_minutes=False, full_first=False)


        configuration.clients\
            = [Client(id=1, voip_limit_month=5000, voip_limit_day=1000, voip_disabled=False, credit=500, balance=1.50,
                      amount_date=None, last_payed_month=None)]

        configuration.usages\
            = [Usage(region=99, client_id=1, id=1, phone_num='74951005555', no_of_lines=1, actual_from='2014-01-01',
                     actual_to='4000-01-01')]

        configuration.prices.append(Price(ndef=7495123, pricelist_id=2, price=5.00,
                  date_from=datetime.now(), deleted=False, date_to='4000-01-01'))

        configuration.tarifs\
            = [Tarif(id=1, name='Tarif1', currency_id=1, status='public', freemin=1000, freemin_for_number=True,
                     pricelist_id=2, paid_redirect=True, tariffication_by_minutes=False,
                     tariffication_full_first_minute=False, tariffication_free_first_seconds=True)]

        configuration.usageTarifs\
            = [UsageTarif(id=1, usage_id=1, tarif_id_local=1, tarif_id_local_mob=1, tarif_id_russia=1,
                          tarif_id_russia_mob=1, tarif_id_intern=1, tarif_id_sng=1,
                          date_activation=datetime.now(), ts=fixTime(datetime.now()))]

        callLength = 15

        stdConnectTime = fixTime(datetime.now())
        stdSetupTime = stdConnectTime - timedelta(seconds=3)
        stdCallEndTime = stdConnectTime + timedelta(seconds=callLength)

        uniqueId = uuid.uuid4().hex[0:16]

        self.finishedCalls = [
            FinishedCall(radacctid=1L, acctsessionid=1L, acctuniqueid=uniqueId,
                         setuptime=stdSetupTime, connecttime=stdConnectTime,
                         disconnecttime=stdCallEndTime, acctsessiontime=callLength,
                         callingstationid='74951005555', calledstationid='74951234567',
                         nasipaddress=Inet('1.2.3.4'), acctdelaytime=0, disconnectcause=16, direction='out',
                         prefix_oper=3, prefix_des=3, in_oper=99, out_oper=2, incd_noa=0, incg_noa=0,
                         in_route='ABC_99', out_route='DEF_02', mysql_id=None, redirectnum='')
        ]

        self.expectedCalls = [
            CalculatedCall(id=1L, time=stdConnectTime, direction_out=True,
                           usage_num=74951005555L, phone_num=74951234567L,
                           len=callLength, usage_id=1, pricelist_mcn_id=2, operator_id=2, free_min_groups_id=1,
                           dest=-1, mob=False, month=stripTime(stdConnectTime), day=stripTime(stdConnectTime), amount=0,
                           amount_op=4, client_id=1, region=99, geo_id=7495, srv_region_id=99, pricelist_op_id=1,
                           price=50000, price_op=1555, len_mcn=callLength, prefix_mcn=7495123, prefix_op=101,
                           prefix_geo=74951L, len_op=callLength, geo_operator_id=5000012, redirect_num=None)
        ]

    def testCallMatch(self):
        sleep(5)

        self.verifyCalls()

if __name__ == "__main__":
    unittest.main()
