ALTER TABLE server ADD COLUMN h_local_events character varying;
UPDATE server SET h_local_events='2 10 50';
