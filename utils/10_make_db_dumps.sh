#!/bin/bash

####################################################################################################################################################
#####
#####  Выгружаем схемы и данные с боевых северов.
#####
####################################################################################################################################################

HOST_DB_MAIN=eridanus.mcn.ru
NAME_DB_MAIN=nispd

HOST_DB_REGIONAL=reg99.mcntelecom.ru
NAME_DB_REGIONAL=nispd99

DB_USER=pgsqltest

####################################################################################################################################################

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`


echo "]]] Туда складываем дампы баз: [$DIR/SQL]"

# Здесь живёт $REGIONS_LIST
# . "$DIR/regions-list"

# Московская региональная база

echo "]]] 1. Дампим текущую схему московской региональной БД. сервер [$HOST_DB_REGIONAL],база [$NAME_DB_REGIONAL]"
/usr/pgsql-9.4/bin/pg_dump -c -s --disable-trigger -O -h $HOST_DB_REGIONAL -U $DB_USER $NAME_DB_REGIONAL > $DIR/SQL/$NAME_DB_REGIONAL.schema.sql

echo "]]] 2. Дампим данные ЦЕНТРАЛЬНОЙ БД без схем  calls_raw,calls_aggr,calls_cdr. сервер [$HOST_DB_MAIN],база [$NAME_DB_MAIN]"
/usr/pgsql-9.4/bin/pg_dump -a -N calls_raw -N calls_aggr -N calls_cdr --disable-triggers -O -h $HOST_DB_MAIN -U $DB_USER $NAME_DB_MAIN > $DIR/SQL/$NAME_DB_MAIN.data_wo_calls_data.sql

# Центральная база
echo "]]] 3. Дампим cхему центральной БД. сервер [$HOST_DB_MAIN],база [$NAME_DB_MAIN]"
/usr/pgsql-9.4/bin/pg_dump -c -s -O -h $HOST_DB_MAIN -U $DB_USER $NAME_DB_MAIN > $DIR/SQL/$NAME_DB_MAIN.schema.sql

# Схему с calls'ами докатываем.
echo "]]] 4. Дампим схемы calls_raw,calls_cdr и calls_aggr центральной БД без данных. сервер [$HOST_DB_MAIN],база [$NAME_DB_MAIN]"
/usr/pgsql-9.4/bin/pg_dump -s -O -n calls_raw -n calls_aggr -n calls_cdr -h $HOST_DB_MAIN -U $DB_USER $NAME_DB_MAIN > $DIR/SQL/$NAME_DB_MAIN.calls_schema.sql
