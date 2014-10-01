from classes.DbNispdRegion import DbNispdRegion

class Tariff(DbNispdRegion.orm()):
    __tablename__ = "tarif"
    __table_args__ = {'schema': 'billing', 'autoload': True}

    @staticmethod
    def connection(): return DbNispdRegion.connection()

    def __init__(self, id, pricelist_id, freemin = 0, freemin_for_number = True, paid_redirect = True,
                 tariffication_by_minutes = False, tariffication_full_first_minute = False,
                 tariffication_free_first_seconds = True
    ):
        super(Tariff, self).__init__(
            id = id,
            pricelist_id = pricelist_id,
            freemin = freemin,
            freemin_for_number = freemin_for_number,
            paid_redirect = paid_redirect,
            tariffication_by_minutes = tariffication_by_minutes,
            tariffication_full_first_minute = tariffication_full_first_minute,
            tariffication_free_first_seconds = tariffication_free_first_seconds,
            name = 'Test tariff',
            currency_id = 1,
            status = 'public'
        )