#!/bin/bash

echo 'Запускаем functional-тесты'
$DIR/functional/ftest
[[ $? -ne 0 ]] && exit
