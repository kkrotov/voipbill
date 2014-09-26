# -*- coding: utf-8 -*-

class Config:
    def __init__(self, **kwds):
        self.__dict__.update(kwds)

    def merge(self, otherCfg):
        self.__dict__.update(otherCfg.__dict__)
        return self

def makeConnectionString(dbName, user, password):
    connectionString = "host='localhost'"
    if dbName:
        connectionString += " dbname='" + dbName + "'"
    connectionString += " user='" + user + "' password='" + password + "'"
    return connectionString

def makeAdminConnectionString(dbName):
    return makeConnectionString(dbName, baseCfg.dbmanageruser, baseCfg.dbmanagerpassword)