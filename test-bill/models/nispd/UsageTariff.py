from classes.DbNispd import DbNispd
from datetime import datetime
from classes.sqldb import fixTime

class UsageTariff(DbNispd.orm()):
    __tablename__ = "tarif_change_log"
    __table_args__ = {'schema': 'billing', 'autoload': True}

    @staticmethod
    def connection(): return DbNispd.connection()

    def __init__(self, id, usage_id,
                 tarif_id_local = None, tarif_id_local_mob = None, tarif_id_russia = None,
                 tarif_id_russia_mob = None, tarif_id_intern = None, tarif_id_sng = None,
                 date_activation = None,
                 ts = None,
                 tariff_id = None
    ):
        if date_activation is None:
            date_activation = datetime.now()

        if ts is None:
            ts = fixTime(date_activation)

        if tariff_id is not None:
            tarif_id_local = tariff_id
            tarif_id_local_mob = tariff_id
            tarif_id_russia = tariff_id
            tarif_id_russia_mob = tariff_id
            tarif_id_intern = tariff_id
            tarif_id_sng = tariff_id

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