DROP INDEX calls_raw.calls_raw_201702_id;

DROP INDEX calls_raw.calls_raw_201703_id;

DROP INDEX calls_raw.calls_raw_201703_server_id_connect_time;

DROP INDEX calls_raw.calls_raw_201702_server_id_connect_time;

CREATE INDEX calls_raw_account_id ON calls_raw.calls_raw (account_id);

CREATE INDEX calls_raw_201702_account_id ON calls_raw.calls_raw_201702 (account_id);

CREATE INDEX calls_raw_201703_account_id ON calls_raw.calls_raw_201703 (account_id);

CREATE INDEX calls_raw_connect_time ON calls_raw.calls_raw (connect_time);