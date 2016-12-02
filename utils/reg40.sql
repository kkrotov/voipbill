ALTER TABLE server ADD COLUMN emergency_prefixlist_id integer;
COMMENT ON COLUMN server.emergency_prefixlist_id IS 'На эти номера биллер пропускает всегда, даже с блокировками.';
