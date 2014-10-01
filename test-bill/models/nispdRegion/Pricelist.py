from classes.DbNispdRegion import DbNispdRegion

class Pricelist(DbNispdRegion.orm()):
    __tablename__ = "pricelist"
    __table_args__ = {'schema': 'voip', 'autoload': True}

    @staticmethod
    def connection(): return DbNispdRegion.connection()

    def __init__(self, id, type, region = 99, operator_id = 2, name = 'Test pricelist'):
        super(Pricelist, self).__init__(
            id = id,
            type = type,
            region = region,
            operator_id = operator_id,
            name = name,
            initiate_mgmn_cost = 0,
            initiate_zona_cost = 0
        )

