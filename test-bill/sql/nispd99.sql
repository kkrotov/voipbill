--
-- PostgreSQL database dump
--

-- Dumped from database version 9.1.7
-- Dumped by pg_dump version 9.1.14
-- Started on 2014-09-01 22:22:31 MSK

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- TOC entry 10 (class 2615 OID 20781820)
-- Name: billing; Type: SCHEMA; Schema: -; Owner: eivanov
--

CREATE SCHEMA billing;


ALTER SCHEMA billing OWNER TO eivanov;

--
-- TOC entry 11 (class 2615 OID 20871770)
-- Name: calls; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA calls;


ALTER SCHEMA calls OWNER TO postgres;

--
-- TOC entry 12 (class 2615 OID 20781821)
-- Name: geo; Type: SCHEMA; Schema: -; Owner: eivanov
--

CREATE SCHEMA geo;


ALTER SCHEMA geo OWNER TO eivanov;

--
-- TOC entry 224 (class 3079 OID 12506)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 3171 (class 0 OID 0)
-- Dependencies: 224
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


--
-- TOC entry 223 (class 3079 OID 20807923)
-- Name: adminpack; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS adminpack WITH SCHEMA pg_catalog;


--
-- TOC entry 3172 (class 0 OID 0)
-- Dependencies: 223
-- Name: EXTENSION adminpack; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION adminpack IS 'administrative functions for PostgreSQL';


SET search_path = public, pg_catalog;

--
-- TOC entry 562 (class 1247 OID 19295)
-- Dependencies: 13 170
-- Name: dblink_pkey_results; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE dblink_pkey_results AS (
	"position" integer,
	colname text
);


ALTER TYPE public.dblink_pkey_results OWNER TO postgres;

SET search_path = billing, pg_catalog;

--
-- TOC entry 239 (class 1255 OID 20781822)
-- Dependencies: 10 723
-- Name: get_amounts(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) RETURNS SETOF record
    LANGUAGE plpgsql
    AS $$
DECLARE m_date DATE;
DECLARE d_date DATE;
BEGIN
	m_date = date_trunc('month', now()::timestamp without time zone);
	d_date = date_trunc('day', now()::timestamp  without time zone);    
/* 
	delete from billing.clients_counters;

    insert into billing.clients_counters(client_id, amount_month, amount_month_sum, amount_day, amount_day_sum, amount_sum, voip_auto_disabled)    
*/
    return query 
        select c.id,
        	m_date,
      		COALESCE(m.m_sum, 0),
            d_date,
		    COALESCE(d.d_sum, 0),
            COALESCE(a.a_sum, 0),
            cast(
            	(c.voip_limit_day != 0 and c.voip_limit_day < coalesce(d.d_sum::numeric * 1.18 / 100::numeric,0)) or
            	(c.voip_limit_month != 0 and c.voip_limit_month < coalesce(m.m_sum::numeric * 1.18 / 100::numeric,0)) or
            	(c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0)) or
            	c.voip_disabled as boolean
        	),
            cast(
            	(c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0))
                and coalesce(c.last_payed_month < m_date, true) as boolean
        	)
        from billing.clients c
        left join 
            (select client_id, sum(amount) m_sum from billing.calls c
              where 
                  c.time >= date_trunc('month',m_date) and
                  c.time <  date_trunc('month',m_date) + '1 month'::interval
              group by client_id) as m
        on c.id = m.client_id
        left join 
            (select client_id, sum(amount) d_sum from billing.calls c
              where 
                  c.time >= date_trunc('day',d_date) and
                  c.time <  date_trunc('day',d_date) + '1 day'::interval
              group by client_id) as d
        on c.id = d.client_id
        left join
            (select cl.id client_id, sum(amount) a_sum 
                from billing.clients cl
                left join billing.calls c
                on c.client_id=cl.id and
                      c.time >= cl.amount_date
					--	 and c.time <  cl.amount_date + '1 month'::interval
                where cl.amount_date is not null
                group by cl.id) as a
        on c.id = a.client_id  
        where 
        (
    		COALESCE(m.m_sum, 0) != 0	OR
            COALESCE(d.d_sum, 0) != 0	OR
            COALESCE(a.a_sum, 0) != 0	OR
            cast(
            	(c.voip_limit_day != 0 and c.voip_limit_day < coalesce(d.d_sum::numeric * 1.18 / 100::numeric,0)) or
            	(c.voip_limit_month != 0 and c.voip_limit_month < coalesce(m.m_sum::numeric * 1.18 / 100::numeric,0)) or
            	(c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0)) or
            	c.voip_disabled as boolean
        	)	OR
            cast(
            	(c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0))
                and coalesce(c.last_payed_month < m_date, true) as boolean
        	)
        )	; 
/*        
	return query 
	    select 	r.client_id,
		        r.amount_month,
                r.amount_month_sum,
                r.amount_day,
        		r.amount_day_sum,
        		r.amount_sum,
				r.voip_auto_disabled
		from billing.clients_counters r;
*/        
END;
$$;


ALTER FUNCTION billing.get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) OWNER TO eivanov;

--
-- TOC entry 236 (class 1255 OID 20781823)
-- Dependencies: 10 723
-- Name: get_events(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION get_events() RETURNS SETOF record
    LANGUAGE plpgsql
    AS $$
BEGIN
	update billing.clients set sync = 2 where sync=1;

    insert into billing.clients_counters(client_id, amount_sum)    
        select c.id, COALESCE(a.a_sum,0) a_sum from billing.clients c
        left join
            (select cl.id client_id, sum(amount) a_sum 
                from billing.clients cl
                left join billing.usage u on u.client_id=cl.id
                left join billing.calls c
                on c.usage_id=u.id and
                      c.time >= cl.amount_date
					--	 and c.time <  cl.amount_date + '1 month'::interval
                where cl.sync = 2 and cl.amount_date is not null
                group by cl.id) as a
        on c.id = a.client_id        
        where (a.a_sum is not null) 
        	and c.sync = 2;   
        
	return query 
	    select 	r.client_id, 
                c.amount_date,
        		r.amount_sum
		from billing.clients_counters r
        left join billing.clients c on c.id=r.client_id
        where c.sync= 2;
END;
$$;


ALTER FUNCTION billing.get_events() OWNER TO eivanov;

--
-- TOC entry 240 (class 1255 OID 20781824)
-- Dependencies: 10 723
-- Name: reget_amounts(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) RETURNS SETOF record
    LANGUAGE plpgsql
    AS $$
BEGIN
	if EXISTS (SELECT id from billing.clients where sync=1 limit 1) then
		update billing.clients set sync = 2 where sync=1;
    end if;
/*    
    insert into billing.clients_counters(client_id, amount_sum, amount_month, amount_month_sum, amount_day, amount_day_sum, voip_auto_disabled)    
        select c.id, COALESCE(a.a_sum,0), COALESCE(cc.amount_month,'1900-01-01'::date), COALESCE(cc.amount_month_sum,0), COALESCE(cc.amount_day,'1900-01-01'::date), COALESCE(cc.amount_day_sum,0), COALESCE(cc.voip_auto_disabled,false) from billing.clients c
*/    
	return query     
        select c.id, COALESCE(a.a_sum,0) from billing.clients c
        left join
            (select cl.id client_id, sum(amount) a_sum 
                from billing.clients cl
                left join billing.usage u on u.client_id=cl.id
                left join billing.calls c
                on c.usage_id=u.id and
                      c.time >= cl.amount_date
					--	 and c.time <  cl.amount_date + '1 month'::interval
                where cl.sync = 2 and cl.amount_date is not null
                group by cl.id) as a
        on c.id = a.client_id        
        where c.sync = 2;   
/*        
	return query 
	    select 	r.client_id, 
        		r.amount_sum
		from billing.clients_counters r
        left join billing.clients c on c.id=r.client_id
        where c.sync = 2;
*/        
END;
$$;


ALTER FUNCTION billing.reget_amounts(OUT r_client_id integer, OUT a_sum bigint) OWNER TO eivanov;

--
-- TOC entry 241 (class 1255 OID 20781825)
-- Dependencies: 10 723
-- Name: tr_clients(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION tr_clients() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
    new.sync = 1;
	return new;
END;
$$;


ALTER FUNCTION billing.tr_clients() OWNER TO eivanov;

--
-- TOC entry 242 (class 1255 OID 20781826)
-- Dependencies: 10 723
-- Name: tr_clients_after(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION tr_clients_after() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE p_nothing BOOLEAN;
BEGIN
	
	BEGIN
		INSERT INTO billing.events(event)VALUES('clients');
	EXCEPTION WHEN OTHERS THEN	
		p_nothing = null;
    END;
    
    return null;

END;
$$;


ALTER FUNCTION billing.tr_clients_after() OWNER TO eivanov;

--
-- TOC entry 243 (class 1255 OID 20781827)
-- Dependencies: 10 723
-- Name: tr_defs_after(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION tr_defs_after() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	
  INSERT INTO billing.events(event)VALUES('price');
  
  return null;

EXCEPTION WHEN OTHERS THEN	
	return null;
END;
$$;


ALTER FUNCTION billing.tr_defs_after() OWNER TO eivanov;

--
-- TOC entry 252 (class 1255 OID 20914102)
-- Dependencies: 723 10
-- Name: tr_network_prefix_after(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION tr_network_prefix_after() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	
  INSERT INTO billing.events(event)VALUES('network_prefix');
  
  return null;

EXCEPTION WHEN OTHERS THEN	
	return null;
END;
$$;


ALTER FUNCTION billing.tr_network_prefix_after() OWNER TO eivanov;

--
-- TOC entry 237 (class 1255 OID 20781828)
-- Dependencies: 10 723
-- Name: tr_operator_after(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION tr_operator_after() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	
  INSERT INTO billing.events(event)VALUES('operator');
  
  return null;

EXCEPTION WHEN OTHERS THEN	
	return null;
END;
$$;


ALTER FUNCTION billing.tr_operator_after() OWNER TO eivanov;

--
-- TOC entry 249 (class 1255 OID 20884493)
-- Dependencies: 723 10
-- Name: tr_pricelist_after(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION tr_pricelist_after() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	
  INSERT INTO billing.events(event)VALUES('pricelist');
  
  return null;

EXCEPTION WHEN OTHERS THEN	
	return null;
END;
$$;


ALTER FUNCTION billing.tr_pricelist_after() OWNER TO eivanov;

--
-- TOC entry 244 (class 1255 OID 20781829)
-- Dependencies: 723 10
-- Name: tr_tarif_after(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION tr_tarif_after() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	
  INSERT INTO billing.events(event)VALUES('usage');
  
  return null;

EXCEPTION WHEN OTHERS THEN	
	return null;
END;
$$;


ALTER FUNCTION billing.tr_tarif_after() OWNER TO eivanov;

--
-- TOC entry 245 (class 1255 OID 20781830)
-- Dependencies: 10 723
-- Name: tr_tarif_fm_after(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION tr_tarif_fm_after() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	
  INSERT INTO billing.events(event)VALUES('fmins');
  
  return null;

EXCEPTION WHEN OTHERS THEN	
	return null;
END;
$$;


ALTER FUNCTION billing.tr_tarif_fm_after() OWNER TO eivanov;

--
-- TOC entry 246 (class 1255 OID 20781831)
-- Dependencies: 10 723
-- Name: tr_tarif_log_after(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION tr_tarif_log_after() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE p_nothing BOOLEAN;
BEGIN
	
	BEGIN
		INSERT INTO billing.events(event)VALUES('usage');
	EXCEPTION WHEN OTHERS THEN	
		p_nothing = null;
    END;

	BEGIN
		INSERT INTO billing.events(event)VALUES('fmins');
	EXCEPTION WHEN OTHERS THEN	
		p_nothing = null;
    END;
        
  return null;

END;
$$;


ALTER FUNCTION billing.tr_tarif_log_after() OWNER TO eivanov;

--
-- TOC entry 247 (class 1255 OID 20781832)
-- Dependencies: 723 10
-- Name: tr_usage_after(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION tr_usage_after() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	
  INSERT INTO billing.events(event)VALUES('usage');
  
  return null;

EXCEPTION WHEN OTHERS THEN	
	return null;
END;
$$;


ALTER FUNCTION billing.tr_usage_after() OWNER TO eivanov;

--
-- TOC entry 238 (class 1255 OID 20781833)
-- Dependencies: 10 723
-- Name: truncate_all(); Type: FUNCTION; Schema: billing; Owner: eivanov
--

CREATE FUNCTION truncate_all() RETURNS boolean
    LANGUAGE plpgsql
    AS $$
BEGIN

    truncate billing.usage;
    truncate billing.tarif;
    truncate billing.tarif_change_log;
    truncate billing.events;
    truncate billing.defs;
    truncate billing.clients_counters;
    truncate billing.clients;
    truncate geo.prefix;

	return true;
END;
$$;


ALTER FUNCTION billing.truncate_all() OWNER TO eivanov;

SET search_path = calls, pg_catalog;

--
-- TOC entry 251 (class 1255 OID 20872069)
-- Dependencies: 723 11
-- Name: get_amounts(); Type: FUNCTION; Schema: calls; Owner: eivanov
--

CREATE FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) RETURNS SETOF record
    LANGUAGE plpgsql
    AS $$
DECLARE m_date DATE;
DECLARE d_date DATE;
BEGIN
	m_date = date_trunc('month', now()::timestamp without time zone);
	d_date = date_trunc('day', now()::timestamp  without time zone);    

    return query 
        select c.id,
        	m_date,
      		COALESCE(m.m_sum, 0),
            d_date,
		    COALESCE(d.d_sum, 0),
            COALESCE(a.a_sum, 0),
            cast(
            	(c.voip_limit_day != 0 and c.voip_limit_day < coalesce(d.d_sum::numeric * 1.18 / 100::numeric,0)) or
            	(c.voip_limit_month != 0 and c.voip_limit_month < coalesce(m.m_sum::numeric * 1.18 / 100::numeric,0)) or
            	(c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0)) or
            	c.voip_disabled as boolean
        	),
            cast(
            	(c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0))
                and coalesce(c.last_payed_month < m_date, true) as boolean
        	)
        from billing.clients c
        left join 
            (select client_id, sum(amount) m_sum from calls.calls c
              where 
                  c.time >= date_trunc('month',m_date) and
                  c.time <  date_trunc('month',m_date) + '1 month'::interval
              group by client_id) as m
        on c.id = m.client_id
        left join 
            (select client_id, sum(amount) d_sum from calls.calls c
              where 
                  c.time >= date_trunc('day',d_date) and
                  c.time <  date_trunc('day',d_date) + '1 day'::interval
              group by client_id) as d
        on c.id = d.client_id
        left join
            (select cl.id client_id, sum(amount) a_sum 
                from billing.clients cl
                left join calls.calls c
                on c.client_id=cl.id and
                      c.time >= cl.amount_date
					--	 and c.time <  cl.amount_date + '1 month'::interval
                where cl.amount_date is not null
                group by cl.id) as a
        on c.id = a.client_id  
        where 
        (
    		COALESCE(m.m_sum, 0) != 0	OR
            COALESCE(d.d_sum, 0) != 0	OR
            COALESCE(a.a_sum, 0) != 0	OR
            cast(
            	(c.voip_limit_day != 0 and c.voip_limit_day < coalesce(d.d_sum::numeric * 1.18 / 100::numeric,0)) or
            	(c.voip_limit_month != 0 and c.voip_limit_month < coalesce(m.m_sum::numeric * 1.18 / 100::numeric,0)) or
            	(c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0)) or
            	c.voip_disabled as boolean
        	)	OR
            cast(
            	(c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0))
                and coalesce(c.last_payed_month < m_date, true) as boolean
        	)
        )	; 
       
END;
$$;


ALTER FUNCTION calls.get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) OWNER TO eivanov;

--
-- TOC entry 248 (class 1255 OID 20872070)
-- Dependencies: 723 11
-- Name: reget_amounts(); Type: FUNCTION; Schema: calls; Owner: eivanov
--

CREATE FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) RETURNS SETOF record
    LANGUAGE plpgsql
    AS $$
BEGIN
	if EXISTS (SELECT id from billing.clients where sync=1 limit 1) then
		update billing.clients set sync = 2 where sync=1;
    end if;

	return query     
        select c.id, COALESCE(a.a_sum,0) from billing.clients c
        left join
            (select cl.id client_id, sum(amount) a_sum 
                from billing.clients cl
                left join billing.usage u on u.client_id=cl.id
                left join calls.calls c
                on c.usage_id=u.id and
                      c.time >= cl.amount_date
					--	 and c.time <  cl.amount_date + '1 month'::interval
                where cl.sync = 2 and cl.amount_date is not null
                group by cl.id) as a
        on c.id = a.client_id        
        where c.sync = 2;   

END;
$$;


ALTER FUNCTION calls.reget_amounts(OUT r_client_id integer, OUT a_sum bigint) OWNER TO eivanov;

SET search_path = geo, pg_catalog;

--
-- TOC entry 250 (class 1255 OID 20781834)
-- Dependencies: 723 12
-- Name: tr_prefix_after(); Type: FUNCTION; Schema: geo; Owner: eivanov
--

CREATE FUNCTION tr_prefix_after() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	
  INSERT INTO billing.events(event)VALUES('dest');
  
  return null;

EXCEPTION WHEN OTHERS THEN	
	return null;
END;
$$;


ALTER FUNCTION geo.tr_prefix_after() OWNER TO eivanov;

SET search_path = billing, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- TOC entry 171 (class 1259 OID 20781839)
-- Dependencies: 2848 2849 2850 2851 10
-- Name: clients; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE clients (
    id integer NOT NULL,
    voip_limit_month integer NOT NULL,
    voip_limit_day integer NOT NULL,
    voip_disabled boolean,
    credit integer DEFAULT 0 NOT NULL,
    balance numeric(12,2) DEFAULT 0 NOT NULL,
    amount_date timestamp(0) without time zone DEFAULT '2000-01-01 00:00:00'::timestamp without time zone,
    last_payed_month date,
    sync integer DEFAULT 1 NOT NULL
);


ALTER TABLE billing.clients OWNER TO eivanov;

--
-- TOC entry 221 (class 1259 OID 20956061)
-- Dependencies: 2898 2899 2900 2901 2902 2903 10
-- Name: counters_client; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE counters_client (
    client_id integer NOT NULL,
    month date DEFAULT '1900-01-01'::date NOT NULL,
    month_sum integer DEFAULT 0 NOT NULL,
    day date DEFAULT '1900-01-01'::date NOT NULL,
    day_sum integer DEFAULT 0 NOT NULL,
    unaccounted_from date DEFAULT '1900-01-01'::date NOT NULL,
    unaccounted_sum integer DEFAULT 0 NOT NULL
);


ALTER TABLE billing.counters_client OWNER TO eivanov;

--
-- TOC entry 218 (class 1259 OID 20955933)
-- Dependencies: 10
-- Name: counters_client_marker; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE counters_client_marker (
    marker bigint NOT NULL
);


ALTER TABLE billing.counters_client_marker OWNER TO eivanov;

--
-- TOC entry 219 (class 1259 OID 20955939)
-- Dependencies: 2897 10
-- Name: counters_freemin; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE counters_freemin (
    client_id integer NOT NULL,
    month date NOT NULL,
    used_seconds integer DEFAULT 0 NOT NULL
);


ALTER TABLE billing.counters_freemin OWNER TO eivanov;

--
-- TOC entry 220 (class 1259 OID 20955949)
-- Dependencies: 10
-- Name: counters_freemin_marker; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE counters_freemin_marker (
    month date NOT NULL,
    marker bigint
);


ALTER TABLE billing.counters_freemin_marker OWNER TO eivanov;

--
-- TOC entry 172 (class 1259 OID 20781846)
-- Dependencies: 10
-- Name: defs; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE defs (
    ndef bigint NOT NULL,
    pricelist_id smallint NOT NULL,
    date_from date NOT NULL,
    deleted boolean NOT NULL,
    date_to date NOT NULL,
    price numeric(8,4)
);


ALTER TABLE billing.defs OWNER TO eivanov;

--
-- TOC entry 173 (class 1259 OID 20781849)
-- Dependencies: 10
-- Name: events; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE events (
    event character varying NOT NULL
);


ALTER TABLE billing.events OWNER TO eivanov;

--
-- TOC entry 174 (class 1259 OID 20781855)
-- Dependencies: 10
-- Name: instance_settings; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE instance_settings (
    id integer NOT NULL,
    region_id character varying[] NOT NULL,
    city_prefix character varying[] NOT NULL
);


ALTER TABLE billing.instance_settings OWNER TO eivanov;

--
-- TOC entry 217 (class 1259 OID 20914093)
-- Dependencies: 10
-- Name: network_prefix; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE network_prefix (
    prefix character varying(20) NOT NULL,
    instance_id smallint NOT NULL,
    operator_id smallint NOT NULL,
    date_from date NOT NULL,
    deleted boolean NOT NULL,
    date_to date,
    network_type_id integer
);


ALTER TABLE billing.network_prefix OWNER TO eivanov;

--
-- TOC entry 175 (class 1259 OID 20781861)
-- Dependencies: 2852 2853 10
-- Name: operator; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE operator (
    region smallint NOT NULL,
    id smallint NOT NULL,
    default_pricelist_id smallint,
    term_in_cost numeric(8,2) NOT NULL,
    term_out_cost numeric(8,2) DEFAULT 0 NOT NULL,
    term_out_local_cost numeric(8,2) DEFAULT 0 NOT NULL,
    pricelist_id smallint,
    local_network_id smallint,
    local_network_pricelist_id smallint,
    client_7800_pricelist_id smallint,
    operator_7800_pricelist_id smallint
);


ALTER TABLE billing.operator OWNER TO eivanov;

--
-- TOC entry 216 (class 1259 OID 20884410)
-- Dependencies: 2895 2896 10
-- Name: pricelist; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE pricelist (
    id smallint NOT NULL,
    region smallint,
    operator_id smallint,
    tariffication_by_minutes boolean DEFAULT false,
    tariffication_full_first_minute boolean DEFAULT false
);


ALTER TABLE billing.pricelist OWNER TO eivanov;

--
-- TOC entry 176 (class 1259 OID 20781864)
-- Dependencies: 2854 2855 2856 2857 10
-- Name: tarif; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE tarif (
    id smallint NOT NULL,
    freemin integer NOT NULL,
    freemin_for_number boolean DEFAULT false NOT NULL,
    pricelist_id smallint,
    paid_redirect boolean NOT NULL,
    tariffication_by_minutes boolean DEFAULT false NOT NULL,
    tariffication_full_first_minute boolean DEFAULT true NOT NULL,
    tariffication_free_first_seconds boolean DEFAULT true NOT NULL
);


ALTER TABLE billing.tarif OWNER TO eivanov;

--
-- TOC entry 222 (class 1259 OID 21367088)
-- Dependencies: 10
-- Name: tarif_change_log; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE tarif_change_log (
    id integer NOT NULL,
    usage_id integer NOT NULL,
    ts timestamp(0) without time zone,
    date_activation date,
    tarif_id_local integer,
    tarif_id_local_mob integer,
    tarif_id_russia integer,
    tarif_id_russia_mob integer,
    tarif_id_intern integer,
    tarif_id_sng integer
);


ALTER TABLE billing.tarif_change_log OWNER TO eivanov;

--
-- TOC entry 177 (class 1259 OID 20781871)
-- Dependencies: 2858 10
-- Name: usage; Type: TABLE; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE TABLE usage (
    id integer NOT NULL,
    client_id integer NOT NULL,
    phone_num character varying(15),
    actual_from date,
    actual_to date,
    no_of_lines smallint DEFAULT 0 NOT NULL,
    region smallint
);


ALTER TABLE billing.usage OWNER TO eivanov;

SET search_path = calls, pg_catalog;

--
-- TOC entry 179 (class 1259 OID 20871771)
-- Dependencies: 11
-- Name: calls; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls (
    id bigint NOT NULL,
    "time" timestamp without time zone NOT NULL,
    direction_out boolean NOT NULL,
    usage_num bigint,
    phone_num bigint,
    len integer NOT NULL,
    usage_id integer,
    pricelist_mcn_id smallint,
    operator_id smallint,
    free_min_groups_id smallint,
    dest smallint NOT NULL,
    mob boolean NOT NULL,
    month date NOT NULL,
    day date NOT NULL,
    amount integer,
    amount_op integer,
    client_id integer,
    region smallint,
    geo_id integer,
    pricelist_op_id smallint,
    price integer,
    price_op integer,
    len_mcn integer,
    prefix_mcn bigint,
    prefix_op bigint,
    prefix_geo bigint,
    len_op integer,
    geo_operator_id integer,
    redirect_num bigint
);


ALTER TABLE calls.calls OWNER TO eivanov;

--
-- TOC entry 180 (class 1259 OID 20871777)
-- Dependencies: 2859 179 11
-- Name: calls_201201; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201201 (
    CONSTRAINT chk_calls_201201 CHECK ((("time" >= '2012-01-01'::date) AND ("time" < '2012-02-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201201 OWNER TO eivanov;

--
-- TOC entry 181 (class 1259 OID 20871785)
-- Dependencies: 2860 11 179
-- Name: calls_201202; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201202 (
    CONSTRAINT chk_calls_201202 CHECK ((("time" >= '2012-02-01'::date) AND ("time" < '2012-03-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201202 OWNER TO eivanov;

--
-- TOC entry 182 (class 1259 OID 20871793)
-- Dependencies: 2861 179 11
-- Name: calls_201203; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201203 (
    CONSTRAINT chk_calls_201203 CHECK ((("time" >= '2012-03-01'::date) AND ("time" < '2012-04-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201203 OWNER TO eivanov;

--
-- TOC entry 183 (class 1259 OID 20871801)
-- Dependencies: 2862 11 179
-- Name: calls_201204; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201204 (
    CONSTRAINT chk_calls_201204 CHECK ((("time" >= '2012-04-01'::date) AND ("time" < '2012-05-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201204 OWNER TO eivanov;

--
-- TOC entry 184 (class 1259 OID 20871809)
-- Dependencies: 2863 11 179
-- Name: calls_201205; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201205 (
    CONSTRAINT chk_calls_201205 CHECK ((("time" >= '2012-05-01'::date) AND ("time" < '2012-06-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201205 OWNER TO eivanov;

--
-- TOC entry 185 (class 1259 OID 20871817)
-- Dependencies: 2864 179 11
-- Name: calls_201206; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201206 (
    CONSTRAINT chk_calls_201206 CHECK ((("time" >= '2012-06-01'::date) AND ("time" < '2012-07-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201206 OWNER TO eivanov;

--
-- TOC entry 186 (class 1259 OID 20871825)
-- Dependencies: 2865 179 11
-- Name: calls_201207; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201207 (
    CONSTRAINT chk_calls_201207 CHECK ((("time" >= '2012-07-01'::date) AND ("time" < '2012-08-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201207 OWNER TO eivanov;

--
-- TOC entry 187 (class 1259 OID 20871833)
-- Dependencies: 2866 11 179
-- Name: calls_201208; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201208 (
    CONSTRAINT chk_calls_201208 CHECK ((("time" >= '2012-08-01'::date) AND ("time" < '2012-09-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201208 OWNER TO eivanov;

--
-- TOC entry 188 (class 1259 OID 20871841)
-- Dependencies: 2867 179 11
-- Name: calls_201209; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201209 (
    CONSTRAINT chk_calls_201209 CHECK ((("time" >= '2012-09-01'::date) AND ("time" < '2012-10-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201209 OWNER TO eivanov;

--
-- TOC entry 189 (class 1259 OID 20871849)
-- Dependencies: 2868 11 179
-- Name: calls_201210; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201210 (
    CONSTRAINT chk_calls_201210 CHECK ((("time" >= '2012-10-01'::date) AND ("time" < '2012-11-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201210 OWNER TO eivanov;

--
-- TOC entry 190 (class 1259 OID 20871857)
-- Dependencies: 2869 179 11
-- Name: calls_201211; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201211 (
    CONSTRAINT chk_calls_201211 CHECK ((("time" >= '2012-11-01'::date) AND ("time" < '2012-12-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201211 OWNER TO eivanov;

--
-- TOC entry 191 (class 1259 OID 20871865)
-- Dependencies: 2870 179 11
-- Name: calls_201212; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201212 (
    CONSTRAINT chk_calls_201212 CHECK ((("time" >= '2012-12-01'::date) AND ("time" < '2013-01-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201212 OWNER TO eivanov;

--
-- TOC entry 192 (class 1259 OID 20871873)
-- Dependencies: 2871 11 179
-- Name: calls_201301; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201301 (
    CONSTRAINT chk_calls_201301 CHECK ((("time" >= '2013-01-01'::date) AND ("time" < '2013-02-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201301 OWNER TO eivanov;

--
-- TOC entry 193 (class 1259 OID 20871881)
-- Dependencies: 2872 11 179
-- Name: calls_201302; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201302 (
    CONSTRAINT chk_calls_201302 CHECK ((("time" >= '2013-02-01'::date) AND ("time" < '2013-03-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201302 OWNER TO eivanov;

--
-- TOC entry 194 (class 1259 OID 20871889)
-- Dependencies: 2873 179 11
-- Name: calls_201303; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201303 (
    CONSTRAINT chk_calls_201303 CHECK ((("time" >= '2013-03-01'::date) AND ("time" < '2013-04-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201303 OWNER TO eivanov;

--
-- TOC entry 195 (class 1259 OID 20871897)
-- Dependencies: 2874 11 179
-- Name: calls_201304; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201304 (
    CONSTRAINT chk_calls_201304 CHECK ((("time" >= '2013-04-01'::date) AND ("time" < '2013-05-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201304 OWNER TO eivanov;

--
-- TOC entry 196 (class 1259 OID 20871905)
-- Dependencies: 2875 11 179
-- Name: calls_201305; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201305 (
    CONSTRAINT chk_calls_201305 CHECK ((("time" >= '2013-05-01'::date) AND ("time" < '2013-06-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201305 OWNER TO eivanov;

--
-- TOC entry 197 (class 1259 OID 20871913)
-- Dependencies: 2876 179 11
-- Name: calls_201306; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201306 (
    CONSTRAINT chk_calls_201306 CHECK ((("time" >= '2013-06-01'::date) AND ("time" < '2013-07-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201306 OWNER TO eivanov;

--
-- TOC entry 198 (class 1259 OID 20871921)
-- Dependencies: 2877 11 179
-- Name: calls_201307; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201307 (
    CONSTRAINT chk_calls_201307 CHECK ((("time" >= '2013-07-01'::date) AND ("time" < '2013-08-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201307 OWNER TO eivanov;

--
-- TOC entry 199 (class 1259 OID 20871929)
-- Dependencies: 2878 11 179
-- Name: calls_201308; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201308 (
    CONSTRAINT chk_calls_201308 CHECK ((("time" >= '2013-08-01'::date) AND ("time" < '2013-09-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201308 OWNER TO eivanov;

--
-- TOC entry 200 (class 1259 OID 20871937)
-- Dependencies: 2879 179 11
-- Name: calls_201309; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201309 (
    CONSTRAINT chk_calls_201309 CHECK ((("time" >= '2013-09-01'::date) AND ("time" < '2013-10-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201309 OWNER TO eivanov;

--
-- TOC entry 201 (class 1259 OID 20871945)
-- Dependencies: 2880 179 11
-- Name: calls_201310; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201310 (
    CONSTRAINT chk_calls_201310 CHECK ((("time" >= '2013-10-01'::date) AND ("time" < '2013-11-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201310 OWNER TO eivanov;

--
-- TOC entry 202 (class 1259 OID 20871953)
-- Dependencies: 2881 11 179
-- Name: calls_201311; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201311 (
    CONSTRAINT chk_calls_201311 CHECK ((("time" >= '2013-11-01'::date) AND ("time" < '2013-12-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201311 OWNER TO eivanov;

--
-- TOC entry 203 (class 1259 OID 20871961)
-- Dependencies: 2882 11 179
-- Name: calls_201312; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201312 (
    CONSTRAINT chk_calls_201312 CHECK ((("time" >= '2013-12-01'::date) AND ("time" < '2014-01-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201312 OWNER TO eivanov;

--
-- TOC entry 204 (class 1259 OID 20871969)
-- Dependencies: 2883 179 11
-- Name: calls_201401; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201401 (
    CONSTRAINT chk_calls_201401 CHECK ((("time" >= '2014-01-01'::date) AND ("time" < '2014-02-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201401 OWNER TO eivanov;

--
-- TOC entry 205 (class 1259 OID 20871977)
-- Dependencies: 2884 11 179
-- Name: calls_201402; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201402 (
    CONSTRAINT chk_calls_201402 CHECK ((("time" >= '2014-02-01'::date) AND ("time" < '2014-03-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201402 OWNER TO eivanov;

--
-- TOC entry 206 (class 1259 OID 20871985)
-- Dependencies: 2885 179 11
-- Name: calls_201403; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201403 (
    CONSTRAINT chk_calls_201403 CHECK ((("time" >= '2014-03-01'::date) AND ("time" < '2014-04-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201403 OWNER TO eivanov;

--
-- TOC entry 207 (class 1259 OID 20871993)
-- Dependencies: 2886 179 11
-- Name: calls_201404; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201404 (
    CONSTRAINT chk_calls_201404 CHECK ((("time" >= '2014-04-01'::date) AND ("time" < '2014-05-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201404 OWNER TO eivanov;

--
-- TOC entry 208 (class 1259 OID 20872001)
-- Dependencies: 2887 11 179
-- Name: calls_201405; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201405 (
    CONSTRAINT chk_calls_201405 CHECK ((("time" >= '2014-05-01'::date) AND ("time" < '2014-06-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201405 OWNER TO eivanov;

--
-- TOC entry 209 (class 1259 OID 20872009)
-- Dependencies: 2888 11 179
-- Name: calls_201406; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201406 (
    CONSTRAINT chk_calls_201406 CHECK ((("time" >= '2014-06-01'::date) AND ("time" < '2014-07-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201406 OWNER TO eivanov;

--
-- TOC entry 210 (class 1259 OID 20872017)
-- Dependencies: 2889 11 179
-- Name: calls_201407; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201407 (
    CONSTRAINT chk_calls_201407 CHECK ((("time" >= '2014-07-01'::date) AND ("time" < '2014-08-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201407 OWNER TO eivanov;

--
-- TOC entry 211 (class 1259 OID 20872025)
-- Dependencies: 2890 11 179
-- Name: calls_201408; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201408 (
    CONSTRAINT chk_calls_201408 CHECK ((("time" >= '2014-08-01'::date) AND ("time" < '2014-09-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201408 OWNER TO eivanov;

--
-- TOC entry 212 (class 1259 OID 20872033)
-- Dependencies: 2891 11 179
-- Name: calls_201409; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201409 (
    CONSTRAINT chk_calls_201409 CHECK ((("time" >= '2014-09-01'::date) AND ("time" < '2014-10-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201409 OWNER TO eivanov;

--
-- TOC entry 213 (class 1259 OID 20872041)
-- Dependencies: 2892 179 11
-- Name: calls_201410; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201410 (
    CONSTRAINT chk_calls_201410 CHECK ((("time" >= '2014-10-01'::date) AND ("time" < '2014-11-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201410 OWNER TO eivanov;

--
-- TOC entry 214 (class 1259 OID 20872049)
-- Dependencies: 2893 179 11
-- Name: calls_201411; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201411 (
    CONSTRAINT chk_calls_201411 CHECK ((("time" >= '2014-11-01'::date) AND ("time" < '2014-12-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201411 OWNER TO eivanov;

--
-- TOC entry 215 (class 1259 OID 20872057)
-- Dependencies: 2894 179 11
-- Name: calls_201412; Type: TABLE; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE TABLE calls_201412 (
    CONSTRAINT chk_calls_201412 CHECK ((("time" >= '2014-12-01'::date) AND ("time" < '2015-01-01'::date)))
)
INHERITS (calls);


ALTER TABLE calls.calls_201412 OWNER TO eivanov;

SET search_path = geo, pg_catalog;

--
-- TOC entry 178 (class 1259 OID 20781875)
-- Dependencies: 12
-- Name: prefix; Type: TABLE; Schema: geo; Owner: eivanov; Tablespace: 
--

CREATE TABLE prefix (
    prefix character varying(11) NOT NULL,
    geo_id integer,
    mob boolean,
    dest smallint,
    region integer,
    operator_id integer
);


ALTER TABLE geo.prefix OWNER TO eivanov;

SET search_path = billing, pg_catalog;

--
-- TOC entry 3043 (class 2606 OID 20956071)
-- Dependencies: 221 221 3162
-- Name: client_counters_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY counters_client
    ADD CONSTRAINT client_counters_pkey PRIMARY KEY (client_id);


--
-- TOC entry 2906 (class 2606 OID 20781881)
-- Dependencies: 171 171 3162
-- Name: clients_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY clients
    ADD CONSTRAINT clients_pkey PRIMARY KEY (id);


--
-- TOC entry 3041 (class 2606 OID 20955953)
-- Dependencies: 220 220 3162
-- Name: counters_freemin_marker_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY counters_freemin_marker
    ADD CONSTRAINT counters_freemin_marker_pkey PRIMARY KEY (month);


--
-- TOC entry 2908 (class 2606 OID 20781883)
-- Dependencies: 172 172 172 172 172 3162
-- Name: defs_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY defs
    ADD CONSTRAINT defs_pkey PRIMARY KEY (pricelist_id, ndef, deleted, date_from);


--
-- TOC entry 2910 (class 2606 OID 20781885)
-- Dependencies: 173 173 3162
-- Name: events_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY events
    ADD CONSTRAINT events_pkey PRIMARY KEY (event);


--
-- TOC entry 3039 (class 2606 OID 20955944)
-- Dependencies: 219 219 219 3162
-- Name: freemin_counters_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY counters_freemin
    ADD CONSTRAINT freemin_counters_pkey PRIMARY KEY (client_id, month);


--
-- TOC entry 2912 (class 2606 OID 20781887)
-- Dependencies: 174 174 3162
-- Name: instance_settings_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY instance_settings
    ADD CONSTRAINT instance_settings_pkey PRIMARY KEY (id);


--
-- TOC entry 3037 (class 2606 OID 20914097)
-- Dependencies: 217 217 217 217 217 217 3162
-- Name: network_prefix_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY network_prefix
    ADD CONSTRAINT network_prefix_pkey PRIMARY KEY (prefix, instance_id, operator_id, date_from, deleted);


--
-- TOC entry 2914 (class 2606 OID 20781889)
-- Dependencies: 175 175 175 3162
-- Name: operator_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY operator
    ADD CONSTRAINT operator_pkey PRIMARY KEY (region, id);


--
-- TOC entry 3035 (class 2606 OID 20884416)
-- Dependencies: 216 216 3162
-- Name: pricelist_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY pricelist
    ADD CONSTRAINT pricelist_pkey PRIMARY KEY (id);


--
-- TOC entry 3045 (class 2606 OID 21367092)
-- Dependencies: 222 222 3162
-- Name: tarif_change_log_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY tarif_change_log
    ADD CONSTRAINT tarif_change_log_pkey PRIMARY KEY (id);


--
-- TOC entry 2916 (class 2606 OID 20781893)
-- Dependencies: 176 176 3162
-- Name: tarif_pkey; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY tarif
    ADD CONSTRAINT tarif_pkey PRIMARY KEY (id);


--
-- TOC entry 2920 (class 2606 OID 20781895)
-- Dependencies: 177 177 3162
-- Name: voip_usage_idx; Type: CONSTRAINT; Schema: billing; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY usage
    ADD CONSTRAINT voip_usage_idx PRIMARY KEY (id);


SET search_path = calls, pg_catalog;

--
-- TOC entry 2925 (class 2606 OID 20872066)
-- Dependencies: 179 179 3162
-- Name: pk_calls; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls
    ADD CONSTRAINT pk_calls PRIMARY KEY (id);


--
-- TOC entry 2928 (class 2606 OID 20871783)
-- Dependencies: 180 180 3162
-- Name: pk_calls_201201; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201201
    ADD CONSTRAINT pk_calls_201201 PRIMARY KEY (id);


--
-- TOC entry 2931 (class 2606 OID 20871791)
-- Dependencies: 181 181 3162
-- Name: pk_calls_201202; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201202
    ADD CONSTRAINT pk_calls_201202 PRIMARY KEY (id);


--
-- TOC entry 2934 (class 2606 OID 20871799)
-- Dependencies: 182 182 3162
-- Name: pk_calls_201203; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201203
    ADD CONSTRAINT pk_calls_201203 PRIMARY KEY (id);


--
-- TOC entry 2937 (class 2606 OID 20871807)
-- Dependencies: 183 183 3162
-- Name: pk_calls_201204; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201204
    ADD CONSTRAINT pk_calls_201204 PRIMARY KEY (id);


--
-- TOC entry 2940 (class 2606 OID 20871815)
-- Dependencies: 184 184 3162
-- Name: pk_calls_201205; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201205
    ADD CONSTRAINT pk_calls_201205 PRIMARY KEY (id);


--
-- TOC entry 2943 (class 2606 OID 20871823)
-- Dependencies: 185 185 3162
-- Name: pk_calls_201206; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201206
    ADD CONSTRAINT pk_calls_201206 PRIMARY KEY (id);


--
-- TOC entry 2946 (class 2606 OID 20871831)
-- Dependencies: 186 186 3162
-- Name: pk_calls_201207; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201207
    ADD CONSTRAINT pk_calls_201207 PRIMARY KEY (id);


--
-- TOC entry 2949 (class 2606 OID 20871839)
-- Dependencies: 187 187 3162
-- Name: pk_calls_201208; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201208
    ADD CONSTRAINT pk_calls_201208 PRIMARY KEY (id);


--
-- TOC entry 2952 (class 2606 OID 20871847)
-- Dependencies: 188 188 3162
-- Name: pk_calls_201209; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201209
    ADD CONSTRAINT pk_calls_201209 PRIMARY KEY (id);


--
-- TOC entry 2955 (class 2606 OID 20871855)
-- Dependencies: 189 189 3162
-- Name: pk_calls_201210; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201210
    ADD CONSTRAINT pk_calls_201210 PRIMARY KEY (id);


--
-- TOC entry 2958 (class 2606 OID 20871863)
-- Dependencies: 190 190 3162
-- Name: pk_calls_201211; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201211
    ADD CONSTRAINT pk_calls_201211 PRIMARY KEY (id);


--
-- TOC entry 2961 (class 2606 OID 20871871)
-- Dependencies: 191 191 3162
-- Name: pk_calls_201212; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201212
    ADD CONSTRAINT pk_calls_201212 PRIMARY KEY (id);


--
-- TOC entry 2964 (class 2606 OID 20871879)
-- Dependencies: 192 192 3162
-- Name: pk_calls_201301; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201301
    ADD CONSTRAINT pk_calls_201301 PRIMARY KEY (id);


--
-- TOC entry 2967 (class 2606 OID 20871887)
-- Dependencies: 193 193 3162
-- Name: pk_calls_201302; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201302
    ADD CONSTRAINT pk_calls_201302 PRIMARY KEY (id);


--
-- TOC entry 2970 (class 2606 OID 20871895)
-- Dependencies: 194 194 3162
-- Name: pk_calls_201303; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201303
    ADD CONSTRAINT pk_calls_201303 PRIMARY KEY (id);


--
-- TOC entry 2973 (class 2606 OID 20871903)
-- Dependencies: 195 195 3162
-- Name: pk_calls_201304; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201304
    ADD CONSTRAINT pk_calls_201304 PRIMARY KEY (id);


--
-- TOC entry 2976 (class 2606 OID 20871911)
-- Dependencies: 196 196 3162
-- Name: pk_calls_201305; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201305
    ADD CONSTRAINT pk_calls_201305 PRIMARY KEY (id);


--
-- TOC entry 2979 (class 2606 OID 20871919)
-- Dependencies: 197 197 3162
-- Name: pk_calls_201306; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201306
    ADD CONSTRAINT pk_calls_201306 PRIMARY KEY (id);


--
-- TOC entry 2982 (class 2606 OID 20871927)
-- Dependencies: 198 198 3162
-- Name: pk_calls_201307; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201307
    ADD CONSTRAINT pk_calls_201307 PRIMARY KEY (id);


--
-- TOC entry 2985 (class 2606 OID 20871935)
-- Dependencies: 199 199 3162
-- Name: pk_calls_201308; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201308
    ADD CONSTRAINT pk_calls_201308 PRIMARY KEY (id);


--
-- TOC entry 2988 (class 2606 OID 20871943)
-- Dependencies: 200 200 3162
-- Name: pk_calls_201309; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201309
    ADD CONSTRAINT pk_calls_201309 PRIMARY KEY (id);


--
-- TOC entry 2991 (class 2606 OID 20871951)
-- Dependencies: 201 201 3162
-- Name: pk_calls_201310; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201310
    ADD CONSTRAINT pk_calls_201310 PRIMARY KEY (id);


--
-- TOC entry 2994 (class 2606 OID 20871959)
-- Dependencies: 202 202 3162
-- Name: pk_calls_201311; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201311
    ADD CONSTRAINT pk_calls_201311 PRIMARY KEY (id);


--
-- TOC entry 2997 (class 2606 OID 20871967)
-- Dependencies: 203 203 3162
-- Name: pk_calls_201312; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201312
    ADD CONSTRAINT pk_calls_201312 PRIMARY KEY (id);


--
-- TOC entry 3000 (class 2606 OID 20871975)
-- Dependencies: 204 204 3162
-- Name: pk_calls_201401; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201401
    ADD CONSTRAINT pk_calls_201401 PRIMARY KEY (id);


--
-- TOC entry 3003 (class 2606 OID 20871983)
-- Dependencies: 205 205 3162
-- Name: pk_calls_201402; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201402
    ADD CONSTRAINT pk_calls_201402 PRIMARY KEY (id);


--
-- TOC entry 3006 (class 2606 OID 20871991)
-- Dependencies: 206 206 3162
-- Name: pk_calls_201403; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201403
    ADD CONSTRAINT pk_calls_201403 PRIMARY KEY (id);


--
-- TOC entry 3009 (class 2606 OID 20871999)
-- Dependencies: 207 207 3162
-- Name: pk_calls_201404; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201404
    ADD CONSTRAINT pk_calls_201404 PRIMARY KEY (id);


--
-- TOC entry 3012 (class 2606 OID 20872007)
-- Dependencies: 208 208 3162
-- Name: pk_calls_201405; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201405
    ADD CONSTRAINT pk_calls_201405 PRIMARY KEY (id);


--
-- TOC entry 3015 (class 2606 OID 20872015)
-- Dependencies: 209 209 3162
-- Name: pk_calls_201406; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201406
    ADD CONSTRAINT pk_calls_201406 PRIMARY KEY (id);


--
-- TOC entry 3018 (class 2606 OID 20872023)
-- Dependencies: 210 210 3162
-- Name: pk_calls_201407; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201407
    ADD CONSTRAINT pk_calls_201407 PRIMARY KEY (id);


--
-- TOC entry 3021 (class 2606 OID 20872031)
-- Dependencies: 211 211 3162
-- Name: pk_calls_201408; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201408
    ADD CONSTRAINT pk_calls_201408 PRIMARY KEY (id);


--
-- TOC entry 3024 (class 2606 OID 20872039)
-- Dependencies: 212 212 3162
-- Name: pk_calls_201409; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201409
    ADD CONSTRAINT pk_calls_201409 PRIMARY KEY (id);


--
-- TOC entry 3027 (class 2606 OID 20872047)
-- Dependencies: 213 213 3162
-- Name: pk_calls_201410; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201410
    ADD CONSTRAINT pk_calls_201410 PRIMARY KEY (id);


--
-- TOC entry 3030 (class 2606 OID 20872055)
-- Dependencies: 214 214 3162
-- Name: pk_calls_201411; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201411
    ADD CONSTRAINT pk_calls_201411 PRIMARY KEY (id);


--
-- TOC entry 3033 (class 2606 OID 20872063)
-- Dependencies: 215 215 3162
-- Name: pk_calls_201412; Type: CONSTRAINT; Schema: calls; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY calls_201412
    ADD CONSTRAINT pk_calls_201412 PRIMARY KEY (id);


SET search_path = geo, pg_catalog;

--
-- TOC entry 2922 (class 2606 OID 20781897)
-- Dependencies: 178 178 3162
-- Name: prefix_pkey; Type: CONSTRAINT; Schema: geo; Owner: eivanov; Tablespace: 
--

ALTER TABLE ONLY prefix
    ADD CONSTRAINT prefix_pkey PRIMARY KEY (prefix);


SET search_path = billing, pg_catalog;

--
-- TOC entry 2904 (class 1259 OID 20781900)
-- Dependencies: 171 3162
-- Name: clients_idx; Type: INDEX; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE INDEX clients_idx ON clients USING btree (sync);


--
-- TOC entry 2917 (class 1259 OID 20781902)
-- Dependencies: 177 3162
-- Name: usage_idx; Type: INDEX; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE INDEX usage_idx ON usage USING btree (actual_from);


--
-- TOC entry 2918 (class 1259 OID 20781903)
-- Dependencies: 177 3162
-- Name: usage_idx1; Type: INDEX; Schema: billing; Owner: eivanov; Tablespace: 
--

CREATE INDEX usage_idx1 ON usage USING btree (region);


SET search_path = calls, pg_catalog;

--
-- TOC entry 2926 (class 1259 OID 20871784)
-- Dependencies: 180 3162
-- Name: idx_calls_201201_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201201_time ON calls_201201 USING btree ("time");


--
-- TOC entry 2929 (class 1259 OID 20871792)
-- Dependencies: 181 3162
-- Name: idx_calls_201202_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201202_time ON calls_201202 USING btree ("time");


--
-- TOC entry 2932 (class 1259 OID 20871800)
-- Dependencies: 182 3162
-- Name: idx_calls_201203_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201203_time ON calls_201203 USING btree ("time");


--
-- TOC entry 2935 (class 1259 OID 20871808)
-- Dependencies: 183 3162
-- Name: idx_calls_201204_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201204_time ON calls_201204 USING btree ("time");


--
-- TOC entry 2938 (class 1259 OID 20871816)
-- Dependencies: 184 3162
-- Name: idx_calls_201205_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201205_time ON calls_201205 USING btree ("time");


--
-- TOC entry 2941 (class 1259 OID 20871824)
-- Dependencies: 185 3162
-- Name: idx_calls_201206_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201206_time ON calls_201206 USING btree ("time");


--
-- TOC entry 2944 (class 1259 OID 20871832)
-- Dependencies: 186 3162
-- Name: idx_calls_201207_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201207_time ON calls_201207 USING btree ("time");


--
-- TOC entry 2947 (class 1259 OID 20871840)
-- Dependencies: 187 3162
-- Name: idx_calls_201208_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201208_time ON calls_201208 USING btree ("time");


--
-- TOC entry 2950 (class 1259 OID 20871848)
-- Dependencies: 188 3162
-- Name: idx_calls_201209_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201209_time ON calls_201209 USING btree ("time");


--
-- TOC entry 2953 (class 1259 OID 20871856)
-- Dependencies: 189 3162
-- Name: idx_calls_201210_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201210_time ON calls_201210 USING btree ("time");


--
-- TOC entry 2956 (class 1259 OID 20871864)
-- Dependencies: 190 3162
-- Name: idx_calls_201211_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201211_time ON calls_201211 USING btree ("time");


--
-- TOC entry 2959 (class 1259 OID 20871872)
-- Dependencies: 191 3162
-- Name: idx_calls_201212_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201212_time ON calls_201212 USING btree ("time");


--
-- TOC entry 2962 (class 1259 OID 20871880)
-- Dependencies: 192 3162
-- Name: idx_calls_201301_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201301_time ON calls_201301 USING btree ("time");


--
-- TOC entry 2965 (class 1259 OID 20871888)
-- Dependencies: 193 3162
-- Name: idx_calls_201302_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201302_time ON calls_201302 USING btree ("time");


--
-- TOC entry 2968 (class 1259 OID 20871896)
-- Dependencies: 194 3162
-- Name: idx_calls_201303_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201303_time ON calls_201303 USING btree ("time");


--
-- TOC entry 2971 (class 1259 OID 20871904)
-- Dependencies: 195 3162
-- Name: idx_calls_201304_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201304_time ON calls_201304 USING btree ("time");


--
-- TOC entry 2974 (class 1259 OID 20871912)
-- Dependencies: 196 3162
-- Name: idx_calls_201305_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201305_time ON calls_201305 USING btree ("time");


--
-- TOC entry 2977 (class 1259 OID 20871920)
-- Dependencies: 197 3162
-- Name: idx_calls_201306_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201306_time ON calls_201306 USING btree ("time");


--
-- TOC entry 2980 (class 1259 OID 20871928)
-- Dependencies: 198 3162
-- Name: idx_calls_201307_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201307_time ON calls_201307 USING btree ("time");


--
-- TOC entry 2983 (class 1259 OID 20871936)
-- Dependencies: 199 3162
-- Name: idx_calls_201308_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201308_time ON calls_201308 USING btree ("time");


--
-- TOC entry 2986 (class 1259 OID 20871944)
-- Dependencies: 200 3162
-- Name: idx_calls_201309_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201309_time ON calls_201309 USING btree ("time");


--
-- TOC entry 2989 (class 1259 OID 20871952)
-- Dependencies: 201 3162
-- Name: idx_calls_201310_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201310_time ON calls_201310 USING btree ("time");


--
-- TOC entry 2992 (class 1259 OID 20871960)
-- Dependencies: 202 3162
-- Name: idx_calls_201311_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201311_time ON calls_201311 USING btree ("time");


--
-- TOC entry 2995 (class 1259 OID 20871968)
-- Dependencies: 203 3162
-- Name: idx_calls_201312_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201312_time ON calls_201312 USING btree ("time");


--
-- TOC entry 2998 (class 1259 OID 20871976)
-- Dependencies: 204 3162
-- Name: idx_calls_201401_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201401_time ON calls_201401 USING btree ("time");


--
-- TOC entry 3001 (class 1259 OID 20871984)
-- Dependencies: 205 3162
-- Name: idx_calls_201402_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201402_time ON calls_201402 USING btree ("time");


--
-- TOC entry 3004 (class 1259 OID 20871992)
-- Dependencies: 206 3162
-- Name: idx_calls_201403_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201403_time ON calls_201403 USING btree ("time");


--
-- TOC entry 3007 (class 1259 OID 20872000)
-- Dependencies: 207 3162
-- Name: idx_calls_201404_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201404_time ON calls_201404 USING btree ("time");


--
-- TOC entry 3010 (class 1259 OID 20872008)
-- Dependencies: 208 3162
-- Name: idx_calls_201405_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201405_time ON calls_201405 USING btree ("time");


--
-- TOC entry 3013 (class 1259 OID 20872016)
-- Dependencies: 209 3162
-- Name: idx_calls_201406_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201406_time ON calls_201406 USING btree ("time");


--
-- TOC entry 3016 (class 1259 OID 20872024)
-- Dependencies: 210 3162
-- Name: idx_calls_201407_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201407_time ON calls_201407 USING btree ("time");


--
-- TOC entry 3019 (class 1259 OID 20872032)
-- Dependencies: 211 3162
-- Name: idx_calls_201408_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201408_time ON calls_201408 USING btree ("time");


--
-- TOC entry 3022 (class 1259 OID 20872040)
-- Dependencies: 212 3162
-- Name: idx_calls_201409_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201409_time ON calls_201409 USING btree ("time");


--
-- TOC entry 3025 (class 1259 OID 20872048)
-- Dependencies: 213 3162
-- Name: idx_calls_201410_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201410_time ON calls_201410 USING btree ("time");


--
-- TOC entry 3028 (class 1259 OID 20872056)
-- Dependencies: 214 3162
-- Name: idx_calls_201411_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201411_time ON calls_201411 USING btree ("time");


--
-- TOC entry 3031 (class 1259 OID 20872064)
-- Dependencies: 215 3162
-- Name: idx_calls_201412_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_201412_time ON calls_201412 USING btree ("time");


--
-- TOC entry 2923 (class 1259 OID 20871775)
-- Dependencies: 179 3162
-- Name: idx_calls_time; Type: INDEX; Schema: calls; Owner: eivanov; Tablespace: 
--

CREATE INDEX idx_calls_time ON calls USING btree ("time") WHERE false;


SET search_path = billing, pg_catalog;

--
-- TOC entry 3159 (class 2618 OID 20857041)
-- Dependencies: 171 171 171 171 171 171 171 171 171 171 171 3162
-- Name: clients_rl_duplicate; Type: RULE; Schema: billing; Owner: eivanov
--

CREATE RULE clients_rl_duplicate AS ON INSERT TO clients WHERE (EXISTS (SELECT x.id FROM clients x WHERE (x.id = new.id))) DO INSTEAD UPDATE clients SET voip_limit_month = new.voip_limit_month, voip_limit_day = new.voip_limit_day, voip_disabled = new.voip_disabled, balance = new.balance, credit = new.credit, amount_date = new.amount_date, last_payed_month = new.last_payed_month WHERE (clients.id = new.id);


--
-- TOC entry 3160 (class 2618 OID 21367099)
-- Dependencies: 222 222 222 222 222 222 222 222 222 222 222 222 222 3162
-- Name: tarif_change_log_rl_duplicate; Type: RULE; Schema: billing; Owner: eivanov
--

CREATE RULE tarif_change_log_rl_duplicate AS ON INSERT TO tarif_change_log WHERE (EXISTS (SELECT x.id FROM tarif_change_log x WHERE (x.id = new.id))) DO INSTEAD UPDATE tarif_change_log SET usage_id = new.usage_id, ts = new.ts, date_activation = new.date_activation, tarif_id_local = new.tarif_id_local, tarif_id_local_mob = new.tarif_id_local_mob, tarif_id_russia = new.tarif_id_russia, tarif_id_russia_mob = new.tarif_id_russia_mob, tarif_id_intern = new.tarif_id_intern, tarif_id_sng = new.tarif_id_sng WHERE (tarif_change_log.id = new.id);


--
-- TOC entry 3158 (class 2618 OID 20781908)
-- Dependencies: 177 177 177 177 177 177 177 177 177 177 3162
-- Name: usage_rl_duplicate; Type: RULE; Schema: billing; Owner: eivanov
--

CREATE RULE usage_rl_duplicate AS ON INSERT TO usage WHERE (EXISTS (SELECT x.id FROM usage x WHERE (x.id = new.id))) DO INSTEAD UPDATE usage SET actual_from = new.actual_from, actual_to = new.actual_to, client_id = new.client_id, phone_num = new.phone_num, no_of_lines = new.no_of_lines, region = new.region WHERE (usage.id = new.id);


--
-- TOC entry 3049 (class 2620 OID 20781910)
-- Dependencies: 172 243 3162
-- Name: after; Type: TRIGGER; Schema: billing; Owner: eivanov
--

CREATE TRIGGER after AFTER INSERT OR DELETE OR UPDATE OR TRUNCATE ON defs FOR EACH STATEMENT EXECUTE PROCEDURE tr_defs_after();


--
-- TOC entry 3052 (class 2620 OID 20781911)
-- Dependencies: 177 247 3162
-- Name: after; Type: TRIGGER; Schema: billing; Owner: eivanov
--

CREATE TRIGGER after AFTER INSERT OR DELETE OR UPDATE OR TRUNCATE ON usage FOR EACH STATEMENT EXECUTE PROCEDURE tr_usage_after();


--
-- TOC entry 3051 (class 2620 OID 20781913)
-- Dependencies: 244 176 3162
-- Name: after; Type: TRIGGER; Schema: billing; Owner: eivanov
--

CREATE TRIGGER after AFTER INSERT OR DELETE OR UPDATE OR TRUNCATE ON tarif FOR EACH STATEMENT EXECUTE PROCEDURE tr_tarif_after();


--
-- TOC entry 3046 (class 2620 OID 20781914)
-- Dependencies: 171 242 3162
-- Name: after; Type: TRIGGER; Schema: billing; Owner: eivanov
--

CREATE TRIGGER after AFTER INSERT OR DELETE OR UPDATE OR TRUNCATE ON clients FOR EACH STATEMENT EXECUTE PROCEDURE tr_clients_after();


--
-- TOC entry 3055 (class 2620 OID 20914104)
-- Dependencies: 252 217 3162
-- Name: after; Type: TRIGGER; Schema: billing; Owner: eivanov
--

CREATE TRIGGER after AFTER INSERT OR DELETE OR UPDATE OR TRUNCATE ON network_prefix FOR EACH STATEMENT EXECUTE PROCEDURE tr_network_prefix_after();


--
-- TOC entry 3056 (class 2620 OID 21367093)
-- Dependencies: 246 222 3162
-- Name: after; Type: TRIGGER; Schema: billing; Owner: eivanov
--

CREATE TRIGGER after AFTER INSERT OR DELETE OR UPDATE OR TRUNCATE ON tarif_change_log FOR EACH STATEMENT EXECUTE PROCEDURE tr_tarif_log_after();


--
-- TOC entry 3047 (class 2620 OID 20781915)
-- Dependencies: 171 241 3162
-- Name: clients_insert_tr; Type: TRIGGER; Schema: billing; Owner: eivanov
--

CREATE TRIGGER clients_insert_tr BEFORE INSERT ON clients FOR EACH ROW EXECUTE PROCEDURE tr_clients();


--
-- TOC entry 3048 (class 2620 OID 20781916)
-- Dependencies: 171 241 171 3162
-- Name: clients_update_tr; Type: TRIGGER; Schema: billing; Owner: eivanov
--

CREATE TRIGGER clients_update_tr BEFORE UPDATE ON clients FOR EACH ROW WHEN (((old.amount_date <> new.amount_date) OR ((old.amount_date IS NULL) AND (new.amount_date IS NOT NULL)))) EXECUTE PROCEDURE tr_clients();


--
-- TOC entry 3050 (class 2620 OID 20781917)
-- Dependencies: 175 237 3162
-- Name: operator_tr; Type: TRIGGER; Schema: billing; Owner: eivanov
--

CREATE TRIGGER operator_tr AFTER INSERT OR DELETE OR UPDATE OR TRUNCATE ON operator FOR EACH STATEMENT EXECUTE PROCEDURE tr_operator_after();


--
-- TOC entry 3054 (class 2620 OID 20884494)
-- Dependencies: 249 216 3162
-- Name: pricelist_tr; Type: TRIGGER; Schema: billing; Owner: eivanov
--

CREATE TRIGGER pricelist_tr AFTER INSERT OR DELETE OR UPDATE OR TRUNCATE ON pricelist FOR EACH STATEMENT EXECUTE PROCEDURE tr_pricelist_after();


SET search_path = geo, pg_catalog;

--
-- TOC entry 3053 (class 2620 OID 20781918)
-- Dependencies: 178 250 3162
-- Name: after; Type: TRIGGER; Schema: geo; Owner: eivanov
--

CREATE TRIGGER after AFTER INSERT OR DELETE OR UPDATE ON prefix FOR EACH STATEMENT EXECUTE PROCEDURE tr_prefix_after();


--
-- TOC entry 3166 (class 0 OID 0)
-- Dependencies: 10
-- Name: billing; Type: ACL; Schema: -; Owner: eivanov
--

REVOKE ALL ON SCHEMA billing FROM PUBLIC;
REVOKE ALL ON SCHEMA billing FROM eivanov;
GRANT ALL ON SCHEMA billing TO eivanov;
GRANT USAGE ON SCHEMA billing TO g_bill_daemon_local;
GRANT USAGE ON SCHEMA billing TO g_stat;
GRANT USAGE ON SCHEMA billing TO kostik;
GRANT USAGE ON SCHEMA billing TO g_readonly;


--
-- TOC entry 3167 (class 0 OID 0)
-- Dependencies: 11
-- Name: calls; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA calls FROM PUBLIC;
REVOKE ALL ON SCHEMA calls FROM postgres;
GRANT ALL ON SCHEMA calls TO postgres;
GRANT USAGE ON SCHEMA calls TO g_bill_daemon_local;
GRANT USAGE ON SCHEMA calls TO g_readonly;


--
-- TOC entry 3168 (class 0 OID 0)
-- Dependencies: 12
-- Name: geo; Type: ACL; Schema: -; Owner: eivanov
--

REVOKE ALL ON SCHEMA geo FROM PUBLIC;
REVOKE ALL ON SCHEMA geo FROM eivanov;
GRANT ALL ON SCHEMA geo TO eivanov;
GRANT USAGE ON SCHEMA geo TO g_bill_daemon_local;
GRANT USAGE ON SCHEMA geo TO kostik;
GRANT USAGE ON SCHEMA geo TO g_readonly;


--
-- TOC entry 3170 (class 0 OID 0)
-- Dependencies: 13
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;
GRANT USAGE ON SCHEMA public TO g_bill_daemon_local;
GRANT USAGE ON SCHEMA public TO kostik;
GRANT USAGE ON SCHEMA public TO g_readonly;


SET search_path = billing, pg_catalog;

--
-- TOC entry 3173 (class 0 OID 0)
-- Dependencies: 239
-- Name: get_amounts(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) FROM PUBLIC;
REVOKE ALL ON FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) FROM eivanov;
GRANT ALL ON FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) TO eivanov;
GRANT ALL ON FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) TO PUBLIC;
GRANT ALL ON FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) TO g_bill_daemon_local;


--
-- TOC entry 3174 (class 0 OID 0)
-- Dependencies: 236
-- Name: get_events(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION get_events() FROM PUBLIC;
REVOKE ALL ON FUNCTION get_events() FROM eivanov;
GRANT ALL ON FUNCTION get_events() TO eivanov;
GRANT ALL ON FUNCTION get_events() TO PUBLIC;
GRANT ALL ON FUNCTION get_events() TO g_bill_daemon_local;


--
-- TOC entry 3175 (class 0 OID 0)
-- Dependencies: 240
-- Name: reget_amounts(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) FROM PUBLIC;
REVOKE ALL ON FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) FROM eivanov;
GRANT ALL ON FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) TO eivanov;
GRANT ALL ON FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) TO PUBLIC;
GRANT ALL ON FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) TO g_bill_daemon_local;


--
-- TOC entry 3176 (class 0 OID 0)
-- Dependencies: 241
-- Name: tr_clients(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_clients() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_clients() FROM eivanov;
GRANT ALL ON FUNCTION tr_clients() TO eivanov;
GRANT ALL ON FUNCTION tr_clients() TO PUBLIC;
GRANT ALL ON FUNCTION tr_clients() TO g_bill_daemon_local;


--
-- TOC entry 3177 (class 0 OID 0)
-- Dependencies: 242
-- Name: tr_clients_after(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_clients_after() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_clients_after() FROM eivanov;
GRANT ALL ON FUNCTION tr_clients_after() TO eivanov;
GRANT ALL ON FUNCTION tr_clients_after() TO PUBLIC;
GRANT ALL ON FUNCTION tr_clients_after() TO g_bill_daemon_local;


--
-- TOC entry 3178 (class 0 OID 0)
-- Dependencies: 243
-- Name: tr_defs_after(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_defs_after() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_defs_after() FROM eivanov;
GRANT ALL ON FUNCTION tr_defs_after() TO eivanov;
GRANT ALL ON FUNCTION tr_defs_after() TO PUBLIC;
GRANT ALL ON FUNCTION tr_defs_after() TO g_bill_daemon_local;


--
-- TOC entry 3179 (class 0 OID 0)
-- Dependencies: 252
-- Name: tr_network_prefix_after(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_network_prefix_after() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_network_prefix_after() FROM eivanov;
GRANT ALL ON FUNCTION tr_network_prefix_after() TO eivanov;
GRANT ALL ON FUNCTION tr_network_prefix_after() TO PUBLIC;
GRANT ALL ON FUNCTION tr_network_prefix_after() TO g_bill_daemon_local;


--
-- TOC entry 3180 (class 0 OID 0)
-- Dependencies: 237
-- Name: tr_operator_after(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_operator_after() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_operator_after() FROM eivanov;
GRANT ALL ON FUNCTION tr_operator_after() TO eivanov;
GRANT ALL ON FUNCTION tr_operator_after() TO PUBLIC;
GRANT ALL ON FUNCTION tr_operator_after() TO g_bill_daemon_local;


--
-- TOC entry 3181 (class 0 OID 0)
-- Dependencies: 249
-- Name: tr_pricelist_after(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_pricelist_after() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_pricelist_after() FROM eivanov;
GRANT ALL ON FUNCTION tr_pricelist_after() TO eivanov;
GRANT ALL ON FUNCTION tr_pricelist_after() TO PUBLIC;
GRANT ALL ON FUNCTION tr_pricelist_after() TO g_bill_daemon_local;


--
-- TOC entry 3182 (class 0 OID 0)
-- Dependencies: 244
-- Name: tr_tarif_after(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_tarif_after() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_tarif_after() FROM eivanov;
GRANT ALL ON FUNCTION tr_tarif_after() TO eivanov;
GRANT ALL ON FUNCTION tr_tarif_after() TO PUBLIC;
GRANT ALL ON FUNCTION tr_tarif_after() TO g_bill_daemon_local;


--
-- TOC entry 3183 (class 0 OID 0)
-- Dependencies: 245
-- Name: tr_tarif_fm_after(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_tarif_fm_after() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_tarif_fm_after() FROM eivanov;
GRANT ALL ON FUNCTION tr_tarif_fm_after() TO eivanov;
GRANT ALL ON FUNCTION tr_tarif_fm_after() TO PUBLIC;
GRANT ALL ON FUNCTION tr_tarif_fm_after() TO g_bill_daemon_local;


--
-- TOC entry 3184 (class 0 OID 0)
-- Dependencies: 246
-- Name: tr_tarif_log_after(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_tarif_log_after() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_tarif_log_after() FROM eivanov;
GRANT ALL ON FUNCTION tr_tarif_log_after() TO eivanov;
GRANT ALL ON FUNCTION tr_tarif_log_after() TO PUBLIC;
GRANT ALL ON FUNCTION tr_tarif_log_after() TO g_bill_daemon_local;


--
-- TOC entry 3185 (class 0 OID 0)
-- Dependencies: 247
-- Name: tr_usage_after(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_usage_after() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_usage_after() FROM eivanov;
GRANT ALL ON FUNCTION tr_usage_after() TO eivanov;
GRANT ALL ON FUNCTION tr_usage_after() TO PUBLIC;
GRANT ALL ON FUNCTION tr_usage_after() TO g_bill_daemon_local;


--
-- TOC entry 3186 (class 0 OID 0)
-- Dependencies: 238
-- Name: truncate_all(); Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON FUNCTION truncate_all() FROM PUBLIC;
REVOKE ALL ON FUNCTION truncate_all() FROM eivanov;
GRANT ALL ON FUNCTION truncate_all() TO eivanov;
GRANT ALL ON FUNCTION truncate_all() TO PUBLIC;
GRANT ALL ON FUNCTION truncate_all() TO g_bill_daemon_local;


SET search_path = calls, pg_catalog;

--
-- TOC entry 3187 (class 0 OID 0)
-- Dependencies: 251
-- Name: get_amounts(); Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) FROM PUBLIC;
REVOKE ALL ON FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) FROM eivanov;
GRANT ALL ON FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) TO eivanov;
GRANT ALL ON FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) TO PUBLIC;
GRANT ALL ON FUNCTION get_amounts(OUT r_client_id integer, OUT m_date date, OUT m_sum bigint, OUT d_date date, OUT d_sum bigint, OUT a_sum bigint, OUT disabled boolean, OUT disabled_local boolean) TO g_bill_daemon_local;


--
-- TOC entry 3188 (class 0 OID 0)
-- Dependencies: 248
-- Name: reget_amounts(); Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) FROM PUBLIC;
REVOKE ALL ON FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) FROM eivanov;
GRANT ALL ON FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) TO eivanov;
GRANT ALL ON FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) TO PUBLIC;
GRANT ALL ON FUNCTION reget_amounts(OUT r_client_id integer, OUT a_sum bigint) TO g_bill_daemon_local;


SET search_path = geo, pg_catalog;

--
-- TOC entry 3189 (class 0 OID 0)
-- Dependencies: 250
-- Name: tr_prefix_after(); Type: ACL; Schema: geo; Owner: eivanov
--

REVOKE ALL ON FUNCTION tr_prefix_after() FROM PUBLIC;
REVOKE ALL ON FUNCTION tr_prefix_after() FROM eivanov;
GRANT ALL ON FUNCTION tr_prefix_after() TO eivanov;
GRANT ALL ON FUNCTION tr_prefix_after() TO PUBLIC;
GRANT ALL ON FUNCTION tr_prefix_after() TO g_bill_daemon_local;


SET search_path = billing, pg_catalog;

--
-- TOC entry 3190 (class 0 OID 0)
-- Dependencies: 171
-- Name: clients; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE clients FROM PUBLIC;
REVOKE ALL ON TABLE clients FROM eivanov;
GRANT ALL ON TABLE clients TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE clients TO g_bill_daemon_local;
GRANT SELECT ON TABLE clients TO g_stat;
GRANT SELECT ON TABLE clients TO kostik;
GRANT SELECT ON TABLE clients TO g_readonly;


--
-- TOC entry 3191 (class 0 OID 0)
-- Dependencies: 221
-- Name: counters_client; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE counters_client FROM PUBLIC;
REVOKE ALL ON TABLE counters_client FROM eivanov;
GRANT ALL ON TABLE counters_client TO eivanov;
GRANT SELECT ON TABLE counters_client TO g_readonly;


--
-- TOC entry 3192 (class 0 OID 0)
-- Dependencies: 218
-- Name: counters_client_marker; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE counters_client_marker FROM PUBLIC;
REVOKE ALL ON TABLE counters_client_marker FROM eivanov;
GRANT ALL ON TABLE counters_client_marker TO eivanov;
GRANT SELECT ON TABLE counters_client_marker TO g_readonly;


--
-- TOC entry 3193 (class 0 OID 0)
-- Dependencies: 219
-- Name: counters_freemin; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE counters_freemin FROM PUBLIC;
REVOKE ALL ON TABLE counters_freemin FROM eivanov;
GRANT ALL ON TABLE counters_freemin TO eivanov;
GRANT SELECT ON TABLE counters_freemin TO g_readonly;


--
-- TOC entry 3194 (class 0 OID 0)
-- Dependencies: 220
-- Name: counters_freemin_marker; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE counters_freemin_marker FROM PUBLIC;
REVOKE ALL ON TABLE counters_freemin_marker FROM eivanov;
GRANT ALL ON TABLE counters_freemin_marker TO eivanov;
GRANT SELECT ON TABLE counters_freemin_marker TO g_readonly;


--
-- TOC entry 3195 (class 0 OID 0)
-- Dependencies: 172
-- Name: defs; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE defs FROM PUBLIC;
REVOKE ALL ON TABLE defs FROM eivanov;
GRANT ALL ON TABLE defs TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE defs TO g_bill_daemon_local;
GRANT SELECT ON TABLE defs TO kostik;
GRANT SELECT ON TABLE defs TO g_readonly;


--
-- TOC entry 3196 (class 0 OID 0)
-- Dependencies: 173
-- Name: events; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE events FROM PUBLIC;
REVOKE ALL ON TABLE events FROM eivanov;
GRANT ALL ON TABLE events TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE events TO g_bill_daemon_local;
GRANT SELECT ON TABLE events TO kostik;
GRANT SELECT ON TABLE events TO g_readonly;


--
-- TOC entry 3197 (class 0 OID 0)
-- Dependencies: 174
-- Name: instance_settings; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE instance_settings FROM PUBLIC;
REVOKE ALL ON TABLE instance_settings FROM eivanov;
GRANT ALL ON TABLE instance_settings TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE instance_settings TO g_bill_daemon_local;
GRANT SELECT ON TABLE instance_settings TO kostik;
GRANT SELECT ON TABLE instance_settings TO g_readonly;


--
-- TOC entry 3198 (class 0 OID 0)
-- Dependencies: 217
-- Name: network_prefix; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE network_prefix FROM PUBLIC;
REVOKE ALL ON TABLE network_prefix FROM eivanov;
GRANT ALL ON TABLE network_prefix TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE network_prefix TO g_bill_daemon_local;
GRANT SELECT ON TABLE network_prefix TO g_readonly;


--
-- TOC entry 3199 (class 0 OID 0)
-- Dependencies: 175
-- Name: operator; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE operator FROM PUBLIC;
REVOKE ALL ON TABLE operator FROM eivanov;
GRANT ALL ON TABLE operator TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE operator TO g_bill_daemon_local;
GRANT SELECT ON TABLE operator TO kostik;
GRANT SELECT ON TABLE operator TO g_readonly;


--
-- TOC entry 3200 (class 0 OID 0)
-- Dependencies: 216
-- Name: pricelist; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE pricelist FROM PUBLIC;
REVOKE ALL ON TABLE pricelist FROM eivanov;
GRANT ALL ON TABLE pricelist TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pricelist TO g_bill_daemon_local;
GRANT SELECT ON TABLE pricelist TO g_readonly;


--
-- TOC entry 3201 (class 0 OID 0)
-- Dependencies: 176
-- Name: tarif; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE tarif FROM PUBLIC;
REVOKE ALL ON TABLE tarif FROM eivanov;
GRANT ALL ON TABLE tarif TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE tarif TO g_bill_daemon_local;
GRANT SELECT ON TABLE tarif TO kostik;
GRANT SELECT ON TABLE tarif TO g_readonly;


--
-- TOC entry 3202 (class 0 OID 0)
-- Dependencies: 222
-- Name: tarif_change_log; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE tarif_change_log FROM PUBLIC;
REVOKE ALL ON TABLE tarif_change_log FROM eivanov;
GRANT ALL ON TABLE tarif_change_log TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE tarif_change_log TO g_bill_daemon_local;
GRANT SELECT ON TABLE tarif_change_log TO kostik;
GRANT SELECT ON TABLE tarif_change_log TO g_readonly;


--
-- TOC entry 3203 (class 0 OID 0)
-- Dependencies: 177
-- Name: usage; Type: ACL; Schema: billing; Owner: eivanov
--

REVOKE ALL ON TABLE usage FROM PUBLIC;
REVOKE ALL ON TABLE usage FROM eivanov;
GRANT ALL ON TABLE usage TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE usage TO g_bill_daemon_local;
GRANT SELECT ON TABLE usage TO kostik;
GRANT SELECT ON TABLE usage TO g_readonly;


SET search_path = calls, pg_catalog;

--
-- TOC entry 3204 (class 0 OID 0)
-- Dependencies: 179
-- Name: calls; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls FROM PUBLIC;
REVOKE ALL ON TABLE calls FROM eivanov;
GRANT ALL ON TABLE calls TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls TO g_readonly;


--
-- TOC entry 3205 (class 0 OID 0)
-- Dependencies: 180
-- Name: calls_201201; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201201 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201201 FROM eivanov;
GRANT ALL ON TABLE calls_201201 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201201 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201201 TO g_readonly;


--
-- TOC entry 3206 (class 0 OID 0)
-- Dependencies: 181
-- Name: calls_201202; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201202 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201202 FROM eivanov;
GRANT ALL ON TABLE calls_201202 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201202 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201202 TO g_readonly;


--
-- TOC entry 3207 (class 0 OID 0)
-- Dependencies: 182
-- Name: calls_201203; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201203 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201203 FROM eivanov;
GRANT ALL ON TABLE calls_201203 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201203 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201203 TO g_readonly;


--
-- TOC entry 3208 (class 0 OID 0)
-- Dependencies: 183
-- Name: calls_201204; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201204 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201204 FROM eivanov;
GRANT ALL ON TABLE calls_201204 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201204 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201204 TO g_readonly;


--
-- TOC entry 3209 (class 0 OID 0)
-- Dependencies: 184
-- Name: calls_201205; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201205 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201205 FROM eivanov;
GRANT ALL ON TABLE calls_201205 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201205 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201205 TO g_readonly;


--
-- TOC entry 3210 (class 0 OID 0)
-- Dependencies: 185
-- Name: calls_201206; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201206 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201206 FROM eivanov;
GRANT ALL ON TABLE calls_201206 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201206 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201206 TO g_readonly;


--
-- TOC entry 3211 (class 0 OID 0)
-- Dependencies: 186
-- Name: calls_201207; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201207 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201207 FROM eivanov;
GRANT ALL ON TABLE calls_201207 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201207 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201207 TO g_readonly;


--
-- TOC entry 3212 (class 0 OID 0)
-- Dependencies: 187
-- Name: calls_201208; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201208 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201208 FROM eivanov;
GRANT ALL ON TABLE calls_201208 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201208 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201208 TO g_readonly;


--
-- TOC entry 3213 (class 0 OID 0)
-- Dependencies: 188
-- Name: calls_201209; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201209 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201209 FROM eivanov;
GRANT ALL ON TABLE calls_201209 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201209 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201209 TO g_readonly;


--
-- TOC entry 3214 (class 0 OID 0)
-- Dependencies: 189
-- Name: calls_201210; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201210 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201210 FROM eivanov;
GRANT ALL ON TABLE calls_201210 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201210 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201210 TO g_readonly;


--
-- TOC entry 3215 (class 0 OID 0)
-- Dependencies: 190
-- Name: calls_201211; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201211 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201211 FROM eivanov;
GRANT ALL ON TABLE calls_201211 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201211 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201211 TO g_readonly;


--
-- TOC entry 3216 (class 0 OID 0)
-- Dependencies: 191
-- Name: calls_201212; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201212 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201212 FROM eivanov;
GRANT ALL ON TABLE calls_201212 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201212 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201212 TO g_readonly;


--
-- TOC entry 3217 (class 0 OID 0)
-- Dependencies: 192
-- Name: calls_201301; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201301 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201301 FROM eivanov;
GRANT ALL ON TABLE calls_201301 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201301 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201301 TO g_readonly;


--
-- TOC entry 3218 (class 0 OID 0)
-- Dependencies: 193
-- Name: calls_201302; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201302 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201302 FROM eivanov;
GRANT ALL ON TABLE calls_201302 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201302 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201302 TO g_readonly;


--
-- TOC entry 3219 (class 0 OID 0)
-- Dependencies: 194
-- Name: calls_201303; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201303 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201303 FROM eivanov;
GRANT ALL ON TABLE calls_201303 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201303 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201303 TO g_readonly;


--
-- TOC entry 3220 (class 0 OID 0)
-- Dependencies: 195
-- Name: calls_201304; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201304 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201304 FROM eivanov;
GRANT ALL ON TABLE calls_201304 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201304 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201304 TO g_readonly;


--
-- TOC entry 3221 (class 0 OID 0)
-- Dependencies: 196
-- Name: calls_201305; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201305 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201305 FROM eivanov;
GRANT ALL ON TABLE calls_201305 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201305 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201305 TO g_readonly;


--
-- TOC entry 3222 (class 0 OID 0)
-- Dependencies: 197
-- Name: calls_201306; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201306 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201306 FROM eivanov;
GRANT ALL ON TABLE calls_201306 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201306 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201306 TO g_readonly;


--
-- TOC entry 3223 (class 0 OID 0)
-- Dependencies: 198
-- Name: calls_201307; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201307 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201307 FROM eivanov;
GRANT ALL ON TABLE calls_201307 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201307 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201307 TO g_readonly;


--
-- TOC entry 3224 (class 0 OID 0)
-- Dependencies: 199
-- Name: calls_201308; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201308 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201308 FROM eivanov;
GRANT ALL ON TABLE calls_201308 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201308 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201308 TO g_readonly;


--
-- TOC entry 3225 (class 0 OID 0)
-- Dependencies: 200
-- Name: calls_201309; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201309 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201309 FROM eivanov;
GRANT ALL ON TABLE calls_201309 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201309 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201309 TO g_readonly;


--
-- TOC entry 3226 (class 0 OID 0)
-- Dependencies: 201
-- Name: calls_201310; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201310 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201310 FROM eivanov;
GRANT ALL ON TABLE calls_201310 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201310 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201310 TO g_readonly;


--
-- TOC entry 3227 (class 0 OID 0)
-- Dependencies: 202
-- Name: calls_201311; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201311 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201311 FROM eivanov;
GRANT ALL ON TABLE calls_201311 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201311 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201311 TO g_readonly;


--
-- TOC entry 3228 (class 0 OID 0)
-- Dependencies: 203
-- Name: calls_201312; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201312 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201312 FROM eivanov;
GRANT ALL ON TABLE calls_201312 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201312 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201312 TO g_readonly;


--
-- TOC entry 3229 (class 0 OID 0)
-- Dependencies: 204
-- Name: calls_201401; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201401 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201401 FROM eivanov;
GRANT ALL ON TABLE calls_201401 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201401 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201401 TO g_readonly;


--
-- TOC entry 3230 (class 0 OID 0)
-- Dependencies: 205
-- Name: calls_201402; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201402 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201402 FROM eivanov;
GRANT ALL ON TABLE calls_201402 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201402 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201402 TO g_readonly;


--
-- TOC entry 3231 (class 0 OID 0)
-- Dependencies: 206
-- Name: calls_201403; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201403 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201403 FROM eivanov;
GRANT ALL ON TABLE calls_201403 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201403 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201403 TO g_readonly;


--
-- TOC entry 3232 (class 0 OID 0)
-- Dependencies: 207
-- Name: calls_201404; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201404 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201404 FROM eivanov;
GRANT ALL ON TABLE calls_201404 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201404 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201404 TO g_readonly;


--
-- TOC entry 3233 (class 0 OID 0)
-- Dependencies: 208
-- Name: calls_201405; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201405 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201405 FROM eivanov;
GRANT ALL ON TABLE calls_201405 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201405 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201405 TO g_readonly;


--
-- TOC entry 3234 (class 0 OID 0)
-- Dependencies: 209
-- Name: calls_201406; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201406 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201406 FROM eivanov;
GRANT ALL ON TABLE calls_201406 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201406 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201406 TO g_readonly;


--
-- TOC entry 3235 (class 0 OID 0)
-- Dependencies: 210
-- Name: calls_201407; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201407 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201407 FROM eivanov;
GRANT ALL ON TABLE calls_201407 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201407 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201407 TO g_readonly;


--
-- TOC entry 3236 (class 0 OID 0)
-- Dependencies: 211
-- Name: calls_201408; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201408 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201408 FROM eivanov;
GRANT ALL ON TABLE calls_201408 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201408 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201408 TO g_readonly;


--
-- TOC entry 3237 (class 0 OID 0)
-- Dependencies: 212
-- Name: calls_201409; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201409 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201409 FROM eivanov;
GRANT ALL ON TABLE calls_201409 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201409 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201409 TO g_readonly;


--
-- TOC entry 3238 (class 0 OID 0)
-- Dependencies: 213
-- Name: calls_201410; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201410 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201410 FROM eivanov;
GRANT ALL ON TABLE calls_201410 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201410 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201410 TO g_readonly;


--
-- TOC entry 3239 (class 0 OID 0)
-- Dependencies: 214
-- Name: calls_201411; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201411 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201411 FROM eivanov;
GRANT ALL ON TABLE calls_201411 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201411 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201411 TO g_readonly;


--
-- TOC entry 3240 (class 0 OID 0)
-- Dependencies: 215
-- Name: calls_201412; Type: ACL; Schema: calls; Owner: eivanov
--

REVOKE ALL ON TABLE calls_201412 FROM PUBLIC;
REVOKE ALL ON TABLE calls_201412 FROM eivanov;
GRANT ALL ON TABLE calls_201412 TO eivanov;
GRANT SELECT,INSERT,DELETE ON TABLE calls_201412 TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_201412 TO g_readonly;


SET search_path = geo, pg_catalog;

--
-- TOC entry 3241 (class 0 OID 0)
-- Dependencies: 178
-- Name: prefix; Type: ACL; Schema: geo; Owner: eivanov
--

REVOKE ALL ON TABLE prefix FROM PUBLIC;
REVOKE ALL ON TABLE prefix FROM eivanov;
GRANT ALL ON TABLE prefix TO eivanov;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE prefix TO g_bill_daemon_local;
GRANT SELECT ON TABLE prefix TO kostik;
GRANT SELECT ON TABLE prefix TO g_readonly;


-- Completed on 2014-09-01 22:22:32 MSK

--
-- PostgreSQL database dump complete
--

