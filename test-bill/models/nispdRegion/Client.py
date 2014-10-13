from classes.DbNispdRegion import DbNispdRegion
from sqlalchemy import *

class Client(DbNispdRegion.orm()):
    __tablename__ = "clients"
    __table_args__ = (PrimaryKeyConstraint('id'), {'schema': 'billing', 'autoload': True})

    @staticmethod
    def connection(): return DbNispdRegion.connection()

    def __init__(self, id, voip_limit_month = -1, voip_limit_day= -1, voip_disabled = False, credit = -1, balance = 0, amount_date = None, last_payed_month = None):
        super(Client, self).__init__(
            id = id,
            voip_limit_month = voip_limit_month,
            voip_limit_day = voip_limit_day,
            voip_disabled = voip_disabled,
            credit = credit,
            balance = balance,
            amount_date = amount_date,
            last_payed_month = last_payed_month
        )
