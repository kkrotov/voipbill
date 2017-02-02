ALTER TABLE server ADD COLUMN h_event_management character varying;
UPDATE server SET h_event_management='2 10 50';
