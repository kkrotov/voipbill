#!/bin/bash

####################################################################################################################################################
#####
#####  Удаляем тестовые базы данных
#####
####################################################################################################################################################

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

HOST_DB_TEST_MAIN=127.0.0.1
NAME_DB_TEST_MAIN=nispd

HOST_DB_TEST_REGIONAL=127.0.0.1
NAME_DB_TEST_REGIONAL=nispd99

DB_USER=pgsqltest

####################################################################################################################################################

# Здесь живёт $REGIONS_LIST
. "$DIR/regions-list"

    echo "]]] 1. Удаляем центральную БД [$NAME_DB_TEST_MAIN] на сервере [$HOST_DB_TEST_MAIN]."
    /usr/pgsql-9.4/bin/dropdb -h $HOST_DB_TEST_MAIN -U postgres --if-exists $NAME_DB_TEST_MAIN || exit 1

    echo "]]] 2. Удаляем региональные БД для регионов [`echo $REGIONS_LIST`]"
    for region_id in `echo $REGIONS_LIST`
    do
       echo "      * Удаляем nispd${region_id}_test  на сервере [$HOST_DB_TEST_REGIONAL]"
       /usr/pgsql-9.4/bin/dropdb -h $HOST_DB_TEST_REGIONAL -U postgres --if-exists nispd${region_id} || exit 1
    done

