#!/bin/python
# -*- coding: utf-8 -*-

def migrate(centralDb, regionalDb, regionsList) :

  regConnections = [(regionalDb(reg), int(reg)) for reg in regionsList]

  # Мигрируем региональные БД
  for (regConn, region_id) in regConnections :
    curReg = regConn.cursor()
    curReg.execute('''
CREATE OR REPLACE FUNCTION calls_cdr.tr_partitioning()
  RETURNS trigger AS
$BODY$
declare
        relname varchar;
        rel_exists text;
        suffix varchar;
        this_mon timestamp;
        next_mon timestamp;
begin
        suffix := to_char(new.setup_time, 'YYYYMM');
        --raise notice '%', suffix;
        relname := 'calls_cdr.cdr_' || suffix;
        --raise notice '%', relname;
        EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;

        IF rel_exists IS NULL OR rel_exists = ''
        THEN
                EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.setup_time) || ' );' INTO this_mon;
                EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.setup_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;

                EXECUTE 'CREATE TABLE calls_cdr.cdr_' || suffix ||
                        ' (CONSTRAINT cdr_' || suffix || '_pkey PRIMARY KEY (id),' ||
                        ' CONSTRAINT cdr_' || suffix || '_connect_time_check CHECK (' ||
                        'connect_time >= ' || quote_literal(this_mon) || '::timestamp without time zone AND ' ||
                        'connect_time < ' || quote_literal(next_mon) || '::timestamp without time zone)' ||
                        ') INHERITS (calls_cdr.cdr) WITH (OIDS=FALSE)';

                EXECUTE format('CREATE INDEX cdr_' || suffix || '_hash ON calls_cdr.cdr_' || suffix || ' USING btree (hash);');

                EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO postgres';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_bill_daemon_local';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_readonly';
        END IF;

        EXECUTE format('insert into calls_cdr.cdr_' || to_char( new.setup_time, 'YYYYMM') ||
                '(id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,dst_route,' ||
                'src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party) ' ||
                'values ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18,$19)')
                USING new.id,new.call_id, new.nas_ip,new.src_number,new.dst_number,new.redirect_number,new.setup_time,new.connect_time,new.disconnect_time,new.session_time,
                        new.disconnect_cause,new.src_route,new.dst_route,new.src_noa,new.dst_noa,new.hash,new.dst_replace,new.call_finished,new.releasing_party;

        return null;
end;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION calls_cdr.tr_partitioning()
  OWNER TO postgres;
    ''')

    curReg.execute('''
CREATE TABLE calls_cdr.cdr_unfinished
(
  call_id bigint NOT NULL,
  setup_time timestamp without time zone,
  hash uuid,
  dst_route character varying(32),
  releasing_party character varying,
  release_timestamp timestamp without time zone,
  disconnect_cause smallint,
  CONSTRAINT cdr_unfinished_unique UNIQUE (hash, dst_route)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE calls_cdr.cdr_unfinished
  OWNER TO postgres;
GRANT ALL ON TABLE calls_cdr.cdr_unfinished TO postgres;
GRANT SELECT ON TABLE calls_cdr.cdr_unfinished TO g_bill_daemon_local;
GRANT SELECT ON TABLE calls_cdr.cdr_unfinished TO g_readonly;
    ''')

    curReg.execute('''
CREATE INDEX cdr_unfinished_hash
  ON calls_cdr.cdr_unfinished
  USING btree
  (hash);
    ''')

    curReg.execute('''
CREATE TRIGGER partitioning
  BEFORE INSERT
  ON calls_cdr.cdr_unfinished
  FOR EACH ROW
  EXECUTE PROCEDURE calls_cdr.tr_partitioning_cdr_unfinished();
    ''')

    curReg.execute('''
CREATE OR REPLACE FUNCTION calls_cdr.tr_partitioning_cdr_unfinished()
  RETURNS trigger AS
$BODY$
declare
        relname varchar;
        rel_exists text;
        suffix varchar;
        this_mon timestamp;
        next_mon timestamp;
begin
        suffix := to_char(new.setup_time, 'YYYYMM');
        --raise notice '%', suffix;

        relname := 'calls_cdr.cdr_unfinished_' || suffix;
        --raise notice '%', relname;
        EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;

        IF rel_exists IS NULL OR rel_exists = ''
        THEN
                EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.setup_time) || ' );' INTO this_mon;
                EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.setup_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;

                EXECUTE 'CREATE TABLE calls_cdr.cdr_unfinished_' || suffix ||
                        ' (CONSTRAINT cdr_unfinished_' || suffix || '_unique UNIQUE (hash, dst_route),' ||
                        ' CONSTRAINT cdr_unfinished_' || suffix || '_time_check CHECK (setup_time >= ' ||
                        quote_literal(this_mon) || '::timestamp without time zone AND setup_time < ' || quote_literal(next_mon) || '::timestamp without time zone)' ||
                        ') INHERITS (calls_cdr.cdr_unfinished) WITH (OIDS=FALSE)';

                EXECUTE format('CREATE INDEX cdr_unfinished_' || suffix || '_hash ON calls_cdr.cdr_unfinished_' || suffix || ' USING btree (hash);');

                EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO postgres';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_bill_daemon_local';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_readonly';
        END IF;

        EXECUTE format('insert into calls_cdr.cdr_unfinished_' || to_char( new.setup_time, 'YYYYMM') || ' (call_id,setup_time,hash,dst_route,releasing_party,release_timestamp,disconnect_cause) values ($1,$2,$3,$4,$5,$6,$7)')
                USING new.call_id, new.setup_time, new.hash, new.dst_route, new.releasing_party, new.release_timestamp, new.disconnect_cause;

        return null;
end;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION calls_cdr.tr_partitioning_cdr_unfinished()
  OWNER TO postgres;
    ''')

    curReg.execute('''
CREATE OR REPLACE FUNCTION calls_raw.tr_partitioning()
  RETURNS trigger AS
$BODY$
declare
	relname varchar;
	rel_exists text;
	suffix varchar;
	this_mon timestamp;
	next_mon timestamp;
begin
	suffix := to_char(new.connect_time, 'YYYYMM');
	--raise notice '%', suffix;
	relname := 'calls_raw.calls_raw_' || suffix;
	--raise notice '%', relname;
	EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;
	
	IF rel_exists IS NULL OR rel_exists = ''
	THEN
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.connect_time) || ' );' INTO this_mon;
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.connect_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;
			
		EXECUTE 'CREATE TABLE calls_raw.calls_raw_' || suffix || 
			' (CONSTRAINT calls_raw_' || suffix || '_pkey PRIMARY KEY (id),' ||
			' CONSTRAINT calls_raw_' || suffix || '_connect_time_check CHECK (' || 
			'connect_time >= ' || quote_literal(this_mon) || '::timestamp without time zone AND ' || 
			'connect_time < ' || quote_literal(next_mon) || '::timestamp without time zone)' || 
			') INHERITS (calls_raw.calls_raw) WITH (OIDS=FALSE)';

		EXECUTE format('CREATE INDEX calls_raw_' || suffix || '_cdr_id ON calls_raw.calls_raw_' || suffix || ' USING btree (cdr_id)');
		EXECUTE 'CREATE INDEX calls_raw_' || suffix || '_connect_time ON calls_raw.calls_raw_' || suffix || ' USING btree (connect_time)';

		EXECUTE 'ALTER TABLE ' || relname || ' OWNER TO postgres';
		EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO postgres';
		EXECUTE 'GRANT SELECT, INSERT, DELETE ON TABLE ' || relname || ' TO g_bill_daemon_local';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_readonly';
	END IF;

        EXECUTE format('INSERT INTO ' || relname || ' SELECT ($1).*') USING NEW;
	return null;        
end;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION calls_raw.tr_partitioning()
  OWNER TO postgres;
    ''')

    curReg.execute('''
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
        begin

            p_hash = calls_cdr.get_hash(p_call_id, setup_time::timestamp);
   
            insert into calls_cdr.cdr_unfinished (
                call_id, setup_time, hash, dst_route, releasing_party, release_timestamp, disconnect_cause
            ) values (
                p_call_id, setup_time::timestamp, p_hash, dst_route, releasing_party, release_timestamp::timestamp, disconnect_cause
            );

            delete from calls_cdr.start where call_id = p_call_id;
    
        end;
        $BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION insert_cdr_unfinished(bigint, character varying, character varying, character varying, character varying, smallint)
  OWNER TO postgres;
    ''')

    regConn.commit()
    regConn.close()

