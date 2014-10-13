from classes.DbNispd import DbNispd

class Call(DbNispd.orm()):
    __tablename__ = "calls"
    __table_args__ = {'schema': 'calls', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()
