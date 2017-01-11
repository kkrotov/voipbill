CREATE OR REPLACE FUNCTION calls_cdr.tr_partitioning()
  RETURNS trigger AS
$BODY$
declare
        relname varchar;
        rel_exists text;
        suffix varchar;
        this_mon timestamp;
        next_mon timestamp;
        rec_exists boolean;
begin
        suffix := to_char(new.connect_time, 'YYYYMM');
        --raise notice '%', suffix;
        relname := 'calls_cdr.cdr_' || suffix;
        --raise notice '%', relname;
        EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;

        IF rel_exists IS NULL OR rel_exists = ''
        THEN
                EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.connect_time) || ' );' INTO this_mon;
                EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(new.connect_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;

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

        EXECUTE 'SELECT EXISTS (SELECT id FROM ' || relname || ' WHERE hash=' || quote_literal(new.hash) || ')' INTO rec_exists;
        IF NOT rec_exists
        THEN
                EXECUTE format('insert into ' || relname ||
                        '(id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,dst_route,' ||
                        'src_noa,dst_noa,hash,dst_replace,call_finished,releasing_party,in_sig_call_id, out_sig_call_id) ' ||
                        'values ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18,$19,$20,$21)')
                        USING new.id,new.call_id, new.nas_ip,new.src_number,new.dst_number,new.redirect_number,new.setup_time,new.connect_time,new.disconnect_time,new.session_time,
                                new.disconnect_cause,new.src_route,new.dst_route,new.src_noa,new.dst_noa,new.hash,new.dst_replace,new.call_finished,new.releasing_party,
                                new.in_sig_call_id, new.out_sig_call_id;
        ELSE

                --raise notice 'in_sig_call_id=%', new.in_sig_call_id;
                --raise notice 'out_sig_call_id=%', new.out_sig_call_id;
                EXECUTE format('update ' || relname ||
                        ' set nas_ip=$1,src_number=$2,dst_number=$3,redirect_number=$4,setup_time=$5,connect_time=$6,disconnect_time=$7,session_time=$8,disconnect_cause=$9,' ||
                        'src_route=$10,dst_route=$11,src_noa=$12,dst_noa=$13,dst_replace=$14,call_finished=$15,releasing_party=$16,in_sig_call_id=$17, out_sig_call_id=$18 where hash=$19')
                        USING new.nas_ip,new.src_number,new.dst_number,new.redirect_number,new.setup_time,new.connect_time,new.disconnect_time,new.session_time,
                                new.disconnect_cause,new.src_route,new.dst_route,new.src_noa,new.dst_noa,new.dst_replace,new.call_finished,new.releasing_party,
                                new.in_sig_call_id, new.out_sig_call_id, new.hash;
        END IF;

        return null;
end;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION calls_cdr.tr_partitioning()
  OWNER TO postgres;


-- Function: calls_cdr.create_calls_cdr_partition(timestamp without time zone)

-- DROP FUNCTION calls_cdr.create_calls_cdr_partition(timestamp without time zone);

CREATE OR REPLACE FUNCTION calls_cdr.create_calls_cdr_partition(connect_time timestamp without time zone)
  RETURNS boolean AS
$BODY$
declare
	relname varchar;
	rel_exists text;
	suffix varchar;
	this_mon timestamp;
	next_mon timestamp;
BEGIN
	suffix := to_char(connect_time, 'YYYYMM');
	relname := 'calls_cdr.cdr_' || suffix;
	--raise notice '%', relname;
	EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;
	
	IF rel_exists IS NULL OR rel_exists = ''
	THEN
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(connect_time) || ' );' INTO this_mon;
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(connect_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;
			
		EXECUTE 'CREATE TABLE ' || relname || 
			' (CONSTRAINT cdr_' || suffix || '_pkey PRIMARY KEY (id),' ||
			' CONSTRAINT cdr_' || suffix || '_connect_time_check CHECK (' || 
			'connect_time >= ' || quote_literal(this_mon) || '::timestamp without time zone AND ' || 
			'connect_time < ' || quote_literal(next_mon) || '::timestamp without time zone)' || 
			') INHERITS (calls_cdr.cdr) WITH (OIDS=FALSE)';

		EXECUTE format('CREATE INDEX cdr_' || suffix || '_hash ON calls_cdr.cdr_' || suffix || ' USING btree (hash);');

		EXECUTE 'ALTER TABLE ' || relname || ' OWNER TO postgres';
		EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO postgres';
		EXECUTE 'GRANT SELECT, INSERT, DELETE ON TABLE ' || relname || ' TO g_bill_daemon_local';
		EXECUTE 'GRANT SELECT, INSERT, DELETE ON TABLE ' || relname || ' TO g_bill_daemon_remote';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_readonly';
	END IF;
	return true;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION calls_cdr.create_calls_cdr_partition(timestamp without time zone)
  OWNER TO postgres;
