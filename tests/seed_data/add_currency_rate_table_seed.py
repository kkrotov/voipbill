#!/bin/python
# -*- coding: utf-8 -*-

def migrate(centralDb, regionalDb, regionsList) :

  # Центральный сервер
  conn = centralDb()

  # Мигрируем центральную БД
  cur = conn.cursor()
  cur.execute('''

    INSERT INTO billing.currency_rate (id, date, currency, rate) VALUES
      (1, '2016-02-25', 'USD', 76.3928),
      (2, '2016-02-25', 'EUR', 84.0779),
      (3, '2016-02-25', 'HUF', 27.2369),
      (4, '2016-02-24', 'USD', 77.1326),
      (5, '2016-02-24', 'EUR', 85.8563),
      (6, '2016-02-24', 'HUF', 27.8216);

  ''')
  conn.commit()
  conn.close()

