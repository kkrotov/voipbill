ALTER TABLE billing.stats_account ADD COLUMN sum_mn_day double precision DEFAULT 0;
ALTER TABLE billing.clients ADD COLUMN voip_limit_mn_day integer DEFAULT 0;
ALTER TABLE billing.clients_locks ADD COLUMN is_mn_overran boolean DEFAULT false;

CREATE OR REPLACE RULE stats_account_rl AS
    ON INSERT TO billing.stats_account
       WHERE (EXISTS ( SELECT x.account_id
                  FROM billing.stats_account x
                            WHERE x.server_id = new.server_id 
                            AND x.account_id = new.account_id)) 
      DO INSTEAD  UPDATE billing.stats_account SET amount_month = new.amount_month, 
      sum_month = new.sum_month, amount_day = new.amount_day, sum_day = new.sum_day, sum_mn_day = new.sum_mn_day, sum = new.sum, amount_date = new.amount_date
                              WHERE stats_account.server_id = new.server_id AND stats_account.account_id = new.account_id;
                              


CREATE OR REPLACE RULE clients_rl_duplicate AS
    ON INSERT TO billing.clients
       WHERE (EXISTS ( SELECT x.id
                  FROM billing.clients x
                        WHERE x.id = new.id)) 
    DO INSTEAD  UPDATE billing.clients SET 
    
    voip_limit_month = new.voip_limit_month, voip_limit_day = new.voip_limit_day, 
    voip_limit_mn_day = new.voip_limit_mn_day, voip_disabled = new.voip_disabled, 
    balance = new.balance, credit = new.credit, 
    amount_date = COALESCE(new.amount_date, '2000-01-01 00:00:00'::timestamp without time zone), last_payed_month = new.last_payed_month, 
    organization_id = new.organization_id, price_include_vat = new.price_include_vat, 
    timezone_offset = new.timezone_offset, is_blocked = new.is_blocked, anti_fraud_disabled = new.anti_fraud_disabled, account_version = new.account_version
      WHERE clients.id = new.id;