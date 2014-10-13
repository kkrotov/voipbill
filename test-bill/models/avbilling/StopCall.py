from classes.DbAvbilling import DbAvbilling

class StopCall(DbAvbilling.orm()):
    __tablename__ = "radacct_voip_stop"
    __table_args__ = {'schema': 'public', 'autoload': True}

    @staticmethod
    def connection(): return DbAvbilling.connection()


