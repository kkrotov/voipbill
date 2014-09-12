#!/usr/bin/python


from db_setup import *
import sys
import glob
import unittest

def runTests():
    test_file_strings = glob.glob('test_*.py')
    module_strings = [str[0:len(str)-3] for str in test_file_strings]
    suites = [unittest.defaultTestLoader.loadTestsFromName(str) for str  in module_strings]
    testSuite = unittest.TestSuite(suites)

    text_runner = unittest.TextTestRunner(verbosity=2).run(testSuite)



if __name__ == "__main__":

    for arg in sys.argv:
        if arg == 'recreatedb':
            recreateTestDatabases()
            exit(0)

    runTests()
