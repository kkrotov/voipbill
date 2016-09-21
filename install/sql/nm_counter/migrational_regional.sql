ALTER TABLE billing.clients ADD COLUMN voip_limit_mn_day integer DEFAULT 0;
ALTER TABLE billing.stats_account ADD COLUMN sum_mn_day double precision DEFAULT 0;

CREATE OR REPLACE RULE stats_account_rl AS
    ON INSERT TO billing.stats_account
       WHERE (EXISTS ( SELECT x.account_id
                  FROM billing.stats_account x
                            WHERE x.account_id = new.account_id)) 
                            DO INSTEAD  UPDATE billing.stats_account SET amount_month = new.amount_month, sum_month = new.sum_month, 
                            amount_day = new.amount_day, sum_day = new.sum_day, sum_mn_day = new.sum_mn_day, sum = new.sum, amount_date = new.amount_date
                              WHERE stats_account.account_id = new.account_id;
                              
