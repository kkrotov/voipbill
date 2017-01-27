#!/bin/sh

( ./app_bill -c ./app_bill99.conf -p pid99.pid ) &
( ./app_bill -c ./app_bill89.conf -p pid89.pid ) &