#!/bin/bash

chmod 0600 ~/.pgpass

# Ставим pip (установщик пакетов Python)
yum install python-pip
pip install --upgrade pip

# Ставим компилятор C/C++
wget http://people.centos.org/tru/devtools-2/devtools-2.repo -O /etc/yum.repos.d/devtools-2.repo
yum install devtoolset-2-gcc devtoolset-2-binutils devtoolset-2-gcc-c++

yum install git
yum install svn
yum install cmake
yum install cxxtest
yum install openssl-devel
yum install curl-devel
yum install postgresql94-devel
yum install epel-release
yum install java-1.8.0-openjdk-devel
yum install postgresql94-plpython

# TODO шаги здесь

/usr/pgsql-9.4/bin/psql nispd_test -U postgres -c "CREATE ROLE eivanov LOGIN ENCRYPTED PASSWORD 'md51dba7a09189e7ea0d4a5c65b3007397d' SUPERUSER INHERIT CREATEDB CREATEROLE REPLICATION"
/usr/pgsql-9.4/bin/psql nispd_test -U postgres -c "CREATE ROLE borisenko LOGIN ENCRYPTED PASSWORD 'md51dba7a09189e7ea0d4a5c65b3007397d' SUPERUSER INHERIT CREATEDB CREATEROLE REPLICATION"
/usr/pgsql-9.4/bin/psql nispd_test -U postgres -c 'CREATE ROLE g_bill_daemon_remote NOSUPERUSER NOINHERIT NOCREATEDB NOCREATEROLE NOREPLICATION'
/usr/pgsql-9.4/bin/psql nispd_test -U postgres -c 'CREATE ROLE g_stat NOSUPERUSER NOINHERIT NOCREATEDB NOCREATEROLE NOREPLICATION'
/usr/pgsql-9.4/bin/psql nispd_test -U postgres -c 'CREATE ROLE g_readonly NOSUPERUSER NOINHERIT NOCREATEDB NOCREATEROLE NOREPLICATION'
/usr/pgsql-9.4/bin/psql nispd_test -U postgres -c 'CREATE ROLE g_voipgui NOSUPERUSER NOINHERIT NOCREATEDB NOCREATEROLE NOREPLICATION'
/usr/pgsql-9.4/bin/psql nispd_test -U postgres -c "CREATE ROLE readonly LOGIN ENCRYPTED PASSWORD 'md576baf430da25297e4826168f7f75a0e5' NOSUPERUSER INHERIT NOCREATEDB NOCREATEROLE NOREPLICATION"
/usr/pgsql-9.4/bin/psql nispd_test -U postgres -c 'GRANT g_readonly TO readonly'
/usr/pgsql-9.4/bin/psql nispd_test -U postgres -c 'CREATE ROLE gopenca NOSUPERUSER NOINHERIT NOCREATEDB NOCREATEROLE NOREPLICATION'

/usr/pgsql-9.4/bin/createlang nispd_test -U postgres plpythonu

yum install python-devel
scl enable devtoolset-2 bash << \EOF
PATH=$PATH:/usr/pgsql-9.4/bin
pip install psycopg2

EOF

pip install unittest2
pip install xmlrunner
pip install pgpubsub
pip install requests
