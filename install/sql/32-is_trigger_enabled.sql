CREATE OR REPLACE FUNCTION nnp.is_trigger_enabled(table_name text,trigger_name text)
  RETURNS boolean AS
$BODY$
DECLARE
	is_enabled boolean;
BEGIN
	EXECUTE 'select case when trg.tgenabled=''O'' then true else false end as status '
		'FROM pg_trigger trg '
			'JOIN pg_class tbl on trg.tgrelid = tbl.oid '
			'JOIN pg_namespace ns ON ns.oid = tbl.relnamespace '
		'WHERE trg.tgname = '|| quote_literal(trigger_name) ||
		' and ns.nspname ||''.''||tbl.relname=' || quote_literal(table_name) INTO is_enabled;

	return is_enabled;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE SECURITY DEFINER
  COST 100;
ALTER FUNCTION nnp.is_trigger_enabled(text, text)
  OWNER TO postgres;
GRANT EXECUTE ON FUNCTION nnp.is_trigger_enabled(text, text) TO public;
GRANT EXECUTE ON FUNCTION nnp.is_trigger_enabled(text, text) TO postgres;
