-- Function: backlog.undo_event(integer)

-- DROP FUNCTION backlog.undo_event(integer);

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
        relname:=r.schema_name||'.'||r.table_name;
        --raise notice '%', relname;
        --raise notice '%', r.event_type;
        colnames:=ARRAY(SELECT quote_ident(attname) FROM pg_catalog.pg_attribute
                                       WHERE  attrelid = relname::regclass -- regclass used as OID
                                       AND    attnum > 0               -- exclude system columns
                                       AND    attisdropped = FALSE     -- exclude dropped columns
                                 );
        
        CASE
            WHEN r.event_type='INSERT' THEN
                sql_stat := 'DELETE FROM '||relname||' WHERE '||colnames[1]||
                            '=(SELECT '||colnames[1]||' FROM '||r.backup_name||' WHERE event_id='||r.id||')';
                            
            WHEN r.event_type='DELETE' THEN
                cols := array_to_string(colnames, ',');
                --raise notice '%', cols;
                sql_stat := 'INSERT INTO '||relname||'('||cols||') SELECT '||cols||' FROM '||r.backup_name||' WHERE event_id='||r.id;
                
            WHEN r.event_type='UPDATE' THEN
                sql_stat := 'UPDATE '||relname||' SET ';
                i := 1;
                FOREACH cname IN ARRAY colnames
                LOOP
                   IF i>1 THEN
                   
                       IF i>2 THEN
                           sql_stat := sql_stat||', ';
                       END IF;
                       sql_stat := sql_stat||cname||'='||r.backup_name||'.'||cname;
                   END IF;
                   
                   i := i+1;
                END LOOP;
                sql_stat := sql_stat||' FROM '||r.backup_name||' WHERE '||r.backup_name||'.event_id='||r.id||' AND '
                                    ||relname||'.'||colnames[1]||'=(SELECT '||colnames[1]||' FROM '||r.backup_name||' WHERE event_id='||r.id||')';
        END CASE;
        --raise notice '%', sql_stat;
        EXECUTE sql_stat;
        
    END LOOP;
    
    RETURN true;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION backlog.undo_event(integer)
  OWNER TO postgres;
