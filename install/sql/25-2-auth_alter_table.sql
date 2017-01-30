ALTER TABLE auth.trunk_rule DROP CONSTRAINT trunk_rule_pkey;
ALTER TABLE auth.trunk_rule RENAME TO trunk_rule_old;

CREATE TABLE auth.trunk_rule
(
  id serial,
  trunk_id integer NOT NULL,
  outgoing boolean NOT NULL,
  "order" smallint NOT NULL,
  prefixlist_id integer NOT NULL,
  CONSTRAINT trunk_rule_pkey UNIQUE (trunk_id, outgoing, "order")
)
WITH (
  OIDS=FALSE
);
ALTER TABLE auth.trunk_rule OWNER TO postgres;
GRANT ALL ON TABLE auth.trunk_rule TO postgres;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE auth.trunk_rule TO g_bill_daemon_local;
GRANT SELECT ON TABLE auth.trunk_rule TO g_readonly;

insert into auth.trunk_rule (trunk_id,outgoing,"order",prefixlist_id) select trunk_id,outgoing,"order",prefixlist_id from auth.trunk_rule_old;
-- Trigger: backlog_notify on auth.trunk_rule

-- DROP TRIGGER backlog_notify ON auth.trunk_rule;

CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.trunk_rule
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-- Trigger: notify on auth.trunk_rule

-- DROP TRIGGER notify ON auth.trunk_rule;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.trunk_rule
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

ALTER TABLE auth.trunk_group_item DROP CONSTRAINT trunk_group_item_pkey;
ALTER TABLE auth.trunk_group_item RENAME TO trunk_group_item_old;

CREATE TABLE auth.trunk_group_item
(
  id serial,
  trunk_group_id integer NOT NULL,
  trunk_id integer NOT NULL,
  CONSTRAINT trunk_group_item_pkey PRIMARY KEY (trunk_group_id, trunk_id)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE auth.trunk_group_item
  OWNER TO postgres;
GRANT ALL ON TABLE auth.trunk_group_item TO postgres;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE auth.trunk_group_item TO g_bill_daemon_local;
GRANT SELECT ON TABLE auth.trunk_group_item TO g_readonly;

insert into auth.trunk_group_item (trunk_group_id,trunk_id) select trunk_group_id,trunk_id from auth.trunk_group_item_old;

-- Trigger: backlog_notify on auth.trunk_group_item

-- DROP TRIGGER backlog_notify ON auth.trunk_group_item;

CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.trunk_group_item
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-- Trigger: notify on auth.trunk_group_item

-- DROP TRIGGER notify ON auth.trunk_group_item;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.trunk_group_item
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();



ALTER TABLE auth.route_table_route DROP CONSTRAINT route_table_route_pkey;
ALTER TABLE auth.route_table_route RENAME TO route_table_route_old;
CREATE TABLE auth.route_table_route
(
  id serial,
  route_table_id integer NOT NULL,
  "order" smallint NOT NULL,
  a_number_id integer,
  b_number_id integer,
  outcome_id integer,
  outcome_route_table_id integer,
  CONSTRAINT route_table_route_pkey PRIMARY KEY (route_table_id, "order")
)
WITH (
  OIDS=FALSE
);
ALTER TABLE auth.route_table_route
  OWNER TO postgres;
GRANT ALL ON TABLE auth.route_table_route TO postgres;
GRANT SELECT ON TABLE auth.route_table_route TO g_readonly;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE auth.route_table_route TO g_bill_daemon_local;

insert into auth.route_table_route(route_table_id,"order",a_number_id,b_number_id,outcome_id,outcome_route_table_id) 
	select route_table_id,"order",a_number_id,b_number_id,outcome_id,outcome_route_table_id from auth.route_table_route_old;

-- Trigger: backlog_notify on auth.route_table_route

-- DROP TRIGGER backlog_notify ON auth.route_table_route;

CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.route_table_route
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-- Trigger: notify on auth.route_table_route

-- DROP TRIGGER notify ON auth.route_table_route;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.route_table_route
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();


ALTER TABLE auth.prefixlist_prefix DROP CONSTRAINT prefixlist_prefix_pkey;

ALTER TABLE auth.prefixlist_prefix RENAME TO prefixlist_prefix_old;

CREATE TABLE auth.prefixlist_prefix
(
  id serial,
  prefixlist_id integer NOT NULL,
  prefix character varying(20) NOT NULL,
  CONSTRAINT prefixlist_prefix_pkey PRIMARY KEY (prefixlist_id, prefix)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE auth.prefixlist_prefix
  OWNER TO postgres;
GRANT ALL ON TABLE auth.prefixlist_prefix TO postgres;
GRANT SELECT ON TABLE auth.prefixlist_prefix TO g_readonly;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE auth.prefixlist_prefix TO g_bill_daemon_local;


insert into auth.prefixlist_prefix (prefixlist_id, prefix) select prefixlist_id, prefix from auth.prefixlist_prefix_old;

-- Trigger: backlog_notify on auth.prefixlist_prefix

-- DROP TRIGGER backlog_notify ON auth.prefixlist_prefix;

CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.prefixlist_prefix
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-- Trigger: notify on auth.prefixlist_prefix

-- DROP TRIGGER notify ON auth.prefixlist_prefix;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.prefixlist_prefix
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

