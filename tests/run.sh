#!/bin/bash

# run.sh [аргумент]
#  агумент = waitsync - скрипт ждет отсутствия команд на синхронизацию в табличке queue
# либо агумент = timeout в секундах, по умолчанию 120с
# 

if [ "$1" > 0 ]; then
  timeout="$1";
else
  timeout=120
fi


THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

# Здесь живёт $REGIONS_LIST
. "$DIR/regions-list"

COUNT=0

for region_id in `echo $REGIONS_LIST`
do
  (./app_bill -c app_bill${region_id}.conf -p /var/run/app_bill${region_id}.pid; echo "Daemon ${region_id} finished with exit code $?") &
  echo "Daemon ${region_id} runned with pid $!"


if [ "$1" = "waitsync" ]; then

  while true
  do
    echo "проверяем наличие компанд синхронизации для региона $region_id"
    res=`/usr/pgsql-9.4/bin/psql -t -U postgres nispd_test -c "select count(*) from event.queue where server_id=${region_id}"`
    if [ "$res" -eq 0 ]; then
       break            
    fi     
    sleep 5s
    echo "sleep 5sec : Команд в очереди для сервера  $region_id : $res"
  done


else
  # Каждому процессу даём поработать две минуты, иначе текущие версии демона падают в разных местах
  sleep  "$timeout"s
fi


done

