CREATE OR REPLACE FUNCTION calls_raw.delete_old_partitions (year varchar) RETURNS void AS $$
DECLARE
    tables record;
BEGIN
    FOR tables IN EXECUTE 'SELECT table_name AS name FROM information_schema.tables WHERE table_schema=''calls_raw'' AND table_name LIKE ''calls_raw_' || year || '%%'''
    LOOP
        EXECUTE 'DROP TABLE calls_raw.' || tables.name;
    END LOOP;
END;
$$
LANGUAGE 'plpgsql';