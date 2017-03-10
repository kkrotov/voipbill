CREATE UNIQUE INDEX temp_idx ON calls_raw.calls_raw (server_id, id);

ALTER TABLE calls_raw.calls_raw DROP CONSTRAINT calls_raw_pkey,
    ADD CONSTRAINT calls_raw_pkey PRIMARY KEY USING INDEX temp_idx;

CREATE UNIQUE INDEX temp_idx ON calls_raw.calls_raw_201702 (server_id, id);

ALTER TABLE calls_raw.calls_raw_201702 DROP CONSTRAINT calls_raw_201702_pkey,
    ADD CONSTRAINT calls_raw_201702_pkey PRIMARY KEY USING INDEX temp_idx;

CREATE UNIQUE INDEX temp_idx ON calls_raw.calls_raw_201703 (server_id, id);

ALTER TABLE calls_raw.calls_raw_201703 DROP CONSTRAINT calls_raw_201703_pkey,
    ADD CONSTRAINT calls_raw_201703_pkey PRIMARY KEY USING INDEX temp_idx;

