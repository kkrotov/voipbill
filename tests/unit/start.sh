#!/bin/bash

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

echo 'Запускаеи unit-тесты'
$DIR/utest
[[ $? -ne 0 ]] && exit

