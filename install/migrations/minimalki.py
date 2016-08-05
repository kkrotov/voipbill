#!/bin/python
# -*- coding: utf-8 -*-

def migrate(centralDb, regionalDb, regionsList) :

  # Региональные сервера
  regConnections = [(regionalDb(reg), int(reg)) for reg in regionsList]

  # Мигрируем региональные БД
  for (regConn, region_id) in regConnections :
    curReg = regConn.cursor()

    curReg.execute('''

CREATE TABLE billing.stats_trunk_settings (
id INTEGER NOT NULL,
trunk_settings_id INTEGER NOT NULL,
activation_dt TIMESTAMP(0) WITHOUT TIME ZONE NOT NULL,
expire_dt TIMESTAMP(0) WITHOUT TIME ZONE NOT NULL,
used_seconds INTEGER NOT NULL,
used_credit DOUBLE PRECISION NOT NULL,
min_call_id BIGINT NOT NULL,
max_call_id BIGINT NOT NULL,
CONSTRAINT stats_trunk_settings_pkey PRIMARY KEY(id)
)
WITH (oids = false);

CREATE INDEX idx_stats_trunk_settings__max_call_id ON billing.stats_trunk_settings
USING btree (max_call_id);

CREATE RULE stats_trunk_settings_rl AS ON INSERT TO billing.stats_trunk_settings
WHERE (EXISTS ( SELECT x.id
FROM billing.stats_trunk_settings x
WHERE x.id = new.id))
DO INSTEAD (
UPDATE billing.stats_trunk_settings SET trunk_settings_id = new.trunk_settings_id, activation_dt = new.activation_dt, expire_dt = new.expire_dt, used_seconds = new.used_seconds, used_credit = new.used_credit, min_call_id = new.min_call_id, max_call_id = new.max_call_id
WHERE stats_trunk_settings.id = new.id;
);

ALTER TABLE calls_raw.calls_raw
  ADD COLUMN trunk_settings_stats_id INTEGER;

GRANT SELECT,INSERT,UPDATE,DELETE ON billing.stats_trunk_settings TO g_bill_daemon_local;
GRANT SELECT ON billing.stats_trunk_settings TO g_readonly;

    ''')

    regConn.commit()
    regConn.close()


  # Центральный сервер
  conn = centralDb()

  # Мигрируем центральную БД
  cur = conn.cursor()
  cur.execute('''

CREATE TABLE billing.stats_trunk_settings (
id INTEGER NOT NULL,
trunk_settings_id INTEGER NOT NULL,
activation_dt TIMESTAMP(0) WITHOUT TIME ZONE NOT NULL,
expire_dt TIMESTAMP(0) WITHOUT TIME ZONE NOT NULL,
used_seconds INTEGER NOT NULL,
used_credit DOUBLE PRECISION NOT NULL,
min_call_id BIGINT NOT NULL,
max_call_id BIGINT NOT NULL,
CONSTRAINT stats_trunk_settings_pkey PRIMARY KEY(id)
)
WITH (oids = false);

CREATE INDEX idx_stats_trunk_settings__max_call_id ON billing.stats_trunk_settings
USING btree (max_call_id);

CREATE RULE stats_trunk_settings_rl AS ON INSERT TO billing.stats_trunk_settings
WHERE (EXISTS ( SELECT x.id
FROM billing.stats_trunk_settings x
WHERE x.id = new.id))
DO INSTEAD (
UPDATE billing.stats_trunk_settings SET trunk_settings_id = new.trunk_settings_id, activation_dt = new.activation_dt, expire_dt = new.expire_dt, used_seconds = new.used_seconds, used_credit = new.used_credit, min_call_id = new.min_call_id, max_call_id = new.max_call_id
WHERE stats_trunk_settings.id = new.id;
);

ALTER TABLE calls_raw.calls_raw
  ADD COLUMN trunk_settings_stats_id INTEGER;

GRANT SELECT,INSERT,UPDATE,DELETE ON billing.stats_trunk_settings TO g_bill_daemon_remote;
GRANT SELECT ON billing.stats_trunk_settings TO g_readonly;
GRANT SELECT ON billing.stats_trunk_settings TO g_stat;

  ''')
  conn.commit()
  conn.close()

