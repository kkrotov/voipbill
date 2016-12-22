CREATE OR REPLACE FUNCTION calls_raw.add_raw_indexes (index_fields varchar[]) RETURNS void AS $$
DECLARE
	tables record;
	date varchar;
	count integer;
	index_field varchar;
	year varchar;
BEGIN
	year := to_char(now(), 'YYYY');

	FOREACH index_field IN ARRAY index_fields
	LOOP
		FOR tables IN EXECUTE 'SELECT
															table_name AS name
														FROM
															information_schema.tables
														WHERE
															table_schema=''calls_raw''
																AND
															table_name LIKE ''calls_raw_' || year || '%'''
		LOOP
			IF NOT EXISTS (SELECT
											 *
										 FROM
											 pg_indexes
										 WHERE
											 tablename = tables.name
											 AND
											 schemaname = 'calls_raw'
											 AND
											 indexname = tables.name || '_' || index_field || '_idx') THEN

				date := substr(tables.name, 11, 6);
				RAISE NOTICE '%', 'CREATE INDEX ON calls_raw.calls_raw_' || date || ' USING btree (' || index_field || ')';
				EXECUTE 'CREATE INDEX ON calls_raw.calls_raw_' || date || ' USING btree (' || index_field || ')';
			ELSE
				RAISE NOTICE '% index is already exist in table %', index_field, tables.name;
			END IF;
		END LOOP;
	END LOOP;
END;
$$
LANGUAGE 'plpgsql';

--select calls_raw.add_raw_indexes('{nnp_operator_id, nnp_region_id, nnp_country_prefix, src_number, dst_number}'::varchar[]);