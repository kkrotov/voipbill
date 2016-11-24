#!/bin/bash

echo 'Запускаеи unit-тесты'
$DIR/unit/utest > ../TEST_unit.xml
[[ $? -ne 0 ]] && exit

