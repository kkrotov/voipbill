from classes.DbNispd import DbNispd

class Operator(DbNispd.orm()):
    __tablename__ = "operator"
    __table_args__ = {'schema': 'voip', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()

    def __init__(self, id, name = 'Test operator', region = 99):
         super(Operator, self).__init__(
            region = region,
            id = id,
            name = name,
            term_in_cost = 0
        )

