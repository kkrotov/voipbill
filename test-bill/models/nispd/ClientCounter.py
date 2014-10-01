from classes.DbNispd import DbNispd

class ClientCounter(DbNispd.orm()):
    __tablename__ = "clients_counters"
    __table_args__ = {'schema': 'billing', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()
