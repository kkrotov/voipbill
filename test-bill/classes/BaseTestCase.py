# -*- coding: utf-8 -*-
import unittest
from classes.sqldb import *
from classes.DbAvbilling import *
from classes.DbNispd import *
from classes.DbNispdRegion import *
from models.nispd.Call import *
from models.nispd.Server import *
from models.nispd.GeoPrefix import *
from models.nispdRegion.GeoPrefix import *
from models.nispd.LocalNetwork import *
from models.nispd.Price import *
from classes.OpenCAEmulator import *
from classes.AppBill import *
from datetime import datetime

class BaseTestCase(unittest.TestCase):
    bill = None
    openCA = None

    def setUp(self):
        self.before()
#        self.startOpenCA()
#        self.startBillingApp()

    def tearDown(self):
        pass
#        self.stopBillingApp()
#        self.stopOpenCA()

    def startBillingApp(self):
        self.bill = AppBill()
        self.bill.start()


    def stopBillingApp(self):
        if self.bill:
            self.bill.stop()

    def waitBillingApp(self, timeout=300):
        if self.bill:
            self.bill.join(timeout)

    def execBillingApp(self, threads):
        cfg.appbill_threads = threads
        self.startBillingApp()
        self.waitBillingApp()

    def startOpenCA(self):
        self.openCA = OpenCAEmulator()
        self.openCA.start()


    def stopOpenCA(self):
        if self.openCA:
            self.openCA.stop()

    def before(self):
        pass


    def cleanup(self, modelClasses):
        for modelClass in modelClasses:
            model = modelClass()
            model.connection().execute('DELETE FROM ' + str(model.__table__))
            model.connection().commit()


    def fixture(self, fixtureList):
        tablesToCleanup = []

        touchedTables = {}
        for item in fixtureList:
            table = str(item.__table__)
            if not (table in touchedTables):
                touchedTables[table] = True
                tablesToCleanup.insert(0, (table, item.connection(),))

        for (table, connection) in tablesToCleanup:
            connection.execute('DELETE FROM ' + table + ' CASCADE')
            connection.commit()

        for item in fixtureList:
            connection = item.connection()
            connection.add(item)
            connection.commit()



    def verifyCalls(self, expectedCalls):
        for expectedCall in expectedCalls:
            self.verifyCall(expectedCall)

    def verifyCall(self, expectedCdr):
        calls = DbNispd.connection().query().filter_by(id=expectedCdr.id).all()

        if (len(calls) == 0):
            self.fail("call with id " + str(expectedCdr.id) + " not found")

        calculatedCall = calls[0]

        for column in Call.__table__.columns:
            expected = getattr(expectedCdr, column.name, None)
            calculated = getattr(calculatedCall, column.name, None)
            self.assertEqual(expected, calculated, "calls." + column.name
                             + " should be '" + str(expected) + "', but calculated as '" + str(calculated) + "'")
