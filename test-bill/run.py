#!/usr/bin/python
import sys, os, glob, unittest
from classes.DbNispd import DbNispd

def runTests():

    if len(sys.argv) > 1:
        testFiles = glob.glob(sys.argv[1])
    else:
        testFiles = glob.glob('tests/*Test.py')

    testClassNames = map(
        lambda r: 'tests.' + os.path.splitext(os.path.basename(r))[0],
        testFiles
    )

    testSuites = unittest.defaultTestLoader.loadTestsFromNames(testClassNames)

    unittest.TextTestRunner(verbosity=2).run(testSuites)


if __name__ == "__main__":
    sys.path.insert(0, os.path.dirname(__file__))
    runTests()
