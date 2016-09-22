-- DROP FUNCTION event.tr_notify();

--CREATE OR REPLACE FUNCTION event.tr_notify()
--  RETURNS trigger AS
--$BODY$
--BEGIN
--	
--	perform event.notify(cast(TG_RELNAME as varchar));
--	return null;
--
--END;
--$BODY$
--  LANGUAGE plpgsql VOLATILE
--  COST 100;
--ALTER FUNCTION event.tr_notify()
--  OWNER TO postgres;


DROP TRIGGER IF EXISTS notify on nnp.destination;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.destination
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp.account_tariff_light;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.account_tariff_light
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp.number_range;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.number_range
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp.number_range_prefix;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.number_range_prefix
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp.operator;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.operator
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp."package";
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp."package"
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp.package_minute;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.package_minute
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp.package_price;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.package_price
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp.package_pricelist;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.package_pricelist
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp.prefix;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.prefix
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp.prefix_destination;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.prefix_destination
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();

DROP TRIGGER IF EXISTS  notify on nnp.region;
CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
  ON nnp.region
  FOR EACH STATEMENT
  EXECUTE PROCEDURE event.tr_notify();
