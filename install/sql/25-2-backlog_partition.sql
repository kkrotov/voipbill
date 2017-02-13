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

CREATE OR REPLACE FUNCTION backlog.notify_table_update()
  RETURNS trigger AS
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
        EXECUTE 'INSERT INTO ' || relname || ' SELECT ($1).*,' || param USING NEW;
    ELSE

        EXECUTE 'INSERT INTO ' || relname || ' SELECT ($1).*,' || param USING OLD;
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
CREATE TABLE backlog.auth_airp (LIKE auth.airp)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_airp OWNER TO postgres;

ALTER TABLE backlog.auth_airp 
  ADD COLUMN event_id bigint;

ALTER TABLE backlog.auth_airp
  ADD CONSTRAINT auth_airp_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_attribute (LIKE auth.attribute)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_attribute OWNER TO postgres;

ALTER TABLE backlog.auth_attribute 
  ADD COLUMN event_id bigint;

ALTER TABLE backlog.auth_attribute
  ADD CONSTRAINT auth_attribute_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_attribute_group (LIKE auth.attribute_group)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_attribute_group OWNER TO postgres;

ALTER TABLE backlog.auth_attribute_group
  ADD COLUMN event_id bigint;

ALTER TABLE backlog.auth_attribute_group 
  ADD CONSTRAINT auth_attribute_group_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_attribute_value (LIKE auth.attribute_value)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_attribute_value OWNER TO postgres;

ALTER TABLE backlog.auth_attribute_value
  ADD COLUMN event_id bigint;

ALTER TABLE backlog.auth_attribute_value 
  ADD CONSTRAINT auth_attribute_value_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_destination (LIKE auth.destination)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_destination OWNER TO postgres;

ALTER TABLE backlog.auth_destination
  ADD COLUMN event_id bigint;

ALTER TABLE backlog.auth_destination
  ADD CONSTRAINT auth_destination_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_hub (LIKE auth.hub)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_hub OWNER TO postgres;

ALTER TABLE backlog.auth_hub
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_hub
  ADD CONSTRAINT auth_hub_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_number (LIKE auth."number")
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_number OWNER TO postgres;

ALTER TABLE backlog.auth_number
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_number
  ADD CONSTRAINT auth_number_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_outcome (LIKE auth.outcome)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_outcome OWNER TO postgres;

ALTER TABLE backlog.auth_outcome
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_outcome
  ADD CONSTRAINT auth_outcome FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_prefixlist (LIKE auth.prefixlist)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_prefixlist  OWNER TO postgres;

ALTER TABLE backlog.auth_prefixlist
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_prefixlist
  ADD CONSTRAINT auth_prefixlist FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_prefixlist_prefix (LIKE auth.prefixlist_prefix)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_prefixlist_prefix  OWNER TO postgres;

ALTER TABLE backlog.auth_prefixlist_prefix
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_prefixlist_prefix
  ADD CONSTRAINT auth_prefixlist_prefix_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_release_reason (LIKE auth.release_reason)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_release_reason OWNER TO postgres;

ALTER TABLE backlog.auth_release_reason
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_release_reason
  ADD CONSTRAINT auth_release_reason_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_route_case (LIKE auth.route_case)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_route_case  OWNER TO postgres;

ALTER TABLE backlog.auth_route_case
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_route_case
  ADD CONSTRAINT auth_route_case_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_route_case_trunk (LIKE auth.route_case_trunk)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_route_case_trunk OWNER TO postgres;

ALTER TABLE backlog.auth_route_case_trunk
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_route_case_trunk
  ADD CONSTRAINT auth_route_case_trunk_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_route_table (LIKE auth.route_table)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_route_table  OWNER TO postgres;

ALTER TABLE backlog.auth_route_table
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_route_table
  ADD CONSTRAINT auth_route_table_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_route_table_route (LIKE auth.route_table_route)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_route_table_route  OWNER TO postgres;

ALTER TABLE backlog.auth_route_table_route
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_route_table_route
  ADD CONSTRAINT auth_route_table_route_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_routing_report_data (LIKE auth.routing_report_data)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_routing_report_data OWNER TO postgres;

ALTER TABLE backlog.auth_routing_report_data
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_routing_report_data
  ADD CONSTRAINT auth_routing_report_data FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk (LIKE auth.trunk)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk OWNER TO postgres;

ALTER TABLE backlog.auth_trunk
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_trunk
  ADD CONSTRAINT auth_trunk_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_group (LIKE auth.trunk_group)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_group  OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_group
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_trunk_group
  ADD CONSTRAINT auth_trunk_group_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_group_item (LIKE auth.trunk_group_item)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_group_item OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_group_item
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_trunk_group_item
  ADD CONSTRAINT auth_trunk_grop_item_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_number_preprocessing (LIKE auth.trunk_number_preprocessing)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_number_preprocessing  OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_number_preprocessing
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_trunk_number_preprocessing
  ADD CONSTRAINT auth_trunk_number_preprocessing_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_priority (LIKE auth.trunk_priority)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_priority  OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_priority
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_trunk_priority
  ADD CONSTRAINT auth_trunk_priority_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_rule (LIKE auth.trunk_rule)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_rule OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_rule
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_trunk_rule
  ADD CONSTRAINT auth_trunk_rule_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_trunk_trunk_rule (LIKE auth.trunk_trunk_rule)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_trunk_trunk_rule OWNER TO postgres;

ALTER TABLE backlog.auth_trunk_trunk_rule
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_trunk_trunk_rule
  ADD CONSTRAINT auth_trunk_trunk_rule_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_user (LIKE auth."user")
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_user OWNER TO postgres;

ALTER TABLE backlog.auth_user
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_user
  ADD CONSTRAINT auth_user_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_test_auth (LIKE auth.test_auth)
WITH (
  OIDS=FALSE
);

ALTER TABLE backlog.auth_test_auth OWNER TO postgres;

ALTER TABLE backlog.auth_test_auth
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_test_auth
  ADD CONSTRAINT auth_test_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;

-------------------------------------------------------------------------------
CREATE TABLE backlog.auth_test_call (LIKE auth.test_call)
WITH (
  OIDS=FALSE
);
ALTER TABLE backlog.auth_test_call OWNER TO postgres;

ALTER TABLE backlog.auth_test_call
  ADD COLUMN event_id bigint NOT NULL;

ALTER TABLE backlog.auth_test_call
  ADD CONSTRAINT auth_test_call_fk FOREIGN KEY (event_id)
      REFERENCES backlog.events (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION;
