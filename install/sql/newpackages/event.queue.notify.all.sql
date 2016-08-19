-- Function: event.notify_all(integer)

-- DROP FUNCTION event.notify_all(integer);

CREATE OR REPLACE FUNCTION event.notify_all(p_server_id integer)
  RETURNS boolean AS
$BODY$
BEGIN

	delete from event.queue where server_id=p_server_id;

	perform event.notify('airp', 0, p_server_id);
	perform event.notify('client', 0, p_server_id);
	perform event.notify('organization', 0, p_server_id);
	perform event.notify('geo', 0, p_server_id);
	perform event.notify('instance_settings', 0, p_server_id);
	perform event.notify('number', 0, p_server_id);
	perform event.notify('operator', 0, p_server_id);
	perform event.notify('outcome', 0, p_server_id);
	perform event.notify('number', 0, p_server_id);
	perform event.notify('prefixlist', 0, p_server_id);
	perform event.notify('pricelist', 0, p_server_id);
	perform event.notify('release_reason', 0, p_server_id);
	perform event.notify('route_case', 0, p_server_id);
	perform event.notify('route_table', 0, p_server_id);
	perform event.notify('route_table_route', 0, p_server_id);
	perform event.notify('routing_report_data', 0, p_server_id);
	perform event.notify('server', 0, p_server_id);
	perform event.notify('tariff', 0, p_server_id);
	perform event.notify('tariff_change_log', 0, p_server_id);
	perform event.notify('trunk', 0, p_server_id);
	perform event.notify('trunk_number_preprocessing', 0, p_server_id);
	perform event.notify('trunk_priority', 0, p_server_id);
	perform event.notify('trunk_rule', 0, p_server_id);
	perform event.notify('service_number', 0, p_server_id);
	perform event.notify('service_trunk', 0, p_server_id);
	perform event.notify('service_trunk_settings', 0, p_server_id);    
	perform event.notify('service_number_package', 0, p_server_id); 
	perform event.notify('stat_destination_prefixlists', 0, p_server_id); 
	perform event.notify('stat_prefixlist', 0, p_server_id); 
	perform event.notify('tariff_package', 0, p_server_id); 
        perform event.notify('geo_prefix', 0, p_server_id);
	perform event.notify('network_prefix', 0, p_server_id);
	perform event.notify('mob_prefix', 0, p_server_id);
	perform event.notify('prefixlist_prefix', 0, p_server_id);
        perform event.notify('defs', 0, p_server_id);
	perform event.notify('nnp_account_tariff_light', 0, p_server_id);
	perform event.notify('nnp_destination', 0, p_server_id);
	perform event.notify('nnp_number_range_prefix', 0, p_server_id);
	perform event.notify('nnp_number_range', 0, p_server_id);
	perform event.notify('nnp_operator', 0, p_server_id);
	perform event.notify('nnp_package_minute', 0, p_server_id);
	perform event.notify('nnp_package_pricelist', 0, p_server_id);
	perform event.notify('nnp_package_price', 0, p_server_id);
	perform event.notify('nnp_package', 0, p_server_id);
	perform event.notify('nnp_prefix_destination', 0, p_server_id);
	perform event.notify('nnp_prefix', 0, p_server_id);
	perform event.notify('nnp_region', 0, p_server_id);
      
	return true;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.notify_all(integer)
  OWNER TO eivanov;

