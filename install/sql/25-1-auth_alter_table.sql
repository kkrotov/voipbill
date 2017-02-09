-----------------------------------------------------------------------------------------------
-- auth.prefixlist_prefix

DROP TRIGGER notify ON auth.prefixlist_prefix;
ALTER TABLE auth.prefixlist_prefix DROP CONSTRAINT prefixlist_prefix_pkey;
ALTER TABLE auth.prefixlist_prefix DROP CONSTRAINT prefixlist_prefix_fk;
ALTER TABLE auth.prefixlist_prefix RENAME TO prefixlist_prefix_old;

CREATE TABLE auth.prefixlist_prefix
(
  id serial,
  prefixlist_id integer NOT NULL,
  prefix character varying(20) NOT NULL,
  CONSTRAINT prefixlist_prefix_pkey PRIMARY KEY (prefixlist_id, prefix),
  CONSTRAINT prefixlist_prefix_fk FOREIGN KEY (prefixlist_id)
      REFERENCES auth.prefixlist (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE
)
WITH (
  OIDS=FALSE
);
ALTER TABLE auth.prefixlist_prefix OWNER TO postgres;
GRANT SELECT ON TABLE auth.prefixlist_prefix TO g_readonly;
GRANT SELECT ON TABLE auth.prefixlist_prefix TO g_bill_daemon_remote;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE auth.prefixlist_prefix TO g_voipgui;
GRANT SELECT, INSERT, DELETE ON TABLE auth.prefixlist_prefix TO g_stat;

insert into auth.prefixlist_prefix (prefixlist_id, prefix) select prefixlist_id, prefix from auth.prefixlist_prefix_old;

-- Trigger: notify on auth.prefixlist_prefix

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.prefixlist_prefix
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify_prefixlist_prefix();

-----------------------------------------------------------------------------------------------
-- auth.route_case_trunk

ALTER TABLE auth.route_case_trunk DROP CONSTRAINT route_case_operator_pkey;
ALTER TABLE auth.route_case_trunk DROP CONSTRAINT route_case_operator_fk;
ALTER TABLE auth.route_case_trunk DROP CONSTRAINT route_case_operator_fk1;
ALTER TABLE auth.route_case_trunk RENAME TO route_case_trunk_old; 

CREATE TABLE auth.route_case_trunk
(
  id serial,
  route_case_id integer NOT NULL,
  trunk_id integer NOT NULL,
  priority smallint NOT NULL,
  weight smallint NOT NULL,
  CONSTRAINT route_case_operator_pkey PRIMARY KEY (route_case_id, trunk_id),
  CONSTRAINT route_case_operator_fk FOREIGN KEY (route_case_id)
      REFERENCES auth.route_case (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT route_case_operator_fk1 FOREIGN KEY (trunk_id)
      REFERENCES auth.trunk (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);
ALTER TABLE auth.route_case_trunk OWNER TO postgres;
GRANT SELECT ON TABLE auth.route_case_trunk TO g_bill_daemon_remote;
GRANT SELECT ON TABLE auth.route_case_trunk TO g_readonly;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE auth.route_case_trunk TO g_voipgui;
GRANT SELECT, INSERT, DELETE ON TABLE auth.route_case_trunk TO g_stat;

INSERT INTO auth.route_case_trunk (route_case_id, trunk_id, priority, weight) select route_case_id, trunk_id, priority, weight FROM auth.route_case_trunk_old;

-----------------------------------------------------------------------------------------------
-- auth.trunk_group_item
DROP INDEX auth.idx_trunk_group_item__trunk_id;
DROP TRIGGER notify ON auth.trunk_group_item;
ALTER TABLE auth.trunk_group_item DROP CONSTRAINT trunk_group_item_pkey;
ALTER TABLE auth.trunk_group_item DROP CONSTRAINT fk_trunk_group_item__trunk_group_id;
ALTER TABLE auth.trunk_group_item DROP CONSTRAINT fk_trunk_group_item__trunk_id;
ALTER TABLE auth.trunk_group_item RENAME TO trunk_group_item_old;

CREATE TABLE auth.trunk_group_item
(
  id serial,
  trunk_group_id integer NOT NULL,
  trunk_id integer NOT NULL,
  CONSTRAINT trunk_group_item_pkey PRIMARY KEY (trunk_group_id, trunk_id),
  CONSTRAINT fk_trunk_group_item__trunk_group_id FOREIGN KEY (trunk_group_id)
      REFERENCES auth.trunk_group (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_trunk_group_item__trunk_id FOREIGN KEY (trunk_id)
      REFERENCES auth.trunk (id) MATCH FULL
      ON UPDATE RESTRICT ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);
ALTER TABLE auth.trunk_group_item OWNER TO postgres;
GRANT SELECT ON TABLE auth.trunk_group_item TO g_bill_daemon_remote;
GRANT SELECT, INSERT, DELETE ON TABLE auth.trunk_group_item TO g_stat;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE auth.trunk_group_item TO g_voipgui;
GRANT SELECT ON TABLE auth.trunk_group_item TO g_readonly;

insert into auth.trunk_group_item (trunk_group_id,trunk_id) select trunk_group_id,trunk_id from auth.trunk_group_item_old;

CREATE INDEX idx_trunk_group_item__trunk_id
  ON auth.trunk_group_item_old
  USING btree
  (trunk_id);

-- Trigger: notify on auth.trunk_group_item

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.trunk_group_item
  FOR EACH STATEMENT
  EXECUTE PROCEDURE tr_notify_trunk_group_item();

-----------------------------------------------------------------------------------------------
-- auth.route_table_route

DROP TRIGGER notify ON auth.route_table_route;
ALTER TABLE auth.route_table_route DROP CONSTRAINT route_table_route_pkey;
ALTER TABLE auth.route_table_route DROP CONSTRAINT route_table_route_fk;
ALTER TABLE auth.route_table_route DROP CONSTRAINT route_table_route_fk1;
ALTER TABLE auth.route_table_route DROP CONSTRAINT route_table_route_fk2;
ALTER TABLE auth.route_table_route DROP CONSTRAINT route_table_route_fk3;
ALTER TABLE auth.route_table_route DROP CONSTRAINT route_table_route_fk4;
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
  CONSTRAINT route_table_route_pkey PRIMARY KEY (route_table_id, "order"),
  CONSTRAINT route_table_route_fk FOREIGN KEY (route_table_id)
      REFERENCES auth.route_table (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT route_table_route_fk1 FOREIGN KEY (a_number_id)
      REFERENCES auth."number" (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT route_table_route_fk2 FOREIGN KEY (b_number_id)
      REFERENCES auth."number" (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT route_table_route_fk3 FOREIGN KEY (outcome_id)
      REFERENCES auth.outcome (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT route_table_route_fk4 FOREIGN KEY (outcome_route_table_id)
      REFERENCES auth.route_table (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);
ALTER TABLE auth.route_table_route OWNER TO postgres;
GRANT SELECT ON TABLE auth.route_table_route TO g_readonly;
GRANT SELECT ON TABLE auth.route_table_route TO g_bill_daemon_remote;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE auth.route_table_route TO g_voipgui;
GRANT SELECT, INSERT, DELETE ON TABLE auth.route_table_route TO g_stat;

insert into auth.route_table_route(route_table_id,"order",a_number_id,b_number_id,outcome_id,outcome_route_table_id) 
	select route_table_id,"order",a_number_id,b_number_id,outcome_id,outcome_route_table_id from auth.route_table_route_old;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.route_table_route
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify_route_table_route();


-----------------------------------------------------------------------------------------------
-- auth.routing_report_data

DROP TRIGGER notify ON auth.routing_report_data;
ALTER TABLE auth.routing_report_data DROP CONSTRAINT routing_report_data_pkey;
ALTER TABLE auth.routing_report_data RENAME TO routing_report_data_old;

CREATE TABLE auth.routing_report_data
(
  id serial,
  server_id integer NOT NULL,
  prefix character varying(20) NOT NULL,
  prices numeric(8,2)[],
  locks boolean[],
  priorities integer[],
  orders smallint[],
  routes integer[],
  CONSTRAINT routing_report_data_pkey PRIMARY KEY (server_id, prefix)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE auth.routing_report_data OWNER TO postgres;
GRANT SELECT ON TABLE auth.routing_report_data TO g_readonly;
GRANT SELECT ON TABLE auth.routing_report_data TO g_bill_daemon_remote;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE auth.routing_report_data TO g_voipgui;
GRANT SELECT, INSERT, DELETE ON TABLE auth.routing_report_data TO g_stat;

INSERT INTO auth.routing_report_data (server_id, prefix, prices, locks, priorities, orders, routes) 
	SELECT server_id, prefix, prices, locks, priorities, orders, routes FROM auth.routing_report_data_old;

-- Trigger: notify on auth.routing_report_data

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.routing_report_data
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify_routing_report_data();
