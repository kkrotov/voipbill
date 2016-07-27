#!/bin/python
# -*- coding: utf-8 -*-

def migrate(centralDb, regionalDb, regionsList) :

  regConnections = [(regionalDb(reg), int(reg)) for reg in regionsList]

  # Мигрируем региональные БД
  for (regConn, region_id) in regConnections :
    curReg = regConn.cursor()
    curReg.execute('''
	ALTER TABLE calls_cdr.cdr ADD COLUMN call_finished character varying;
    ''')
    curReg.execute('''
	ALTER TABLE calls_cdr.cdr ADD COLUMN releasing_party character varying;
    ''')
    curReg.execute('''
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
	    call_finished character varying DEFAULT NULL::character varying,
	    releasing_party character varying DEFAULT NULL::character varying)
	  RETURNS void AS
	$BODY$
	declare
		p_id bigint;
	    p_hash uuid;
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
   
	    insert into calls_cdr.cdr (
	        id,hash,call_id,nas_ip,
	        src_number,dst_number,redirect_number,session_time,
	        setup_time,connect_time,disconnect_time,disconnect_cause,
	        src_route,dst_route,src_noa,dst_noa,dst_replace,call_finished,releasing_party
	    ) values (
	        p_id,p_hash,p_call_id,nas_ip,
	        src_number,dst_number,redirect_number,session_time,
	        setup_time::timestamp,connect_time::timestamp,disconnect_time::timestamp,disconnect_cause,
	        src_route,dst_route,src_noa,dst_noa,dst_replace,call_finished,releasing_party
	    );

	    delete from calls_cdr.start where call_id = p_call_id;
    
	end;
	$BODY$
	  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
	  COST 100;
    ''')
    regConn.commit()
    regConn.close()

