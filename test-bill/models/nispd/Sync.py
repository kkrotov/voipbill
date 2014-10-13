from classes.DbNispd import DbNispd

class Sync(DbNispd.orm()):
    __tablename__ = "sync"
    __table_args__ = {'schema': 'billing', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()
