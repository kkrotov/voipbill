CREATE OR REPLACE FUNCTION calls_cdr.create_calls_cdr_partition(connect_time timestamp without time zone)
  RETURNS boolean AS
$BODY$
declare
	relname varchar;
	rel_exists text;
	suffix varchar;
	this_mon timestamp;
	next_mon timestamp;
BEGIN
	suffix := to_char(connect_time, 'YYYYMM');
	relname := 'calls_cdr.cdr_' || suffix;
	--raise notice '%', relname;
	EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;
	
	IF rel_exists IS NULL OR rel_exists = ''
	THEN
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(connect_time) || ' );' INTO this_mon;
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(connect_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;
			
		EXECUTE 'CREATE TABLE ' || relname || 
			' (CONSTRAINT cdr_' || suffix || '_pkey PRIMARY KEY (server_id,id)' ||
			') INHERITS (calls_cdr.cdr) WITH (OIDS=FALSE) TABLESPACE reg_cdr';

		EXECUTE format('CREATE INDEX cdr_' || suffix || '_hash ON calls_cdr.cdr_' || suffix || ' USING btree (hash);');

		EXECUTE 'ALTER TABLE ' || relname || ' OWNER TO postgres';
		EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO postgres';
		EXECUTE 'GRANT SELECT, INSERT, DELETE ON TABLE ' || relname || ' TO g_bill_daemon_local';
		EXECUTE 'GRANT SELECT, INSERT, DELETE ON TABLE ' || relname || ' TO g_bill_daemon_remote';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_readonly';
	END IF;
	return true;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION calls_cdr.create_calls_cdr_partition(timestamp without time zone)
  OWNER TO postgres;
