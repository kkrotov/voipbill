from classes.Db import Db
from config.config import cfg

class DbNispdRegion(Db):

    def getDbName(self):
        return cfg.dbname_nispd99

    @staticmethod
    def instance():
        if (DbNispdRegion.__instance is None):
            DbNispdRegion.__instance = DbNispdRegion()
        return DbNispdRegion.__instance

    @staticmethod
    def connection():
        return DbNispdRegion.instance().getConnection()

    @staticmethod
    def orm():
        return DbNispdRegion.instance().getOrm()


    __instance = None
    __ORM = None
