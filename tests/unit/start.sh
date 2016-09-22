#!/bin/bash

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

echo 'Unit testing...'
$DIR/utest
[[ $? -ne 0 ]] && exit

