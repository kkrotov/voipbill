ALTER TABLE calls_cdr.cdr_unfinished ADD COLUMN src_number character varying(32);
ALTER TABLE calls_cdr.cdr_unfinished ADD COLUMN dst_number character varying(32);
ALTER TABLE calls_cdr.cdr_unfinished ADD COLUMN src_route character varying(32);


-- Function: calls_cdr.tr_partitioning_cdr_unfinished()

-- DROP FUNCTION calls_cdr.tr_partitioning_cdr_unfinished();

CREATE OR REPLACE FUNCTION calls_cdr.tr_partitioning_cdr_unfinished()
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
	
	relname := 'calls_cdr.cdr_unfinished_' || suffix;
	--raise notice '%', relname;
	EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;
	
	IF rel_exists IS NULL OR rel_exists = ''
	THEN
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.setup_time) || ' );' INTO this_mon;
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.setup_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;
			
		EXECUTE 'CREATE TABLE calls_cdr.cdr_unfinished_' || suffix || 
			' (CONSTRAINT cdr_unfinished_' || suffix || '_unique UNIQUE (hash, dst_route),' ||
			' CONSTRAINT cdr_unfinished_' || suffix || '_time_check CHECK (setup_time >= ' || 
			quote_literal(this_mon) || '::timestamp without time zone AND setup_time < ' || quote_literal(next_mon) || '::timestamp without time zone)' || 
			') INHERITS (calls_cdr.cdr_unfinished) WITH (OIDS=FALSE)';

		EXECUTE format('CREATE INDEX cdr_unfinished_' || suffix || '_hash ON calls_cdr.cdr_unfinished_' || suffix || ' USING btree (hash);');

		EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO postgres';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_bill_daemon_local';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_readonly';
	END IF;
	
	EXECUTE format('insert into calls_cdr.cdr_unfinished_' || to_char( new.setup_time, 'YYYYMM') || 
			' (call_id,setup_time,hash,dst_route,releasing_party,release_timestamp,disconnect_cause,src_number,dst_number,src_route) values ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10)')
		USING new.call_id, new.setup_time, new.hash, new.dst_route, new.releasing_party, new.release_timestamp, new.disconnect_cause, new.src_number, new.dst_number, new.src_route;
	
	return null;
end;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION calls_cdr.tr_partitioning_cdr_unfinished()
  OWNER TO postgres;


-- Function: insert_cdr_unfinished(bigint, character varying, character varying, character varying, character varying, character varying, character varying, character varying, smallint)

-- DROP FUNCTION insert_cdr_unfinished(bigint, character varying, character varying, character varying, character varying, character varying, character varying, character varying, smallint);

CREATE OR REPLACE FUNCTION insert_cdr_unfinished(
    p_call_id bigint,
    setup_time character varying,
    src_number character varying,
    dst_number character varying,
    src_route character varying,
    dst_route character varying,
    releasing_party character varying,
    release_timestamp character varying,
    disconnect_cause smallint)
  RETURNS void AS
$BODY$
        declare 
            p_hash uuid;
            p_setup_timestamp timestamp;
            p_release_timestamp timestamp;
        begin

	    if setup_time = '' or setup_time is null then 
                p_setup_timestamp = null;
            else
                select into p_setup_timestamp to_timestamp(setup_time, 'YYYY-MM-DD HH24:MI:SS.US')::timestamp without time zone; 
            end if;

            if release_timestamp = '' or release_timestamp is null then
                p_release_timestamp = null;
            else
                select into p_release_timestamp to_timestamp(release_timestamp, 'YYYY-MM-DD HH24:MI:SS.US')::timestamp without time zone; 
            end if;
            
            p_hash = calls_cdr.get_hash(p_call_id, setup_time::timestamp);
   
            insert into calls_cdr.cdr_unfinished (
                call_id, setup_time, hash, dst_route, releasing_party, release_timestamp, disconnect_cause, src_number, dst_number, src_route
            ) values (
                p_call_id, p_setup_timestamp, p_hash, dst_route, releasing_party, p_release_timestamp, disconnect_cause, src_number, dst_number, src_route
            );

            delete from calls_cdr.start where call_id = p_call_id;
    
        end;
        $BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION insert_cdr_unfinished(bigint, character varying, character varying, character varying, character varying, character varying, character varying, character varying, smallint)
  OWNER TO postgres;

