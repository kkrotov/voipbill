ALTER TABLE calls_cdr.cdr ADD COLUMN in_sig_call_id uuid;
ALTER TABLE calls_cdr.cdr ADD COLUMN out_sig_call_id uuid;

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

