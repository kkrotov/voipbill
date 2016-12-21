CREATE OR REPLACE FUNCTION calls_cdr.add_cdr_check () RETURNS void AS $$
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
														table_name LIKE ''cdr_20%'''
	 LOOP
			 EXECUTE 'SELECT 
								  * 
								FROM 
									information_schema.table_constraints 
								WHERE 
									table_name = ''' || tables.name || ''' AND table_schema = ''calls_cdr'' 
										AND 
									constraint_name = ''' || tables.name || '_connect_time_check''';

			 GET DIAGNOSTICS count = ROW_COUNT;
			 RAISE NOTICE 'count: %', count;
			 IF count = 0 THEN
				 year := substr(tables.name, 5, 4);
				 month := substr(tables.name, 9, 2);
				 next_month := month::int + 1;
				 next_year = year;
				 IF next_month::int = 13 THEN
						 next_year := year + 1;
						 next_month = 1;
				 END IF;
				 IF length((next_month)::varchar) = 1 THEN
						 next_month := '0' || next_month;
				 END IF;
				 RAISE NOTICE '%', 'ALTER TABLE calls_cdr.cdr_' || year || month || ' ADD CHECK (((connect_time >= ''' || year || '-' || month || '-01 00:00:00''::timestamp without time zone) AND (connect_time < ''' || next_year || '-' || next_month || '-01 00:00:00''::timestamp without time zone)))';
				 EXECUTE 'ALTER TABLE calls_cdr.cdr_' || year || month || ' ADD CHECK (((connect_time >= ''' || year || '-' || month || '-01 00:00:00''::timestamp without time zone) AND (connect_time < ''' || next_year || '-' || next_month || '-01 00:00:00''::timestamp without time zone)))';
				ELSE
					RAISE NOTICE 'connect_time check is already exist in table %', tables.name;
				END IF;
	 END LOOP;
END;
$$
LANGUAGE 'plpgsql';