import unittest, sys, os
from classes.OpenCAEmulator import OpenCAEmulator

class OpenCAEmulatorTest(unittest.TestCase):

    def setUp(self):
        self.openCA = OpenCAEmulator()
        self.openCA.start()


    def testSleep(self):
        pass


    def tearDown(self):
        self.openCA.stop()


if __name__ == "__main__":
    sys.path.insert(0, os.path.dirname(os.path.dirname(__file__)))
    unittest.main()

