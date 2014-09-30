from classes.Db import Db
from config.config import cfg

class DbNispd(Db):

    def getDbName(self):
        return cfg.dbname_nispd

    @staticmethod
    def instance():
        if (DbNispd.__instance is None):
            DbNispd.__instance = DbNispd()
        return DbNispd.__instance

    @staticmethod
    def connection():
        return DbNispd.instance().getConnection()

    @staticmethod
    def orm():
        return DbNispd.instance().getOrm()


    __instance = None
