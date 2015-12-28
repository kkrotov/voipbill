#!/bin/bash

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

# Здесь живёт $REGIONS_LIST
. "$DIR/regions-list"

echo "Удаляем центральную БД"
/usr/pgsql-9.4/bin/dropdb -U postgres --if-exists nispd_test || exit 1
echo "Создаём пустую центральную БД без схемы"
/usr/pgsql-9.4/bin/createdb -U postgres nispd_test

echo "Дампим последнюю схему московской региональной БД..."
/usr/pgsql-9.4/bin/pg_dump -s -O -h reg99.mcntelecom.ru -U pgsqltest nispd99 > nispd99.sql

for region_id in `echo $REGIONS_LIST`
do
  echo "Удаляем региональную БД $region_id"
  /usr/pgsql-9.4/bin/dropdb -U postgres --if-exists nispd${region_id}_test || exit 1
  echo "Создаём пустую региональную БД $region_id без схемы"
  /usr/pgsql-9.4/bin/createdb -U postgres nispd${region_id}_test
  echo "Импортируем схему региональной БД $region_id..."
  /usr/pgsql-9.4/bin/psql -U postgres nispd${region_id}_test < nispd99.sql
done

