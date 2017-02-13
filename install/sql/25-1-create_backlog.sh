#!/bin/bash
PGDUMP='/usr/pgsql-9.4/bin/pg_dump'
DATABASE='nispd_test'
SCHEMA='auth'

##################################################################################################
alter_tables () {
echo '
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
drop table auth.prefixlist_prefix_old;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.prefixlist_prefix
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify_prefixlist_prefix();

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
DROP TABLE auth.route_case_trunk_old;

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
DROP TABLE auth.trunk_group_item_old;

CREATE INDEX idx_trunk_group_item__trunk_id
  ON auth.trunk_group_item
  USING btree
  (trunk_id);

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.trunk_group_item
  FOR EACH STATEMENT
  EXECUTE PROCEDURE tr_notify_trunk_group_item();

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
DROP TABLE auth.route_table_route_old;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.route_table_route
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify_route_table_route();

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
DROP TABLE auth.routing_report_data_old;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON auth.routing_report_data
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify_routing_report_data();'
}

##################################################################################################
create_backlog_schema() {

echo 'CREATE SCHEMA backlog AUTHORIZATION postgres;'
echo '
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
GRANT ALL ON TABLE backlog.events TO g_stat;'

echo '
CREATE OR REPLACE FUNCTION backlog.notify_table_update()
  RETURNS trigger AS
$BODY$
DECLARE
    relname varchar;
    rel_exists text;
    param integer;
BEGIN'

echo '
    relname := '\''backlog.'\'' || TG_TABLE_SCHEMA || '\''_'\'' || TG_TABLE_NAME;'

echo '
    EXECUTE '\''SELECT to_regclass('\''|| quote_literal(relname) ||'\'');'\'' INTO rel_exists;
    IF (rel_exists IS NULL) OR (rel_exists='\'''\'')
    THEN

        EXECUTE '\''INSERT INTO backlog.events (event_type, schema_name, table_name) '\''
                '\''VALUES('\''|| quote_literal(TG_OP) ||'\'','\''|| quote_literal(TG_TABLE_SCHEMA) || '\'','\'' || quote_literal(TG_TABLE_NAME) || '\'')'\'';
        RETURN NULL;

    END IF;

    EXECUTE '\''INSERT INTO backlog.events (event_type, schema_name, table_name, backup_name, username) '\''
            '\''VALUES('\''|| quote_literal(TG_OP) ||'\'','\''|| quote_literal(TG_TABLE_SCHEMA) || '\'','\'' || quote_literal(TG_TABLE_NAME) || '\'','\'' || quote_literal(relname) || '\'','\'' || quote_literal(current_user) || '\'')'\'';
    EXECUTE '\''select currval('\'' || quote_literal('\''backlog.events_id_seq'\'') || '\'')'\'' INTO param;

    IF (TG_OP='\''INSERT'\'')
    THEN
        EXECUTE '\''INSERT INTO '\'' || relname || '\'' SELECT ($1).*,'\'' || param USING NEW;
    ELSE

        EXECUTE '\''INSERT INTO '\'' || relname || '\'' SELECT ($1).*,'\'' || param USING OLD;
    END IF;

    RETURN NULL;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION backlog.notify_table_update()
  OWNER TO postgres;'

for tablename in `psql -d $DATABASE -t -c "SELECT table_name FROM information_schema.tables WHERE table_schema = '$SCHEMA' order by table_name"`
do
	echo 'CREATE TABLE backlog.'$SCHEMA'_'$tablename' (LIKE '$SCHEMA'."'$tablename'") WITH (OIDS=FALSE);'
	echo 'ALTER TABLE backlog.'$SCHEMA'_'$tablename' OWNER TO postgres;'
	echo 'ALTER TABLE backlog.'$SCHEMA'_'$tablename' 
		ADD COLUMN event_id bigint;'
	echo 'ALTER TABLE backlog.'$SCHEMA'_'$tablename' 
		ADD CONSTRAINT '$SCHEMA'_'$tablename'_fk FOREIGN KEY (event_id) 
		REFERENCES backlog.events (id) MATCH SIMPLE ON UPDATE NO ACTION ON DELETE NO ACTION;'
done
#<<COMMENT
#COMMENT
}

##################################################################################################
create_auth_triggers () {

echo 'SET client_min_messages TO WARNING;'
for tablename in `psql -d $DATABASE -t -c "SELECT table_name FROM information_schema.tables WHERE table_schema = '$SCHEMA' order by table_name"`
do
echo '
DROP TRIGGER IF EXISTS backlog_notify ON '$SCHEMA'."'$tablename'";
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON '$SCHEMA'."'$tablename'"
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();
'
done
}

##################################################################################################
create_undo_func () {

echo '
CREATE OR REPLACE FUNCTION backlog.undo_event(event_id integer)
  RETURNS boolean AS
$BODY$
DECLARE 
    relname varchar;
    colnames text[];
    cols text;
    cname text;
    i integer;
    r record;
    sql_stat text;
BEGIN

    FOR r IN SELECT id, event_time, event_type, username, schema_name, table_name, backup_name 
    FROM backlog.events
    WHERE id>=event_id order by id desc
    LOOP
        relname:=r.schema_name||'\''.'\''||r.table_name;
        colnames:=ARRAY(SELECT quote_ident(attname) FROM pg_catalog.pg_attribute
                                       WHERE  attrelid = relname::regclass -- regclass used as OID
                                       AND    attnum > 0               -- exclude system columns
                                       AND    attisdropped = FALSE     -- exclude dropped columns
                                 );
        
        CASE
            WHEN r.event_type='\''INSERT'\'' THEN
                sql_stat := '\''DELETE FROM '\''||relname||'\'' WHERE '\''||colnames[1]||
                            '\''=(SELECT '\''||colnames[1]||'\'' FROM '\''||r.backup_name||'\'' WHERE event_id='\''||r.id||'\'')'\'';
                            
            WHEN r.event_type='\''DELETE'\'' THEN
                cols := array_to_string(colnames, '\'','\'');
                sql_stat := '\''INSERT INTO '\''||relname||'\''('\''||cols||'\'') SELECT '\''||cols||'\'' FROM '\''||r.backup_name||'\'' WHERE event_id='\''||r.id;
                
            WHEN r.event_type='\''UPDATE'\'' THEN
                sql_stat := '\''UPDATE '\''||relname||'\'' SET '\'';
                i := 1;
                FOREACH cname IN ARRAY colnames
                LOOP
                   IF i>1 THEN
                   
                       IF i>2 THEN
                           sql_stat := sql_stat||'\'', '\'';
                       END IF;
                       sql_stat := sql_stat||cname||'\''='\''||r.backup_name||'\''.'\''||cname;
                   END IF;
                   
                   i := i+1;
                END LOOP;
                sql_stat := sql_stat||'\'' FROM '\''||r.backup_name||'\'' WHERE '\''||r.backup_name||'\''.event_id='\''||r.id||'\'' AND '\''
                                    ||relname||'\''.'\''||colnames[1]||'\''=(SELECT '\''||colnames[1]||'\'' FROM '\''||r.backup_name||'\'' WHERE event_id='\''||r.id||'\'')'\'';
        END CASE;
        EXECUTE sql_stat;
        
    END LOOP;
    
    RETURN true;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION backlog.undo_event(integer)
  OWNER TO postgres;
'
}

##################################################################################################
echo "1.Altering tables..."
psql -d $DATABASE -t -c "$(alter_tables)"
echo "2.Creating backlog schema..."
psql -d $DATABASE -t -c "$(create_backlog_schema)"
echo "3.Creating triggers..."
psql -d $DATABASE -t -c "$(create_auth_triggers)"
echo "4.Creating undo function..."
psql -d $DATABASE -t -c "$(create_undo_func)"
echo "Done."
