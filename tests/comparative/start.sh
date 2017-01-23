#!/bin/bash

echo 'New month table testing'
python "$DIR/comparative/new_month_tables.py"
[[ $? -ne 0 ]] && exit


echo 'Voip Routing testing'
python "$DIR/comparative/voip_routing.py"
[[ $? -ne 0 ]] && exit

echo 'Billing testing'
python "$DIR/comparative/billing.py"
[[ $? -ne 0 ]] && exit

echo 'Auth testing'
python "$DIR/comparative/auth_comparer.py"
[[ $? -ne 0 ]] && exit
