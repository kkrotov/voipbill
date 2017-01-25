#!/bin/bash

echo 'New month table testing'
python "$DIR/comparative/new_month_tables.py"


echo 'Voip Routing testing'
python "$DIR/comparative/voip_routing.py"

echo 'Billing testing'
python "$DIR/comparative/billing.py"

echo 'Auth testing'
python "$DIR/comparative/auth_comparer.py"
