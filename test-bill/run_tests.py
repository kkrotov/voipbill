#!/usr/bin/python
# -*- coding: utf-8 -*-

SKIP_RECREATE_DB = False
SKIP_DROP_DB_AFTER_TEST = False

from db_setup import *
import sys
import glob
import unittest

def runTests(skipRecreateDb, skipDropDbAfterTest):
    test_file_strings = glob.glob('test_*.py')
    module_strings = [str[0:len(str)-3] for str in test_file_strings]
    suites = [unittest.defaultTestLoader.loadTestsFromName(str) for str
              in module_strings]
    testSuite = unittest.TestSuite(suites)

    if not skipRecreateDb:
        recreateDb()

    text_runner = unittest.TextTestRunner(verbosity=2).run(testSuite)

    if not skipDropDbAfterTest:
        dropTestDatabases()

if __name__ == "__main__":
    skipRecreateDb = SKIP_RECREATE_DB
    skipDropDbAfterTest = SKIP_DROP_DB_AFTER_TEST

    for arg in sys.argv:
        if arg == '-create':
            skipDropDbAfterTest = True
        if arg == '-use':
            skipRecreateDb = True
            skipDropDbAfterTest = True

    runTests(skipRecreateDb, skipDropDbAfterTest)
