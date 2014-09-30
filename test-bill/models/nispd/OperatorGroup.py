from classes.DbNispd import DbNispd

class OperatorGroup(DbNispd.orm()):
    __tablename__ = "operator_group"
    __table_args__ = {'schema': 'voip', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()

    def __init__(self, id = 1, name = 'Test operator group'):
         super(OperatorGroup, self).__init__(
            id = id,
            name = name,
        )

