from classes.DbNispdRegion import DbNispdRegion
from datetime import datetime
from classes.sqldb import fixTime

class UsageTariff(DbNispdRegion.orm()):
    __tablename__ = "tarif_change_log"
    __table_args__ = {'schema': 'billing', 'autoload': True}

    @staticmethod
    def connection(): return DbNispdRegion.connection()

    def __init__(self, id, usage_id,
                 tarif_id_local, tarif_id_local_mob, tarif_id_russia,
                 tarif_id_russia_mob, tarif_id_intern, tarif_id_sng,
                 date_activation = None,
                 ts = None
    ):
        if date_activation is None:
            date_activation = datetime.now()

        if ts is None:
            ts = fixTime(date_activation)

        super(UsageTariff, self).__init__(
            id = id,
            usage_id = usage_id,
            tarif_id_local = tarif_id_local,
            tarif_id_local_mob  = tarif_id_local_mob,
            tarif_id_russia = tarif_id_russia,
            tarif_id_russia_mob = tarif_id_russia_mob,
            tarif_id_intern = tarif_id_intern,
            tarif_id_sng = tarif_id_sng,
            date_activation = date_activation,
            ts = ts
        )