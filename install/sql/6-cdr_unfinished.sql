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
        rec_exists boolean;
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

        EXECUTE 'SELECT EXISTS (SELECT id FROM ' || relname || ' WHERE hash=' || quote_literal(new.hash) || ')' INTO rec_exists;
        IF NOT rec_exists
        THEN

		EXECUTE format('insert into ' || relname || 
			'(id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,dst_route,' || 
			'src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party) ' ||
			'values ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18,$19)')
			USING new.id,new.call_id, new.nas_ip,new.src_number,new.dst_number,new.redirect_number,new.setup_time,new.connect_time,new.disconnect_time,new.session_time,
				new.disconnect_cause,new.src_route,new.dst_route,new.src_noa,new.dst_noa,new.hash,new.dst_replace,new.call_finished,new.releasing_party;
        ELSE

		EXECUTE format('update ' || relname || 
			' set nas_ip=$1,src_number=$2,dst_number=$3,redirect_number=$4,setup_time=$5,connect_time=$6,disconnect_time=$7,session_time=$8,disconnect_cause=$9,' ||
                        'src_route=$10,dst_route=$11,src_noa=$12,dst_noa=$13,dst_replace=$14,call_finished=$15,releasing_party=$16 where hash=$17')
			USING new.nas_ip,new.src_number,new.dst_number,new.redirect_number,new.setup_time,new.connect_time,new.disconnect_time,new.session_time,
				new.disconnect_cause,new.src_route,new.dst_route,new.src_noa,new.dst_noa,new.dst_replace,new.call_finished,new.releasing_party,new.hash;
	END IF;

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

