#!/bin/bash

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

echo 'Запускаеи functional-тесты'
$DIR/ftest > ./TEST-functional
[[ $? -ne 0 ]] && exit
