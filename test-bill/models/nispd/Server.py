from classes.DbNispd import DbNispd

class Server(DbNispd.orm()):
    __tablename__ = "instance_settings"
    __table_args__ = {'schema': 'billing', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()

    def __init__(self, id, region_id, city_prefix, city_geo_id, active = True):
        super(Server, self).__init__(
            id = id,
            region_id = region_id,
            city_prefix = city_prefix,
            city_geo_id = city_geo_id,
            active = active
        )