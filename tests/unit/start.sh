#!/bin/bash

echo 'Запускаеи unit-тесты'
$DIR/unit/utest
[[ $? -ne 0 ]] && exit

