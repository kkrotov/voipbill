DROP TRIGGER IF EXISTS notify ON nnp.account_tariff_light;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_account_tariff_light();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_account_tariff_light()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_account_tariff_light',old.id);
    ELSE
		PERFORM event.notify('nnp_account_tariff_light',new.id);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_account_tariff_light()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.account_tariff_light
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_account_tariff_light();





DROP TRIGGER IF EXISTS notify ON nnp.destination;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_destination();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_destination()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_destination',old.id);
    ELSE
		PERFORM event.notify('nnp_destination',new.id);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_destination()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.destination
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_destination();





DROP TRIGGER IF EXISTS notify ON nnp.number_range_prefix;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_number_range_prefix();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_number_range_prefix()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_number_range_prefix',0);
    ELSE
		PERFORM event.notify('nnp_number_range_prefix',0);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_number_range_prefix()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.number_range_prefix
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_number_range_prefix();





DROP TRIGGER IF EXISTS notify ON nnp.number_range;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_number_range();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_number_range()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_number_range',old.id);
    ELSE
		PERFORM event.notify('nnp_number_range',new.id);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_number_range()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.number_range
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_number_range();





DROP TRIGGER IF EXISTS notify ON nnp.operator;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_operator();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_operator()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_operator',old.id);
    ELSE
		PERFORM event.notify('nnp_operator',new.id);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_operator()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.operator
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_operator();





DROP TRIGGER IF EXISTS notify ON nnp.package_minute;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_package_minute();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_package_minute()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_package_minute',old.id);
    ELSE
		PERFORM event.notify('nnp_package_minute',new.id);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_package_minute()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.package_minute
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_package_minute();





DROP TRIGGER IF EXISTS notify ON nnp.package_pricelist;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_package_pricelist();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_package_pricelist()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_package_pricelist',old.id);
    ELSE
		PERFORM event.notify('nnp_package_pricelist',new.id);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_package_pricelist()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.package_pricelist
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_package_pricelist();





DROP TRIGGER IF EXISTS notify ON nnp.package_price;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_package_price();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_package_price()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_package_price',old.id);
    ELSE
		PERFORM event.notify('nnp_package_price',new.id);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_package_price()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.package_price
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_package_price();





DROP TRIGGER IF EXISTS notify ON nnp."package";
DROP FUNCTION IF EXISTS event.tr_notify_nnp_package();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_package()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_package',old.tariff_id);
    ELSE
		PERFORM event.notify('nnp_package',new.tariff_id);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_package()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp."package"
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_package();





DROP TRIGGER IF EXISTS notify ON nnp.prefix_destination;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_prefix_destination();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_prefix_destination()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_prefix_destination',0);
    ELSE
		PERFORM event.notify('nnp_prefix_destination',0);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_prefix_destination()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.prefix_destination
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_prefix_destination();





DROP TRIGGER IF EXISTS notify ON nnp.prefix;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_prefix();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_prefix()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_prefix',old.id);
    ELSE
		PERFORM event.notify('nnp_prefix',new.id);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_prefix()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.prefix
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_prefix();





DROP TRIGGER IF EXISTS notify ON nnp.region;
DROP FUNCTION IF EXISTS event.tr_notify_nnp_region();

CREATE OR REPLACE FUNCTION event.tr_notify_nnp_region()
  RETURNS trigger AS
$BODY$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('nnp_region',old.id);
    ELSE
		PERFORM event.notify('nnp_region',new.id);    
    END IF;
    return null;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_nnp_region()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON nnp.region
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_nnp_region();



