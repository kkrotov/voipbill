ALTER TABLE nnp.account_tariff_light 
ADD COLUMN tariffication_by_minutes boolean DEFAULT false,
ADD COLUMN tariffication_full_first_minute boolean DEFAULT true,
ADD COLUMN tariffication_free_first_seconds boolean  DEFAULT true;

ALTER TABLE calls_raw.calls_raw 
ADD COLUMN nnp_package_minute_id integer,
ADD COLUMN nnp_package_price_id integer,
ADD COLUMN nnp_package_pricelist_id integer;
