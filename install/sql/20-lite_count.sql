CREATE OR REPLACE FUNCTION calls_cdr.lite_count(IN query text, OUT count bigint) RETURNS bigint AS $BODY$
DECLARE
	t_data	text;
BEGIN

	FOR t_data IN
		EXECUTE 'EXPLAIN ' || query
	LOOP
		SELECT t.t[1]::bigint INTO count FROM (
			SELECT regexp_matches(t[1], E'(\\d+)') as t FROM regexp_matches(t_data, E'(rows=\\d+)') AS t
		) AS t;
		RETURN;
	END LOOP;
END;
$BODY$ LANGUAGE 'plpgsql';

/*SELECT * FROM calls_cdr.lite_count('SELECT * FROM test_table');*/