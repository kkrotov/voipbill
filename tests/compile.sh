#!/bin/bash

echo 'Compiling app_bill...'

scl enable devtoolset-3 bash << \EOF

mkdir -p libs/boost

pushd libs/boost
svn checkout http://svn.boost.org/svn/boost/tags/release/Boost_1_55_0/libs/system
svn checkout http://svn.boost.org/svn/boost/tags/release/Boost_1_55_0/libs/program_options
svn checkout http://svn.boost.org/svn/boost/tags/release/Boost_1_55_0/boost

ln -sfT "$PWD"/boost /usr/include/boost
ln -sfT /usr/pgsql-9.4/include/ /usr/include/postgresql

popd

cmake .
make -j4

EOF

