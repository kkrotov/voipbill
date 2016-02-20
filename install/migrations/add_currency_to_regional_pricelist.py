#!/bin/python
# -*- coding: utf-8 -*-

def migrate(centralDb, regionalDb, regionsList) :

  # Региональные сервера
  regConnections = [(regionalDb(reg), int(reg)) for reg in regionsList]

  # Мигрируем региональные БД
  for (regConn, region_id) in regConnections :
    curReg = regConn.cursor()

    curReg.execute('''

      ALTER TABLE billing.pricelist ADD COLUMN currency_id CHARACTER(3);

    ''')

    regConn.commit()
    regConn.close()


  # Центральный сервер
  conn = centralDb()

  # Мигрируем центральную БД
  cur = conn.cursor()
  cur.execute('''

    -- Говорим регионам, что прайслисты поменялись, чтобы они забрали поле currency_id
    SELECT event.notify('pricelist',0);

  ''')
  conn.commit()
  conn.close()
