CREATE OR REPLACE FUNCTION calls_raw.delete_old_partitions (year varchar) RETURNS void AS $$
DECLARE
	tables record;
BEGIN
  RAISE NOTICE '%', year;

  FOR tables IN EXECUTE 'SELECT
						   table_name AS name
						 FROM
						   information_schema.tables
						 WHERE
						   table_schema=''calls_raw''
							 AND
						   table_name LIKE ''calls_raw_' || year || '%'''
    LOOP
        EXECUTE 'DROP TABLE ' || tables.name;
        RAISE NOTICE 'Table % was deleted', tables.name;
    END LOOP;

END;
$$
LANGUAGE 'plpgsql';

-- select calls_raw.delete_old_partitions('2012')