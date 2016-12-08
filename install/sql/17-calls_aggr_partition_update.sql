CREATE OR REPLACE FUNCTION calls_aggr.create_calls_aggr_partition(aggr_time timestamp without time zone)
  RETURNS boolean AS
$BODY$
declare
	relname varchar;
	rel_exists text;
	suffix varchar;
	this_mon timestamp;
	next_mon timestamp;
BEGIN
	suffix := to_char(aggr_time, 'YYYYMM');
	relname := 'calls_aggr.calls_aggr_' || suffix;
	--raise notice '%', relname;
	EXECUTE 'SELECT to_regclass('|| quote_literal(relname) ||');' INTO rel_exists;

	IF rel_exists IS NULL OR rel_exists = ''
	THEN
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(aggr_time) || ' );' INTO this_mon;
		EXECUTE 'select date_trunc(''month'', TIMESTAMP ' || quote_literal(aggr_time) || ' + INTERVAL ''1 MON'');' INTO next_mon;

		EXECUTE 'CREATE TABLE ' || relname ||
			' (CONSTRAINT calls_aggr_' || suffix || '_idx UNIQUE (aggr_time, orig, trunk_id, account_id, trunk_service_id, number_service_id, destination_id, mob), ' ||
			'  CONSTRAINT calls_aggr_' || suffix || '_time_check CHECK (aggr_time >= '|| quote_literal(this_mon) || '::timestamp without time zone AND ' ||
			'  aggr_time < ' || quote_literal(next_mon) || '::timestamp without time zone)'
			') INHERITS (calls_aggr.calls_aggr) WITH (OIDS=FALSE)';

		EXECUTE 'ALTER TABLE ' || relname || ' OWNER TO postgres';
		EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO postgres';
		--EXECUTE 'GRANT SELECT, INSERT, DELETE ON TABLE ' || relname || ' TO g_bill_daemon_local';
		--EXECUTE 'GRANT ALL ON TABLE ' || relname || ' TO g_bill_daemon_remote';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_readonly';
		EXECUTE 'GRANT SELECT ON TABLE ' || relname || ' TO g_stat';

		EXECUTE 'CREATE INDEX calls_aggr_'|| suffix ||'_number_service_id ON ' || relname || ' USING btree (number_service_id)';

	END IF;
	return true;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION calls_aggr.create_calls_aggr_partition(timestamp without time zone)
  OWNER TO postgres;


CREATE OR REPLACE FUNCTION calls_aggr.calls_aggr_update(server_ids character varying)
  RETURNS integer AS
$BODY$
DECLARE
     sql varchar;
     date varchar;
     count integer;
BEGIN
     EXECUTE 'SELECT calls_aggr.create_calls_aggr_partition(now() at time zone ''utc'');';

	server_ids := btrim(server_ids, '()');

	date := to_char(now(), 'YYYYMM');

	sql := '

delete

			from calls_aggr.calls_aggr_' || date || '

where
			aggr_time >= date_trunc(''hour'', now() at time zone ''utc'') - INTERVAL ''2 hour''

				and

			server_id IN (' || server_ids || ');

insert into calls_aggr.calls_aggr_' || date || ' (

       server_id, aggr_time, orig, trunk_id, account_id,

       trunk_service_id, number_service_id, destination_id, mob,

       last_call_id, billed_time, cost, tax_cost, interconnect_cost,

       total_calls, notzero_calls

)
select

       server_id, date_trunc(''hour'', connect_time) as aggr_time, orig, trunk_id, account_id,

       trunk_service_id, number_service_id, destination_id, mob,

       max(id) as last_call_id, sum(billed_time) as billed_time,

       sum(cost) as cost, sum(tax_cost) as tax_cost,

       sum(interconnect_cost) as interconnect_cost, sum(1) as total_calls,

       sum(case abs(cost) >= 0.0001 when true then 1 else 0 end) as notzero_calls

   from calls_raw.calls_raw_' || date || '

	 where

			 connect_time < date_trunc(''hour'', now() at time zone ''utc'')

					and

			 connect_time >= (SELECT COALESCE(max(aggr_time), to_timestamp(0)) from calls_aggr.calls_aggr_' || date || ')::TIMESTAMP + INTERVAL ''1 hour''

					and

			 server_id IN (' || server_ids || ')

   group by

       server_id, aggr_time, orig, trunk_id, account_id,

       trunk_service_id, number_service_id, destination_id, mob;

	 insert into calls_aggr.calls_aggr_' || date || ' (server_id, aggr_time)

	 select

				s.s::int AS server_id,

				gs.gs AS aggr_time

			from

			 (select * from unnest(string_to_array(' || server_ids || '::varchar, '','')) AS s limit 1) AS s,

				generate_series (
					date_trunc(''hour'', now() at time zone ''utc'') - INTERVAL ''2 hour'',
					date_trunc(''hour'', now() at time zone ''utc'') - INTERVAL ''1 hour'',
					''1 hour''::interval) AS gs;';
	EXECUTE sql;
	GET DIAGNOSTICS count = ROW_COUNT;
	RETURN count;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION calls_aggr.calls_aggr_update(character varying)
OWNER TO postgres;