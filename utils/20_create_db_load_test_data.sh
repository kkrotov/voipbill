#!/bin/bash

####################################################################################################################################################
#####
#####  Загрузка реальных данных в тестовый серевер БД
#####
####################################################################################################################################################

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

HOST_DB_TEST_MAIN=127.0.0.1
NAME_DB_TEST_MAIN=nispd_test

HOST_DB_TEST_REGIONAL=127.0.0.1
NAME_DB_TEST_REGIONAL=nispd99_test

DB_USER=pgsqltest

DB_DUMP_SCHEMA_MAIN=$DIR/SQL/nispd.schema.sql
DB_DUMP_DATA_MAIN_WO_CALLS=$DIR/SQL/nispd.data_wo_calls_data.sql

DB_DUMP_SCHEMA_MAIN_CALLS=$DIR/SQL/nispd.calls_schema.sql  
DB_DUMP_REGIONAL_SCHEMA=$DIR/SQL/nispd99.schema.sql  

####################################################################################################################################################


main_db_test_create_and_load() {

    echo "### Работаем с БД [$NAME_DB_TEST_MAIN] на сервере [$HOST_DB_TEST_MAIN]."
    echo "]]] 1. Удаляем центральную БД"
    /usr/pgsql-9.4/bin/dropdb -h $HOST_DB_TEST_MAIN -U postgres --if-exists $NAME_DB_TEST_MAIN || exit 1

    echo "]]] 2. Создаём пустую центральную БД"
    /usr/pgsql-9.4/bin/createdb -h $HOST_DB_TEST_MAIN -U postgres $NAME_DB_TEST_MAIN || exit 1

    echo "]]] 3. Вручную удаляем тип  dblink_pkey_results"
    /usr/pgsql-9.4/bin/psql -U postgres -h $HOST_DB_TEST_MAIN  $NAME_DB_TEST_MAIN -c "drop type dblink_pkey_results"

    echo "]]] 4. Создаем схему в центральной базе из [$DB_DUMP_SCHEMA_MAIN]"
    /usr/pgsql-9.4/bin/psql -h $HOST_DB_TEST_MAIN -U postgres $NAME_DB_TEST_MAIN < $DB_DUMP_SCHEMA_MAIN || exit 1

    echo "]]] 5. Загружаем тестовые данные в базу из [$DB_DUMP_DATA_MAIN_WO_CALLS]"
    /usr/pgsql-9.4/bin/psql -U postgres -h $HOST_DB_TEST_MAIN -d $NAME_DB_TEST_MAIN < $DB_DUMP_DATA_MAIN_WO_CALLS 

    echo "]]] 6. Загружаем схему calls из [$DB_DUMP_SCHEMA_MAIN_CALLS]"
    /usr/pgsql-9.4/bin/psql -U postgres -h $HOST_DB_TEST_MAIN  $NAME_DB_TEST_MAIN < $DB_DUMP_SCHEMA_MAIN_CALLS


}

regional_db_test_create_and_load() {
    echo "### Работаем с БД [$HOST_DB_TEST_REGIONAL] на сервере [$NAME_DB_TEST_REGIONAL]."

    echo "]]] 1. Удаляем региональную БД"
    /usr/pgsql-9.4/bin/dropdb -h $HOST_DB_TEST_REGIONAL -U postgres --if-exists $NAME_DB_TEST_REGIONAL || exit 1

    echo "]]] 2. Создаём пустую центральную БД"
    /usr/pgsql-9.4/bin/createdb -h  $HOST_DB_TEST_REGIONAL -U postgres  $NAME_DB_TEST_REGIONAL || exit 1

    echo "]]] 3. Вручную удаляем тип  dblink_pkey_results"
    /usr/pgsql-9.4/bin/psql -U postgres -h $HOST_DB_TEST_REGIONAL  $NAME_DB_TEST_REGIONAL -c "drop type dblink_pkey_results"

    echo "]]] 4. Загружаем схему из [$DB_DUMP_REGIONAL_SCHEMA]"
    /usr/pgsql-9.4/bin/psql -U postgres -h $HOST_DB_TEST_REGIONAL  $NAME_DB_TEST_REGIONAL < $DB_DUMP_REGIONAL_SCHEMA

#    echo "]]] 5. Загружаем схему calls из [$DB_DUMP_SCHEMA_MAIN_CALLS]"
#    /usr/pgsql-9.4/bin/psql -U postgres -h $HOST_DB_TEST_REGIONAL  $NAME_DB_TEST_REGIONAL < $DB_DUMP_SCHEMA_MAIN_CALLS

}

main_db_test_create_and_load

regional_db_test_create_and_load
