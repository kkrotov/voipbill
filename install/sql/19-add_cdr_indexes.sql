CREATE OR REPLACE FUNCTION calls_cdr.add_cdr_indexes (index_fields varchar[]) RETURNS void AS $$
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
															table_schema=''calls_cdr'' 
																AND 
															table_name LIKE ''cdr_' || year || '%'''
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

						 date := substr(tables.name, 5, 6);
						 RAISE NOTICE '%', 'CREATE INDEX ON calls_cdr.cdr_' || date || ' USING btree (' || index_field || ')';
						 EXECUTE 'CREATE INDEX ON calls_cdr.cdr_' || date || ' USING btree (' || index_field || ')';
					ELSE
						 RAISE NOTICE '% index is already exist in table %', index_field, tables.name;
					END IF;
		 END LOOP;
	END LOOP;
END;
$$
LANGUAGE 'plpgsql';