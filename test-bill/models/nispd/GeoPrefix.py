from classes.DbNispd import DbNispd

class GeoPrefix(DbNispd.orm()):
    __tablename__ = "prefix"
    __table_args__ = {'schema': 'geo', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()

    def __init__(self, prefix, geo_id = 0, mob = False, dest = 7, region = 0, operator_id = 0):
        super(GeoPrefix, self).__init__(
            prefix = prefix,
            geo_id = geo_id,
            mob = mob,
            dest = dest,
            region = region,
            operator_id = operator_id
        )
