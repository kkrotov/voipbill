ALTER TABLE nnp.account_tariff_light DROP COLUMN "number";
ALTER TABLE nnp.account_tariff_light ADD COLUMN service_number_id integer;

--TER TABLE nnp.account_tariff_light ALTER COLUMN "number" SET NOT NULL;