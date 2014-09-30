from sqlalchemy import *
from sqlalchemy.orm import sessionmaker
from sqlalchemy.ext.declarative import declarative_base
from config.config import cfg

class Db:
    __user = cfg.dbmanageruser
    __password = cfg.dbmanagerpassword
    __connection = None
    __orm = None

    def getDbName(self):
        return ''

    def getConnection(self):
        if self.__connection is None:
            engine = create_engine(
                            'postgresql+psycopg2://' + self.__user + ':' + self.__password + '@localhost/' + self.getDbName(),
                            client_encoding = 'utf8',
                            echo = False
                    )
            SessionClass = sessionmaker(bind = engine)
            self.__connection = SessionClass()
        return self.__connection

    def getOrm(self):
        if self.__orm is None:
            self.__orm = declarative_base(bind = self.getConnection().bind)
        return self.__orm