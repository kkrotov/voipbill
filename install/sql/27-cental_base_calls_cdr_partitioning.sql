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

		EXECUTE 'CREATE TABLE ' || relname || ' (LIKE calls_cdr.cdr INCLUDING ALL) INHERITS (calls_cdr.cdr) WITH (OIDS=FALSE)';
                EXECUTE 'ALTER TABLE ' || relname || ' ADD CONSTRAINT cdr_' || suffix || '_pkey PRIMARY KEY (id)';
                EXECUTE 'ALTER TABLE ' || relname || ' ADD CONSTRAINT cdr_' || suffix || '_connect_time_check CHECK (' || 
                        'connect_time >= ' || quote_literal(this_mon) || '::timestamp without time zone AND ' || 
                        'connect_time < ' || quote_literal(next_mon) || '::timestamp without time zone)';

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



CREATE OR REPLACE FUNCTION calls_cdr.create_calls_cdr_unfinished_partition(setup_time timestamp without time zone)
  RETURNS boolean AS
$BODY$
declare
	relname varchar;
	rel_exists text;
	suffix varchar;
	this_mon timestamp;
	next_mon timestamp;
begin
	suffix := to_char(setup_time, 'YYYYMM');
	--raise notice '%', suffix;
	
	relname := 'calls_cdr.cdr_unfinished_' || suffix;
	--raise notice '%', relname;
	EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;
	
	IF rel_exists IS NULL OR rel_exists = ''
	THEN
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(setup_time) || ' );' INTO this_mon;
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(setup_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;
		EXECUTE 'CREATE TABLE ' || relname || ' (LIKE calls_cdr.cdr_unfinished INCLUDING ALL) INHERITS (calls_cdr.cdr_unfinished) WITH (OIDS=FALSE)';
		EXECUTE 'ALTER TABLE ' || relname || ' ADD CONSTRAINT cdr_unfinished_' || suffix || '_setup_time_check CHECK (' ||
                        'setup_time >= ' || quote_literal(this_mon) || '::timestamp without time zone AND ' ||
                        'setup_time < ' || quote_literal(next_mon) || '::timestamp without time zone)';

		EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO postgres';
		EXECUTE 'GRANT SELECT, INSERT, DELETE ON TABLE ' || relname || ' TO g_bill_daemon_remote';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_readonly';
	END IF;
	
	return true;
end;
$BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION calls_cdr.create_calls_cdr_unfinished_partition(timestamp without time zone)
  OWNER TO postgres;
