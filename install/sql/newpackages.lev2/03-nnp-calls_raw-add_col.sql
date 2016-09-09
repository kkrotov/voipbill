ALTER TABLE calls_raw.calls_raw
 ADD COLUMN account_version integer NOT NULL DEFAULT 4,
 ADD COLUMN stats_nnp_package_minute_id integer
