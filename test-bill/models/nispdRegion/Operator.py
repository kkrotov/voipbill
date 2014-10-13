from classes.DbNispdRegion import DbNispdRegion

class Operator(DbNispdRegion.orm()):
    __tablename__ = "operator"
    __table_args__ = {'schema': 'voip', 'autoload': True}

    @staticmethod
    def connection(): return DbNispdRegion.connection()

    def __init__(self, id, name = 'Test operator', region = 99):
         super(Operator, self).__init__(
            region = region,
            id = id,
            name = name,
            term_in_cost = 0
        )

