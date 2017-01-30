CREATE SCHEMA backlog AUTHORIZATION postgres;

CREATE TABLE backlog.events
(
  id bigserial NOT NULL,
  event_time timestamp without time zone DEFAULT now(),
  event_type character varying,
  username character varying,
  schema_name character varying,
  table_name character varying,
  backup_name character varying,
  CONSTRAINT backlog_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);

ALTER TABLE backlog.events OWNER TO postgres;
GRANT ALL ON TABLE backlog.events TO postgres;
GRANT SELECT ON TABLE backlog.events TO g_readonly;
GRANT ALL ON TABLE backlog.events TO g_stat;

CREATE OR REPLACE FUNCTION backlog.notify_table_update() RETURNS trigger AS
$BODY$
DECLARE 
    relname varchar;
    rel_exists text;
    param integer;
BEGIN

    relname := 'backlog.' || TG_TABLE_SCHEMA || '_' || TG_TABLE_NAME;
    --RAISE NOTICE '%', relname;

    EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;
    IF (rel_exists IS NULL) OR (rel_exists='') 
    THEN

        EXECUTE 'INSERT INTO backlog.events (event_type, schema_name, table_name) '
                'VALUES('|| quote_literal(TG_OP) ||','|| quote_literal(TG_TABLE_SCHEMA) || ',' || quote_literal(TG_TABLE_NAME) || ')';
        RETURN NULL;

    END IF;
    
    EXECUTE 'INSERT INTO backlog.events (event_type, schema_name, table_name, backup_name, username) '
            'VALUES('|| quote_literal(TG_OP) ||','|| quote_literal(TG_TABLE_SCHEMA) || ',' || quote_literal(TG_TABLE_NAME) || ',' || quote_literal(relname) || ',' || quote_literal(current_user) || ')';
    EXECUTE 'select currval(' || quote_literal('backlog.events_id_seq') || ')' INTO param;

    IF (TG_OP='INSERT')
    THEN
        EXECUTE 'INSERT INTO ' || relname || ' SELECT ' || param || ', ($1).*' USING NEW;
    ELSE

        EXECUTE 'INSERT INTO ' || relname || ' SELECT ' || param || ', ($1).*' USING OLD;
        --RAISE NOTICE '%', param;
    END IF;
    
    RETURN NULL;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION backlog.notify_table_update()
  OWNER TO postgres;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_airp
(
  event_id bigint,
  id integer NOT NULL,
  server_id integer NOT NULL,
  name character varying(50) NOT NULL
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_airp OWNER TO postgres;

ALTER TABLE backlog.auth_airp
  ADD CONSTRAINT auth_airp_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_destination
(
  event_id bigint,
  id integer NOT NULL,
  server_id integer NOT NULL,
  name character varying(50) NOT NULL,
  prefixlist_ids integer[] NOT NULL
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_destination OWNER TO postgres;

ALTER TABLE backlog.auth_destination
  ADD CONSTRAINT auth_destination_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_hub
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  dt timestamp without time zone DEFAULT now(),
  name character varying(50) NOT NULL,
  note text
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_hub OWNER TO postgres;

ALTER TABLE backlog.auth_hub
  ADD CONSTRAINT auth_hub_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_number
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  server_id integer NOT NULL,
  name character varying(50) NOT NULL,
  prefixlist_ids integer[] NOT NULL,
  sw_shared boolean DEFAULT false
)
WITH (
  OIDS=FALSE
);
ALTER TABLE auth."number"  OWNER TO postgres;

ALTER TABLE backlog.auth_number
  ADD CONSTRAINT auth_number_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_outcome
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  server_id integer NOT NULL,
  name character varying(50) NOT NULL,
  calling_station_id character varying(20),
  called_station_id character varying(20),
  type_id integer NOT NULL,
  route_case_id integer,
  release_reason_id integer,
  airp_id integer,
  route_case_1_id integer,
  route_case_2_id integer,
  sw_shared boolean DEFAULT false
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_outcome OWNER TO postgres;

ALTER TABLE backlog.auth_outcome
  ADD CONSTRAINT auth_outcome FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_prefixlist
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  server_id integer NOT NULL,
  name character varying(50) NOT NULL,
  sw_shared boolean DEFAULT false
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_prefixlist  OWNER TO postgres;

ALTER TABLE backlog.auth_prefixlist
  ADD CONSTRAINT auth_prefixlist FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_prefixlist_prefix
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  prefixlist_id integer NOT NULL,
  prefix character varying(20) NOT NULL
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_prefixlist_prefix  OWNER TO postgres;

ALTER TABLE backlog.auth_prefixlist_prefix
  ADD CONSTRAINT auth_prefixlist_prefix_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_release_reason
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  server_id integer NOT NULL,
  name character varying(50) NOT NULL,
  sw_shared boolean DEFAULT false
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_release_reason OWNER TO postgres;

ALTER TABLE backlog.auth_release_reason
  ADD CONSTRAINT auth_release_reason_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_route_case
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  server_id integer NOT NULL,
  name character varying(50) NOT NULL,
  sw_shared boolean DEFAULT false
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_route_case  OWNER TO postgres;

ALTER TABLE backlog.auth_route_case
  ADD CONSTRAINT auth_route_case_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_route_table
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  server_id integer NOT NULL,
  name character varying(50) NOT NULL,
  sw_shared boolean DEFAULT false
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_route_table  OWNER TO postgres;

ALTER TABLE backlog.auth_route_table
  ADD CONSTRAINT auth_route_table_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_route_table_route
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  route_table_id integer NOT NULL,
  "order" smallint NOT NULL,
  a_number_id integer,
  b_number_id integer,
  outcome_id integer,
  outcome_route_table_id integer
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_route_table_route  OWNER TO postgres;

ALTER TABLE backlog.auth_route_table_route
  ADD CONSTRAINT auth_route_table_route_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_routing_report_data
(
  event_id bigint NOT NULL,
  prefix character varying(20) NOT NULL,
  routes integer[]
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_routing_report_data  OWNER TO postgres;

ALTER TABLE backlog.auth_routing_report_data
  ADD CONSTRAINT auth_routing_report_data FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  server_id integer NOT NULL,
  code smallint,
  name character varying(50) NOT NULL,
  source_rule_default_allowed boolean NOT NULL DEFAULT true,
  destination_rule_default_allowed boolean NOT NULL DEFAULT true,
  default_priority smallint NOT NULL DEFAULT 0,
  trunk_name character varying(32) COLLATE pg_catalog."ar_EG.utf8" NOT NULL,
  auto_routing boolean NOT NULL DEFAULT false,
  route_table_id integer,
  our_trunk boolean NOT NULL DEFAULT false,
  trunk_name_alias character varying(32),
  auth_by_number boolean NOT NULL DEFAULT false,
  orig_redirect_number boolean NOT NULL DEFAULT false,
  term_redirect_number boolean NOT NULL DEFAULT false,
  source_trunk_rule_default_allowed boolean DEFAULT true,
  orig_redirect_number_7800 boolean NOT NULL DEFAULT false,
  capacity integer,
  sw_minimalki boolean DEFAULT false, -- При включенном флаге на оригинационном плече начинают работать минималки на терминационном плече
  sw_shared boolean DEFAULT false,
  load_warning integer,
  road_to_region integer
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk OWNER TO postgres;

ALTER TABLE backlog.auth_trunk
  ADD CONSTRAINT auth_trunk_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_group
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  name character varying(50) NOT NULL,
  server_id integer NOT NULL
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_group  OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_group
  ADD CONSTRAINT auth_trunk_group_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_group_item
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  trunk_group_id integer NOT NULL,
  trunk_id integer NOT NULL
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_group_item OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_group_item
  ADD CONSTRAINT auth_trunk_grop_item_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_number_preprocessing
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  trunk_id integer NOT NULL,
  "order" integer NOT NULL,
  src boolean NOT NULL,
  noa smallint NOT NULL,
  length smallint NOT NULL,
  prefix character varying(10) NOT NULL
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_number_preprocessing  OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_number_preprocessing
  ADD CONSTRAINT auth_trunk_number_preprocessing_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_priority
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  trunk_id integer NOT NULL,
  "order" smallint NOT NULL,
  priority smallint NOT NULL,
  prefixlist_id integer,
  trunk_group_id integer,
  number_id_filter_a integer,
  number_id_filter_b integer
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_priority  OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_priority
  ADD CONSTRAINT auth_trunk_priority_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_rule
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  trunk_id integer NOT NULL,
  outgoing boolean NOT NULL,
  "order" smallint NOT NULL,
  prefixlist_id integer NOT NULL
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_rule  OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_rule
  ADD CONSTRAINT auth_trunk_rule_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_trunk_rule
(
  event_id bigint NOT NULL,
  id integer NOT NULL,
  trunk_id integer NOT NULL,
  "order" smallint NOT NULL,
  trunk_group_id integer NOT NULL,
  number_id_filter_a integer,
  number_id_filter_b integer
)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_trunk_rule OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_trunk_rule
  ADD CONSTRAINT auth_trunk_trunk_rule_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;
