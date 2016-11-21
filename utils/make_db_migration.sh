#!/bin/sh

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

# Здесь живёт $REGIONS_LIST
. "$DIR/regions-list"

for region_id in `echo $REGIONS_LIST`
do

HOST="reg${region_id}.mcntelecom.ru"
DBNAME="nispd${region_id}"
USER=doleynik_rw

    echo "Делаем правку : HOST $HOST : USER $USER : DBNAME $DBNAME"
    /usr/pgsql-9.4/bin/psql -h $HOST -U $USER $DBNAME < reg40.sql

done

HOST="eridanus.mcn.ru"
DBNAME="nispd"
USER=pgsqltest

#echo "Делаем правку : HOST $HOST : USER $USER : DBNAME $DBNAME : SQL: $SQL"
#/usr/pgsql-9.4/bin/psql -h $HOST -U $USER $DBNAME -c "$SQL" || exit 1

