#!/usr/bin/python


from tests import *
import sys
import os
import glob
import unittest

def runTests():

    sys.path.insert(0, os.path.dirname(__file__) + r"/tests")
    sys.path.insert(0, os.path.dirname(__file__) + r"/classes")

    suites = unittest.defaultTestLoader.loadTestsFromNames(
        map(lambda r: os.path.splitext(os.path.basename(r))[0], glob.glob('tests/*Test.py')))

    text_runner = unittest.TextTestRunner(verbosity=2).run(suites)



if __name__ == "__main__":
    runTests()
