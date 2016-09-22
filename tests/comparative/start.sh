#!/bin/bash

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

echo 'New month table testing'
python "$DIR/new_month_tables.py"
[[ $? -ne 0 ]] && exit


echo 'Voip Routing testing'
python "$DIR/voip_routing.py"
[[ $? -ne 0 ]] && exit

echo 'Billing testing'
python "$DIR/billing.py"
[[ $? -ne 0 ]] && exit

