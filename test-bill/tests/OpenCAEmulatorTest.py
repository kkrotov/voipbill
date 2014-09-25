#!/usr/bin/python
# -*- coding: utf-8 -*-

import unittest
import sys, os
sys.path.insert(0, os.path.dirname(__file__) + r"/../../test-bill")

from classes.sqldb import *
import datetime
from classes.openca import *
from classes.model import avbilling
from datetime import datetime

class OpenCAEmulatorTest(unittest.TestCase):
    def setUp(self):
        cleanTables([TableCleaner(connection=avbilling, tables=[
                    'public.radacct_voip_start',
                    'public.radacct_voip_stop'])])

        self.openCA = OpenCARealtimeEmulator()


    def testOpenCAGeneratesRecords(self):
        callrecords = self.openCA.generateCall(datetime.now(), 15, '74951005555', '74951005555', 99, 99)

    def tearDown(self):
        self.openCA.stop()

if __name__ == "__main__":
    unittest.main()

