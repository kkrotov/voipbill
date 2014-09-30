from classes.DbNispdRegion import DbNispdRegion

class Call(DbNispdRegion.orm()):
    __tablename__ = "calls"
    __table_args__ = {'schema': 'calls', 'autoload': True}

    @staticmethod
    def connection(): return DbNispdRegion.connection()
