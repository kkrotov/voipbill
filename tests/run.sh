#!/bin/bash

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

# Здесь живёт $REGIONS_LIST
. "$DIR/regions-list"

COUNT=0

for region_id in `echo $REGIONS_LIST`
do
  (./app_bill -c app_bill${region_id}.conf -p /var/run/app_bill${region_id}.pid; echo "Daemon ${region_id} finished with exit code $?") &
  echo "Daemon ${region_id} runned with pid $!"

  # Каждому процессу даём поработать две минуты, иначе текущие версии демона падают в разных местах
  sleep 250s

done

