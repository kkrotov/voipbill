<?php

$dbmain   = pg_connect("host=127.0.0.1 dbname=nispd_test user=pgadmin password=NbhLhjn") or die('Could not connect: ' . pg_last_error());
$dbregion = pg_connect("host=127.0.0.1 dbname=nispd99_test user=pgadmin password=NbhLhjn") or die('Could not connect: ' . pg_last_error());

function clearCounters($db)
{
    $query = 'delete from billing.stats_account';
    $result = pg_query($db,$query) or die('Ошибка запроса: ' . pg_last_error());

    $query = 'delete from billing.stats_nnp_package_minute';
    $result = pg_query($db,$query) or die('Ошибка запроса: ' . pg_last_error());
}

function clearCallRaws($db)
{
    $query = 'delete from calls_raw.calls_raw';
    $result = pg_query($db,$query) or die('Ошибка запроса: ' . pg_last_error());
}

function clearCallCDR($db)
{
    $query = 'delete from calls_cdr.cdr';
    $result = pg_query($db,$query) or die('Ошибка запроса: ' . pg_last_error());
}


function getLastCallID($db) {
    $query = 'select max(call_id) from calls_cdr.cdr';
    $result = pg_query($db,$query) or die('Ошибка запроса: ' . pg_last_error());

    if($row = pg_fetch_row($result)) $maxid = $row[0]+1;
     else $maxid = 1;
    return $maxid;
}

function DoRecalcCurrentMounth($db) {
    echo "]]] Формируем задание на пересчет за месяц.\n";
    $query = "insert into billing.tasks (region_id,task,params) values(99,'recalc_current_month','');";
    $result = pg_query($db,$query) or die('Ошибка запроса: ' . pg_last_error());
}

function doCall($db,$v,&$maxid) {

    $id                = $maxid;
    $call_id           = $maxid;
    $nasp_ip           = "'85.94.32.233'";
    $src_number        = "'".$v['src_number']."'";
    $dst_number        = "'".$v['dst_number']."'";
    $redirect_number   = "''";

    $setup_time        ="'". $v["dt"]."'::timestamp";
    $connect_time      ="'". $v["dt"]."'::timestamp";
    $disconnect_time   = "'".$v["dt"]."'::timestamp";
    $session_time      = 60; 
    $disconnect_cause  = 16;
    $src_route         = "'".$v["src_route"]."'";
    $dst_route         = "'".$v["dst_route"]."'";
    $src_noa           = 3;
    $dst_noa           = 3;
    $hash              = "calls_cdr.get_hash(".$id.",'".$v["dt"]."'::timestamp)";
    $dst_replace       = "''";


    $sql = "insert into calls_cdr.cdr (id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,dst_route,src_noa,dst_noa,hash,dst_replace) ";
    $sql.= "values ($id,$call_id,$nasp_ip,$src_number,$dst_number,$redirect_number,$setup_time,$connect_time,$disconnect_time,$session_time,$disconnect_cause,$src_route,$dst_route,$src_noa,$dst_noa,$hash,$dst_replace);";

    $result = pg_query($db,$sql) or die('Ошибка запроса: ' . pg_last_error());

    echo "SQL:".$sql."\n";

    $maxid++;
}

function doCalls($db,$calls, &$id) {

    foreach($calls as $k=>$v)
      {
          doCall($db,$v,$id);
      }
}

function doPrepare_otfix_mn_counter($dbmain,$dbregion)
{

echo "Заполняем базу тестовыми звонками\n";

echo "]] 1.Удаляем счетчики billing.stats_account на региональной и центральной базе.\n";

clearCounters($dbmain);
clearCounters($dbregion);

echo "]] 2.Удаляем обсчитанные звонки calls_raw.calls_raw  на региональной и центральной базе.\n";

clearCallRaws($dbmain);
clearCallRaws($dbregion);

echo "]] 3. Очищаем записи в таблице CDR в региональной базе.\n";

clearCallCDR($dbregion);
clearCallCDR($dbmain);

}

doPrepare_otfix_mn_counter($dbmain,$dbregion);

echo "]] 4. Создаем записи в таблице CDR в региональной базе.\n";

$maxid = getLastCallID($dbregion);

$calls[] = array ( "src_number" => '74996851549' , "dst_number" => '79164631212' , "src_route"=>"mcn_msk_ast16_99" , "dst_route"=>"", "dt"=>"2016-11-08 18:03:00");

$calls[] = array ( "src_number" => '79119888723' , "dst_number" => '73832870122' , "src_route"=>"smg_MTS_Nat" , "dst_route"=>"smg_MTS_Nat", "dt"=>"2016-11-09 18:01:00");
$calls[] = array ( "src_number" => '79119888723' , "dst_number" => '79263747216' , "src_route"=>"smg_MTS_Nat" , "dst_route"=>"smg_MTS_Nat", "dt"=>"2016-11-09 18:02:00");
$calls[] = array ( "src_number" => '78126465198' , "dst_number" => '79262559281' , "src_route"=>"mcn_msk_ast16_99" , "dst_route"=>"smg_MTS_Nat", "dt"=>"2016-11-09 18:03:00");

doCalls($dbregion,$calls,$maxid);

//DoRecalcCurrentMounth($dbmain);




