-- Function: calls_raw.tr_partitioning()

-- DROP FUNCTION calls_raw.tr_partitioning();

-- автоматическое создание таблиц call_raw_YYYYMM в момент появления первой записи с необходимым connect_time
CREATE OR REPLACE FUNCTION calls_raw.tr_partitioning()
  RETURNS trigger AS
$BODY$
declare
	relname varchar;
	rel_exists text;
	suffix varchar;
	this_mon timestamp;
	next_mon timestamp;
begin
	suffix := to_char(new.connect_time, 'YYYYMM');
	--raise notice '%', suffix;
	relname := 'calls_raw.calls_raw_' || suffix;
	--raise notice '%', relname;
	EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;
	
	IF rel_exists IS NULL OR rel_exists = ''
	THEN
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.connect_time) || ' );' INTO this_mon;
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.connect_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;
			
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

        EXECUTE format('INSERT INTO ' || relname || ' SELECT ($1).*') USING NEW;
	return null;        
end;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION calls_raw.tr_partitioning()
  OWNER TO postgres;

