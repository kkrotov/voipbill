#!/usr/bin/python
import time, sys, psycopg2, configparser, ftplib, os, xml.etree.ElementTree, hashlib, syslog, time, datetime

config = configparser.ConfigParser()
config.readfp(open('EmulateCall.conf'))
db_host         = config.get('db','host')
db_port         = config.get('db','port')
db_user         = config.get('db','user')
db_password     = config.get('db','password')
db_database     = config.get('db','database')

class EmulateCall():
    db = None
    def init(self):
        self.db = psycopg2.connect(host=db_host, port=db_port, user=db_user, password=db_password, database=db_database)
        self.db.set_isolation_level(0)

    def addCall(self):
        cur = self.db.cursor()
        currenttime = datetime.datetime.now()
        setuptime = currenttime - datetime.timedelta(0 ,0, 0, 0, 3)
        connecttime = setuptime + datetime.timedelta(0, 13)
        cur.execute("INSERT INTO calls_cdr.start (call_id, nas_ip, src_number, dst_number, setup_time, connect_time, src_route, dst_route, src_noa, dst_noa) VALUES (1024564, '85.94.32.233', '74992130265', '73952280350', %s, %s, 'mcn_msk_ast16_99', 'BeeLoc_02', 3, 3)",  (setuptime, str(connecttime),))
        cur.close()

    def run(self):
        self.init()
        while True:
            try:
                self.addCall()
                time.sleep(5)
                continue
            except KeyboardInterrupt:
                break
            except:
                message = 'EmulateCall: error: ' + str(sys.exc_info()[1])
                syslog.syslog(syslog.LOG_ERR, message)
                time.sleep(10)

if __name__ == '__main__':
    syslog.openlog('EmulateCall', syslog.LOG_PID, syslog.LOG_LOCAL0)
    EmulateCall().run()

