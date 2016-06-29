#!/bin/bash

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

# Здесь живёт $REGIONS_LIST
. "$DIR/regions-list"

# Быстрый вариант восстановления схемы и основных данных центральной БД:
echo "Дампим центральную БД без схем calls_raw, calls_aggr, nnp ..."
/usr/pgsql-9.4/bin/pg_dump -Fc -N calls_raw -N calls_aggr -N nnp -O -h eridanus.mcn.ru -U pgsqltest nispd > nispd_full.sql

echo "Удаляем тип dblink_pkey_results"
/usr/pgsql-9.4/bin/psql -U postgres nispd_test -c "drop type dblink_pkey_results"

echo "Импортируем центральную БД без схем calls_raw и calls_aggr..."
/usr/pgsql-9.4/bin/pg_restore -U postgres -d nispd_test -Fc nispd_full.sql

# Схему с calls'ами докатываем.
echo "Дампим схемы calls_raw и calls_aggr центральной БД без данных..."
/usr/pgsql-9.4/bin/pg_dump -s -O -n calls_raw -n calls_aggr -h eridanus.mcn.ru -U pgsqltest nispd > nispd_calls_schema.sql

echo "Импортируем схемы calls_raw и calls_aggr в центральную БД без данных..."
/usr/pgsql-9.4/bin/psql -U postgres nispd_test < nispd_calls_schema.sql

for region_id in `echo $REGIONS_LIST`
do
  echo "Говорим демону биллинга, чтобы забрал в региональную БД $region_id все данные из центральной БД"
  /usr/pgsql-9.4/bin/psql -U postgres nispd_test -c "SELECT event.notify_all(${region_id})"
done

