#!/bin/bash

echo 'Запускаем functional-тесты'
$DIR/functional/ftest > ../TEST-functional.XML
[[ $? -ne 0 ]] && exit

