#!/bin/python
# -*- coding: utf-8 -*-

def migrate(centralDb, regionalDb, regionsList) :

  # Региональные сервера
  regConnections = [(regionalDb(reg), int(reg)) for reg in regionsList]

  # Мигрируем региональные БД
  for (regConn, region_id) in regConnections :
    curReg = regConn.cursor()

    curReg.execute('''

      ALTER TABLE billing.service_trunk_settings ADD COLUMN minimum_margin_type SMALLINT DEFAULT 0;
      ALTER TABLE billing.service_trunk_settings ADD COLUMN minimum_margin NUMERIC(10,5) DEFAULT 0;

    ''')

    regConn.commit()
    regConn.close()


  # Центральный сервер
  conn = centralDb()

  # Мигрируем центральную БД
  cur = conn.cursor()
  cur.execute('''

      ALTER TABLE billing.service_trunk_settings ADD COLUMN minimum_margin_type SMALLINT DEFAULT 0;
      ALTER TABLE billing.service_trunk_settings ADD COLUMN minimum_margin NUMERIC(10,5) DEFAULT 0;
      
      REPLACE RULE service_trunk_settings_rl_duplicate AS
        ON INSERT TO billing.service_trunk_settings
        WHERE (EXISTS (SELECT x.id FROM billing.service_trunk_settings x WHERE x.id = new.id))
        DO INSTEAD
          UPDATE billing.service_trunk_settings
            SET trunk_id = new.trunk_id, type = new.type, "order" = new."order",
                src_number_id = new.src_number_id, dst_number_id = new.dst_number_id,
                pricelist_id = new.pricelist_id, minimum_minutes = new.minimum_minutes, minimum_cost = new.minimum_cost,
                minimum_margin_type = new.minimum_margin_type, minimum_margin = new.minimum_margin
            WHERE service_trunk_settings.id = new.id;

  ''')
  conn.commit()
  conn.close()


