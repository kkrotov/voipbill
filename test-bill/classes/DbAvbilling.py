from classes.Db import Db
from config.config import cfg

class DbAvbilling(Db):

    def getDbName(self):
        return cfg.dbname_avbilling

    @staticmethod
    def instance():
        if (DbAvbilling.__instance is None):
            DbAvbilling.__instance = DbAvbilling()
        return DbAvbilling.__instance

    @staticmethod
    def connection():
        return DbAvbilling.instance().getConnection()

    @staticmethod
    def orm():
        return DbAvbilling.instance().getOrm()


    __instance = None
    __ORM = None
