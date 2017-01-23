CREATE OR REPLACE FUNCTION calls_cdr.add_unfinished_check () RETURNS void AS $$
DECLARE
  tables record;
  year integer;
  month varchar;
  next_month varchar;
  next_year integer;
  count integer;
BEGIN
  FOR tables IN EXECUTE 'SELECT
														table_name AS name
													FROM
														information_schema.tables
													WHERE
														table_schema=''calls_cdr''
															AND
														table_name LIKE ''cdr_unfinished_20%'''
  LOOP
    IF NOT EXISTS (SELECT
                     *
                   FROM
                     information_schema.table_constraints
                   WHERE
                     table_name = tables.name AND table_schema = 'calls_cdr'
                     AND
                     constraint_name = tables.name || '_setup_time_check') THEN

      year := substr(tables.name, 16, 4);
      month := substr(tables.name, 20, 2);
      next_month := month::int + 1;
      next_year = year;
      IF next_month::int = 13 THEN
        next_year := year + 1;
        next_month = 1;
      END IF;
      IF length((next_month)::varchar) = 1 THEN
        next_month := '0' || next_month;
      END IF;
      RAISE NOTICE '%', 'ALTER TABLE calls_cdr.cdr_unfinished_' || year || month || ' ADD CHECK (((setup_time >= ''' || year || '-' || month || '-01 00:00:00''::timestamp without time zone) AND (setup_time < ''' || next_year || '-' || next_month || '-01 00:00:00''::timestamp without time zone)))';
      EXECUTE 'ALTER TABLE calls_cdr.cdr_unfinished_' || year || month || ' ADD CHECK (((setup_time >= ''' || year || '-' || month || '-01 00:00:00''::timestamp without time zone) AND (setup_time < ''' || next_year || '-' || next_month || '-01 00:00:00''::timestamp without time zone)))';
    ELSE
      RAISE NOTICE 'setup_time check is already exist in table %', tables.name;
    END IF;
  END LOOP;
END;
$$
LANGUAGE 'plpgsql';