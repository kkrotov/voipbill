CREATE TABLE nnp.country
(
  code integer NOT NULL,
  name character varying(255),
  name_rus character varying(255),
  prefix integer NOT NULL,
  CONSTRAINT country_pkey PRIMARY KEY (code)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE nnp.country
  OWNER TO postgres;
GRANT ALL ON TABLE nnp.country TO postgres;
GRANT SELECT ON TABLE nnp.country TO g_readonly;
GRANT ALL ON TABLE nnp.country TO g_stat;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE nnp.country TO g_bill_daemon_remote;
GRANT ALL ON TABLE nnp.country TO g_bill_daemon_local;

ALTER TABLE calls_raw.calls_raw ADD COLUMN country_code integer;
