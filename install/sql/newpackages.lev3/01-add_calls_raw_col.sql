ALTER TABLE calls_raw.calls_raw
     ADD COLUMN nnp_operator_id integer,
     ADD COLUMN nnp_region_id integer,
     ADD COLUMN nnp_city_id integer,
     ADD COLUMN nnp_country_prefix integer,
     ADD COLUMN nnp_ndc integer,
     ADD COLUMN nnp_is_mob boolean,
     ADD COLUMN trunk_group_id integer
;
     