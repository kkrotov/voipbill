#!/bin/python
# -*- coding: utf-8 -*-

def migrate(centralDb, regionalDb, regionsList) :

  regConnections = [(regionalDb(reg), int(reg)) for reg in regionsList]

  # Мигрируем региональные БД
  for (regConn, region_id) in regConnections :
    curReg = regConn.cursor()
    curReg.execute('''
	ALTER TABLE billing.clients ADD COLUMN credit_term integer NOT NULL DEFAULT -1;
    ''')
    regConn.commit()
    regConn.close()


  # Центральный сервер
  conn = centralDb()
  # Мигрируем центральную БД
  cur = conn.cursor()
  cur.execute('''
	ALTER TABLE billing.clients ADD COLUMN credit_term integer NOT NULL DEFAULT -1;

  ''')
  conn.commit()
  conn.close()