#!/bin/bash

####################################################################################################################################################
#####
#####  Организуем перенос справочников биллинга из центральной в регианальную базу с помощью тестового запуска демона
#####
####################################################################################################################################################

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

HOST_DB_TEST_MAIN=127.0.0.1
NAME_DB_TEST_MAIN=nispd_test

HOST_DB_TEST_REGIONAL=127.0.0.1
NAME_DB_TEST_REGIONAL=nispd99_test

DB_USER=pgsqltest

region_id=10

pid_file=/var/run/app_bill.pid


####################################################################################################################################################


   echo "Говорим демону биллинга, чтобы забрал в региональную БД $region_id все данные из центральной БД"
  /usr/pgsql-9.4/bin/psql -U postgres -h $HOST_DB_TEST_MAIN $NAME_DB_TEST_MAIN -c "delete from event.queue;SELECT event.notify_all(${region_id});"

exit

  ($DIR/../app_bill -c $DIR/../build/app_bill.conf -p ${pid_file} ; echo "Daemon ${region_id} finished with exit code $?") &

  echo "Daemon ${region_id} runned with pid $!"

  while true
  do
    echo "проверяем наличие компанд синхронизации для региона $region_id"
    res=`/usr/pgsql-9.4/bin/psql -h $HOST_DB_TEST_MAIN -t -U postgres $NAME_DB_TEST_MAIN -c "select count(*) from event.queue where server_id=${region_id}"`
    if [ ! -f /var/run/app_bill.pid ]
    then
        echo "Демона в памяти нет - выходим."
        break
    fi

    if [ "$res" -eq 0 ]; then
       break
    fi
    sleep 5s
    echo "sleep 5sec : Команд в очереди для сервера  $region_id : $res"
  done

  kill `cat ${pid_file}`
  