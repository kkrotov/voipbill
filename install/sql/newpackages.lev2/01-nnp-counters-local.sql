DROP TABLE IF EXISTS  billing.stats_nnp_package_minute;

CREATE TABLE billing.stats_nnp_package_minute
(
  id integer NOT NULL,
  nnp_account_tariff_light_id integer NOT NULL,
  nnp_package_minute_id  integer NOT NULL,
  activate_from timestamp(0) without time zone NOT NULL,
  deactivate_from timestamp(0) without time zone NOT NULL,
  used_seconds integer NOT NULL,
  paid_seconds integer NOT NULL,
  used_credit double precision NOT NULL,
  min_call_id bigint NOT NULL,
  max_call_id bigint NOT NULL,
  CONSTRAINT stats_nnp_package_minute_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);

ALTER TABLE billing.stats_nnp_package_minute
  OWNER TO postgres;
GRANT ALL ON TABLE billing.stats_nnp_package_minute TO postgres;
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE billing.stats_nnp_package_minute TO g_bill_daemon_local;
GRANT SELECT ON TABLE billing.stats_nnp_package_minute TO g_readonly;
-- 
-- -- Index: billing.idx_stats_package__max_call_id
-- 


CREATE INDEX idx_stats_nnp_package_minute_max_call_id
  ON billing.stats_nnp_package_minute
  USING btree
  (max_call_id);

-- Rule: stats_package_rl ON billing.stats_package

-- DROP RULE stats_package_rl ON billing.stats_package;

CREATE OR REPLACE RULE stats_nnp_package_minute_rl AS
    ON INSERT TO billing.stats_nnp_package_minute
   WHERE (EXISTS ( SELECT x.nnp_account_tariff_light_id,x.nnp_package_minute_id
           FROM billing.stats_nnp_package_minute x
          WHERE x.id = new.id )) DO INSTEAD  
          
          UPDATE billing.stats_nnp_package_minute
          
          SET   
                nnp_account_tariff_light_id = new.nnp_account_tariff_light_id,
                nnp_package_minute_id = new.nnp_package_minute_id,
        	activate_from = new.activate_from , 
                deactivate_from = new.deactivate_from, 
                used_seconds = new.used_seconds, 
                paid_seconds = new.paid_seconds, 
                used_credit = new.used_credit, 
                min_call_id = new.min_call_id, 
                max_call_id = new.max_call_id
  WHERE stats_nnp_package_minute.id = new.id ;



