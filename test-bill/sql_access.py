#!/usr/bin/python
# -*- coding: utf-8 -*-

# NOTE: to execute tests, 'adminpack' extension in PostgreSQL should be installed,
# `sudo yum install postgresql91 postgresql91-server postgresql91-libs postgresql91-contrib postgresql91-devel`
# fixes that.

try:
    import psycopg2
    import psycopg2.extras
    import psycopg2.errorcodes
except Exception as oEx:
    print('''Please, install 'psycopg2' extension for Python:
  sudo yum install python-psycopg2.
Also, to execute tests, 'adminpack' extension in PostgreSQL should be installed:
  sudo yum install postgresql91 postgresql91-server postgresql91-libs postgresql91-contrib postgresql91-devel
fixes that.''')
    raise oEx

# print('psycopg2 version is ' + psycopg2.__version__ + '\n')

# Inet type is disabled by default in psycopg2
psycopg2.extras.register_inet()

class DbWrap(object):
    def __init__(self, conn_str):
        self.connected = False
        self.db = psycopg2.connect(conn_str)
        self.connected = True
        self.db.set_isolation_level(psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT)
        self.db.autocommit = True

    def __del__(self):
        self.close()

    def close(self):
        if (self.connected):
            self.db.close()

    def cursor(self):
        return self.db.cursor()
