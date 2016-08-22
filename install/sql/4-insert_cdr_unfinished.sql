DROP FUNCTION insert_cdr_unfinished(bigint, character varying, character varying, character varying, character varying, smallint);

CREATE OR REPLACE FUNCTION insert_cdr_unfinished(
    p_call_id bigint,
    setup_time character varying,
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
                call_id, setup_time, hash, dst_route, releasing_party, release_timestamp, disconnect_cause
            ) values (
                p_call_id, p_setup_timestamp, p_hash, dst_route, releasing_party, p_release_timestamp, disconnect_cause
            );

            delete from calls_cdr.start where call_id = p_call_id;
    
        end;
        $BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION insert_cdr_unfinished(bigint, character varying, character varying, character varying, character varying, smallint)
  OWNER TO postgres;

