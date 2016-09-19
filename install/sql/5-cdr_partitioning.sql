-- Function: calls_cdr.tr_partitioning()

-- DROP FUNCTION calls_cdr.tr_partitioning();

CREATE OR REPLACE FUNCTION calls_cdr.tr_partitioning()
  RETURNS trigger AS
$BODY$
declare
	relname varchar;
	rel_exists text;
	suffix varchar;
	this_mon timestamp;
	next_mon timestamp;
begin
	suffix := to_char(new.setup_time, 'YYYYMM');
	--raise notice '%', suffix;
	relname := 'calls_cdr.cdr_' || suffix;
	--raise notice '%', relname;
	EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;
	
	IF rel_exists IS NULL OR rel_exists = ''
	THEN
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.setup_time) || ' );' INTO this_mon;
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.setup_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;
			
		EXECUTE 'CREATE TABLE calls_cdr.cdr_' || suffix || 
			' (CONSTRAINT cdr_' || suffix || '_pkey PRIMARY KEY (id),' ||
			' CONSTRAINT cdr_' || suffix || '_setup_time_check CHECK (' || 
			'setup_time >= ' || quote_literal(this_mon) || '::timestamp without time zone AND ' || 
			'setup_time < ' || quote_literal(next_mon) || '::timestamp without time zone)' || 
			') INHERITS (calls_cdr.cdr) WITH (OIDS=FALSE)';

		EXECUTE format('CREATE INDEX cdr_' || suffix || '_hash ON calls_cdr.cdr_' || suffix || ' USING btree (hash);');

		EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO postgres';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_bill_daemon_local';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_readonly';
	END IF;

	--EXECUTE format('INSERT INTO ' || relname || ' SELECT ($1).*') USING NEW;
	EXECUTE format('insert into calls_cdr.cdr_' || to_char( new.setup_time, 'YYYYMM') || 
		'(id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,dst_route,' || 
		'src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party) ' ||
		'values ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18,$19)')
		USING new.id,new.call_id, new.nas_ip,new.src_number,new.dst_number,new.redirect_number,new.setup_time,new.connect_time,new.disconnect_time,new.session_time,
			new.disconnect_cause,new.src_route,new.dst_route,new.src_noa,new.dst_noa,new.hash,new.dst_replace,new.call_finished,new.releasing_party;
	
	return null;        
end;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION calls_cdr.tr_partitioning()
  OWNER TO postgres;

