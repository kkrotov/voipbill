from classes.DbNispd import DbNispd

class Usage(DbNispd.orm()):
    __tablename__ = "usage"
    __table_args__ = {'schema': 'billing', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()

    def __init__(self, id, client_id, phone_num, region = 99, no_of_lines = 1, actual_from = '2014-01-01', actual_to='4000-01-01'):
        super(Usage, self).__init__(
            id = id,
            client_id = client_id,
            region = region,
            phone_num = phone_num,
            no_of_lines = no_of_lines,
            actual_from = actual_from,
            actual_to = actual_to
        )
