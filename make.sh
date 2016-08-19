#!/bin/sh

scl enable devtoolset-3 bash << \EOF
cmake .
make -j4

EOF
