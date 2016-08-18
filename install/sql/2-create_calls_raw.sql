-- Function: create_calls_raw(timestamp without time zone)

-- DROP FUNCTION create_calls_raw(timestamp without time zone);

CREATE OR REPLACE FUNCTION create_calls_raw(connect_time timestamp without time zone)
  RETURNS void AS
$BODY$
declare
	relname varchar;
	rel_exists text;
	suffix varchar;
	this_mon timestamp;
	next_mon timestamp;
begin
	suffix := to_char(connect_time, 'YYYYMM');
	--raise notice '%', suffix;
	relname := 'calls_raw.calls_raw_' || suffix;
	--raise notice '%', relname;
	EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;
	
	IF rel_exists IS NULL OR rel_exists = ''
	THEN
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(connect_time) || ' );' INTO this_mon;
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(connect_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;
			
		EXECUTE 'CREATE TABLE calls_raw.calls_raw_' || suffix || 
			' (CONSTRAINT calls_raw_' || suffix || '_pkey PRIMARY KEY (id),' ||
			' CONSTRAINT calls_raw_' || suffix || '_connect_time_check CHECK (' || 
			'connect_time >= ' || quote_literal(this_mon) || '::timestamp without time zone AND ' || 
			'connect_time < ' || quote_literal(next_mon) || '::timestamp without time zone)' || 
			') INHERITS (calls_raw.calls_raw) WITH (OIDS=FALSE)';

		EXECUTE format('CREATE INDEX calls_raw_' || suffix || '_cdr_id ON calls_raw.calls_raw_' || suffix || ' USING btree (cdr_id)');
		EXECUTE 'CREATE INDEX calls_raw_' || suffix || '_connect_time ON calls_raw.calls_raw_' || suffix || ' USING btree (connect_time)';

		EXECUTE 'ALTER TABLE ' || relname || ' OWNER TO postgres';
		EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO postgres';
		EXECUTE 'GRANT SELECT, INSERT, DELETE ON TABLE ' || relname || ' TO g_bill_daemon_local';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_readonly';
	END IF;

end;
$BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION create_calls_raw(timestamp without time zone)
  OWNER TO postgres;

