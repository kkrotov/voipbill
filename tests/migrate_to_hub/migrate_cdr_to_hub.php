<?php


$dbregion = pg_connect("host=85.94.32.172 dbname=nispd78    user=bill_daemon_local") or die('Could not connect: ' . pg_last_error());
//$dbhub    = pg_connect("host=reg12.mcntelecom.ru dbname=nispd12 user=doleynik_rw") or die('Could not connect: ' . pg_last_error());
//$dbcentral  = pg_connect("host=eridanus  dbname=nispd user=bill_daemon_remote") or die('Could not connect: ' . pg_last_error());

$dbhub    = pg_connect("host=127.0.0.1    dbname=nispd99_test user=pgadmin") or die('Could not connect: ' . pg_last_error());
$dbcentral  = pg_connect("host=127.0.0.1  dbname=nispd_test user=pgadmin") or die('Could not connect: ' . pg_last_error());

$hub_id   = 2;
$region_id= 78; 

function getNextCdrID($db) {
    $query = 'select max(call_id) from calls_cdr.cdr';
    $result = pg_query($db,$query) or die('Ошибка запроса: ' . pg_last_error());

    if($row = pg_fetch_row($result)) $maxid = $row[0]+1;
     else $maxid = 1;
    return $maxid;
}

function deleteCounter($db,$id) {
    $query = "DELETE FROM billing.stats_account WHERE server_id=$id;";
    $result = pg_query($db,$query) or die('Ошибка запроса: ' . pg_last_error());
}

function getCDR($db,$db_trg,$table_name,$next_id) {

$trunk_translate = array ( "mcn_tmn_ast58_78" => "tmn_mcn_ast48" ,
                           "smg_RTK_Loc"      => "tmn_smg_RTK_Loc" , 
                           "mcn_tmn_loop_78"  => "tmn_smg_loop" ,
                           "smg_Beeline_Loc"  => "tmn_smg_Beeline_Loc" );

   $query =  "select id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,".
             "       session_time,disconnect_cause,src_route,dst_route,src_noa,dst_noa,hash,dst_replace,in_sig_call_id,out_sig_call_id ".
             "from $table_name";
   

   $result = pg_query($db,$query) or die('Ошибка запроса: ' . pg_last_error());
   while( $row = pg_fetch_row($result)) {

    $id                = "nextval('calls_cdr.cdr_id_seq'::regclass)";
    $call_id           = $row[1];
    $nasp_ip           = "'".$row[2]."'"; 
    $src_number        = "'".$row[3]."'"; 
    $dst_number        = "'".$row[4]."'"; 
    $redirect_number   = "'".$row[5]."'"; 

    $setup_time        = "'".$row[6]."'::timestamp";
    $connect_time      = "'".$row[7]."'::timestamp";
    $disconnect_time   = "'".$row[8]."'::timestamp";
    $session_time      = $row[9];
    $disconnect_cause  = $row[10];
    $src_route         = "'".$trunk_translate[$row[11]]."'";
    $dst_route         = "'".$trunk_translate[$row[12]]."'";
    $src_noa           = "'".$row[13]."'"; 
    $dst_noa           = "'".$row[14]."'";
    $hash              = "'".$row[15]."'";
    $dst_replace       = "'".$row[16]."'";
    $in_sig_call_id    = "'".$row[17]."'::uuid";
    $out_sig_call_id   = "'".$row[18]."'::uuid";

    $sql = "insert into calls_cdr.cdr (id,call_id,nas_ip,src_number,dst_number,redirect_number,setup_time,connect_time,disconnect_time,session_time,disconnect_cause,src_route,dst_route,src_noa,dst_noa,hash,dst_replace) ";
    $sql.= "values ($id,$call_id,$nasp_ip,$src_number,$dst_number,$redirect_number,$setup_time,$connect_time,$disconnect_time,$session_time,$disconnect_cause,$src_route,$dst_route,$src_noa,$dst_noa,$hash,$dst_replace);";

    pg_query($db_trg,$sql) or die('Ошибка запроса: ' . pg_last_error());

    echo $sql."\n"; $next_id++;
   
   }


}

echo "Выполняем миграцию CDR и региона [$region_id] в хаб[$hub_id].\n";

//echo "]] В регионе next_cdr_id = ".getNextCdrID($dbregion)."\n";
//echo "]] Hа хабе   next_cdr_id = ".getNextCdrID($dbhub)."\n";

getCDR($dbregion,$dbhub,'calls_cdr.cdr_201611',getNextCdrID($dbhub));

echo "]] Удаляем счетчики из региона $region_id\n";
deleteCounter($dbcentral,$region_id);
