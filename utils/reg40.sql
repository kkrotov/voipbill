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
	relname := 'calls_raw.calls_raw_' || suffix;
	EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;
	
	IF rel_exists IS NULL OR rel_exists = ''
	THEN
		EXECUTE 'select calls_raw.create_calls_raw_partition(' || quote_literal(new.connect_time) || '::timestamp without time zone)';
	END IF;

        EXECUTE format('INSERT INTO ' || relname || ' SELECT ($1).*') USING NEW;
	return null;        
end
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION calls_raw.tr_partitioning()
  OWNER TO postgres;
