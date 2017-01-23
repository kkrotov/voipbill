ALTER TABLE calls_cdr.cdr ADD COLUMN in_sig_call_id uuid;
ALTER TABLE calls_cdr.cdr ADD COLUMN out_sig_call_id uuid;

DROP FUNCTION insert_cdr(bigint, inet, character varying, character varying, character varying, bigint, character varying, character varying, character varying, smallint, character varying, character varying, smallint, smallint, character varying);

CREATE OR REPLACE FUNCTION insert_cdr(
    p_call_id bigint,
    nas_ip inet,
    src_number character varying,
    dst_number character varying,
    redirect_number character varying,
    session_time bigint,
    setup_time character varying,
    connect_time character varying,
    disconnect_time character varying,
    disconnect_cause smallint,
    src_route character varying,
    dst_route character varying,
    src_noa smallint,
    dst_noa smallint,
    dst_replace character varying DEFAULT NULL::character varying,
    in_sig_call_id character varying DEFAULT NULL::character varying,
    out_sig_call_id character varying DEFAULT NULL::character varying)
  RETURNS void AS
$BODY$
declare
    p_id bigint;
    p_hash uuid;
    in_sig_call_id_hash uuid;
    out_sig_call_id_hash uuid;
begin

    if connect_time = '' or connect_time is null then 
    	connect_time = setup_time;
    end if;

	if disconnect_time = '' or disconnect_time is null then 
    	disconnect_time = connect_time;
    end if;
        
    if dst_number ~ '^0' then
        dst_number = substring(dst_number from '^0\d{4}(.+)');
    end if;

    p_hash = calls_cdr.get_hash(p_call_id, setup_time::timestamp);
    p_id = nextval('calls_cdr.cdr_id_seq'::regclass);
    in_sig_call_id_hash = md5(in_sig_call_id);
    out_sig_call_id_hash = md5(out_sig_call_id);
   
    insert into calls_cdr.cdr (
        id,hash,call_id,nas_ip,
        src_number,dst_number,redirect_number,session_time,
        setup_time,connect_time,disconnect_time,disconnect_cause,
        src_route,dst_route,src_noa,dst_noa,dst_replace,
        in_sig_call_id, out_sig_call_id
    ) values (
        p_id,p_hash,p_call_id,nas_ip,
        src_number,dst_number,redirect_number,session_time,
        setup_time::timestamp,connect_time::timestamp,disconnect_time::timestamp,disconnect_cause,
        src_route,dst_route,src_noa,dst_noa,dst_replace,
        in_sig_call_id_hash, out_sig_call_id_hash
    );

    delete from calls_cdr.start where call_id = p_call_id;
    
end;
$BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION insert_cdr(bigint, inet, character varying, character varying, character varying, bigint, character varying, character varying, character varying, smallint, character varying, character varying, smallint, smallint, character varying, character varying, character varying)
  OWNER TO postgres;

DROP FUNCTION insert_cdr2(bigint, inet, character varying, character varying, character varying, bigint, character varying, character varying, character varying, smallint, character varying, character varying, smallint, smallint, character varying, character varying, character varying);

CREATE OR REPLACE FUNCTION insert_cdr2(
    p_call_id bigint,
    nas_ip inet,
    src_number character varying,
    dst_number character varying,
    redirect_number character varying,
    session_time bigint,
    setup_time character varying,
    connect_time character varying,
    disconnect_time character varying,
    disconnect_cause smallint,
    src_route character varying,
    dst_route character varying,
    src_noa smallint,
    dst_noa smallint,
    dst_replace character varying DEFAULT NULL::character varying,
    call_finished character varying DEFAULT NULL::character varying,
    releasing_party character varying DEFAULT NULL::character varying,
    in_sig_call_id character varying DEFAULT NULL::character varying,
    out_sig_call_id character varying DEFAULT NULL::character varying)
  RETURNS void AS
$BODY$
	declare
	    p_id bigint;
	    p_hash uuid;
	    in_sig_call_id_hash uuid;
	    out_sig_call_id_hash uuid;
	begin
	    if connect_time = '' or connect_time is null then 
    		connect_time = setup_time;
	    end if;

	    if disconnect_time = '' or disconnect_time is null then 
	    	disconnect_time = connect_time;
	    end if;
        
	    if dst_number ~ '^0' then
	        dst_number = substring(dst_number from '^0\d{4}(.+)');
	    end if;

	    p_hash = calls_cdr.get_hash(p_call_id, setup_time::timestamp);
	    p_id = nextval('calls_cdr.cdr_id_seq'::regclass);
            in_sig_call_id_hash = md5(in_sig_call_id);
	    out_sig_call_id_hash = md5(out_sig_call_id);
	    
	    insert into calls_cdr.cdr (
	        id,hash,call_id,nas_ip,
	        src_number,dst_number,redirect_number,session_time,
	        setup_time,connect_time,disconnect_time,disconnect_cause,
	        src_route,dst_route,src_noa,dst_noa,dst_replace,call_finished,releasing_party,
	        in_sig_call_id, out_sig_call_id
	    ) values (
	        p_id,p_hash,p_call_id,nas_ip,
	        src_number,dst_number,redirect_number,session_time,
	        setup_time::timestamp,connect_time::timestamp,disconnect_time::timestamp,disconnect_cause,
	        src_route,dst_route,src_noa,dst_noa,dst_replace,call_finished,releasing_party,
	        in_sig_call_id_hash, out_sig_call_id_hash
	    );

	    delete from calls_cdr.start where call_id = p_call_id;
    
	end;
$BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION insert_cdr2(bigint, inet, character varying, character varying, character varying, bigint, character varying, character varying, character varying, smallint, character varying, character varying, smallint, smallint, character varying, character varying, character varying, character varying, character varying)
  OWNER TO postgres;



CREATE OR REPLACE FUNCTION calls_cdr.tr_partitioning()
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
			'src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party,in_sig_call_id, out_sig_call_id) ' ||
			'values ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18,$19,$20,$21)')
			USING new.id,new.call_id, new.nas_ip,new.src_number,new.dst_number,new.redirect_number,new.setup_time,new.connect_time,new.disconnect_time,new.session_time,
				new.disconnect_cause,new.src_route,new.dst_route,new.src_noa,new.dst_noa,new.hash,new.dst_replace,new.call_finished,new.releasing_party,
                                new.in_sig_call_id, new.out_sig_call_id;
        ELSE

                --raise notice 'in_sig_call_id=%', new.in_sig_call_id;
                --raise notice 'out_sig_call_id=%', new.out_sig_call_id;
		EXECUTE format('update ' || relname || 
			' set nas_ip=$1,src_number=$2,dst_number=$3,redirect_number=$4,setup_time=$5,connect_time=$6,disconnect_time=$7,session_time=$8,disconnect_cause=$9,' ||
                        'src_route=$10,dst_route=$11,src_noa=$12,dst_noa=$13,dst_replace=$14,call_finished=$15,releasing_party=$16,in_sig_call_id=$17, out_sig_call_id=$18 where hash=$19')
			USING new.nas_ip,new.src_number,new.dst_number,new.redirect_number,new.setup_time,new.connect_time,new.disconnect_time,new.session_time,
				new.disconnect_cause,new.src_route,new.dst_route,new.src_noa,new.dst_noa,new.dst_replace,new.call_finished,new.releasing_party,
                                new.in_sig_call_id, new.out_sig_call_id, new.hash;
	END IF;

	return null;        
end;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION calls_cdr.tr_partitioning()
  OWNER TO postgres;
