<?php


$a = array("nnp_account_tariff_light"=>array("nnp.account_tariff_light","id"),
           "nnp_destination"=>array("nnp.destination","id"),
           "nnp_number_range_prefix"=>array("nnp.number_range_prefix",'0'),
           "nnp_number_range"=>array("nnp.number_range","id"),
           "nnp_operator"=>array("nnp.operator","id"),
           "nnp_package_minute"=>array("nnp.package_minute","id"),
           "nnp_package_pricelist"=>array("nnp.package_pricelist","id"),
           "nnp_package_price"=>array("nnp.package_price","id"),
           "nnp_package"=>array("nnp.\"package\"","tariff_id"),
           "nnp_prefix_destination"=>array("nnp.prefix_destination",'0'),
           "nnp_prefix"=>array("nnp.prefix","id"),
           "nnp_region"=>array("nnp.region","id")
          );

foreach($a as $k=>$v)
{
  $v1=$k;
  $v2=$v[0];
  if($v[1]=='0') { $v31 = 0; $v32 = 0; } else { $v31="old.".$v[1]; $v32="new.".$v[1];}

//var_dump($v);

$sql="

DROP TRIGGER IF EXISTS notify ON $v2;
DROP FUNCTION IF EXISTS event.tr_notify_$v1();

CREATE OR REPLACE FUNCTION event.tr_notify_$v1()
  RETURNS trigger AS
\$BODY\$
BEGIN
	IF TG_OP = 'DELETE' THEN
		PERFORM event.notify('$v1',$v31);
    ELSE
		PERFORM event.notify('$v1',$v32);    
    END IF;
    return null;
END;
\$BODY\$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION event.tr_notify_$v1()
  OWNER TO postgres;

CREATE TRIGGER notify
  AFTER INSERT OR UPDATE OR DELETE
  ON $v2
  FOR EACH ROW
EXECUTE PROCEDURE event.tr_notify_$v1();
";

  echo $sql."\n\n\n";

}

exit;



