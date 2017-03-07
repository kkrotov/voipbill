CREATE INDEX calls_raw_id ON calls_raw.calls_raw (id);

CREATE INDEX calls_raw_201702_id ON calls_raw.calls_raw_201702 (id);

CREATE INDEX calls_raw_201703_id ON calls_raw.calls_raw_201703 (id);

CREATE INDEX calls_raw_server_id_connect_time ON calls_raw.calls_raw (server_id, connect_time);

CREATE INDEX calls_raw_201702_server_id_connect_time ON calls_raw.calls_raw_201702 (server_id, connect_time);

CREATE INDEX calls_raw_201703_server_id_connect_time ON calls_raw.calls_raw_201703 (server_id, connect_time);

DROP INDEX calls_raw.calls_raw_account_id;

DROP INDEX calls_raw.calls_raw_201702_account_id;

DROP INDEX calls_raw.calls_raw_201703_account_id;

DROP INDEX calls_raw.calls_raw_connect_time;