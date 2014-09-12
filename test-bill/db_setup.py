from model import *

def dropActiveConnections(connection, dbName):
    db = connection.cursor()
    db.execute("SELECT pid, datname, pg_terminate_backend(pid) FROM pg_stat_activity WHERE pid != pg_backend_pid() and datname='"+dbName+"';")

def dropDatabase(connection, dbName):
    print '    Drop database'
    db = connection.cursor()
    db.execute("DROP DATABASE IF EXISTS " + dbName + ";")

def createDatabase(connection, dbName):
    print '    Create database'

    db = connection.cursor()
    db.execute("CREATE DATABASE " + dbName + ";")

def fixBug(connection):
    db = connection.cursor()
    db.execute("DROP TYPE dblink_pkey_results;")

def executeSqlFile(connection, fileName):
    print '    Uploading sql file', fileName

    sql = open(fileName, "r").read()
    db = connection.cursor()
    db.execute(sql)

def recreateDb(dbName, schemaFileName):
    print 'Recreate database', dbName

    connection = DbWrap(cfg['dbmanager_conn_str'])

    dropActiveConnections(connection, dbName)
    dropDatabase(connection, dbName)
    createDatabase(connection, dbName)

    connection = DbWrap(cfg['dbmanager_conn_str'] + " dbname=" + dbName)

    fixBug(connection)
    executeSqlFile(connection, schemaFileName)


def recreateTestDatabases():

    recreateDb(cfg['dbname_avbilling'], './sql/avbilling.sql')
    recreateDb(cfg['dbname_nispd'], './sql/nispd.sql')
    recreateDb(cfg['dbname_nispd99'], './sql/nispd99.sql')

    print 'Done'
