ALTER TABLE nnp.country ADD COLUMN prefixes integer[];
COMMENT ON COLUMN nnp.country.prefixes IS 'Начальные цифры телефонных номеров (префикс страны и начало NDC)';
