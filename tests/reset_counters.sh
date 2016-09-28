#!/bin/bash

# Обнуляем счетчики пакетов и минималок перед каждым запуском группы демонов на тестовый расчет.
# Иначе динамическая маршрутизация и расчет цены работают по разному из-за разных значений счетчиков


THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

# Здесь живёт $REGIONS_LIST
. "$DIR/regions-list"

echo "Очищаем счетчики в центральной базе"
/usr/pgsql-9.4/bin/psql -U postgres nispd_test -c "delete from billing.stats_account; delete from billing.billing.stats_nnp_package_minute;  delete from billing.stats_trunk_settings; delete from billing.stats_package;"


for region_id in `echo $REGIONS_LIST`
do
  echo "Очищаем счетчики в в региональной БД $region_id"
  /usr/pgsql-9.4/bin/psql -U postgres nispd${region_id}_test -c "delete from billing.stats_account; delete from billing.billing.stats_nnp_package_minute;delete from billing.stats_trunk_settings; delete from billing.stats_package;"
done
