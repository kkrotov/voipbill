# -*- coding: utf-8 -*-

import os
import sys

sys.path.insert(0, os.path.dirname(__file__) + r"/../../test-bill")

from sqlalchemy import *
from sqlalchemy.ext.declarative import declarative_base

from sqldb import *
from config.config import cfg


nispd = sqlConnection(cfg.dbmanageruser, cfg.dbmanagerpassword, cfg.dbname_nispd)
nispd99 = sqlConnection(cfg.dbmanageruser, cfg.dbmanagerpassword, cfg.dbname_nispd99)
avbilling = sqlConnection(cfg.dbmanageruser, cfg.dbmanagerpassword, cfg.dbname_avbilling)

NispdTable = declarative_base(bind=nispd.bind)
AvbillingTable = declarative_base(bind=avbilling.bind)


class FinishedCall(AvbillingTable):
    """ Завершившийся звонок.
    Основное назначение - тестирование тарификатора завершившимися звонками.
    Также может использоваться для тестирования настоящих или эмулированных OpenCA и RADIUS.
    """
    __tablename__ = "radacct_voip_stop"
    __table_args__ = {'schema': 'public', 'autoload': True}


class CalculatedCall(NispdTable):
    __tablename__ = "calls"
    __table_args__ = {'schema': 'calls', 'autoload': True}

class Instance(NispdTable):
    __tablename__ = "instance_settings"
    __table_args__ = {'schema': 'billing', 'autoload': True}

class GeoPrefix(NispdTable):
    __tablename__ = "prefix"
    __table_args__ = {'schema': 'geo', 'autoload': True}

class Pricelist(NispdTable):
    __tablename__ = "pricelist"
    __table_args__ = {'schema': 'voip', 'autoload': True}

class LocalNetwork(NispdTable):
    __tablename__ = "network_prefix"
    __table_args__ = {'schema': 'billing', 'autoload': True}

class OperatorGroup(NispdTable):
    __tablename__ = "operator_group"
    __table_args__ = {'schema': 'voip', 'autoload': True}

class Operator(NispdTable):
    __tablename__ = "operator"
    __table_args__ = {'schema': 'voip', 'autoload': True}

class Price(NispdTable):
    __tablename__ = "defs"
    __table_args__ = {'schema': 'billing', 'autoload': True}

class Client(NispdTable):
    __tablename__ = "clients"
    __table_args__ = (PrimaryKeyConstraint('id'), {'schema': 'billing', 'autoload': True})

class Usage(NispdTable):
    __tablename__ = "usage"
    __table_args__ = {'schema': 'billing', 'autoload': True}

class ClientCounter(NispdTable):
    __tablename__ = "clients_counters"
    __table_args__ = {'schema': 'billing', 'autoload': True}

class Tarif(NispdTable):
    __tablename__ = "tarif"
    __table_args__ = {'schema': 'billing', 'autoload': True}

class UsageTarif(NispdTable):
    __tablename__ = "tarif_change_log"
    __table_args__ = {'schema': 'billing', 'autoload': True}
