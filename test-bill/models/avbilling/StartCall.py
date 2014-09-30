from classes.DbAvbilling import DbAvbilling

class StartCall(DbAvbilling.orm()):
    __tablename__ = "radacct_voip_start"
    __table_args__ = {'schema': 'public', 'autoload': True}

    @staticmethod
    def connection(): return DbAvbilling.connection()


