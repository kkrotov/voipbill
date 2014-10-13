from classes.DbNispd import DbNispd

class LocalNetwork(DbNispd.orm()):
    __tablename__ = "network_prefix"
    __table_args__ = {'schema': 'billing', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()


    def __init__(self, network_type_id, prefix, instance_id, operator_id,
                 date_from = None, date_to = None, deleted = None):
        if deleted is None:
            deleted = False

        if date_from is None:
            date_from = '2014-01-01'

        if date_to is None:
            date_to = '4000-01-01'

        super(LocalNetwork, self).__init__(
            network_type_id = network_type_id,
            prefix = prefix,
            instance_id = instance_id,
            operator_id = operator_id,
            date_from = date_from,
            date_to = date_to,
            deleted= deleted
        )

