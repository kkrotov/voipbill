from classes.DbNispd import DbNispd

class Price(DbNispd.orm()):
    __tablename__ = "defs"
    __table_args__ = {'schema': 'billing', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()

    def __init__(self, ndef, pricelist_id, price,
                 date_from = None, date_to = None, deleted = None):
        if deleted is None:
            deleted = False

        if date_from is None:
            date_from = '2014-01-01'

        if date_to is None:
            date_to = '4000-01-01'

        super(Price, self).__init__(
            ndef = ndef,
            pricelist_id = pricelist_id,
            price = price,
            date_from = date_from,
            date_to = date_to,
            deleted= deleted
        )

