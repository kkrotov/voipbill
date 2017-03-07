CREATE INDEX calls_raw_cdr_id ON calls_raw.calls_raw USING btree (cdr_id);

CREATE INDEX calls_raw_number_service_id ON calls_raw.calls_raw USING btree (number_service_id);