CREATE OR REPLACE FUNCTION calls_cdr.add_unfinished_indexes (index_fields varchar[]) RETURNS void AS $$
DECLARE
  tables record;
  date varchar;
  count integer;
  index_field varchar;
  year varchar;
BEGIN
  year := to_char(now() - INTERVAL '1 year', 'YYYY');

  RAISE NOTICE '%', year;

  FOREACH index_field IN ARRAY index_fields
  LOOP
    FOR tables IN EXECUTE 'SELECT
															table_name AS name
														FROM
															information_schema.tables
														WHERE
															table_schema=''calls_cdr''
																AND
															table_name LIKE ''cdr_unfinished_' || year || '%'''
    LOOP
      IF NOT EXISTS (SELECT
                       *
                     FROM
                       pg_indexes
                     WHERE
                       tablename = tables.name
                       AND
                       schemaname = 'calls_cdr'
                       AND
                       indexname = tables.name || '_' || index_field || '_idx') THEN

        date := substr(tables.name, 16, 6);
        RAISE NOTICE '%', 'CREATE INDEX ON calls_cdr.cdr_unfinished_' || date || ' USING btree (' || index_field || ')';
        EXECUTE 'CREATE INDEX ON calls_cdr.cdr_unfinished_' || date || ' USING btree (' || index_field || ')';
      ELSE
        RAISE NOTICE '% index is already exist in table %', index_field, tables.name;
      END IF;
    END LOOP;
  END LOOP;
END;
$$
LANGUAGE 'plpgsql';

--select calls_cdr.add_unfinished_indexes('{setup_time, dst_route, disconnect_cause, src_number, dst_number, src_route}'::varchar[])