#!/bin/python
# -*- coding: utf-8 -*-

def migrate(centralDb, regionalDb, regionsList) :

  # Региональные сервера
  regConnections = [(regionalDb(reg), int(reg)) for reg in regionsList]

  # Мигрируем региональные БД
  for (regConn, region_id) in regConnections :
    curReg = regConn.cursor()

    curReg.execute('''

      CREATE TABLE billing.currency_rate
      (
        id integer NOT NULL,
        date DATE NOT NULL,
        currency character(3) NOT NULL,
        rate numeric(10,4) NOT NULL,
        CONSTRAINT currency_rate_pkey PRIMARY KEY (id)
      )
      WITH (
        OIDS=FALSE
      );

      GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE billing.currency_rate TO g_bill_daemon_local;
      GRANT SELECT ON TABLE billing.currency_rate TO g_readonly;

      CREATE TRIGGER notify
        AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
        ON billing.currency_rate
        FOR EACH STATEMENT
        EXECUTE PROCEDURE event.tr_notify();

    ''')

    regConn.commit()
    regConn.close()


  # Центральный сервер
  conn = centralDb()

  # Мигрируем центральную БД
  cur = conn.cursor()
  cur.execute('''

    CREATE TABLE billing.currency_rate
    (
      id integer NOT NULL,
      date DATE NOT NULL,
      currency character(3) NOT NULL,
      rate numeric(10,4) NOT NULL,
      CONSTRAINT currency_rate_pkey PRIMARY KEY (id)
    )
    WITH (
      OIDS=FALSE
    );

    GRANT SELECT ON TABLE billing.currency_rate TO g_bill_daemon_remote;
    GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE billing.currency_rate TO g_stat;
    GRANT SELECT ON TABLE billing.currency_rate TO g_readonly;


    CREATE OR REPLACE FUNCTION event.tr_notify_currency_rate()
      RETURNS trigger AS
    $BODY$
    BEGIN
      IF TG_OP = 'DELETE' THEN
        PERFORM event.notify('currency_rate',old.id);
      ELSE
        PERFORM event.notify('currency_rate',new.id);    
      END IF;
      return null;
    END;
    $BODY$
      LANGUAGE plpgsql VOLATILE
      COST 100;


    CREATE TRIGGER notify
      AFTER INSERT OR UPDATE OR DELETE
      ON billing.currency_rate
      FOR EACH ROW
      EXECUTE PROCEDURE event.tr_notify_currency_rate();


    SELECT event.notify('currency_rate',0);
  ''')
  conn.commit()
  conn.close()


