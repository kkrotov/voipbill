--
-- PostgreSQL database dump
--

-- Dumped from database version 9.1.7
-- Dumped by pg_dump version 9.1.14
-- Started on 2014-09-01 22:16:05 MSK

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- TOC entry 206 (class 3079 OID 12506)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 3642 (class 0 OID 0)
-- Dependencies: 206
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

--
-- TOC entry 538 (class 1247 OID 19295)
-- Dependencies: 8 163
-- Name: dblink_pkey_results; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE dblink_pkey_results AS (
	"position" integer,
	colname text
);


ALTER TYPE public.dblink_pkey_results OWNER TO postgres;

--
-- TOC entry 228 (class 1255 OID 20933893)
-- Dependencies: 703 8
-- Name: acct_insert_start(bigint, character varying, timestamp with time zone, timestamp with time zone, character varying, character varying, inet, smallint, character varying, smallint, smallint, smallint, smallint, smallint, smallint, character varying, character varying, character varying); Type: FUNCTION; Schema: public; Owner: alexv
--

CREATE FUNCTION acct_insert_start(inacctsessionid bigint DEFAULT 0, inacctuniqueid character varying DEFAULT NULL::character varying, insetuptime timestamp with time zone DEFAULT NULL::timestamp with time zone, inconnecttime timestamp with time zone DEFAULT NULL::timestamp with time zone, incallingstationid character varying DEFAULT NULL::character varying, incalledstationid character varying DEFAULT NULL::character varying, innasipaddress inet DEFAULT NULL::inet, inacctdelaytime smallint DEFAULT 0, indirection character varying DEFAULT NULL::character varying, inprefix_oper smallint DEFAULT 0, inprefix_des smallint DEFAULT 0, inin_oper smallint DEFAULT 0, inout_oper smallint DEFAULT 0, inincd_noa smallint DEFAULT 0, inincg_noa smallint DEFAULT 0, inin_route character varying DEFAULT NULL::character varying, inout_route character varying DEFAULT NULL::character varying, inredirectnum character varying DEFAULT NULL::character varying) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE

BEGIN
   
		INSERT into radacct_voip_start (acctsessionid,acctuniqueid,setuptime,connecttime,
    													   callingstationid,calledstationid,nasipaddress,
                                                           acctdelaytime,direction,prefix_oper,prefix_des,
                                                           in_oper,out_oper,incd_noa,incg_noa,in_route,out_route,redirectnum)
		VALUES (inacctsessionid,inacctuniqueid,insetuptime,inconnecttime,
    				incallingstationid,incalledstationid,innasipaddress,
                    inacctdelaytime,indirection,inprefix_oper,inprefix_des,
                    inin_oper,inout_oper,inincd_noa,inincg_noa,inin_route,inout_route,inredirectnum);

END;
$$;


ALTER FUNCTION public.acct_insert_start(inacctsessionid bigint, inacctuniqueid character varying, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inredirectnum character varying) OWNER TO alexv;

--
-- TOC entry 223 (class 1255 OID 20933892)
-- Dependencies: 8 703
-- Name: acct_insert_stop(character varying, bigint, timestamp with time zone, timestamp with time zone, character varying, character varying, inet, smallint, character varying, smallint, smallint, smallint, smallint, smallint, smallint, character varying, character varying, timestamp with time zone, smallint, integer, character varying); Type: FUNCTION; Schema: public; Owner: alexv
--

CREATE FUNCTION acct_insert_stop(inacctuniqueid character varying DEFAULT NULL::character varying, inacctsessionid bigint DEFAULT 0, insetuptime timestamp with time zone DEFAULT NULL::timestamp with time zone, inconnecttime timestamp with time zone DEFAULT NULL::timestamp with time zone, incallingstationid character varying DEFAULT NULL::character varying, incalledstationid character varying DEFAULT NULL::character varying, innasipaddress inet DEFAULT NULL::inet, inacctdelaytime smallint DEFAULT 0, indirection character varying DEFAULT NULL::character varying, inprefix_oper smallint DEFAULT 0, inprefix_des smallint DEFAULT 0, inin_oper smallint DEFAULT 0, inout_oper smallint DEFAULT 0, inincd_noa smallint DEFAULT 0, inincg_noa smallint DEFAULT 0, inin_route character varying DEFAULT NULL::character varying, inout_route character varying DEFAULT NULL::character varying, indisconnecttime timestamp with time zone DEFAULT NULL::timestamp with time zone, indisconnectcause smallint DEFAULT 0, inacctsessiontime integer DEFAULT 0, inredirectnum character varying DEFAULT NULL::character varying) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE

BEGIN
	
		INSERT into radacct_voip_stop  (acctuniqueid,acctsessionid,setuptime,connecttime,
    													   callingstationid,calledstationid,nasipaddress,
                                                           acctdelaytime,direction,prefix_oper,prefix_des,
                                                           in_oper,out_oper,incd_noa,incg_noa,in_route,out_route,
                                                           disconnecttime,disconnectcause,acctsessiontime,redirectnum)
		 VALUES (inacctuniqueid,inacctsessionid,insetuptime,inconnecttime,
    				incallingstationid,incalledstationid,innasipaddress,
                    inacctdelaytime,indirection,inprefix_oper,inprefix_des,
                    inin_oper,inout_oper,inincd_noa,inincg_noa,inin_route,inout_route,
                    indisconnecttime,indisconnectcause,inacctsessiontime,inredirectnum);
	
END;
$$;


ALTER FUNCTION public.acct_insert_stop(inacctuniqueid character varying, inacctsessionid bigint, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, indisconnecttime timestamp with time zone, indisconnectcause smallint, inacctsessiontime integer, inredirectnum character varying) OWNER TO alexv;

--
-- TOC entry 222 (class 1255 OID 1155160)
-- Dependencies: 8 703
-- Name: acct_insert_stop_part(); Type: FUNCTION; Schema: public; Owner: alexv
--

CREATE FUNCTION acct_insert_stop_part() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN

    DELETE FROM radacct_voip_start where acctuniqueid = NEW.acctuniqueid;

         IF (NEW.connecttime >= DATE '2012-01-01' AND NEW.connecttime < DATE '2012-02-01')THEN
                 INSERT INTO radacct_voip_stop_2012m01 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-02-01' AND NEW.connecttime < DATE '2012-03-01')THEN
                 INSERT INTO radacct_voip_stop_2012m02 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-03-01' AND NEW.connecttime < DATE '2012-04-01')THEN
                 INSERT INTO radacct_voip_stop_2012m03 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-04-01' AND NEW.connecttime < DATE '2012-05-01')THEN
                 INSERT INTO radacct_voip_stop_2012m04 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-05-01' AND NEW.connecttime < DATE '2012-06-01')THEN
                 INSERT INTO radacct_voip_stop_2012m05 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-06-01' AND NEW.connecttime < DATE '2012-07-01')THEN
                 INSERT INTO radacct_voip_stop_2012m06 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-07-01' AND NEW.connecttime < DATE '2012-08-01')THEN
                 INSERT INTO radacct_voip_stop_2012m07 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-08-01' AND NEW.connecttime < DATE '2012-09-01')THEN
                 INSERT INTO radacct_voip_stop_2012m08 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-09-01' AND NEW.connecttime < DATE '2012-10-01')THEN
                 INSERT INTO radacct_voip_stop_2012m09 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-10-01' AND NEW.connecttime < DATE '2012-11-01')THEN
                 INSERT INTO radacct_voip_stop_2012m10 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-11-01' AND NEW.connecttime < DATE '2012-12-01')THEN
                 INSERT INTO radacct_voip_stop_2012m11 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2012-12-01' AND NEW.connecttime < DATE '2013-01-01')THEN
                 INSERT INTO radacct_voip_stop_2012m12 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-01-01' AND NEW.connecttime < DATE '2013-02-01')THEN
                 INSERT INTO radacct_voip_stop_2013m01 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-02-01' AND NEW.connecttime < DATE '2013-03-01')THEN
                 INSERT INTO radacct_voip_stop_2013m02 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-03-01' AND NEW.connecttime < DATE '2013-04-01')THEN
                 INSERT INTO radacct_voip_stop_2013m03 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-04-01' AND NEW.connecttime < DATE '2013-05-01')THEN
                 INSERT INTO radacct_voip_stop_2013m04 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-05-01' AND NEW.connecttime < DATE '2013-06-01')THEN
                 INSERT INTO radacct_voip_stop_2013m05 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-06-01' AND NEW.connecttime < DATE '2013-07-01')THEN
                 INSERT INTO radacct_voip_stop_2013m06 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-07-01' AND NEW.connecttime < DATE '2013-08-01')THEN
                 INSERT INTO radacct_voip_stop_2013m07 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-08-01' AND NEW.connecttime < DATE '2013-09-01')THEN
                 INSERT INTO radacct_voip_stop_2013m08 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-09-01' AND NEW.connecttime < DATE '2013-10-01')THEN
                 INSERT INTO radacct_voip_stop_2013m09 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-10-01' AND NEW.connecttime < DATE '2013-11-01')THEN
                 INSERT INTO radacct_voip_stop_2013m10 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-11-01' AND NEW.connecttime < DATE '2013-12-01')THEN
                 INSERT INTO radacct_voip_stop_2013m11 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2013-12-01' AND NEW.connecttime < DATE '2014-01-01')THEN
                 INSERT INTO radacct_voip_stop_2013m12 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-01-01' AND NEW.connecttime < DATE '2014-02-01')THEN
                 INSERT INTO radacct_voip_stop_2014m01 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-02-01' AND NEW.connecttime < DATE '2014-03-01')THEN
                 INSERT INTO radacct_voip_stop_2014m02 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-03-01' AND NEW.connecttime < DATE '2014-04-01')THEN
                 INSERT INTO radacct_voip_stop_2014m03 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-04-01' AND NEW.connecttime < DATE '2014-05-01')THEN
                 INSERT INTO radacct_voip_stop_2014m04 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-05-01' AND NEW.connecttime < DATE '2014-06-01')THEN
                 INSERT INTO radacct_voip_stop_2014m05 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-06-01' AND NEW.connecttime < DATE '2014-07-01')THEN
                 INSERT INTO radacct_voip_stop_2014m06 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-07-01' AND NEW.connecttime < DATE '2014-08-01')THEN
                 INSERT INTO radacct_voip_stop_2014m07 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-08-01' AND NEW.connecttime < DATE '2014-09-01')THEN
                 INSERT INTO radacct_voip_stop_2014m08 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-09-01' AND NEW.connecttime < DATE '2014-10-01')THEN
                 INSERT INTO radacct_voip_stop_2014m09 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-10-01' AND NEW.connecttime < DATE '2014-11-01')THEN
                 INSERT INTO radacct_voip_stop_2014m10 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-11-01' AND NEW.connecttime < DATE '2014-12-01')THEN
                 INSERT INTO radacct_voip_stop_2014m11 VALUES (NEW.*);
         ELSIF (NEW.connecttime >= DATE '2014-12-01' AND NEW.connecttime < DATE '2015-01-01')THEN
                 INSERT INTO radacct_voip_stop_2014m12 VALUES (NEW.*);
         ELSE
         	-- RAISE EXCEPTION 'Date out of range';
         	INSERT INTO radacct_voip_stop_max VALUES (NEW.*);
         END IF;
	
	RETURN NULL;   
END;
$$;


ALTER FUNCTION public.acct_insert_stop_part() OWNER TO alexv;

--
-- TOC entry 224 (class 1255 OID 1564059)
-- Dependencies: 8 703
-- Name: force_finish_call(integer, integer); Type: FUNCTION; Schema: public; Owner: eivanov
--

CREATE FUNCTION force_finish_call(p_session_id integer, p_timeout integer DEFAULT 5) RETURNS boolean
    LANGUAGE plpgsql
    AS $$
DECLARE cur_start CURSOR (p_session_id integer) IS 
			SELECT 	acctuniqueid, setuptime, connecttime, 
		    		callingstationid, calledstationid, nasipaddress, 
		            acctdelaytime, direction, 
		            prefix_oper, prefix_des, in_oper, out_oper, incd_noa, incg_noa, in_route, out_route
		    FROM radacct_voip_start WHERE acctsessionid=p_session_id;
DECLARE p_disconnect_time TIMESTAMP;
DECLARE p_now TIMESTAMP;
DECLARE p_acctsessiontime INTEGER;
DECLARE p_res BOOLEAN;
BEGIN
	p_res = FALSE;
	p_now = cast(now() as TIMESTAMP) - INTERVAL '1 SECONDS' * p_timeout;

	FOR r IN cur_start(p_session_id) LOOP
        if p_now > r.connecttime then
        	p_disconnect_time = p_now;	
            p_acctsessiontime = cast( date_part('epoch', p_now - r.connecttime) as INTEGER );
        else
        	p_disconnect_time = r.connecttime;
            p_acctsessiontime = 0;
        end if;
		
        
    	INSERT INTO radacct_voip_stop(
        		acctuniqueid, setuptime, connecttime, disconnecttime,
        		acctsessiontime, callingstationid, calledstationid, nasipaddress,
                acctdelaytime, disconnectcause, direction, 
                prefix_oper, prefix_des, in_oper, out_oper, incd_noa, incg_noa, in_route, out_route)
        VALUES(
        		r.acctuniqueid, r.setuptime, r.connecttime, p_disconnect_time,
        		p_acctsessiontime, r.callingstationid, r.calledstationid, r.nasipaddress,
                r.acctdelaytime, 125, r.direction, 
                r.prefix_oper, r.prefix_des, r.in_oper, r.out_oper, r.incd_noa, r.incg_noa, r.in_route, r.out_route
        );        
        
        p_res = TRUE;       
	END LOOP;
    RETURN p_res;    
END;
$$;


ALTER FUNCTION public.force_finish_call(p_session_id integer, p_timeout integer) OWNER TO eivanov;

--
-- TOC entry 220 (class 1255 OID 22917)
-- Dependencies: 8 703
-- Name: hex2dec(character varying); Type: FUNCTION; Schema: public; Owner: alexv
--

CREATE FUNCTION hex2dec(in_hex character varying) RETURNS smallint
    LANGUAGE plpgsql STABLE STRICT
    AS $$
BEGIN

IF (length(in_hex)=1) THEN
   in_hex= '0' || in_hex;
END IF;

RETURN CAST(CAST(('x' || CAST(in_hex AS text)) AS bit(8)) AS INT);
EXCEPTION WHEN others THEN RETURN 0;

END;
$$;


ALTER FUNCTION public.hex2dec(in_hex character varying) OWNER TO alexv;

--
-- TOC entry 218 (class 1255 OID 22918)
-- Dependencies: 8
-- Name: int2ts(integer); Type: FUNCTION; Schema: public; Owner: alexv
--

CREATE FUNCTION int2ts(integer) RETURNS timestamp without time zone
    LANGUAGE sql STABLE STRICT
    AS $_$
SELECT ( TIMESTAMP WITH TIME ZONE 'epoch' + $1 * INTERVAL '1second')::timestamp without time zone;
$_$;


ALTER FUNCTION public.int2ts(integer) OWNER TO alexv;

--
-- TOC entry 225 (class 1255 OID 20741958)
-- Dependencies: 8 703
-- Name: mytest(bigint); Type: FUNCTION; Schema: public; Owner: alexv
--

CREATE FUNCTION mytest(arg1 bigint DEFAULT 0, OUT res bigint) RETURNS bigint
    LANGUAGE plpgsql STRICT
    AS $$
BEGIN
 --#res=ceil(arg1+1);
res = arg1;
END;
$$;


ALTER FUNCTION public.mytest(arg1 bigint, OUT res bigint) OWNER TO alexv;

--
-- TOC entry 227 (class 1255 OID 1155166)
-- Dependencies: 703 8
-- Name: numtoe164(character varying, smallint, smallint, character varying); Type: FUNCTION; Schema: public; Owner: alexv
--

CREATE FUNCTION numtoe164(nas character varying DEFAULT NULL::character varying, oper smallint DEFAULT 0, noa smallint DEFAULT 0, INOUT num character varying DEFAULT NULL::character varying) RETURNS character varying
    LANGUAGE plpgsql STABLE
    AS $_$
BEGIN

IF (nas='openca') THEN
		IF (oper=2 and noa=3) THEN
	 		IF (length(num)=10) THEN
     				num = '7' || num;
     		END IF;
		END IF;
		IF (oper=3 and noa=3) THEN
	 		IF (length(num)=10) THEN
     				num = '7' || num;
     		END IF;
		END IF;
        IF (oper=6 and noa=3) THEN
	 		IF (length(num)=10) THEN
     				num = '7' || num;
     		END IF;
		END IF;
        IF (oper=8 and noa=3) THEN
	 		IF (length(num)=10) THEN
     				num = '7' || num;
     		END IF;
		END IF;
         IF (oper=11 and noa=3) THEN
	 		IF (length(num)=10) THEN
     				num = '7' || num;
     		END IF;
		END IF;
        IF (oper=15 and noa=3) THEN
	 		IF (length(num)=11 and num ~ '^849[589]\d{7}') THEN
     				num = '7' || substring(num from '^8(49[589]\d{7})$');
     		END IF;
		END IF;
         IF (oper=17 and noa=3) THEN
	 		IF (length(num)=10) THEN
     				num = '7' || num;
     		END IF;
		END IF;
         IF (oper=18 and (noa in (0,1,2,3))) THEN
	 		IF (length(num)=10) THEN
     				num = '7' || num;
     		END IF;
		END IF;
         IF (oper=19 and noa=3) THEN
	 		IF (length(num)=10) THEN
     				num = '7' || num;
     		END IF;
		END IF;
END IF;

END;
$_$;


ALTER FUNCTION public.numtoe164(nas character varying, oper smallint, noa smallint, INOUT num character varying) OWNER TO alexv;

--
-- TOC entry 226 (class 1255 OID 20933899)
-- Dependencies: 8 703
-- Name: openca_to_radsql(character varying, bigint, character varying, character varying, character varying, character varying, character varying, character varying, smallint, smallint, smallint, character varying, character varying, character varying, character varying, character varying, smallint, bigint); Type: FUNCTION; Schema: public; Owner: alexv
--

CREATE FUNCTION openca_to_radsql(inaccttype character varying DEFAULT NULL::character varying, inacctsessionid bigint DEFAULT 0, inacctuniqueid character varying DEFAULT NULL::character varying, insetuptime character varying DEFAULT NULL::character varying, inconnecttime character varying DEFAULT NULL::character varying, incallingstationid character varying DEFAULT NULL::character varying, incalledstationid character varying DEFAULT NULL::character varying, innasipaddress character varying DEFAULT NULL::character varying, inacctdelaytime smallint DEFAULT 0, inincd_noa smallint DEFAULT 0, inincg_noa smallint DEFAULT 0, inin_route character varying DEFAULT NULL::character varying, inout_route character varying DEFAULT NULL::character varying, inoutreplacecdpn character varying DEFAULT NULL::character varying, inredirectnum character varying DEFAULT NULL::character varying, indisconnecttime character varying DEFAULT NULL::character varying, indisconnectcause smallint DEFAULT 0, inacctsessiontime bigint DEFAULT 0) RETURNS void
    LANGUAGE plpgsql
    AS $_$
DECLARE
new_setuptime timestamp;
new_connecttime timestamp;
new_disconnecttime timestamp;
new_nasipaddress inet;
insertdup BOOLEAN;
indirection varchar=NULL;
tmpcallingstationid varchar=NULL;
inprefix_oper SMALLINT=0;
inprefix_des SMALLINT=0;
inin_oper  SMALLINT=0;
inout_oper  SMALLINT=0;

BEGIN
insertdup = false;

	new_setuptime=insetuptime::timestamptz;
    IF (inconnecttime != '') THEN
    	new_connecttime=inconnecttime::timestamptz;
    ELSE
    	new_connecttime=insetuptime::timestamptz;
    END IF;
	
    IF ((indisconnecttime != '') or (indisconnecttime is not null)) THEN
    	new_disconnecttime=indisconnecttime::timestamptz;
    END IF;

    
	new_nasipaddress=innasipaddress::inet;
   
	IF (inoutreplacecdpn = '09503') THEN
    	RETURN;
    END IF;
   --#For test Call 
    IF (incalledstationid ~ '^05555') THEN
    	RETURN;
    END IF;
    --#For test Call
    
    IF (incallingstationid = '') THEN
    	incallingstationid = 'anonymous';
    END IF;
    
   IF (inacctsessiontime > 4000000000) THEN
   		inacctsessiontime = 0;
        indisconnectcause = 124;
   END IF;
   
	 
    IF (incallingstationid LIKE '749%=2A%' OR incallingstationid LIKE '749%=2B%') THEN 
	    incallingstationid := substring(incallingstationid from 15);
    END IF;
    
    IF (incalledstationid ~ '^0') THEN
    	inprefix_des=substring(incalledstationid from '^0..(\d{2})');
        incalledstationid=substring(incalledstationid from '^0\d{4}(.+)');
    END IF;
    
    IF ((inin_route ~ '_(\d{2,3})$') AND (inout_route ~ '_(\d{2,3})$'))  THEN
         inin_oper=substring(inin_route from '_(\d{2,3})$');
         inout_oper=substring(inout_route from '_(\d{2,3})$');
               
         IF (inin_oper IN (1,2,3,4,6,8,11,15,17,18,19,26,32)) THEN
         		indirection='in';
                 inprefix_oper=inin_oper;
                incallingstationid= numtoe164('openca',inin_oper,inincg_noa,incallingstationid);
         		incalledstationid= numtoe164('openca',inin_oper,inincd_noa,incalledstationid);
                inprefix_des=9;
         ELSE
         	    indirection='out';
                inprefix_oper= inout_oper;	
             IF ((inredirectnum ~ '^749[589]') AND (inprefix_des IN (21,22,23,24))) THEN
           	     tmpcallingstationid = incallingstationid;
                 incallingstationid = inredirectnum; 
                 inredirectnum = tmpcallingstationid; 
             END IF;
         END IF;
     
         --## Our client calling to our client ##--
         IF (inin_oper= inout_oper)  THEN
			insertdup=true;
            indirection='out';
         END IF;
         --## Our client calling to our client ##-- 
    
     --### temporarily, because SPD_peer and MTT peer to as5400 ###--
--###         IF ((incalledstationid ~'^749[589]') AND (inin_oper=4) )  THEN
--###        	 inin_oper=1;
--###            inprefix_oper= inin_oper;	          
--###        END IF;        
--###         IF ((incalledstationid ~'^749[589]') AND (inout_oper=4) )  THEN
--###         	 inout_oper=1;
--###             inprefix_oper= inout_oper;	          
--###         END IF;
     --### temporarily, because SPD_peer and MTT peer to as5400 ###--
                          

    ELSE
    	indirection='non';      
    END IF;
    
    IF  (inaccttype='start') THEN
    	PERFORM   acct_insert_start(inacctsessionid,inacctuniqueid,new_setuptime,new_connecttime,
    												incallingstationid,incalledstationid,new_nasipaddress,
                    								inacctdelaytime,indirection,inprefix_oper,inprefix_des,
                   								    inin_oper,inout_oper,inincd_noa,inincg_noa,inin_route,inout_route,inredirectnum); 
        IF  (insertdup=true) THEN
     			indirection='in';
                inprefix_des=9;
    			PERFORM  acct_insert_start(inacctsessionid,inacctuniqueid,new_setuptime,new_connecttime,
    														incallingstationid,incalledstationid,new_nasipaddress,
                    										inacctdelaytime,indirection,inprefix_oper,inprefix_des,
                   								   			inin_oper,inout_oper,inincd_noa,inincg_noa,inin_route,inout_route,inredirectnum);                
        END IF;
 	END IF;
   
	IF  (inaccttype='stop') THEN 
    
        	PERFORM   acct_insert_stop(inacctuniqueid,inacctsessionid,new_setuptime,new_connecttime,
    												incallingstationid,incalledstationid,new_nasipaddress,
                    								inacctdelaytime,indirection,inprefix_oper,inprefix_des,
                   								    inin_oper,inout_oper,inincd_noa,inincg_noa,inin_route,inout_route,
                                                    new_disconnecttime,indisconnectcause,inacctsessiontime::integer,inredirectnum); 
        	IF  (insertdup=true) THEN
     			indirection='in';
                 inprefix_des=9;
    			PERFORM  acct_insert_stop(inacctuniqueid,inacctsessionid,new_setuptime,new_connecttime,
    														incallingstationid,incalledstationid,new_nasipaddress,
                    										inacctdelaytime,indirection,inprefix_oper,inprefix_des,
                   								   			inin_oper,inout_oper,inincd_noa,inincg_noa,inin_route,inout_route,
                                                            new_disconnecttime,indisconnectcause,inacctsessiontime::integer,inredirectnum);                
       		 END IF;
    
    END IF;
    

END;
$_$;


ALTER FUNCTION public.openca_to_radsql(inaccttype character varying, inacctsessionid bigint, inacctuniqueid character varying, insetuptime character varying, inconnecttime character varying, incallingstationid character varying, incalledstationid character varying, innasipaddress character varying, inacctdelaytime smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inoutreplacecdpn character varying, inredirectnum character varying, indisconnecttime character varying, indisconnectcause smallint, inacctsessiontime bigint) OWNER TO alexv;

--
-- TOC entry 221 (class 1255 OID 1153204)
-- Dependencies: 703 8
-- Name: strip_dot(character varying); Type: FUNCTION; Schema: public; Owner: alexv
--

CREATE FUNCTION strip_dot(character varying) RETURNS character varying
    LANGUAGE plpgsql STABLE
    AS $_$
 DECLARE
        original_timestamp ALIAS FOR $1;
 BEGIN
        IF original_timestamp = '' THEN
                RETURN NULL;
        END IF;
        IF substring(original_timestamp from 1 for 1) = '.' THEN
                RETURN substring(original_timestamp from 2);
        ELSE
                RETURN original_timestamp;
        END IF;
 END;
$_$;


ALTER FUNCTION public.strip_dot(character varying) OWNER TO alexv;

--
-- TOC entry 219 (class 1255 OID 22921)
-- Dependencies: 8
-- Name: ts2int(timestamp without time zone); Type: FUNCTION; Schema: public; Owner: alexv
--

CREATE FUNCTION ts2int(timestamp without time zone) RETURNS integer
    LANGUAGE sql STABLE STRICT
    AS $_$
select extract('epoch' from $1)::integer;
$_$;


ALTER FUNCTION public.ts2int(timestamp without time zone) OWNER TO alexv;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- TOC entry 166 (class 1259 OID 1156262)
-- Dependencies: 2829 2830 2831 2832 2833 2834 2835 2836 8
-- Name: radacct_voip_start; Type: TABLE; Schema: public; Owner: alexv; Tablespace: 
--

CREATE TABLE radacct_voip_start (
    radacctid bigint NOT NULL,
    acctsessionid bigint DEFAULT 0,
    acctuniqueid character varying(16),
    setuptime timestamp without time zone,
    connecttime timestamp without time zone,
    callingstationid character varying(32),
    calledstationid character varying(32),
    nasipaddress inet NOT NULL,
    acctdelaytime smallint DEFAULT 0,
    direction character varying(3),
    prefix_oper smallint DEFAULT 0,
    prefix_des smallint DEFAULT 0,
    in_oper smallint DEFAULT 0,
    out_oper smallint DEFAULT 0,
    incd_noa smallint DEFAULT 0,
    incg_noa smallint DEFAULT 0,
    in_route character varying(32),
    out_route character varying(32),
    redirectnum character varying(32)
);


ALTER TABLE public.radacct_voip_start OWNER TO alexv;

--
-- TOC entry 165 (class 1259 OID 1156260)
-- Dependencies: 166 8
-- Name: radacct_voip_start_radacctid_seq; Type: SEQUENCE; Schema: public; Owner: alexv
--

CREATE SEQUENCE radacct_voip_start_radacctid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.radacct_voip_start_radacctid_seq OWNER TO alexv;

--
-- TOC entry 3654 (class 0 OID 0)
-- Dependencies: 165
-- Name: radacct_voip_start_radacctid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: alexv
--

ALTER SEQUENCE radacct_voip_start_radacctid_seq OWNED BY radacct_voip_start.radacctid;


--
-- TOC entry 167 (class 1259 OID 1199049)
-- Dependencies: 2837 2838 2839 2840 2841 2842 2843 2844 2845 2846 8
-- Name: radacct_voip_stop; Type: TABLE; Schema: public; Owner: alexv; Tablespace: 
--

CREATE TABLE radacct_voip_stop (
    radacctid bigint NOT NULL,
    acctsessionid bigint DEFAULT 0,
    acctuniqueid character varying(16),
    setuptime timestamp without time zone,
    connecttime timestamp without time zone,
    disconnecttime timestamp without time zone,
    acctsessiontime integer DEFAULT 0,
    callingstationid character varying(32),
    calledstationid character varying(32),
    nasipaddress inet NOT NULL,
    acctdelaytime smallint DEFAULT 0,
    disconnectcause smallint DEFAULT 0,
    direction character varying(3),
    prefix_oper smallint DEFAULT 0,
    prefix_des smallint DEFAULT 0,
    in_oper smallint DEFAULT 0,
    out_oper smallint DEFAULT 0,
    incd_noa smallint DEFAULT 0,
    incg_noa smallint DEFAULT 0,
    in_route character varying(32),
    out_route character varying(32),
    mysql_id integer,
    redirectnum character varying(32)
);


ALTER TABLE public.radacct_voip_stop OWNER TO alexv;

--
-- TOC entry 169 (class 1259 OID 1199069)
-- Dependencies: 2859 8 167
-- Name: radacct_voip_stop_2012m01; Type: TABLE; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m01 (
    CONSTRAINT radacct_voip_stop_2012m01_connecttime_check CHECK (((connecttime >= '2012-01-01'::date) AND (connecttime < '2012-02-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m01 OWNER TO eivanov;

--
-- TOC entry 170 (class 1259 OID 1199088)
-- Dependencies: 2871 8 167
-- Name: radacct_voip_stop_2012m02; Type: TABLE; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m02 (
    CONSTRAINT radacct_voip_stop_2012m02_connecttime_check CHECK (((connecttime >= '2012-02-01'::date) AND (connecttime < '2012-03-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m02 OWNER TO eivanov;

--
-- TOC entry 171 (class 1259 OID 1199107)
-- Dependencies: 2883 8 167
-- Name: radacct_voip_stop_2012m03; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m03 (
    CONSTRAINT radacct_voip_stop_2012m03_connecttime_check CHECK (((connecttime >= '2012-03-01'::date) AND (connecttime < '2012-04-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m03 OWNER TO postgres;

--
-- TOC entry 172 (class 1259 OID 1199126)
-- Dependencies: 2895 167 8
-- Name: radacct_voip_stop_2012m04; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m04 (
    CONSTRAINT radacct_voip_stop_2012m04_connecttime_check CHECK (((connecttime >= '2012-04-01'::date) AND (connecttime < '2012-05-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m04 OWNER TO postgres;

--
-- TOC entry 173 (class 1259 OID 1199145)
-- Dependencies: 2907 8 167
-- Name: radacct_voip_stop_2012m05; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m05 (
    CONSTRAINT radacct_voip_stop_2012m05_connecttime_check CHECK (((connecttime >= '2012-05-01'::date) AND (connecttime < '2012-06-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m05 OWNER TO postgres;

--
-- TOC entry 174 (class 1259 OID 1199164)
-- Dependencies: 2919 167 8
-- Name: radacct_voip_stop_2012m06; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m06 (
    CONSTRAINT radacct_voip_stop_2012m06_connecttime_check CHECK (((connecttime >= '2012-06-01'::date) AND (connecttime < '2012-07-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m06 OWNER TO postgres;

--
-- TOC entry 175 (class 1259 OID 1199183)
-- Dependencies: 2931 167 8
-- Name: radacct_voip_stop_2012m07; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m07 (
    CONSTRAINT radacct_voip_stop_2012m07_connecttime_check CHECK (((connecttime >= '2012-07-01'::date) AND (connecttime < '2012-08-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m07 OWNER TO postgres;

--
-- TOC entry 176 (class 1259 OID 1199202)
-- Dependencies: 2943 167 8
-- Name: radacct_voip_stop_2012m08; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m08 (
    CONSTRAINT radacct_voip_stop_2012m08_connecttime_check CHECK (((connecttime >= '2012-08-01'::date) AND (connecttime < '2012-09-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m08 OWNER TO postgres;

--
-- TOC entry 177 (class 1259 OID 1199221)
-- Dependencies: 2955 167 8
-- Name: radacct_voip_stop_2012m09; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m09 (
    CONSTRAINT radacct_voip_stop_2012m09_connecttime_check CHECK (((connecttime >= '2012-09-01'::date) AND (connecttime < '2012-10-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m09 OWNER TO postgres;

--
-- TOC entry 178 (class 1259 OID 1199240)
-- Dependencies: 2967 167 8
-- Name: radacct_voip_stop_2012m10; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m10 (
    CONSTRAINT radacct_voip_stop_2012m10_connecttime_check CHECK (((connecttime >= '2012-10-01'::date) AND (connecttime < '2012-11-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m10 OWNER TO postgres;

--
-- TOC entry 179 (class 1259 OID 1199259)
-- Dependencies: 2979 167 8
-- Name: radacct_voip_stop_2012m11; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m11 (
    CONSTRAINT radacct_voip_stop_2012m11_connecttime_check CHECK (((connecttime >= '2012-11-01'::date) AND (connecttime < '2012-12-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m11 OWNER TO postgres;

--
-- TOC entry 180 (class 1259 OID 1199278)
-- Dependencies: 2991 8 167
-- Name: radacct_voip_stop_2012m12; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2012m12 (
    CONSTRAINT radacct_voip_stop_2012m12_connecttime_check CHECK (((connecttime >= '2012-12-01'::date) AND (connecttime < '2013-01-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2012m12 OWNER TO postgres;

--
-- TOC entry 181 (class 1259 OID 1199297)
-- Dependencies: 3003 167 8
-- Name: radacct_voip_stop_2013m01; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m01 (
    CONSTRAINT radacct_voip_stop_2013m01_connecttime_check CHECK (((connecttime >= '2013-01-01'::date) AND (connecttime < '2013-02-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m01 OWNER TO postgres;

--
-- TOC entry 182 (class 1259 OID 1199316)
-- Dependencies: 3015 167 8
-- Name: radacct_voip_stop_2013m02; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m02 (
    CONSTRAINT radacct_voip_stop_2013m02_connecttime_check CHECK (((connecttime >= '2013-02-01'::date) AND (connecttime < '2013-03-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m02 OWNER TO postgres;

--
-- TOC entry 183 (class 1259 OID 1199335)
-- Dependencies: 3027 8 167
-- Name: radacct_voip_stop_2013m03; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m03 (
    CONSTRAINT radacct_voip_stop_2013m03_connecttime_check CHECK (((connecttime >= '2013-03-01'::date) AND (connecttime < '2013-04-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m03 OWNER TO postgres;

--
-- TOC entry 184 (class 1259 OID 1199354)
-- Dependencies: 3039 8 167
-- Name: radacct_voip_stop_2013m04; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m04 (
    CONSTRAINT radacct_voip_stop_2013m04_connecttime_check CHECK (((connecttime >= '2013-04-01'::date) AND (connecttime < '2013-05-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m04 OWNER TO postgres;

--
-- TOC entry 185 (class 1259 OID 1199373)
-- Dependencies: 3051 8 167
-- Name: radacct_voip_stop_2013m05; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m05 (
    CONSTRAINT radacct_voip_stop_2013m05_connecttime_check CHECK (((connecttime >= '2013-05-01'::date) AND (connecttime < '2013-06-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m05 OWNER TO postgres;

--
-- TOC entry 186 (class 1259 OID 1199392)
-- Dependencies: 3063 8 167
-- Name: radacct_voip_stop_2013m06; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m06 (
    CONSTRAINT radacct_voip_stop_2013m06_connecttime_check CHECK (((connecttime >= '2013-06-01'::date) AND (connecttime < '2013-07-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m06 OWNER TO postgres;

--
-- TOC entry 187 (class 1259 OID 1199411)
-- Dependencies: 3075 167 8
-- Name: radacct_voip_stop_2013m07; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m07 (
    CONSTRAINT radacct_voip_stop_2013m07_connecttime_check CHECK (((connecttime >= '2013-07-01'::date) AND (connecttime < '2013-08-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m07 OWNER TO postgres;

--
-- TOC entry 188 (class 1259 OID 1199430)
-- Dependencies: 3087 167 8
-- Name: radacct_voip_stop_2013m08; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m08 (
    CONSTRAINT radacct_voip_stop_2013m08_connecttime_check CHECK (((connecttime >= '2013-08-01'::date) AND (connecttime < '2013-09-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m08 OWNER TO postgres;

--
-- TOC entry 189 (class 1259 OID 1199449)
-- Dependencies: 3099 3100 8 167
-- Name: radacct_voip_stop_2013m09; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m09 (
    created_at timestamp without time zone DEFAULT now(),
    CONSTRAINT radacct_voip_stop_2013m09_connecttime_check CHECK (((connecttime >= '2013-09-01'::date) AND (connecttime < '2013-10-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m09 OWNER TO postgres;

--
-- TOC entry 190 (class 1259 OID 1199468)
-- Dependencies: 3112 8 167
-- Name: radacct_voip_stop_2013m10; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m10 (
    CONSTRAINT radacct_voip_stop_2013m10_connecttime_check CHECK (((connecttime >= '2013-10-01'::date) AND (connecttime < '2013-11-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m10 OWNER TO postgres;

--
-- TOC entry 191 (class 1259 OID 1199487)
-- Dependencies: 3124 167 8
-- Name: radacct_voip_stop_2013m11; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m11 (
    CONSTRAINT radacct_voip_stop_2013m11_connecttime_check CHECK (((connecttime >= '2013-11-01'::date) AND (connecttime < '2013-12-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m11 OWNER TO postgres;

--
-- TOC entry 192 (class 1259 OID 1199506)
-- Dependencies: 3136 167 8
-- Name: radacct_voip_stop_2013m12; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2013m12 (
    CONSTRAINT radacct_voip_stop_2013m12_connecttime_check CHECK (((connecttime >= '2013-12-01'::date) AND (connecttime < '2014-01-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2013m12 OWNER TO postgres;

--
-- TOC entry 193 (class 1259 OID 1199525)
-- Dependencies: 3148 8 167
-- Name: radacct_voip_stop_2014m01; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m01 (
    CONSTRAINT radacct_voip_stop_2014m01_connecttime_check CHECK (((connecttime >= '2014-01-01'::date) AND (connecttime < '2014-02-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m01 OWNER TO postgres;

--
-- TOC entry 194 (class 1259 OID 1199544)
-- Dependencies: 3160 167 8
-- Name: radacct_voip_stop_2014m02; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m02 (
    CONSTRAINT radacct_voip_stop_2014m02_connecttime_check CHECK (((connecttime >= '2014-02-01'::date) AND (connecttime < '2014-03-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m02 OWNER TO postgres;

--
-- TOC entry 195 (class 1259 OID 1199563)
-- Dependencies: 3172 167 8
-- Name: radacct_voip_stop_2014m03; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m03 (
    CONSTRAINT radacct_voip_stop_2014m03_connecttime_check CHECK (((connecttime >= '2014-03-01'::date) AND (connecttime < '2014-04-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m03 OWNER TO postgres;

--
-- TOC entry 196 (class 1259 OID 1199582)
-- Dependencies: 3184 167 8
-- Name: radacct_voip_stop_2014m04; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m04 (
    CONSTRAINT radacct_voip_stop_2014m04_connecttime_check CHECK (((connecttime >= '2014-04-01'::date) AND (connecttime < '2014-05-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m04 OWNER TO postgres;

--
-- TOC entry 197 (class 1259 OID 1199601)
-- Dependencies: 3196 167 8
-- Name: radacct_voip_stop_2014m05; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m05 (
    CONSTRAINT radacct_voip_stop_2014m05_connecttime_check CHECK (((connecttime >= '2014-05-01'::date) AND (connecttime < '2014-06-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m05 OWNER TO postgres;

--
-- TOC entry 198 (class 1259 OID 1199620)
-- Dependencies: 3208 167 8
-- Name: radacct_voip_stop_2014m06; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m06 (
    CONSTRAINT radacct_voip_stop_2014m06_connecttime_check CHECK (((connecttime >= '2014-06-01'::date) AND (connecttime < '2014-07-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m06 OWNER TO postgres;

--
-- TOC entry 199 (class 1259 OID 1199639)
-- Dependencies: 3220 167 8
-- Name: radacct_voip_stop_2014m07; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m07 (
    CONSTRAINT radacct_voip_stop_2014m07_connecttime_check CHECK (((connecttime >= '2014-07-01'::date) AND (connecttime < '2014-08-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m07 OWNER TO postgres;

--
-- TOC entry 200 (class 1259 OID 1199658)
-- Dependencies: 3232 8 167
-- Name: radacct_voip_stop_2014m08; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m08 (
    CONSTRAINT radacct_voip_stop_2014m08_connecttime_check CHECK (((connecttime >= '2014-08-01'::date) AND (connecttime < '2014-09-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m08 OWNER TO postgres;

--
-- TOC entry 201 (class 1259 OID 1199677)
-- Dependencies: 3244 167 8
-- Name: radacct_voip_stop_2014m09; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m09 (
    CONSTRAINT radacct_voip_stop_2014m09_connecttime_check CHECK (((connecttime >= '2014-09-01'::date) AND (connecttime < '2014-10-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m09 OWNER TO postgres;

--
-- TOC entry 202 (class 1259 OID 1199696)
-- Dependencies: 3256 167 8
-- Name: radacct_voip_stop_2014m10; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m10 (
    CONSTRAINT radacct_voip_stop_2014m10_connecttime_check CHECK (((connecttime >= '2014-10-01'::date) AND (connecttime < '2014-11-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m10 OWNER TO postgres;

--
-- TOC entry 203 (class 1259 OID 1199715)
-- Dependencies: 3268 8 167
-- Name: radacct_voip_stop_2014m11; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m11 (
    CONSTRAINT radacct_voip_stop_2014m11_connecttime_check CHECK (((connecttime >= '2014-11-01'::date) AND (connecttime < '2014-12-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m11 OWNER TO postgres;

--
-- TOC entry 204 (class 1259 OID 1199734)
-- Dependencies: 3280 8 167
-- Name: radacct_voip_stop_2014m12; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_2014m12 (
    CONSTRAINT radacct_voip_stop_2014m12_connecttime_check CHECK (((connecttime >= '2014-12-01'::date) AND (connecttime < '2015-01-01'::date)))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_2014m12 OWNER TO postgres;

--
-- TOC entry 205 (class 1259 OID 1199753)
-- Dependencies: 3292 167 8
-- Name: radacct_voip_stop_max; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE radacct_voip_stop_max (
    CONSTRAINT radacct_voip_stop_max_connecttime_check CHECK ((connecttime >= '2015-01-01'::date))
)
INHERITS (radacct_voip_stop);


ALTER TABLE public.radacct_voip_stop_max OWNER TO postgres;

--
-- TOC entry 168 (class 1259 OID 1199067)
-- Dependencies: 167 8
-- Name: radacct_voip_stop_radacctid_seq; Type: SEQUENCE; Schema: public; Owner: alexv
--

CREATE SEQUENCE radacct_voip_stop_radacctid_seq
    START WITH 100000001
    INCREMENT BY 1
    MINVALUE 100000001
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.radacct_voip_stop_radacctid_seq OWNER TO alexv;

--
-- TOC entry 3694 (class 0 OID 0)
-- Dependencies: 168
-- Name: radacct_voip_stop_radacctid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: alexv
--

ALTER SEQUENCE radacct_voip_stop_radacctid_seq OWNED BY radacct_voip_stop.radacctid;


--
-- TOC entry 164 (class 1259 OID 1153366)
-- Dependencies: 8
-- Name: test; Type: TABLE; Schema: public; Owner: gradius; Tablespace: 
--

CREATE TABLE test (
    id character varying,
    name character varying,
    "time" timestamp without time zone
);


ALTER TABLE public.test OWNER TO gradius;

--
-- TOC entry 2828 (class 2604 OID 1156265)
-- Dependencies: 166 165 166
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: alexv
--

ALTER TABLE ONLY radacct_voip_start ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_start_radacctid_seq'::regclass);


--
-- TOC entry 2847 (class 2604 OID 1199772)
-- Dependencies: 168 167
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: alexv
--

ALTER TABLE ONLY radacct_voip_stop ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2858 (class 2604 OID 1199773)
-- Dependencies: 169 169 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2848 (class 2604 OID 1199072)
-- Dependencies: 169 169
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2849 (class 2604 OID 1199073)
-- Dependencies: 169 169
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2850 (class 2604 OID 1199074)
-- Dependencies: 169 169
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2851 (class 2604 OID 1199075)
-- Dependencies: 169 169
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2852 (class 2604 OID 1199076)
-- Dependencies: 169 169
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2853 (class 2604 OID 1199077)
-- Dependencies: 169 169
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2854 (class 2604 OID 1199078)
-- Dependencies: 169 169
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2855 (class 2604 OID 1199079)
-- Dependencies: 169 169
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2856 (class 2604 OID 1199080)
-- Dependencies: 169 169
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2857 (class 2604 OID 1199081)
-- Dependencies: 169 169
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m01 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2870 (class 2604 OID 1199774)
-- Dependencies: 170 168 170
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2860 (class 2604 OID 1199091)
-- Dependencies: 170 170
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2861 (class 2604 OID 1199092)
-- Dependencies: 170 170
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2862 (class 2604 OID 1199093)
-- Dependencies: 170 170
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2863 (class 2604 OID 1199094)
-- Dependencies: 170 170
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2864 (class 2604 OID 1199095)
-- Dependencies: 170 170
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2865 (class 2604 OID 1199096)
-- Dependencies: 170 170
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2866 (class 2604 OID 1199097)
-- Dependencies: 170 170
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2867 (class 2604 OID 1199098)
-- Dependencies: 170 170
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2868 (class 2604 OID 1199099)
-- Dependencies: 170 170
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2869 (class 2604 OID 1199100)
-- Dependencies: 170 170
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: eivanov
--

ALTER TABLE ONLY radacct_voip_stop_2012m02 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2882 (class 2604 OID 1199775)
-- Dependencies: 171 168 171
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2872 (class 2604 OID 1199110)
-- Dependencies: 171 171
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2873 (class 2604 OID 1199111)
-- Dependencies: 171 171
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2874 (class 2604 OID 1199112)
-- Dependencies: 171 171
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2875 (class 2604 OID 1199113)
-- Dependencies: 171 171
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2876 (class 2604 OID 1199114)
-- Dependencies: 171 171
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2877 (class 2604 OID 1199115)
-- Dependencies: 171 171
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2878 (class 2604 OID 1199116)
-- Dependencies: 171 171
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2879 (class 2604 OID 1199117)
-- Dependencies: 171 171
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2880 (class 2604 OID 1199118)
-- Dependencies: 171 171
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2881 (class 2604 OID 1199119)
-- Dependencies: 171 171
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m03 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2894 (class 2604 OID 1199776)
-- Dependencies: 172 168 172
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2884 (class 2604 OID 1199129)
-- Dependencies: 172 172
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2885 (class 2604 OID 1199130)
-- Dependencies: 172 172
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2886 (class 2604 OID 1199131)
-- Dependencies: 172 172
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2887 (class 2604 OID 1199132)
-- Dependencies: 172 172
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2888 (class 2604 OID 1199133)
-- Dependencies: 172 172
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2889 (class 2604 OID 1199134)
-- Dependencies: 172 172
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2890 (class 2604 OID 1199135)
-- Dependencies: 172 172
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2891 (class 2604 OID 1199136)
-- Dependencies: 172 172
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2892 (class 2604 OID 1199137)
-- Dependencies: 172 172
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2893 (class 2604 OID 1199138)
-- Dependencies: 172 172
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m04 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2906 (class 2604 OID 1199777)
-- Dependencies: 173 168 173
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2896 (class 2604 OID 1199148)
-- Dependencies: 173 173
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2897 (class 2604 OID 1199149)
-- Dependencies: 173 173
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2898 (class 2604 OID 1199150)
-- Dependencies: 173 173
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2899 (class 2604 OID 1199151)
-- Dependencies: 173 173
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2900 (class 2604 OID 1199152)
-- Dependencies: 173 173
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2901 (class 2604 OID 1199153)
-- Dependencies: 173 173
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2902 (class 2604 OID 1199154)
-- Dependencies: 173 173
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2903 (class 2604 OID 1199155)
-- Dependencies: 173 173
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2904 (class 2604 OID 1199156)
-- Dependencies: 173 173
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2905 (class 2604 OID 1199157)
-- Dependencies: 173 173
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m05 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2918 (class 2604 OID 1199778)
-- Dependencies: 174 174 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2908 (class 2604 OID 1199167)
-- Dependencies: 174 174
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2909 (class 2604 OID 1199168)
-- Dependencies: 174 174
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2910 (class 2604 OID 1199169)
-- Dependencies: 174 174
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2911 (class 2604 OID 1199170)
-- Dependencies: 174 174
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2912 (class 2604 OID 1199171)
-- Dependencies: 174 174
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2913 (class 2604 OID 1199172)
-- Dependencies: 174 174
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2914 (class 2604 OID 1199173)
-- Dependencies: 174 174
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2915 (class 2604 OID 1199174)
-- Dependencies: 174 174
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2916 (class 2604 OID 1199175)
-- Dependencies: 174 174
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2917 (class 2604 OID 1199176)
-- Dependencies: 174 174
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m06 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2930 (class 2604 OID 1199779)
-- Dependencies: 175 175 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2920 (class 2604 OID 1199186)
-- Dependencies: 175 175
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2921 (class 2604 OID 1199187)
-- Dependencies: 175 175
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2922 (class 2604 OID 1199188)
-- Dependencies: 175 175
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2923 (class 2604 OID 1199189)
-- Dependencies: 175 175
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2924 (class 2604 OID 1199190)
-- Dependencies: 175 175
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2925 (class 2604 OID 1199191)
-- Dependencies: 175 175
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2926 (class 2604 OID 1199192)
-- Dependencies: 175 175
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2927 (class 2604 OID 1199193)
-- Dependencies: 175 175
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2928 (class 2604 OID 1199194)
-- Dependencies: 175 175
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2929 (class 2604 OID 1199195)
-- Dependencies: 175 175
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m07 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2942 (class 2604 OID 1199780)
-- Dependencies: 176 168 176
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2932 (class 2604 OID 1199205)
-- Dependencies: 176 176
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2933 (class 2604 OID 1199206)
-- Dependencies: 176 176
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2934 (class 2604 OID 1199207)
-- Dependencies: 176 176
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2935 (class 2604 OID 1199208)
-- Dependencies: 176 176
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2936 (class 2604 OID 1199209)
-- Dependencies: 176 176
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2937 (class 2604 OID 1199210)
-- Dependencies: 176 176
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2938 (class 2604 OID 1199211)
-- Dependencies: 176 176
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2939 (class 2604 OID 1199212)
-- Dependencies: 176 176
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2940 (class 2604 OID 1199213)
-- Dependencies: 176 176
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2941 (class 2604 OID 1199214)
-- Dependencies: 176 176
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m08 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2954 (class 2604 OID 1199781)
-- Dependencies: 177 168 177
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2944 (class 2604 OID 1199224)
-- Dependencies: 177 177
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2945 (class 2604 OID 1199225)
-- Dependencies: 177 177
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2946 (class 2604 OID 1199226)
-- Dependencies: 177 177
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2947 (class 2604 OID 1199227)
-- Dependencies: 177 177
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2948 (class 2604 OID 1199228)
-- Dependencies: 177 177
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2949 (class 2604 OID 1199229)
-- Dependencies: 177 177
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2950 (class 2604 OID 1199230)
-- Dependencies: 177 177
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2951 (class 2604 OID 1199231)
-- Dependencies: 177 177
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2952 (class 2604 OID 1199232)
-- Dependencies: 177 177
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2953 (class 2604 OID 1199233)
-- Dependencies: 177 177
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m09 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2966 (class 2604 OID 1199782)
-- Dependencies: 178 178 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2956 (class 2604 OID 1199243)
-- Dependencies: 178 178
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2957 (class 2604 OID 1199244)
-- Dependencies: 178 178
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2958 (class 2604 OID 1199245)
-- Dependencies: 178 178
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2959 (class 2604 OID 1199246)
-- Dependencies: 178 178
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2960 (class 2604 OID 1199247)
-- Dependencies: 178 178
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2961 (class 2604 OID 1199248)
-- Dependencies: 178 178
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2962 (class 2604 OID 1199249)
-- Dependencies: 178 178
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2963 (class 2604 OID 1199250)
-- Dependencies: 178 178
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2964 (class 2604 OID 1199251)
-- Dependencies: 178 178
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2965 (class 2604 OID 1199252)
-- Dependencies: 178 178
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m10 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2978 (class 2604 OID 1199783)
-- Dependencies: 179 179 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2968 (class 2604 OID 1199262)
-- Dependencies: 179 179
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2969 (class 2604 OID 1199263)
-- Dependencies: 179 179
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2970 (class 2604 OID 1199264)
-- Dependencies: 179 179
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2971 (class 2604 OID 1199265)
-- Dependencies: 179 179
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2972 (class 2604 OID 1199266)
-- Dependencies: 179 179
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2973 (class 2604 OID 1199267)
-- Dependencies: 179 179
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2974 (class 2604 OID 1199268)
-- Dependencies: 179 179
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2975 (class 2604 OID 1199269)
-- Dependencies: 179 179
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2976 (class 2604 OID 1199270)
-- Dependencies: 179 179
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2977 (class 2604 OID 1199271)
-- Dependencies: 179 179
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m11 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 2990 (class 2604 OID 1199784)
-- Dependencies: 180 180 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2980 (class 2604 OID 1199281)
-- Dependencies: 180 180
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2981 (class 2604 OID 1199282)
-- Dependencies: 180 180
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2982 (class 2604 OID 1199283)
-- Dependencies: 180 180
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2983 (class 2604 OID 1199284)
-- Dependencies: 180 180
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2984 (class 2604 OID 1199285)
-- Dependencies: 180 180
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2985 (class 2604 OID 1199286)
-- Dependencies: 180 180
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2986 (class 2604 OID 1199287)
-- Dependencies: 180 180
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2987 (class 2604 OID 1199288)
-- Dependencies: 180 180
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 2988 (class 2604 OID 1199289)
-- Dependencies: 180 180
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 2989 (class 2604 OID 1199290)
-- Dependencies: 180 180
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2012m12 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3002 (class 2604 OID 1199785)
-- Dependencies: 181 168 181
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 2992 (class 2604 OID 1199300)
-- Dependencies: 181 181
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 2993 (class 2604 OID 1199301)
-- Dependencies: 181 181
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 2994 (class 2604 OID 1199302)
-- Dependencies: 181 181
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 2995 (class 2604 OID 1199303)
-- Dependencies: 181 181
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 2996 (class 2604 OID 1199304)
-- Dependencies: 181 181
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 2997 (class 2604 OID 1199305)
-- Dependencies: 181 181
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 2998 (class 2604 OID 1199306)
-- Dependencies: 181 181
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 2999 (class 2604 OID 1199307)
-- Dependencies: 181 181
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3000 (class 2604 OID 1199308)
-- Dependencies: 181 181
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3001 (class 2604 OID 1199309)
-- Dependencies: 181 181
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m01 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3014 (class 2604 OID 1199786)
-- Dependencies: 182 182 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3004 (class 2604 OID 1199319)
-- Dependencies: 182 182
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3005 (class 2604 OID 1199320)
-- Dependencies: 182 182
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3006 (class 2604 OID 1199321)
-- Dependencies: 182 182
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3007 (class 2604 OID 1199322)
-- Dependencies: 182 182
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3008 (class 2604 OID 1199323)
-- Dependencies: 182 182
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3009 (class 2604 OID 1199324)
-- Dependencies: 182 182
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3010 (class 2604 OID 1199325)
-- Dependencies: 182 182
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3011 (class 2604 OID 1199326)
-- Dependencies: 182 182
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3012 (class 2604 OID 1199327)
-- Dependencies: 182 182
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3013 (class 2604 OID 1199328)
-- Dependencies: 182 182
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m02 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3026 (class 2604 OID 1199787)
-- Dependencies: 183 183 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3016 (class 2604 OID 1199338)
-- Dependencies: 183 183
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3017 (class 2604 OID 1199339)
-- Dependencies: 183 183
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3018 (class 2604 OID 1199340)
-- Dependencies: 183 183
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3019 (class 2604 OID 1199341)
-- Dependencies: 183 183
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3020 (class 2604 OID 1199342)
-- Dependencies: 183 183
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3021 (class 2604 OID 1199343)
-- Dependencies: 183 183
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3022 (class 2604 OID 1199344)
-- Dependencies: 183 183
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3023 (class 2604 OID 1199345)
-- Dependencies: 183 183
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3024 (class 2604 OID 1199346)
-- Dependencies: 183 183
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3025 (class 2604 OID 1199347)
-- Dependencies: 183 183
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m03 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3038 (class 2604 OID 1199788)
-- Dependencies: 184 168 184
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3028 (class 2604 OID 1199357)
-- Dependencies: 184 184
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3029 (class 2604 OID 1199358)
-- Dependencies: 184 184
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3030 (class 2604 OID 1199359)
-- Dependencies: 184 184
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3031 (class 2604 OID 1199360)
-- Dependencies: 184 184
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3032 (class 2604 OID 1199361)
-- Dependencies: 184 184
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3033 (class 2604 OID 1199362)
-- Dependencies: 184 184
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3034 (class 2604 OID 1199363)
-- Dependencies: 184 184
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3035 (class 2604 OID 1199364)
-- Dependencies: 184 184
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3036 (class 2604 OID 1199365)
-- Dependencies: 184 184
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3037 (class 2604 OID 1199366)
-- Dependencies: 184 184
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m04 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3050 (class 2604 OID 1199789)
-- Dependencies: 185 185 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3040 (class 2604 OID 1199376)
-- Dependencies: 185 185
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3041 (class 2604 OID 1199377)
-- Dependencies: 185 185
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3042 (class 2604 OID 1199378)
-- Dependencies: 185 185
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3043 (class 2604 OID 1199379)
-- Dependencies: 185 185
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3044 (class 2604 OID 1199380)
-- Dependencies: 185 185
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3045 (class 2604 OID 1199381)
-- Dependencies: 185 185
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3046 (class 2604 OID 1199382)
-- Dependencies: 185 185
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3047 (class 2604 OID 1199383)
-- Dependencies: 185 185
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3048 (class 2604 OID 1199384)
-- Dependencies: 185 185
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3049 (class 2604 OID 1199385)
-- Dependencies: 185 185
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m05 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3062 (class 2604 OID 1199790)
-- Dependencies: 186 168 186
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3052 (class 2604 OID 1199395)
-- Dependencies: 186 186
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3053 (class 2604 OID 1199396)
-- Dependencies: 186 186
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3054 (class 2604 OID 1199397)
-- Dependencies: 186 186
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3055 (class 2604 OID 1199398)
-- Dependencies: 186 186
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3056 (class 2604 OID 1199399)
-- Dependencies: 186 186
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3057 (class 2604 OID 1199400)
-- Dependencies: 186 186
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3058 (class 2604 OID 1199401)
-- Dependencies: 186 186
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3059 (class 2604 OID 1199402)
-- Dependencies: 186 186
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3060 (class 2604 OID 1199403)
-- Dependencies: 186 186
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3061 (class 2604 OID 1199404)
-- Dependencies: 186 186
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m06 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3074 (class 2604 OID 1199791)
-- Dependencies: 187 168 187
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3064 (class 2604 OID 1199414)
-- Dependencies: 187 187
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3065 (class 2604 OID 1199415)
-- Dependencies: 187 187
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3066 (class 2604 OID 1199416)
-- Dependencies: 187 187
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3067 (class 2604 OID 1199417)
-- Dependencies: 187 187
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3068 (class 2604 OID 1199418)
-- Dependencies: 187 187
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3069 (class 2604 OID 1199419)
-- Dependencies: 187 187
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3070 (class 2604 OID 1199420)
-- Dependencies: 187 187
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3071 (class 2604 OID 1199421)
-- Dependencies: 187 187
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3072 (class 2604 OID 1199422)
-- Dependencies: 187 187
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3073 (class 2604 OID 1199423)
-- Dependencies: 187 187
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m07 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3086 (class 2604 OID 1199792)
-- Dependencies: 188 188 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3076 (class 2604 OID 1199433)
-- Dependencies: 188 188
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3077 (class 2604 OID 1199434)
-- Dependencies: 188 188
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3078 (class 2604 OID 1199435)
-- Dependencies: 188 188
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3079 (class 2604 OID 1199436)
-- Dependencies: 188 188
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3080 (class 2604 OID 1199437)
-- Dependencies: 188 188
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3081 (class 2604 OID 1199438)
-- Dependencies: 188 188
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3082 (class 2604 OID 1199439)
-- Dependencies: 188 188
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3083 (class 2604 OID 1199440)
-- Dependencies: 188 188
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3084 (class 2604 OID 1199441)
-- Dependencies: 188 188
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3085 (class 2604 OID 1199442)
-- Dependencies: 188 188
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m08 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3098 (class 2604 OID 1199793)
-- Dependencies: 189 189 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3088 (class 2604 OID 1199452)
-- Dependencies: 189 189
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3089 (class 2604 OID 1199453)
-- Dependencies: 189 189
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3090 (class 2604 OID 1199454)
-- Dependencies: 189 189
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3091 (class 2604 OID 1199455)
-- Dependencies: 189 189
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3092 (class 2604 OID 1199456)
-- Dependencies: 189 189
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3093 (class 2604 OID 1199457)
-- Dependencies: 189 189
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3094 (class 2604 OID 1199458)
-- Dependencies: 189 189
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3095 (class 2604 OID 1199459)
-- Dependencies: 189 189
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3096 (class 2604 OID 1199460)
-- Dependencies: 189 189
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3097 (class 2604 OID 1199461)
-- Dependencies: 189 189
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m09 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3111 (class 2604 OID 1199794)
-- Dependencies: 190 168 190
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3101 (class 2604 OID 1199471)
-- Dependencies: 190 190
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3102 (class 2604 OID 1199472)
-- Dependencies: 190 190
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3103 (class 2604 OID 1199473)
-- Dependencies: 190 190
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3104 (class 2604 OID 1199474)
-- Dependencies: 190 190
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3105 (class 2604 OID 1199475)
-- Dependencies: 190 190
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3106 (class 2604 OID 1199476)
-- Dependencies: 190 190
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3107 (class 2604 OID 1199477)
-- Dependencies: 190 190
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3108 (class 2604 OID 1199478)
-- Dependencies: 190 190
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3109 (class 2604 OID 1199479)
-- Dependencies: 190 190
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3110 (class 2604 OID 1199480)
-- Dependencies: 190 190
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m10 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3123 (class 2604 OID 1199795)
-- Dependencies: 191 168 191
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3113 (class 2604 OID 1199490)
-- Dependencies: 191 191
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3114 (class 2604 OID 1199491)
-- Dependencies: 191 191
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3115 (class 2604 OID 1199492)
-- Dependencies: 191 191
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3116 (class 2604 OID 1199493)
-- Dependencies: 191 191
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3117 (class 2604 OID 1199494)
-- Dependencies: 191 191
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3118 (class 2604 OID 1199495)
-- Dependencies: 191 191
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3119 (class 2604 OID 1199496)
-- Dependencies: 191 191
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3120 (class 2604 OID 1199497)
-- Dependencies: 191 191
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3121 (class 2604 OID 1199498)
-- Dependencies: 191 191
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3122 (class 2604 OID 1199499)
-- Dependencies: 191 191
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m11 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3135 (class 2604 OID 1199796)
-- Dependencies: 192 168 192
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3126 (class 2604 OID 1199509)
-- Dependencies: 192 192
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3125 (class 2604 OID 1199510)
-- Dependencies: 192 192
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3127 (class 2604 OID 1199511)
-- Dependencies: 192 192
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3128 (class 2604 OID 1199512)
-- Dependencies: 192 192
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3129 (class 2604 OID 1199513)
-- Dependencies: 192 192
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3130 (class 2604 OID 1199514)
-- Dependencies: 192 192
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3131 (class 2604 OID 1199515)
-- Dependencies: 192 192
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3132 (class 2604 OID 1199516)
-- Dependencies: 192 192
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3133 (class 2604 OID 1199517)
-- Dependencies: 192 192
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3134 (class 2604 OID 1199518)
-- Dependencies: 192 192
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2013m12 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3147 (class 2604 OID 1199797)
-- Dependencies: 193 193 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3137 (class 2604 OID 1199528)
-- Dependencies: 193 193
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3138 (class 2604 OID 1199529)
-- Dependencies: 193 193
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3139 (class 2604 OID 1199530)
-- Dependencies: 193 193
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3140 (class 2604 OID 1199531)
-- Dependencies: 193 193
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3141 (class 2604 OID 1199532)
-- Dependencies: 193 193
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3142 (class 2604 OID 1199533)
-- Dependencies: 193 193
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3143 (class 2604 OID 1199534)
-- Dependencies: 193 193
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3144 (class 2604 OID 1199535)
-- Dependencies: 193 193
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3145 (class 2604 OID 1199536)
-- Dependencies: 193 193
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3146 (class 2604 OID 1199537)
-- Dependencies: 193 193
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m01 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3159 (class 2604 OID 1199798)
-- Dependencies: 194 194 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3149 (class 2604 OID 1199547)
-- Dependencies: 194 194
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3150 (class 2604 OID 1199548)
-- Dependencies: 194 194
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3151 (class 2604 OID 1199549)
-- Dependencies: 194 194
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3152 (class 2604 OID 1199550)
-- Dependencies: 194 194
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3153 (class 2604 OID 1199551)
-- Dependencies: 194 194
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3154 (class 2604 OID 1199552)
-- Dependencies: 194 194
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3155 (class 2604 OID 1199553)
-- Dependencies: 194 194
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3156 (class 2604 OID 1199554)
-- Dependencies: 194 194
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3157 (class 2604 OID 1199555)
-- Dependencies: 194 194
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3158 (class 2604 OID 1199556)
-- Dependencies: 194 194
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m02 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3171 (class 2604 OID 1199799)
-- Dependencies: 195 168 195
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3161 (class 2604 OID 1199566)
-- Dependencies: 195 195
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3162 (class 2604 OID 1199567)
-- Dependencies: 195 195
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3163 (class 2604 OID 1199568)
-- Dependencies: 195 195
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3164 (class 2604 OID 1199569)
-- Dependencies: 195 195
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3165 (class 2604 OID 1199570)
-- Dependencies: 195 195
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3166 (class 2604 OID 1199571)
-- Dependencies: 195 195
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3167 (class 2604 OID 1199572)
-- Dependencies: 195 195
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3168 (class 2604 OID 1199573)
-- Dependencies: 195 195
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3169 (class 2604 OID 1199574)
-- Dependencies: 195 195
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3170 (class 2604 OID 1199575)
-- Dependencies: 195 195
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m03 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3183 (class 2604 OID 1199800)
-- Dependencies: 196 168 196
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3173 (class 2604 OID 1199585)
-- Dependencies: 196 196
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3174 (class 2604 OID 1199586)
-- Dependencies: 196 196
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3175 (class 2604 OID 1199587)
-- Dependencies: 196 196
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3176 (class 2604 OID 1199588)
-- Dependencies: 196 196
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3177 (class 2604 OID 1199589)
-- Dependencies: 196 196
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3178 (class 2604 OID 1199590)
-- Dependencies: 196 196
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3179 (class 2604 OID 1199591)
-- Dependencies: 196 196
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3180 (class 2604 OID 1199592)
-- Dependencies: 196 196
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3181 (class 2604 OID 1199593)
-- Dependencies: 196 196
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3182 (class 2604 OID 1199594)
-- Dependencies: 196 196
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m04 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3195 (class 2604 OID 1199801)
-- Dependencies: 197 168 197
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3185 (class 2604 OID 1199604)
-- Dependencies: 197 197
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3186 (class 2604 OID 1199605)
-- Dependencies: 197 197
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3187 (class 2604 OID 1199606)
-- Dependencies: 197 197
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3188 (class 2604 OID 1199607)
-- Dependencies: 197 197
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3189 (class 2604 OID 1199608)
-- Dependencies: 197 197
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3190 (class 2604 OID 1199609)
-- Dependencies: 197 197
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3191 (class 2604 OID 1199610)
-- Dependencies: 197 197
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3192 (class 2604 OID 1199611)
-- Dependencies: 197 197
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3193 (class 2604 OID 1199612)
-- Dependencies: 197 197
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3194 (class 2604 OID 1199613)
-- Dependencies: 197 197
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m05 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3207 (class 2604 OID 1199802)
-- Dependencies: 198 198 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3197 (class 2604 OID 1199623)
-- Dependencies: 198 198
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3198 (class 2604 OID 1199624)
-- Dependencies: 198 198
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3199 (class 2604 OID 1199625)
-- Dependencies: 198 198
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3200 (class 2604 OID 1199626)
-- Dependencies: 198 198
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3201 (class 2604 OID 1199627)
-- Dependencies: 198 198
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3202 (class 2604 OID 1199628)
-- Dependencies: 198 198
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3203 (class 2604 OID 1199629)
-- Dependencies: 198 198
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3204 (class 2604 OID 1199630)
-- Dependencies: 198 198
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3205 (class 2604 OID 1199631)
-- Dependencies: 198 198
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3206 (class 2604 OID 1199632)
-- Dependencies: 198 198
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m06 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3219 (class 2604 OID 1199803)
-- Dependencies: 199 199 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3209 (class 2604 OID 1199642)
-- Dependencies: 199 199
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3210 (class 2604 OID 1199643)
-- Dependencies: 199 199
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3211 (class 2604 OID 1199644)
-- Dependencies: 199 199
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3212 (class 2604 OID 1199645)
-- Dependencies: 199 199
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3213 (class 2604 OID 1199646)
-- Dependencies: 199 199
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3214 (class 2604 OID 1199647)
-- Dependencies: 199 199
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3215 (class 2604 OID 1199648)
-- Dependencies: 199 199
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3216 (class 2604 OID 1199649)
-- Dependencies: 199 199
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3217 (class 2604 OID 1199650)
-- Dependencies: 199 199
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3218 (class 2604 OID 1199651)
-- Dependencies: 199 199
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m07 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3231 (class 2604 OID 1199804)
-- Dependencies: 200 168 200
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3221 (class 2604 OID 1199661)
-- Dependencies: 200 200
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3222 (class 2604 OID 1199662)
-- Dependencies: 200 200
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3223 (class 2604 OID 1199663)
-- Dependencies: 200 200
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3224 (class 2604 OID 1199664)
-- Dependencies: 200 200
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3225 (class 2604 OID 1199665)
-- Dependencies: 200 200
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3226 (class 2604 OID 1199666)
-- Dependencies: 200 200
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3227 (class 2604 OID 1199667)
-- Dependencies: 200 200
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3228 (class 2604 OID 1199668)
-- Dependencies: 200 200
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3229 (class 2604 OID 1199669)
-- Dependencies: 200 200
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3230 (class 2604 OID 1199670)
-- Dependencies: 200 200
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m08 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3243 (class 2604 OID 1199805)
-- Dependencies: 201 168 201
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3233 (class 2604 OID 1199680)
-- Dependencies: 201 201
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3234 (class 2604 OID 1199681)
-- Dependencies: 201 201
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3235 (class 2604 OID 1199682)
-- Dependencies: 201 201
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3236 (class 2604 OID 1199683)
-- Dependencies: 201 201
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3237 (class 2604 OID 1199684)
-- Dependencies: 201 201
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3238 (class 2604 OID 1199685)
-- Dependencies: 201 201
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3239 (class 2604 OID 1199686)
-- Dependencies: 201 201
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3240 (class 2604 OID 1199687)
-- Dependencies: 201 201
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3241 (class 2604 OID 1199688)
-- Dependencies: 201 201
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3242 (class 2604 OID 1199689)
-- Dependencies: 201 201
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m09 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3255 (class 2604 OID 1199806)
-- Dependencies: 202 168 202
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3245 (class 2604 OID 1199699)
-- Dependencies: 202 202
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3246 (class 2604 OID 1199700)
-- Dependencies: 202 202
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3247 (class 2604 OID 1199701)
-- Dependencies: 202 202
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3248 (class 2604 OID 1199702)
-- Dependencies: 202 202
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3249 (class 2604 OID 1199703)
-- Dependencies: 202 202
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3250 (class 2604 OID 1199704)
-- Dependencies: 202 202
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3251 (class 2604 OID 1199705)
-- Dependencies: 202 202
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3252 (class 2604 OID 1199706)
-- Dependencies: 202 202
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3253 (class 2604 OID 1199707)
-- Dependencies: 202 202
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3254 (class 2604 OID 1199708)
-- Dependencies: 202 202
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m10 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3267 (class 2604 OID 1199807)
-- Dependencies: 203 203 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3257 (class 2604 OID 1199718)
-- Dependencies: 203 203
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3258 (class 2604 OID 1199719)
-- Dependencies: 203 203
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3259 (class 2604 OID 1199720)
-- Dependencies: 203 203
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3260 (class 2604 OID 1199721)
-- Dependencies: 203 203
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3261 (class 2604 OID 1199722)
-- Dependencies: 203 203
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3262 (class 2604 OID 1199723)
-- Dependencies: 203 203
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3263 (class 2604 OID 1199724)
-- Dependencies: 203 203
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3264 (class 2604 OID 1199725)
-- Dependencies: 203 203
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3265 (class 2604 OID 1199726)
-- Dependencies: 203 203
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3266 (class 2604 OID 1199727)
-- Dependencies: 203 203
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m11 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3279 (class 2604 OID 1199808)
-- Dependencies: 204 168 204
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3269 (class 2604 OID 1199737)
-- Dependencies: 204 204
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3270 (class 2604 OID 1199738)
-- Dependencies: 204 204
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3271 (class 2604 OID 1199739)
-- Dependencies: 204 204
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3272 (class 2604 OID 1199740)
-- Dependencies: 204 204
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3273 (class 2604 OID 1199741)
-- Dependencies: 204 204
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3274 (class 2604 OID 1199742)
-- Dependencies: 204 204
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3275 (class 2604 OID 1199743)
-- Dependencies: 204 204
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3276 (class 2604 OID 1199744)
-- Dependencies: 204 204
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3277 (class 2604 OID 1199745)
-- Dependencies: 204 204
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3278 (class 2604 OID 1199746)
-- Dependencies: 204 204
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_2014m12 ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3291 (class 2604 OID 1199809)
-- Dependencies: 205 205 168
-- Name: radacctid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN radacctid SET DEFAULT nextval('radacct_voip_stop_radacctid_seq'::regclass);


--
-- TOC entry 3281 (class 2604 OID 1199756)
-- Dependencies: 205 205
-- Name: acctsessionid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN acctsessionid SET DEFAULT 0;


--
-- TOC entry 3282 (class 2604 OID 1199757)
-- Dependencies: 205 205
-- Name: acctsessiontime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN acctsessiontime SET DEFAULT 0;


--
-- TOC entry 3283 (class 2604 OID 1199758)
-- Dependencies: 205 205
-- Name: acctdelaytime; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN acctdelaytime SET DEFAULT 0;


--
-- TOC entry 3284 (class 2604 OID 1199759)
-- Dependencies: 205 205
-- Name: disconnectcause; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN disconnectcause SET DEFAULT 0;


--
-- TOC entry 3285 (class 2604 OID 1199760)
-- Dependencies: 205 205
-- Name: prefix_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN prefix_oper SET DEFAULT 0;


--
-- TOC entry 3286 (class 2604 OID 1199761)
-- Dependencies: 205 205
-- Name: prefix_des; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN prefix_des SET DEFAULT 0;


--
-- TOC entry 3287 (class 2604 OID 1199762)
-- Dependencies: 205 205
-- Name: in_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN in_oper SET DEFAULT 0;


--
-- TOC entry 3288 (class 2604 OID 1199763)
-- Dependencies: 205 205
-- Name: out_oper; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN out_oper SET DEFAULT 0;


--
-- TOC entry 3289 (class 2604 OID 1199764)
-- Dependencies: 205 205
-- Name: incd_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN incd_noa SET DEFAULT 0;


--
-- TOC entry 3290 (class 2604 OID 1199765)
-- Dependencies: 205 205
-- Name: incg_noa; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY radacct_voip_stop_max ALTER COLUMN incg_noa SET DEFAULT 0;


--
-- TOC entry 3296 (class 2606 OID 1156278)
-- Dependencies: 166 166 3636
-- Name: radacct_voip_start_pkey; Type: CONSTRAINT; Schema: public; Owner: alexv; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_start
    ADD CONSTRAINT radacct_voip_start_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3305 (class 2606 OID 1199811)
-- Dependencies: 169 169 3636
-- Name: radacct_voip_stop_2012m01_pkey; Type: CONSTRAINT; Schema: public; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m01
    ADD CONSTRAINT radacct_voip_stop_2012m01_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3312 (class 2606 OID 1199813)
-- Dependencies: 170 170 3636
-- Name: radacct_voip_stop_2012m02_pkey; Type: CONSTRAINT; Schema: public; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m02
    ADD CONSTRAINT radacct_voip_stop_2012m02_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3319 (class 2606 OID 1199815)
-- Dependencies: 171 171 3636
-- Name: radacct_voip_stop_2012m03_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m03
    ADD CONSTRAINT radacct_voip_stop_2012m03_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3326 (class 2606 OID 1199817)
-- Dependencies: 172 172 3636
-- Name: radacct_voip_stop_2012m04_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m04
    ADD CONSTRAINT radacct_voip_stop_2012m04_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3333 (class 2606 OID 1199819)
-- Dependencies: 173 173 3636
-- Name: radacct_voip_stop_2012m05_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m05
    ADD CONSTRAINT radacct_voip_stop_2012m05_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3340 (class 2606 OID 1199821)
-- Dependencies: 174 174 3636
-- Name: radacct_voip_stop_2012m06_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m06
    ADD CONSTRAINT radacct_voip_stop_2012m06_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3347 (class 2606 OID 1199823)
-- Dependencies: 175 175 3636
-- Name: radacct_voip_stop_2012m07_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m07
    ADD CONSTRAINT radacct_voip_stop_2012m07_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3354 (class 2606 OID 1199825)
-- Dependencies: 176 176 3636
-- Name: radacct_voip_stop_2012m08_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m08
    ADD CONSTRAINT radacct_voip_stop_2012m08_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3361 (class 2606 OID 1199827)
-- Dependencies: 177 177 3636
-- Name: radacct_voip_stop_2012m09_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m09
    ADD CONSTRAINT radacct_voip_stop_2012m09_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3368 (class 2606 OID 1199829)
-- Dependencies: 178 178 3636
-- Name: radacct_voip_stop_2012m10_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m10
    ADD CONSTRAINT radacct_voip_stop_2012m10_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3375 (class 2606 OID 1199831)
-- Dependencies: 179 179 3636
-- Name: radacct_voip_stop_2012m11_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m11
    ADD CONSTRAINT radacct_voip_stop_2012m11_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3382 (class 2606 OID 1199833)
-- Dependencies: 180 180 3636
-- Name: radacct_voip_stop_2012m12_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2012m12
    ADD CONSTRAINT radacct_voip_stop_2012m12_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3388 (class 2606 OID 1199835)
-- Dependencies: 181 181 3636
-- Name: radacct_voip_stop_2013m01_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m01
    ADD CONSTRAINT radacct_voip_stop_2013m01_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3394 (class 2606 OID 1199837)
-- Dependencies: 182 182 3636
-- Name: radacct_voip_stop_2013m02_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m02
    ADD CONSTRAINT radacct_voip_stop_2013m02_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3400 (class 2606 OID 1199839)
-- Dependencies: 183 183 3636
-- Name: radacct_voip_stop_2013m03_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m03
    ADD CONSTRAINT radacct_voip_stop_2013m03_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3406 (class 2606 OID 1199841)
-- Dependencies: 184 184 3636
-- Name: radacct_voip_stop_2013m04_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m04
    ADD CONSTRAINT radacct_voip_stop_2013m04_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3412 (class 2606 OID 1199843)
-- Dependencies: 185 185 3636
-- Name: radacct_voip_stop_2013m05_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m05
    ADD CONSTRAINT radacct_voip_stop_2013m05_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3418 (class 2606 OID 1199845)
-- Dependencies: 186 186 3636
-- Name: radacct_voip_stop_2013m06_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m06
    ADD CONSTRAINT radacct_voip_stop_2013m06_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3424 (class 2606 OID 1199847)
-- Dependencies: 187 187 3636
-- Name: radacct_voip_stop_2013m07_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m07
    ADD CONSTRAINT radacct_voip_stop_2013m07_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3430 (class 2606 OID 1199849)
-- Dependencies: 188 188 3636
-- Name: radacct_voip_stop_2013m08_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m08
    ADD CONSTRAINT radacct_voip_stop_2013m08_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3436 (class 2606 OID 1199851)
-- Dependencies: 189 189 3636
-- Name: radacct_voip_stop_2013m09_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m09
    ADD CONSTRAINT radacct_voip_stop_2013m09_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3442 (class 2606 OID 1199853)
-- Dependencies: 190 190 3636
-- Name: radacct_voip_stop_2013m10_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m10
    ADD CONSTRAINT radacct_voip_stop_2013m10_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3448 (class 2606 OID 1199855)
-- Dependencies: 191 191 3636
-- Name: radacct_voip_stop_2013m11_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m11
    ADD CONSTRAINT radacct_voip_stop_2013m11_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3454 (class 2606 OID 1199857)
-- Dependencies: 192 192 3636
-- Name: radacct_voip_stop_2013m12_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2013m12
    ADD CONSTRAINT radacct_voip_stop_2013m12_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3460 (class 2606 OID 1199859)
-- Dependencies: 193 193 3636
-- Name: radacct_voip_stop_2014m01_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m01
    ADD CONSTRAINT radacct_voip_stop_2014m01_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3466 (class 2606 OID 1199861)
-- Dependencies: 194 194 3636
-- Name: radacct_voip_stop_2014m02_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m02
    ADD CONSTRAINT radacct_voip_stop_2014m02_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3472 (class 2606 OID 1199863)
-- Dependencies: 195 195 3636
-- Name: radacct_voip_stop_2014m03_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m03
    ADD CONSTRAINT radacct_voip_stop_2014m03_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3478 (class 2606 OID 1199865)
-- Dependencies: 196 196 3636
-- Name: radacct_voip_stop_2014m04_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m04
    ADD CONSTRAINT radacct_voip_stop_2014m04_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3484 (class 2606 OID 1199867)
-- Dependencies: 197 197 3636
-- Name: radacct_voip_stop_2014m05_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m05
    ADD CONSTRAINT radacct_voip_stop_2014m05_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3490 (class 2606 OID 1199869)
-- Dependencies: 198 198 3636
-- Name: radacct_voip_stop_2014m06_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m06
    ADD CONSTRAINT radacct_voip_stop_2014m06_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3496 (class 2606 OID 1199871)
-- Dependencies: 199 199 3636
-- Name: radacct_voip_stop_2014m07_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m07
    ADD CONSTRAINT radacct_voip_stop_2014m07_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3502 (class 2606 OID 1199873)
-- Dependencies: 200 200 3636
-- Name: radacct_voip_stop_2014m08_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m08
    ADD CONSTRAINT radacct_voip_stop_2014m08_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3508 (class 2606 OID 1199875)
-- Dependencies: 201 201 3636
-- Name: radacct_voip_stop_2014m09_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m09
    ADD CONSTRAINT radacct_voip_stop_2014m09_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3514 (class 2606 OID 1199877)
-- Dependencies: 202 202 3636
-- Name: radacct_voip_stop_2014m10_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m10
    ADD CONSTRAINT radacct_voip_stop_2014m10_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3520 (class 2606 OID 1199879)
-- Dependencies: 203 203 3636
-- Name: radacct_voip_stop_2014m11_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m11
    ADD CONSTRAINT radacct_voip_stop_2014m11_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3526 (class 2606 OID 1199881)
-- Dependencies: 204 204 3636
-- Name: radacct_voip_stop_2014m12_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_2014m12
    ADD CONSTRAINT radacct_voip_stop_2014m12_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3532 (class 2606 OID 1199883)
-- Dependencies: 205 205 3636
-- Name: radacct_voip_stop_max_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop_max
    ADD CONSTRAINT radacct_voip_stop_max_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3298 (class 2606 OID 1199885)
-- Dependencies: 167 167 3636
-- Name: radacct_voip_stop_pkey; Type: CONSTRAINT; Schema: public; Owner: alexv; Tablespace: 
--

ALTER TABLE ONLY radacct_voip_stop
    ADD CONSTRAINT radacct_voip_stop_pkey PRIMARY KEY (radacctid);


--
-- TOC entry 3293 (class 1259 OID 1156279)
-- Dependencies: 166 3636
-- Name: ivoipstart_acctuniq; Type: INDEX; Schema: public; Owner: alexv; Tablespace: 
--

CREATE INDEX ivoipstart_acctuniq ON radacct_voip_start USING btree (acctuniqueid);


--
-- TOC entry 3294 (class 1259 OID 1156280)
-- Dependencies: 166 3636
-- Name: ivoipstart_contime; Type: INDEX; Schema: public; Owner: alexv; Tablespace: 
--

CREATE INDEX ivoipstart_contime ON radacct_voip_start USING btree (connecttime);


--
-- TOC entry 3299 (class 1259 OID 1199886)
-- Dependencies: 169 3636
-- Name: ivoipstop_called_2012m01; Type: INDEX; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m01 ON radacct_voip_stop_2012m01 USING btree (calledstationid);


--
-- TOC entry 3306 (class 1259 OID 1199887)
-- Dependencies: 170 3636
-- Name: ivoipstop_called_2012m02; Type: INDEX; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m02 ON radacct_voip_stop_2012m02 USING btree (calledstationid);


--
-- TOC entry 3313 (class 1259 OID 1199888)
-- Dependencies: 171 3636
-- Name: ivoipstop_called_2012m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m03 ON radacct_voip_stop_2012m03 USING btree (calledstationid);


--
-- TOC entry 3320 (class 1259 OID 1199889)
-- Dependencies: 172 3636
-- Name: ivoipstop_called_2012m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m04 ON radacct_voip_stop_2012m04 USING btree (calledstationid);


--
-- TOC entry 3327 (class 1259 OID 1199890)
-- Dependencies: 173 3636
-- Name: ivoipstop_called_2012m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m05 ON radacct_voip_stop_2012m05 USING btree (calledstationid);


--
-- TOC entry 3334 (class 1259 OID 1199891)
-- Dependencies: 174 3636
-- Name: ivoipstop_called_2012m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m06 ON radacct_voip_stop_2012m06 USING btree (calledstationid);


--
-- TOC entry 3341 (class 1259 OID 1199892)
-- Dependencies: 175 3636
-- Name: ivoipstop_called_2012m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m07 ON radacct_voip_stop_2012m07 USING btree (calledstationid);


--
-- TOC entry 3348 (class 1259 OID 1199893)
-- Dependencies: 176 3636
-- Name: ivoipstop_called_2012m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m08 ON radacct_voip_stop_2012m08 USING btree (calledstationid);


--
-- TOC entry 3355 (class 1259 OID 1199894)
-- Dependencies: 177 3636
-- Name: ivoipstop_called_2012m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m09 ON radacct_voip_stop_2012m09 USING btree (calledstationid);


--
-- TOC entry 3362 (class 1259 OID 1199895)
-- Dependencies: 178 3636
-- Name: ivoipstop_called_2012m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m10 ON radacct_voip_stop_2012m10 USING btree (calledstationid);


--
-- TOC entry 3369 (class 1259 OID 1199896)
-- Dependencies: 179 3636
-- Name: ivoipstop_called_2012m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m11 ON radacct_voip_stop_2012m11 USING btree (calledstationid);


--
-- TOC entry 3376 (class 1259 OID 1199897)
-- Dependencies: 180 3636
-- Name: ivoipstop_called_2012m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2012m12 ON radacct_voip_stop_2012m12 USING btree (calledstationid);


--
-- TOC entry 3383 (class 1259 OID 1199898)
-- Dependencies: 181 3636
-- Name: ivoipstop_called_2013m01; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m01 ON radacct_voip_stop_2013m01 USING btree (calledstationid);


--
-- TOC entry 3389 (class 1259 OID 1199899)
-- Dependencies: 182 3636
-- Name: ivoipstop_called_2013m02; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m02 ON radacct_voip_stop_2013m02 USING btree (calledstationid);


--
-- TOC entry 3395 (class 1259 OID 1199900)
-- Dependencies: 183 3636
-- Name: ivoipstop_called_2013m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m03 ON radacct_voip_stop_2013m03 USING btree (calledstationid);


--
-- TOC entry 3401 (class 1259 OID 1199901)
-- Dependencies: 184 3636
-- Name: ivoipstop_called_2013m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m04 ON radacct_voip_stop_2013m04 USING btree (calledstationid);


--
-- TOC entry 3407 (class 1259 OID 1199902)
-- Dependencies: 185 3636
-- Name: ivoipstop_called_2013m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m05 ON radacct_voip_stop_2013m05 USING btree (calledstationid);


--
-- TOC entry 3413 (class 1259 OID 1199903)
-- Dependencies: 186 3636
-- Name: ivoipstop_called_2013m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m06 ON radacct_voip_stop_2013m06 USING btree (calledstationid);


--
-- TOC entry 3419 (class 1259 OID 1199904)
-- Dependencies: 187 3636
-- Name: ivoipstop_called_2013m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m07 ON radacct_voip_stop_2013m07 USING btree (calledstationid);


--
-- TOC entry 3425 (class 1259 OID 1199905)
-- Dependencies: 188 3636
-- Name: ivoipstop_called_2013m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m08 ON radacct_voip_stop_2013m08 USING btree (calledstationid);


--
-- TOC entry 3431 (class 1259 OID 1199906)
-- Dependencies: 189 3636
-- Name: ivoipstop_called_2013m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m09 ON radacct_voip_stop_2013m09 USING btree (calledstationid);


--
-- TOC entry 3437 (class 1259 OID 1199907)
-- Dependencies: 190 3636
-- Name: ivoipstop_called_2013m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m10 ON radacct_voip_stop_2013m10 USING btree (calledstationid);


--
-- TOC entry 3443 (class 1259 OID 1199908)
-- Dependencies: 191 3636
-- Name: ivoipstop_called_2013m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m11 ON radacct_voip_stop_2013m11 USING btree (calledstationid);


--
-- TOC entry 3449 (class 1259 OID 1199909)
-- Dependencies: 192 3636
-- Name: ivoipstop_called_2013m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2013m12 ON radacct_voip_stop_2013m12 USING btree (calledstationid);


--
-- TOC entry 3455 (class 1259 OID 1199910)
-- Dependencies: 193 3636
-- Name: ivoipstop_called_2014m01; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m01 ON radacct_voip_stop_2014m01 USING btree (calledstationid);


--
-- TOC entry 3461 (class 1259 OID 1199911)
-- Dependencies: 194 3636
-- Name: ivoipstop_called_2014m02; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m02 ON radacct_voip_stop_2014m02 USING btree (calledstationid);


--
-- TOC entry 3467 (class 1259 OID 1199912)
-- Dependencies: 195 3636
-- Name: ivoipstop_called_2014m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m03 ON radacct_voip_stop_2014m03 USING btree (calledstationid);


--
-- TOC entry 3473 (class 1259 OID 1199913)
-- Dependencies: 196 3636
-- Name: ivoipstop_called_2014m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m04 ON radacct_voip_stop_2014m04 USING btree (calledstationid);


--
-- TOC entry 3479 (class 1259 OID 1199914)
-- Dependencies: 197 3636
-- Name: ivoipstop_called_2014m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m05 ON radacct_voip_stop_2014m05 USING btree (calledstationid);


--
-- TOC entry 3485 (class 1259 OID 1199915)
-- Dependencies: 198 3636
-- Name: ivoipstop_called_2014m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m06 ON radacct_voip_stop_2014m06 USING btree (calledstationid);


--
-- TOC entry 3491 (class 1259 OID 1199916)
-- Dependencies: 199 3636
-- Name: ivoipstop_called_2014m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m07 ON radacct_voip_stop_2014m07 USING btree (calledstationid);


--
-- TOC entry 3497 (class 1259 OID 1199917)
-- Dependencies: 200 3636
-- Name: ivoipstop_called_2014m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m08 ON radacct_voip_stop_2014m08 USING btree (calledstationid);


--
-- TOC entry 3503 (class 1259 OID 1199918)
-- Dependencies: 201 3636
-- Name: ivoipstop_called_2014m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m09 ON radacct_voip_stop_2014m09 USING btree (calledstationid);


--
-- TOC entry 3509 (class 1259 OID 1199919)
-- Dependencies: 202 3636
-- Name: ivoipstop_called_2014m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m10 ON radacct_voip_stop_2014m10 USING btree (calledstationid);


--
-- TOC entry 3515 (class 1259 OID 1199920)
-- Dependencies: 203 3636
-- Name: ivoipstop_called_2014m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m11 ON radacct_voip_stop_2014m11 USING btree (calledstationid);


--
-- TOC entry 3521 (class 1259 OID 1199921)
-- Dependencies: 204 3636
-- Name: ivoipstop_called_2014m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_2014m12 ON radacct_voip_stop_2014m12 USING btree (calledstationid);


--
-- TOC entry 3527 (class 1259 OID 1199922)
-- Dependencies: 205 3636
-- Name: ivoipstop_called_max; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_called_max ON radacct_voip_stop_max USING btree (calledstationid);


--
-- TOC entry 3300 (class 1259 OID 1199923)
-- Dependencies: 169 3636
-- Name: ivoipstop_calling_2012m01; Type: INDEX; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m01 ON radacct_voip_stop_2012m01 USING btree (callingstationid);


--
-- TOC entry 3307 (class 1259 OID 1199924)
-- Dependencies: 170 3636
-- Name: ivoipstop_calling_2012m02; Type: INDEX; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m02 ON radacct_voip_stop_2012m02 USING btree (callingstationid);


--
-- TOC entry 3314 (class 1259 OID 1199925)
-- Dependencies: 171 3636
-- Name: ivoipstop_calling_2012m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m03 ON radacct_voip_stop_2012m03 USING btree (callingstationid);


--
-- TOC entry 3321 (class 1259 OID 1199926)
-- Dependencies: 172 3636
-- Name: ivoipstop_calling_2012m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m04 ON radacct_voip_stop_2012m04 USING btree (callingstationid);


--
-- TOC entry 3328 (class 1259 OID 1199927)
-- Dependencies: 173 3636
-- Name: ivoipstop_calling_2012m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m05 ON radacct_voip_stop_2012m05 USING btree (callingstationid);


--
-- TOC entry 3335 (class 1259 OID 1199928)
-- Dependencies: 174 3636
-- Name: ivoipstop_calling_2012m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m06 ON radacct_voip_stop_2012m06 USING btree (callingstationid);


--
-- TOC entry 3342 (class 1259 OID 1199929)
-- Dependencies: 175 3636
-- Name: ivoipstop_calling_2012m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m07 ON radacct_voip_stop_2012m07 USING btree (callingstationid);


--
-- TOC entry 3349 (class 1259 OID 1199930)
-- Dependencies: 176 3636
-- Name: ivoipstop_calling_2012m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m08 ON radacct_voip_stop_2012m08 USING btree (callingstationid);


--
-- TOC entry 3356 (class 1259 OID 1199931)
-- Dependencies: 177 3636
-- Name: ivoipstop_calling_2012m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m09 ON radacct_voip_stop_2012m09 USING btree (callingstationid);


--
-- TOC entry 3363 (class 1259 OID 1199932)
-- Dependencies: 178 3636
-- Name: ivoipstop_calling_2012m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m10 ON radacct_voip_stop_2012m10 USING btree (callingstationid);


--
-- TOC entry 3370 (class 1259 OID 1199933)
-- Dependencies: 179 3636
-- Name: ivoipstop_calling_2012m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m11 ON radacct_voip_stop_2012m11 USING btree (callingstationid);


--
-- TOC entry 3377 (class 1259 OID 1199934)
-- Dependencies: 180 3636
-- Name: ivoipstop_calling_2012m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2012m12 ON radacct_voip_stop_2012m12 USING btree (callingstationid);


--
-- TOC entry 3384 (class 1259 OID 1199935)
-- Dependencies: 181 3636
-- Name: ivoipstop_calling_2013m01; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m01 ON radacct_voip_stop_2013m01 USING btree (callingstationid);


--
-- TOC entry 3390 (class 1259 OID 1199936)
-- Dependencies: 182 3636
-- Name: ivoipstop_calling_2013m02; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m02 ON radacct_voip_stop_2013m02 USING btree (callingstationid);


--
-- TOC entry 3396 (class 1259 OID 1199937)
-- Dependencies: 183 3636
-- Name: ivoipstop_calling_2013m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m03 ON radacct_voip_stop_2013m03 USING btree (callingstationid);


--
-- TOC entry 3402 (class 1259 OID 1199938)
-- Dependencies: 184 3636
-- Name: ivoipstop_calling_2013m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m04 ON radacct_voip_stop_2013m04 USING btree (callingstationid);


--
-- TOC entry 3408 (class 1259 OID 1199939)
-- Dependencies: 185 3636
-- Name: ivoipstop_calling_2013m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m05 ON radacct_voip_stop_2013m05 USING btree (callingstationid);


--
-- TOC entry 3414 (class 1259 OID 1199940)
-- Dependencies: 186 3636
-- Name: ivoipstop_calling_2013m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m06 ON radacct_voip_stop_2013m06 USING btree (callingstationid);


--
-- TOC entry 3420 (class 1259 OID 1199941)
-- Dependencies: 187 3636
-- Name: ivoipstop_calling_2013m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m07 ON radacct_voip_stop_2013m07 USING btree (callingstationid);


--
-- TOC entry 3426 (class 1259 OID 1199942)
-- Dependencies: 188 3636
-- Name: ivoipstop_calling_2013m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m08 ON radacct_voip_stop_2013m08 USING btree (callingstationid);


--
-- TOC entry 3432 (class 1259 OID 1199943)
-- Dependencies: 189 3636
-- Name: ivoipstop_calling_2013m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m09 ON radacct_voip_stop_2013m09 USING btree (callingstationid);


--
-- TOC entry 3438 (class 1259 OID 1199944)
-- Dependencies: 190 3636
-- Name: ivoipstop_calling_2013m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m10 ON radacct_voip_stop_2013m10 USING btree (callingstationid);


--
-- TOC entry 3444 (class 1259 OID 1199945)
-- Dependencies: 191 3636
-- Name: ivoipstop_calling_2013m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m11 ON radacct_voip_stop_2013m11 USING btree (callingstationid);


--
-- TOC entry 3450 (class 1259 OID 1199946)
-- Dependencies: 192 3636
-- Name: ivoipstop_calling_2013m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2013m12 ON radacct_voip_stop_2013m12 USING btree (callingstationid);


--
-- TOC entry 3456 (class 1259 OID 1199947)
-- Dependencies: 193 3636
-- Name: ivoipstop_calling_2014m01; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m01 ON radacct_voip_stop_2014m01 USING btree (callingstationid);


--
-- TOC entry 3462 (class 1259 OID 1199948)
-- Dependencies: 194 3636
-- Name: ivoipstop_calling_2014m02; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m02 ON radacct_voip_stop_2014m02 USING btree (callingstationid);


--
-- TOC entry 3468 (class 1259 OID 1199949)
-- Dependencies: 195 3636
-- Name: ivoipstop_calling_2014m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m03 ON radacct_voip_stop_2014m03 USING btree (callingstationid);


--
-- TOC entry 3474 (class 1259 OID 1199950)
-- Dependencies: 196 3636
-- Name: ivoipstop_calling_2014m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m04 ON radacct_voip_stop_2014m04 USING btree (callingstationid);


--
-- TOC entry 3480 (class 1259 OID 1199951)
-- Dependencies: 197 3636
-- Name: ivoipstop_calling_2014m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m05 ON radacct_voip_stop_2014m05 USING btree (callingstationid);


--
-- TOC entry 3486 (class 1259 OID 1199952)
-- Dependencies: 198 3636
-- Name: ivoipstop_calling_2014m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m06 ON radacct_voip_stop_2014m06 USING btree (callingstationid);


--
-- TOC entry 3492 (class 1259 OID 1199953)
-- Dependencies: 199 3636
-- Name: ivoipstop_calling_2014m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m07 ON radacct_voip_stop_2014m07 USING btree (callingstationid);


--
-- TOC entry 3498 (class 1259 OID 1199954)
-- Dependencies: 200 3636
-- Name: ivoipstop_calling_2014m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m08 ON radacct_voip_stop_2014m08 USING btree (callingstationid);


--
-- TOC entry 3504 (class 1259 OID 1199955)
-- Dependencies: 201 3636
-- Name: ivoipstop_calling_2014m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m09 ON radacct_voip_stop_2014m09 USING btree (callingstationid);


--
-- TOC entry 3510 (class 1259 OID 1199956)
-- Dependencies: 202 3636
-- Name: ivoipstop_calling_2014m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m10 ON radacct_voip_stop_2014m10 USING btree (callingstationid);


--
-- TOC entry 3516 (class 1259 OID 1199957)
-- Dependencies: 203 3636
-- Name: ivoipstop_calling_2014m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m11 ON radacct_voip_stop_2014m11 USING btree (callingstationid);


--
-- TOC entry 3522 (class 1259 OID 1199958)
-- Dependencies: 204 3636
-- Name: ivoipstop_calling_2014m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_2014m12 ON radacct_voip_stop_2014m12 USING btree (callingstationid);


--
-- TOC entry 3528 (class 1259 OID 1199959)
-- Dependencies: 205 3636
-- Name: ivoipstop_calling_max; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_calling_max ON radacct_voip_stop_max USING btree (callingstationid);


--
-- TOC entry 3301 (class 1259 OID 1199960)
-- Dependencies: 169 169 3636
-- Name: ivoipstop_ctime_called_2012m01; Type: INDEX; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m01 ON radacct_voip_stop_2012m01 USING btree (connecttime, calledstationid);


--
-- TOC entry 3308 (class 1259 OID 1199961)
-- Dependencies: 170 170 3636
-- Name: ivoipstop_ctime_called_2012m02; Type: INDEX; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m02 ON radacct_voip_stop_2012m02 USING btree (connecttime, calledstationid);


--
-- TOC entry 3315 (class 1259 OID 1199962)
-- Dependencies: 171 171 3636
-- Name: ivoipstop_ctime_called_2012m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m03 ON radacct_voip_stop_2012m03 USING btree (connecttime, calledstationid);


--
-- TOC entry 3322 (class 1259 OID 1199963)
-- Dependencies: 172 172 3636
-- Name: ivoipstop_ctime_called_2012m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m04 ON radacct_voip_stop_2012m04 USING btree (connecttime, calledstationid);


--
-- TOC entry 3329 (class 1259 OID 1199964)
-- Dependencies: 173 173 3636
-- Name: ivoipstop_ctime_called_2012m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m05 ON radacct_voip_stop_2012m05 USING btree (connecttime, calledstationid);


--
-- TOC entry 3336 (class 1259 OID 1199965)
-- Dependencies: 174 174 3636
-- Name: ivoipstop_ctime_called_2012m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m06 ON radacct_voip_stop_2012m06 USING btree (connecttime, calledstationid);


--
-- TOC entry 3343 (class 1259 OID 1199966)
-- Dependencies: 175 175 3636
-- Name: ivoipstop_ctime_called_2012m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m07 ON radacct_voip_stop_2012m07 USING btree (connecttime, calledstationid);


--
-- TOC entry 3350 (class 1259 OID 1199967)
-- Dependencies: 176 176 3636
-- Name: ivoipstop_ctime_called_2012m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m08 ON radacct_voip_stop_2012m08 USING btree (connecttime, calledstationid);


--
-- TOC entry 3357 (class 1259 OID 1199968)
-- Dependencies: 177 177 3636
-- Name: ivoipstop_ctime_called_2012m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m09 ON radacct_voip_stop_2012m09 USING btree (connecttime, calledstationid);


--
-- TOC entry 3364 (class 1259 OID 1199969)
-- Dependencies: 178 178 3636
-- Name: ivoipstop_ctime_called_2012m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m10 ON radacct_voip_stop_2012m10 USING btree (connecttime, calledstationid);


--
-- TOC entry 3371 (class 1259 OID 1199970)
-- Dependencies: 179 179 3636
-- Name: ivoipstop_ctime_called_2012m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m11 ON radacct_voip_stop_2012m11 USING btree (connecttime, calledstationid);


--
-- TOC entry 3378 (class 1259 OID 1199971)
-- Dependencies: 180 180 3636
-- Name: ivoipstop_ctime_called_2012m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2012m12 ON radacct_voip_stop_2012m12 USING btree (connecttime, calledstationid);


--
-- TOC entry 3385 (class 1259 OID 1199972)
-- Dependencies: 181 181 3636
-- Name: ivoipstop_ctime_called_2013m01; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m01 ON radacct_voip_stop_2013m01 USING btree (connecttime, calledstationid);


--
-- TOC entry 3391 (class 1259 OID 1199973)
-- Dependencies: 182 182 3636
-- Name: ivoipstop_ctime_called_2013m02; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m02 ON radacct_voip_stop_2013m02 USING btree (connecttime, calledstationid);


--
-- TOC entry 3397 (class 1259 OID 1199974)
-- Dependencies: 183 183 3636
-- Name: ivoipstop_ctime_called_2013m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m03 ON radacct_voip_stop_2013m03 USING btree (connecttime, calledstationid);


--
-- TOC entry 3403 (class 1259 OID 1199975)
-- Dependencies: 184 184 3636
-- Name: ivoipstop_ctime_called_2013m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m04 ON radacct_voip_stop_2013m04 USING btree (connecttime, calledstationid);


--
-- TOC entry 3409 (class 1259 OID 1199976)
-- Dependencies: 185 185 3636
-- Name: ivoipstop_ctime_called_2013m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m05 ON radacct_voip_stop_2013m05 USING btree (connecttime, calledstationid);


--
-- TOC entry 3415 (class 1259 OID 1199977)
-- Dependencies: 186 186 3636
-- Name: ivoipstop_ctime_called_2013m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m06 ON radacct_voip_stop_2013m06 USING btree (connecttime, calledstationid);


--
-- TOC entry 3421 (class 1259 OID 1199978)
-- Dependencies: 187 187 3636
-- Name: ivoipstop_ctime_called_2013m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m07 ON radacct_voip_stop_2013m07 USING btree (connecttime, calledstationid);


--
-- TOC entry 3427 (class 1259 OID 1199979)
-- Dependencies: 188 188 3636
-- Name: ivoipstop_ctime_called_2013m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m08 ON radacct_voip_stop_2013m08 USING btree (connecttime, calledstationid);


--
-- TOC entry 3433 (class 1259 OID 1199980)
-- Dependencies: 189 189 3636
-- Name: ivoipstop_ctime_called_2013m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m09 ON radacct_voip_stop_2013m09 USING btree (connecttime, calledstationid);


--
-- TOC entry 3439 (class 1259 OID 1199981)
-- Dependencies: 190 190 3636
-- Name: ivoipstop_ctime_called_2013m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m10 ON radacct_voip_stop_2013m10 USING btree (connecttime, calledstationid);


--
-- TOC entry 3445 (class 1259 OID 1199982)
-- Dependencies: 191 191 3636
-- Name: ivoipstop_ctime_called_2013m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m11 ON radacct_voip_stop_2013m11 USING btree (connecttime, calledstationid);


--
-- TOC entry 3451 (class 1259 OID 1199983)
-- Dependencies: 192 192 3636
-- Name: ivoipstop_ctime_called_2013m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2013m12 ON radacct_voip_stop_2013m12 USING btree (connecttime, calledstationid);


--
-- TOC entry 3457 (class 1259 OID 1199984)
-- Dependencies: 193 193 3636
-- Name: ivoipstop_ctime_called_2014m01; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m01 ON radacct_voip_stop_2014m01 USING btree (connecttime, calledstationid);


--
-- TOC entry 3463 (class 1259 OID 1199985)
-- Dependencies: 194 194 3636
-- Name: ivoipstop_ctime_called_2014m02; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m02 ON radacct_voip_stop_2014m02 USING btree (connecttime, calledstationid);


--
-- TOC entry 3469 (class 1259 OID 1199986)
-- Dependencies: 195 195 3636
-- Name: ivoipstop_ctime_called_2014m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m03 ON radacct_voip_stop_2014m03 USING btree (connecttime, calledstationid);


--
-- TOC entry 3475 (class 1259 OID 1199987)
-- Dependencies: 196 196 3636
-- Name: ivoipstop_ctime_called_2014m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m04 ON radacct_voip_stop_2014m04 USING btree (connecttime, calledstationid);


--
-- TOC entry 3481 (class 1259 OID 1199988)
-- Dependencies: 197 197 3636
-- Name: ivoipstop_ctime_called_2014m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m05 ON radacct_voip_stop_2014m05 USING btree (connecttime, calledstationid);


--
-- TOC entry 3487 (class 1259 OID 1199989)
-- Dependencies: 198 198 3636
-- Name: ivoipstop_ctime_called_2014m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m06 ON radacct_voip_stop_2014m06 USING btree (connecttime, calledstationid);


--
-- TOC entry 3493 (class 1259 OID 1199990)
-- Dependencies: 199 199 3636
-- Name: ivoipstop_ctime_called_2014m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m07 ON radacct_voip_stop_2014m07 USING btree (connecttime, calledstationid);


--
-- TOC entry 3499 (class 1259 OID 1199991)
-- Dependencies: 200 200 3636
-- Name: ivoipstop_ctime_called_2014m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m08 ON radacct_voip_stop_2014m08 USING btree (connecttime, calledstationid);


--
-- TOC entry 3505 (class 1259 OID 1199992)
-- Dependencies: 201 201 3636
-- Name: ivoipstop_ctime_called_2014m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m09 ON radacct_voip_stop_2014m09 USING btree (connecttime, calledstationid);


--
-- TOC entry 3511 (class 1259 OID 1199993)
-- Dependencies: 202 202 3636
-- Name: ivoipstop_ctime_called_2014m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m10 ON radacct_voip_stop_2014m10 USING btree (connecttime, calledstationid);


--
-- TOC entry 3517 (class 1259 OID 1199994)
-- Dependencies: 203 203 3636
-- Name: ivoipstop_ctime_called_2014m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m11 ON radacct_voip_stop_2014m11 USING btree (connecttime, calledstationid);


--
-- TOC entry 3523 (class 1259 OID 1199995)
-- Dependencies: 204 204 3636
-- Name: ivoipstop_ctime_called_2014m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_2014m12 ON radacct_voip_stop_2014m12 USING btree (connecttime, calledstationid);


--
-- TOC entry 3529 (class 1259 OID 1199996)
-- Dependencies: 205 205 3636
-- Name: ivoipstop_ctime_called_max; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_called_max ON radacct_voip_stop_max USING btree (connecttime, calledstationid);


--
-- TOC entry 3302 (class 1259 OID 1199997)
-- Dependencies: 169 169 3636
-- Name: ivoipstop_ctime_calling_2012m01; Type: INDEX; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m01 ON radacct_voip_stop_2012m01 USING btree (connecttime, callingstationid);


--
-- TOC entry 3309 (class 1259 OID 1199998)
-- Dependencies: 170 170 3636
-- Name: ivoipstop_ctime_calling_2012m02; Type: INDEX; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m02 ON radacct_voip_stop_2012m02 USING btree (connecttime, callingstationid);


--
-- TOC entry 3316 (class 1259 OID 1199999)
-- Dependencies: 171 171 3636
-- Name: ivoipstop_ctime_calling_2012m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m03 ON radacct_voip_stop_2012m03 USING btree (connecttime, callingstationid);


--
-- TOC entry 3323 (class 1259 OID 1200000)
-- Dependencies: 172 172 3636
-- Name: ivoipstop_ctime_calling_2012m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m04 ON radacct_voip_stop_2012m04 USING btree (connecttime, callingstationid);


--
-- TOC entry 3330 (class 1259 OID 1200001)
-- Dependencies: 173 173 3636
-- Name: ivoipstop_ctime_calling_2012m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m05 ON radacct_voip_stop_2012m05 USING btree (connecttime, callingstationid);


--
-- TOC entry 3337 (class 1259 OID 1200002)
-- Dependencies: 174 174 3636
-- Name: ivoipstop_ctime_calling_2012m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m06 ON radacct_voip_stop_2012m06 USING btree (connecttime, callingstationid);


--
-- TOC entry 3344 (class 1259 OID 1200003)
-- Dependencies: 175 175 3636
-- Name: ivoipstop_ctime_calling_2012m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m07 ON radacct_voip_stop_2012m07 USING btree (connecttime, callingstationid);


--
-- TOC entry 3351 (class 1259 OID 1200004)
-- Dependencies: 176 176 3636
-- Name: ivoipstop_ctime_calling_2012m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m08 ON radacct_voip_stop_2012m08 USING btree (connecttime, callingstationid);


--
-- TOC entry 3358 (class 1259 OID 1200005)
-- Dependencies: 177 177 3636
-- Name: ivoipstop_ctime_calling_2012m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m09 ON radacct_voip_stop_2012m09 USING btree (connecttime, callingstationid);


--
-- TOC entry 3365 (class 1259 OID 1200006)
-- Dependencies: 178 178 3636
-- Name: ivoipstop_ctime_calling_2012m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m10 ON radacct_voip_stop_2012m10 USING btree (connecttime, callingstationid);


--
-- TOC entry 3372 (class 1259 OID 1200007)
-- Dependencies: 179 179 3636
-- Name: ivoipstop_ctime_calling_2012m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m11 ON radacct_voip_stop_2012m11 USING btree (connecttime, callingstationid);


--
-- TOC entry 3379 (class 1259 OID 1200008)
-- Dependencies: 180 180 3636
-- Name: ivoipstop_ctime_calling_2012m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2012m12 ON radacct_voip_stop_2012m12 USING btree (connecttime, callingstationid);


--
-- TOC entry 3386 (class 1259 OID 1200009)
-- Dependencies: 181 181 3636
-- Name: ivoipstop_ctime_calling_2013m01; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m01 ON radacct_voip_stop_2013m01 USING btree (connecttime, callingstationid);


--
-- TOC entry 3392 (class 1259 OID 1200010)
-- Dependencies: 182 182 3636
-- Name: ivoipstop_ctime_calling_2013m02; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m02 ON radacct_voip_stop_2013m02 USING btree (connecttime, callingstationid);


--
-- TOC entry 3398 (class 1259 OID 1200011)
-- Dependencies: 183 183 3636
-- Name: ivoipstop_ctime_calling_2013m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m03 ON radacct_voip_stop_2013m03 USING btree (connecttime, callingstationid);


--
-- TOC entry 3404 (class 1259 OID 1200012)
-- Dependencies: 184 184 3636
-- Name: ivoipstop_ctime_calling_2013m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m04 ON radacct_voip_stop_2013m04 USING btree (connecttime, callingstationid);


--
-- TOC entry 3410 (class 1259 OID 1200013)
-- Dependencies: 185 185 3636
-- Name: ivoipstop_ctime_calling_2013m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m05 ON radacct_voip_stop_2013m05 USING btree (connecttime, callingstationid);


--
-- TOC entry 3416 (class 1259 OID 1200014)
-- Dependencies: 186 186 3636
-- Name: ivoipstop_ctime_calling_2013m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m06 ON radacct_voip_stop_2013m06 USING btree (connecttime, callingstationid);


--
-- TOC entry 3422 (class 1259 OID 1200015)
-- Dependencies: 187 187 3636
-- Name: ivoipstop_ctime_calling_2013m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m07 ON radacct_voip_stop_2013m07 USING btree (connecttime, callingstationid);


--
-- TOC entry 3428 (class 1259 OID 1200016)
-- Dependencies: 188 188 3636
-- Name: ivoipstop_ctime_calling_2013m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m08 ON radacct_voip_stop_2013m08 USING btree (connecttime, callingstationid);


--
-- TOC entry 3434 (class 1259 OID 1200017)
-- Dependencies: 189 189 3636
-- Name: ivoipstop_ctime_calling_2013m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m09 ON radacct_voip_stop_2013m09 USING btree (connecttime, callingstationid);


--
-- TOC entry 3440 (class 1259 OID 1200018)
-- Dependencies: 190 190 3636
-- Name: ivoipstop_ctime_calling_2013m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m10 ON radacct_voip_stop_2013m10 USING btree (connecttime, callingstationid);


--
-- TOC entry 3446 (class 1259 OID 1200019)
-- Dependencies: 191 191 3636
-- Name: ivoipstop_ctime_calling_2013m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m11 ON radacct_voip_stop_2013m11 USING btree (connecttime, callingstationid);


--
-- TOC entry 3452 (class 1259 OID 1200020)
-- Dependencies: 192 192 3636
-- Name: ivoipstop_ctime_calling_2013m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2013m12 ON radacct_voip_stop_2013m12 USING btree (connecttime, callingstationid);


--
-- TOC entry 3458 (class 1259 OID 1200021)
-- Dependencies: 193 193 3636
-- Name: ivoipstop_ctime_calling_2014m01; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m01 ON radacct_voip_stop_2014m01 USING btree (connecttime, callingstationid);


--
-- TOC entry 3464 (class 1259 OID 1200022)
-- Dependencies: 194 194 3636
-- Name: ivoipstop_ctime_calling_2014m02; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m02 ON radacct_voip_stop_2014m02 USING btree (connecttime, callingstationid);


--
-- TOC entry 3470 (class 1259 OID 1200023)
-- Dependencies: 195 195 3636
-- Name: ivoipstop_ctime_calling_2014m03; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m03 ON radacct_voip_stop_2014m03 USING btree (connecttime, callingstationid);


--
-- TOC entry 3476 (class 1259 OID 1200024)
-- Dependencies: 196 196 3636
-- Name: ivoipstop_ctime_calling_2014m04; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m04 ON radacct_voip_stop_2014m04 USING btree (connecttime, callingstationid);


--
-- TOC entry 3482 (class 1259 OID 1200025)
-- Dependencies: 197 197 3636
-- Name: ivoipstop_ctime_calling_2014m05; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m05 ON radacct_voip_stop_2014m05 USING btree (connecttime, callingstationid);


--
-- TOC entry 3488 (class 1259 OID 1200026)
-- Dependencies: 198 198 3636
-- Name: ivoipstop_ctime_calling_2014m06; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m06 ON radacct_voip_stop_2014m06 USING btree (connecttime, callingstationid);


--
-- TOC entry 3494 (class 1259 OID 1200027)
-- Dependencies: 199 199 3636
-- Name: ivoipstop_ctime_calling_2014m07; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m07 ON radacct_voip_stop_2014m07 USING btree (connecttime, callingstationid);


--
-- TOC entry 3500 (class 1259 OID 1200028)
-- Dependencies: 200 200 3636
-- Name: ivoipstop_ctime_calling_2014m08; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m08 ON radacct_voip_stop_2014m08 USING btree (connecttime, callingstationid);


--
-- TOC entry 3506 (class 1259 OID 1200029)
-- Dependencies: 201 201 3636
-- Name: ivoipstop_ctime_calling_2014m09; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m09 ON radacct_voip_stop_2014m09 USING btree (connecttime, callingstationid);


--
-- TOC entry 3512 (class 1259 OID 1200030)
-- Dependencies: 202 202 3636
-- Name: ivoipstop_ctime_calling_2014m10; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m10 ON radacct_voip_stop_2014m10 USING btree (connecttime, callingstationid);


--
-- TOC entry 3518 (class 1259 OID 1200031)
-- Dependencies: 203 203 3636
-- Name: ivoipstop_ctime_calling_2014m11; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m11 ON radacct_voip_stop_2014m11 USING btree (connecttime, callingstationid);


--
-- TOC entry 3524 (class 1259 OID 1200032)
-- Dependencies: 204 204 3636
-- Name: ivoipstop_ctime_calling_2014m12; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_2014m12 ON radacct_voip_stop_2014m12 USING btree (connecttime, callingstationid);


--
-- TOC entry 3530 (class 1259 OID 1200033)
-- Dependencies: 205 205 3636
-- Name: ivoipstop_ctime_calling_max; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX ivoipstop_ctime_calling_max ON radacct_voip_stop_max USING btree (connecttime, callingstationid);


--
-- TOC entry 3303 (class 1259 OID 1200034)
-- Dependencies: 169 3636
-- Name: radacct_voip_stop_2012m01_idx; Type: INDEX; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m01_idx ON radacct_voip_stop_2012m01 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3310 (class 1259 OID 1200035)
-- Dependencies: 170 3636
-- Name: radacct_voip_stop_2012m02_idx; Type: INDEX; Schema: public; Owner: eivanov; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m02_idx ON radacct_voip_stop_2012m02 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3317 (class 1259 OID 1200036)
-- Dependencies: 171 3636
-- Name: radacct_voip_stop_2012m03_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m03_idx ON radacct_voip_stop_2012m03 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3324 (class 1259 OID 1200037)
-- Dependencies: 172 3636
-- Name: radacct_voip_stop_2012m04_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m04_idx ON radacct_voip_stop_2012m04 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3331 (class 1259 OID 1200038)
-- Dependencies: 173 3636
-- Name: radacct_voip_stop_2012m05_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m05_idx ON radacct_voip_stop_2012m05 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3338 (class 1259 OID 1200039)
-- Dependencies: 174 3636
-- Name: radacct_voip_stop_2012m06_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m06_idx ON radacct_voip_stop_2012m06 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3345 (class 1259 OID 1200040)
-- Dependencies: 175 3636
-- Name: radacct_voip_stop_2012m07_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m07_idx ON radacct_voip_stop_2012m07 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3352 (class 1259 OID 1200041)
-- Dependencies: 176 3636
-- Name: radacct_voip_stop_2012m08_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m08_idx ON radacct_voip_stop_2012m08 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3359 (class 1259 OID 1200042)
-- Dependencies: 177 3636
-- Name: radacct_voip_stop_2012m09_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m09_idx ON radacct_voip_stop_2012m09 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3366 (class 1259 OID 1200043)
-- Dependencies: 178 3636
-- Name: radacct_voip_stop_2012m10_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m10_idx ON radacct_voip_stop_2012m10 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3373 (class 1259 OID 1200044)
-- Dependencies: 179 3636
-- Name: radacct_voip_stop_2012m11_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m11_idx ON radacct_voip_stop_2012m11 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3380 (class 1259 OID 1200045)
-- Dependencies: 180 3636
-- Name: radacct_voip_stop_2012m12_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX radacct_voip_stop_2012m12_idx ON radacct_voip_stop_2012m12 USING btree (mysql_id NULLS FIRST);


--
-- TOC entry 3533 (class 2620 OID 1200046)
-- Dependencies: 222 167 3636
-- Name: acct_insert_stop_trg; Type: TRIGGER; Schema: public; Owner: alexv
--

CREATE TRIGGER acct_insert_stop_trg BEFORE INSERT ON radacct_voip_stop FOR EACH ROW EXECUTE PROCEDURE acct_insert_stop_part();


--
-- TOC entry 3641 (class 0 OID 0)
-- Dependencies: 8
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;
GRANT USAGE ON SCHEMA public TO g_bill_daemon_local;


--
-- TOC entry 3643 (class 0 OID 0)
-- Dependencies: 228
-- Name: acct_insert_start(bigint, character varying, timestamp with time zone, timestamp with time zone, character varying, character varying, inet, smallint, character varying, smallint, smallint, smallint, smallint, smallint, smallint, character varying, character varying, character varying); Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON FUNCTION acct_insert_start(inacctsessionid bigint, inacctuniqueid character varying, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inredirectnum character varying) FROM PUBLIC;
REVOKE ALL ON FUNCTION acct_insert_start(inacctsessionid bigint, inacctuniqueid character varying, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inredirectnum character varying) FROM alexv;
GRANT ALL ON FUNCTION acct_insert_start(inacctsessionid bigint, inacctuniqueid character varying, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inredirectnum character varying) TO alexv;
GRANT ALL ON FUNCTION acct_insert_start(inacctsessionid bigint, inacctuniqueid character varying, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inredirectnum character varying) TO PUBLIC;
GRANT ALL ON FUNCTION acct_insert_start(inacctsessionid bigint, inacctuniqueid character varying, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inredirectnum character varying) TO gradius;


--
-- TOC entry 3644 (class 0 OID 0)
-- Dependencies: 223
-- Name: acct_insert_stop(character varying, bigint, timestamp with time zone, timestamp with time zone, character varying, character varying, inet, smallint, character varying, smallint, smallint, smallint, smallint, smallint, smallint, character varying, character varying, timestamp with time zone, smallint, integer, character varying); Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON FUNCTION acct_insert_stop(inacctuniqueid character varying, inacctsessionid bigint, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, indisconnecttime timestamp with time zone, indisconnectcause smallint, inacctsessiontime integer, inredirectnum character varying) FROM PUBLIC;
REVOKE ALL ON FUNCTION acct_insert_stop(inacctuniqueid character varying, inacctsessionid bigint, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, indisconnecttime timestamp with time zone, indisconnectcause smallint, inacctsessiontime integer, inredirectnum character varying) FROM alexv;
GRANT ALL ON FUNCTION acct_insert_stop(inacctuniqueid character varying, inacctsessionid bigint, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, indisconnecttime timestamp with time zone, indisconnectcause smallint, inacctsessiontime integer, inredirectnum character varying) TO alexv;
GRANT ALL ON FUNCTION acct_insert_stop(inacctuniqueid character varying, inacctsessionid bigint, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, indisconnecttime timestamp with time zone, indisconnectcause smallint, inacctsessiontime integer, inredirectnum character varying) TO PUBLIC;
GRANT ALL ON FUNCTION acct_insert_stop(inacctuniqueid character varying, inacctsessionid bigint, insetuptime timestamp with time zone, inconnecttime timestamp with time zone, incallingstationid character varying, incalledstationid character varying, innasipaddress inet, inacctdelaytime smallint, indirection character varying, inprefix_oper smallint, inprefix_des smallint, inin_oper smallint, inout_oper smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, indisconnecttime timestamp with time zone, indisconnectcause smallint, inacctsessiontime integer, inredirectnum character varying) TO gradius;


--
-- TOC entry 3645 (class 0 OID 0)
-- Dependencies: 222
-- Name: acct_insert_stop_part(); Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON FUNCTION acct_insert_stop_part() FROM PUBLIC;
REVOKE ALL ON FUNCTION acct_insert_stop_part() FROM alexv;
GRANT ALL ON FUNCTION acct_insert_stop_part() TO alexv;
GRANT ALL ON FUNCTION acct_insert_stop_part() TO gradius;
GRANT ALL ON FUNCTION acct_insert_stop_part() TO g_bill_daemon_local;


--
-- TOC entry 3646 (class 0 OID 0)
-- Dependencies: 224
-- Name: force_finish_call(integer, integer); Type: ACL; Schema: public; Owner: eivanov
--

REVOKE ALL ON FUNCTION force_finish_call(p_session_id integer, p_timeout integer) FROM PUBLIC;
REVOKE ALL ON FUNCTION force_finish_call(p_session_id integer, p_timeout integer) FROM eivanov;
GRANT ALL ON FUNCTION force_finish_call(p_session_id integer, p_timeout integer) TO eivanov;
GRANT ALL ON FUNCTION force_finish_call(p_session_id integer, p_timeout integer) TO PUBLIC;
GRANT ALL ON FUNCTION force_finish_call(p_session_id integer, p_timeout integer) TO g_bill_daemon_local;


--
-- TOC entry 3647 (class 0 OID 0)
-- Dependencies: 220
-- Name: hex2dec(character varying); Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON FUNCTION hex2dec(in_hex character varying) FROM PUBLIC;
REVOKE ALL ON FUNCTION hex2dec(in_hex character varying) FROM alexv;
GRANT ALL ON FUNCTION hex2dec(in_hex character varying) TO alexv;
GRANT ALL ON FUNCTION hex2dec(in_hex character varying) TO gradius;


--
-- TOC entry 3648 (class 0 OID 0)
-- Dependencies: 218
-- Name: int2ts(integer); Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON FUNCTION int2ts(integer) FROM PUBLIC;
REVOKE ALL ON FUNCTION int2ts(integer) FROM alexv;
GRANT ALL ON FUNCTION int2ts(integer) TO alexv;
GRANT ALL ON FUNCTION int2ts(integer) TO gradius;


--
-- TOC entry 3649 (class 0 OID 0)
-- Dependencies: 227
-- Name: numtoe164(character varying, smallint, smallint, character varying); Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON FUNCTION numtoe164(nas character varying, oper smallint, noa smallint, INOUT num character varying) FROM PUBLIC;
REVOKE ALL ON FUNCTION numtoe164(nas character varying, oper smallint, noa smallint, INOUT num character varying) FROM alexv;
GRANT ALL ON FUNCTION numtoe164(nas character varying, oper smallint, noa smallint, INOUT num character varying) TO alexv;
GRANT ALL ON FUNCTION numtoe164(nas character varying, oper smallint, noa smallint, INOUT num character varying) TO PUBLIC;
GRANT ALL ON FUNCTION numtoe164(nas character varying, oper smallint, noa smallint, INOUT num character varying) TO gradius;


--
-- TOC entry 3650 (class 0 OID 0)
-- Dependencies: 226
-- Name: openca_to_radsql(character varying, bigint, character varying, character varying, character varying, character varying, character varying, character varying, smallint, smallint, smallint, character varying, character varying, character varying, character varying, character varying, smallint, bigint); Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON FUNCTION openca_to_radsql(inaccttype character varying, inacctsessionid bigint, inacctuniqueid character varying, insetuptime character varying, inconnecttime character varying, incallingstationid character varying, incalledstationid character varying, innasipaddress character varying, inacctdelaytime smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inoutreplacecdpn character varying, inredirectnum character varying, indisconnecttime character varying, indisconnectcause smallint, inacctsessiontime bigint) FROM PUBLIC;
REVOKE ALL ON FUNCTION openca_to_radsql(inaccttype character varying, inacctsessionid bigint, inacctuniqueid character varying, insetuptime character varying, inconnecttime character varying, incallingstationid character varying, incalledstationid character varying, innasipaddress character varying, inacctdelaytime smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inoutreplacecdpn character varying, inredirectnum character varying, indisconnecttime character varying, indisconnectcause smallint, inacctsessiontime bigint) FROM alexv;
GRANT ALL ON FUNCTION openca_to_radsql(inaccttype character varying, inacctsessionid bigint, inacctuniqueid character varying, insetuptime character varying, inconnecttime character varying, incallingstationid character varying, incalledstationid character varying, innasipaddress character varying, inacctdelaytime smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inoutreplacecdpn character varying, inredirectnum character varying, indisconnecttime character varying, indisconnectcause smallint, inacctsessiontime bigint) TO alexv;
GRANT ALL ON FUNCTION openca_to_radsql(inaccttype character varying, inacctsessionid bigint, inacctuniqueid character varying, insetuptime character varying, inconnecttime character varying, incallingstationid character varying, incalledstationid character varying, innasipaddress character varying, inacctdelaytime smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inoutreplacecdpn character varying, inredirectnum character varying, indisconnecttime character varying, indisconnectcause smallint, inacctsessiontime bigint) TO PUBLIC;
GRANT ALL ON FUNCTION openca_to_radsql(inaccttype character varying, inacctsessionid bigint, inacctuniqueid character varying, insetuptime character varying, inconnecttime character varying, incallingstationid character varying, incalledstationid character varying, innasipaddress character varying, inacctdelaytime smallint, inincd_noa smallint, inincg_noa smallint, inin_route character varying, inout_route character varying, inoutreplacecdpn character varying, inredirectnum character varying, indisconnecttime character varying, indisconnectcause smallint, inacctsessiontime bigint) TO gradius;


--
-- TOC entry 3651 (class 0 OID 0)
-- Dependencies: 221
-- Name: strip_dot(character varying); Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON FUNCTION strip_dot(character varying) FROM PUBLIC;
REVOKE ALL ON FUNCTION strip_dot(character varying) FROM alexv;
GRANT ALL ON FUNCTION strip_dot(character varying) TO alexv;
GRANT ALL ON FUNCTION strip_dot(character varying) TO PUBLIC;
GRANT ALL ON FUNCTION strip_dot(character varying) TO gradius;


--
-- TOC entry 3652 (class 0 OID 0)
-- Dependencies: 219
-- Name: ts2int(timestamp without time zone); Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON FUNCTION ts2int(timestamp without time zone) FROM PUBLIC;
REVOKE ALL ON FUNCTION ts2int(timestamp without time zone) FROM alexv;
GRANT ALL ON FUNCTION ts2int(timestamp without time zone) TO alexv;
GRANT ALL ON FUNCTION ts2int(timestamp without time zone) TO gradius;


--
-- TOC entry 3653 (class 0 OID 0)
-- Dependencies: 166
-- Name: radacct_voip_start; Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON TABLE radacct_voip_start FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_start FROM alexv;
GRANT ALL ON TABLE radacct_voip_start TO alexv;
GRANT ALL ON TABLE radacct_voip_start TO gradius;
GRANT SELECT,INSERT,DELETE ON TABLE radacct_voip_start TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_start TO g_readonly;


--
-- TOC entry 3655 (class 0 OID 0)
-- Dependencies: 165
-- Name: radacct_voip_start_radacctid_seq; Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON SEQUENCE radacct_voip_start_radacctid_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE radacct_voip_start_radacctid_seq FROM alexv;
GRANT ALL ON SEQUENCE radacct_voip_start_radacctid_seq TO alexv;
GRANT ALL ON SEQUENCE radacct_voip_start_radacctid_seq TO gradius;
GRANT USAGE ON SEQUENCE radacct_voip_start_radacctid_seq TO g_bill_daemon_local;
GRANT SELECT ON SEQUENCE radacct_voip_start_radacctid_seq TO g_readonly;


--
-- TOC entry 3656 (class 0 OID 0)
-- Dependencies: 167
-- Name: radacct_voip_stop; Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON TABLE radacct_voip_stop FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop FROM alexv;
GRANT ALL ON TABLE radacct_voip_stop TO alexv;
GRANT ALL ON TABLE radacct_voip_stop TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop TO g_readonly;


--
-- TOC entry 3657 (class 0 OID 0)
-- Dependencies: 169
-- Name: radacct_voip_stop_2012m01; Type: ACL; Schema: public; Owner: eivanov
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m01 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m01 FROM eivanov;
GRANT ALL ON TABLE radacct_voip_stop_2012m01 TO eivanov;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m01 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m01 TO g_readonly;


--
-- TOC entry 3658 (class 0 OID 0)
-- Dependencies: 170
-- Name: radacct_voip_stop_2012m02; Type: ACL; Schema: public; Owner: eivanov
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m02 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m02 FROM eivanov;
GRANT ALL ON TABLE radacct_voip_stop_2012m02 TO eivanov;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m02 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m02 TO g_readonly;


--
-- TOC entry 3659 (class 0 OID 0)
-- Dependencies: 171
-- Name: radacct_voip_stop_2012m03; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m03 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m03 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m03 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m03 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m03 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m03 TO g_readonly;


--
-- TOC entry 3660 (class 0 OID 0)
-- Dependencies: 172
-- Name: radacct_voip_stop_2012m04; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m04 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m04 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m04 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m04 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m04 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m04 TO g_readonly;


--
-- TOC entry 3661 (class 0 OID 0)
-- Dependencies: 173
-- Name: radacct_voip_stop_2012m05; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m05 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m05 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m05 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m05 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m05 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m05 TO g_readonly;


--
-- TOC entry 3662 (class 0 OID 0)
-- Dependencies: 174
-- Name: radacct_voip_stop_2012m06; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m06 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m06 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m06 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m06 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m06 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m06 TO g_readonly;


--
-- TOC entry 3663 (class 0 OID 0)
-- Dependencies: 175
-- Name: radacct_voip_stop_2012m07; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m07 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m07 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m07 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m07 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m07 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m07 TO g_readonly;


--
-- TOC entry 3664 (class 0 OID 0)
-- Dependencies: 176
-- Name: radacct_voip_stop_2012m08; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m08 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m08 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m08 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m08 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m08 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m08 TO g_readonly;


--
-- TOC entry 3665 (class 0 OID 0)
-- Dependencies: 177
-- Name: radacct_voip_stop_2012m09; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m09 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m09 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m09 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m09 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m09 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m09 TO g_readonly;


--
-- TOC entry 3666 (class 0 OID 0)
-- Dependencies: 178
-- Name: radacct_voip_stop_2012m10; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m10 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m10 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m10 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m10 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m10 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m10 TO g_readonly;


--
-- TOC entry 3667 (class 0 OID 0)
-- Dependencies: 179
-- Name: radacct_voip_stop_2012m11; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m11 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m11 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m11 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m11 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m11 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m11 TO g_readonly;


--
-- TOC entry 3668 (class 0 OID 0)
-- Dependencies: 180
-- Name: radacct_voip_stop_2012m12; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2012m12 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2012m12 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m12 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2012m12 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2012m12 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2012m12 TO g_readonly;


--
-- TOC entry 3669 (class 0 OID 0)
-- Dependencies: 181
-- Name: radacct_voip_stop_2013m01; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m01 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m01 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m01 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m01 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m01 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m01 TO g_readonly;


--
-- TOC entry 3670 (class 0 OID 0)
-- Dependencies: 182
-- Name: radacct_voip_stop_2013m02; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m02 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m02 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m02 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m02 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m02 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m02 TO g_readonly;


--
-- TOC entry 3671 (class 0 OID 0)
-- Dependencies: 183
-- Name: radacct_voip_stop_2013m03; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m03 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m03 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m03 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m03 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m03 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m03 TO g_readonly;


--
-- TOC entry 3672 (class 0 OID 0)
-- Dependencies: 184
-- Name: radacct_voip_stop_2013m04; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m04 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m04 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m04 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m04 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m04 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m04 TO g_readonly;


--
-- TOC entry 3673 (class 0 OID 0)
-- Dependencies: 185
-- Name: radacct_voip_stop_2013m05; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m05 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m05 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m05 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m05 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m05 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m05 TO g_readonly;


--
-- TOC entry 3674 (class 0 OID 0)
-- Dependencies: 186
-- Name: radacct_voip_stop_2013m06; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m06 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m06 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m06 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m06 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m06 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m06 TO g_readonly;


--
-- TOC entry 3675 (class 0 OID 0)
-- Dependencies: 187
-- Name: radacct_voip_stop_2013m07; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m07 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m07 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m07 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m07 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m07 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m07 TO g_readonly;


--
-- TOC entry 3676 (class 0 OID 0)
-- Dependencies: 188
-- Name: radacct_voip_stop_2013m08; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m08 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m08 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m08 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m08 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m08 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m08 TO g_readonly;


--
-- TOC entry 3677 (class 0 OID 0)
-- Dependencies: 189
-- Name: radacct_voip_stop_2013m09; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m09 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m09 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m09 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m09 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m09 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m09 TO g_readonly;


--
-- TOC entry 3678 (class 0 OID 0)
-- Dependencies: 190
-- Name: radacct_voip_stop_2013m10; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m10 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m10 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m10 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m10 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m10 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m10 TO g_readonly;


--
-- TOC entry 3679 (class 0 OID 0)
-- Dependencies: 191
-- Name: radacct_voip_stop_2013m11; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m11 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m11 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m11 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m11 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m11 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m11 TO g_readonly;


--
-- TOC entry 3680 (class 0 OID 0)
-- Dependencies: 192
-- Name: radacct_voip_stop_2013m12; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2013m12 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2013m12 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m12 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2013m12 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2013m12 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2013m12 TO g_readonly;


--
-- TOC entry 3681 (class 0 OID 0)
-- Dependencies: 193
-- Name: radacct_voip_stop_2014m01; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m01 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m01 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m01 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m01 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m01 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m01 TO g_readonly;


--
-- TOC entry 3682 (class 0 OID 0)
-- Dependencies: 194
-- Name: radacct_voip_stop_2014m02; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m02 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m02 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m02 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m02 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m02 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m02 TO g_readonly;


--
-- TOC entry 3683 (class 0 OID 0)
-- Dependencies: 195
-- Name: radacct_voip_stop_2014m03; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m03 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m03 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m03 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m03 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m03 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m03 TO g_readonly;


--
-- TOC entry 3684 (class 0 OID 0)
-- Dependencies: 196
-- Name: radacct_voip_stop_2014m04; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m04 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m04 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m04 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m04 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m04 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m04 TO g_readonly;


--
-- TOC entry 3685 (class 0 OID 0)
-- Dependencies: 197
-- Name: radacct_voip_stop_2014m05; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m05 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m05 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m05 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m05 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m05 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m05 TO g_readonly;


--
-- TOC entry 3686 (class 0 OID 0)
-- Dependencies: 198
-- Name: radacct_voip_stop_2014m06; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m06 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m06 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m06 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m06 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m06 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m06 TO g_readonly;


--
-- TOC entry 3687 (class 0 OID 0)
-- Dependencies: 199
-- Name: radacct_voip_stop_2014m07; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m07 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m07 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m07 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m07 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m07 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m07 TO g_readonly;


--
-- TOC entry 3688 (class 0 OID 0)
-- Dependencies: 200
-- Name: radacct_voip_stop_2014m08; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m08 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m08 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m08 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m08 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m08 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m08 TO g_readonly;


--
-- TOC entry 3689 (class 0 OID 0)
-- Dependencies: 201
-- Name: radacct_voip_stop_2014m09; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m09 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m09 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m09 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m09 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m09 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m09 TO g_readonly;


--
-- TOC entry 3690 (class 0 OID 0)
-- Dependencies: 202
-- Name: radacct_voip_stop_2014m10; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m10 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m10 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m10 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m10 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m10 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m10 TO g_readonly;


--
-- TOC entry 3691 (class 0 OID 0)
-- Dependencies: 203
-- Name: radacct_voip_stop_2014m11; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m11 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m11 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m11 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m11 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m11 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m11 TO g_readonly;


--
-- TOC entry 3692 (class 0 OID 0)
-- Dependencies: 204
-- Name: radacct_voip_stop_2014m12; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_2014m12 FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_2014m12 FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m12 TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_2014m12 TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_2014m12 TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_2014m12 TO g_readonly;


--
-- TOC entry 3693 (class 0 OID 0)
-- Dependencies: 205
-- Name: radacct_voip_stop_max; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE radacct_voip_stop_max FROM PUBLIC;
REVOKE ALL ON TABLE radacct_voip_stop_max FROM postgres;
GRANT ALL ON TABLE radacct_voip_stop_max TO postgres;
GRANT ALL ON TABLE radacct_voip_stop_max TO gradius;
GRANT SELECT,INSERT ON TABLE radacct_voip_stop_max TO g_bill_daemon_local;
GRANT SELECT ON TABLE radacct_voip_stop_max TO g_readonly;


--
-- TOC entry 3695 (class 0 OID 0)
-- Dependencies: 168
-- Name: radacct_voip_stop_radacctid_seq; Type: ACL; Schema: public; Owner: alexv
--

REVOKE ALL ON SEQUENCE radacct_voip_stop_radacctid_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE radacct_voip_stop_radacctid_seq FROM alexv;
GRANT ALL ON SEQUENCE radacct_voip_stop_radacctid_seq TO alexv;
GRANT ALL ON SEQUENCE radacct_voip_stop_radacctid_seq TO gradius;
GRANT USAGE ON SEQUENCE radacct_voip_stop_radacctid_seq TO g_bill_daemon_local;
GRANT SELECT ON SEQUENCE radacct_voip_stop_radacctid_seq TO g_readonly;


--
-- TOC entry 3696 (class 0 OID 0)
-- Dependencies: 164
-- Name: test; Type: ACL; Schema: public; Owner: gradius
--

REVOKE ALL ON TABLE test FROM PUBLIC;
REVOKE ALL ON TABLE test FROM gradius;
GRANT ALL ON TABLE test TO gradius;
GRANT ALL ON TABLE test TO alexv;
GRANT SELECT,INSERT ON TABLE test TO g_bill_daemon_local;
GRANT SELECT ON TABLE test TO g_readonly;


-- Completed on 2014-09-01 22:16:06 MSK

--
-- PostgreSQL database dump complete
--

