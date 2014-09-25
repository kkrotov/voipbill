# -*- coding: utf-8 -*-

import socket
import uuid
from datetime import datetime
from datetime import timedelta
import threading

from sqldb import *
from config.config import *

from classes.model import avbilling

def printlog(text):
    #log = open(logfile, 'a+')
    time = str(datetime.now())
    #log.write(time + ': ' + text + "\n")
    #log.close()
    print(time + ': ' + text + '\n')

# TODO: add condition list to wait from test instead of predefined timeout
class OpenCARealtimeEmulator(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.active = False

        self.defaultConnectTime = cfg.openca_defaultConnectTime
        self.nasipaddress = cfg.openca_nasipaddress
        self.operatorRouteNamePrefix = cfg.openca_operatorRouteNamePrefix

        self.avbilling = sqlConnection(cfg.avbillinguser, cfg.avbillingpassword, cfg.dbname_avbilling, False)

        self.udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.udp.bind(('', cfg.openca_udpPort))

        self.nextSessionId = cfg.openca_lastSessionId
        self.currentCalls = {}
        self.localLocked = {}
        self.globalLocked = {}

        # testing results:
        self.localBlacklistQueried = False
        self.globalBlacklistQueried = False

        self.active = True
        self.start()

    def __del__(self):
        self.stop()

    def stop(self):
        try:
            self.udp.shutdown(socket.SHUT_RD)
        except:
            pass
        if self.active:
            self.active = False
            self.join()
            self.udp.close()

    def select(self):
        return ','.join([str(key) for key in self.currentCalls.keys()])

    def blacklist_local(self):
        return ','.join(self.localLocked.keys())

    def blacklist_global(self):
        return ','.join(self.globalLocked.keys())

    def lock_local(self, number):
        print( 'Lock local ' + number )
        self.localLocked[number] = True
        return True

    def lock_global(self, number):
        print( 'Lock global ' + number )
        self.globalLocked[number] = True
        return True

    def unlock_local(self, number):
        print( 'Unlock local ' + number )
        self.localLocked.pop(number, None)
        return True

    def unlock_global(self, number):
        print( 'Unlock global ' + number )
        self.globalLocked.pop(number, None)
        return True

    def kill_call(self, callid, code):
        try:
            print( 'Kill call ' + callid + ' (' + self.currentCalls[callid]['caller'] + '->' + self.currentCalls[callid]['callee'] + ')' )
            self.emulateCallEnd(callid, int(code))
            return True
        except:
            return False

    def run(self):
        while self.active:
            data,addr = self.udp.recvfrom(1024*20)
            if addr is None:
                break

            res = '0'

            if True:
            #try:
                data = data.split(' ')
                cmd = data[0]

                if cmd == 'KILL' and len(data) == 3:
                    print(cmd)
                    if self.kill_call( data[2], data[1] ):
                        res = '1'
                elif cmd == 'SELECT' and len(data) == 1:
                    print(cmd)
                    res = self.select()
                    print res
                elif cmd == 'LOCK_LOCAL' and len(data) == 2:
                    print(cmd)
                    if self.lock_local( data[1] ):
                        res = '1'
                elif cmd == 'LOCK_GLOBAL' and len(data) == 2:
                    print(cmd)
                    if self.lock_global( data[1] ):
                        res = '1'
                elif cmd == 'UNLOCK_LOCAL' and len(data) == 2:
                    print(cmd)
                    if self.unlock_local( data[1] ):
                        res = '1'
                elif cmd == 'UNLOCK_GLOBAL' and len(data) == 2:
                    print(cmd)
                    if self.unlock_global( data[1] ):
                        res = '1'
                elif cmd == 'READ_BLACKLIST_LOCAL' and len(data) == 1:
                    res = self.blacklist_local()
                    self.localBlacklistQueried = True
                elif cmd == 'READ_BLACKLIST_GLOBAL' and len(data) == 1:
                    res = self.blacklist_global()
                    self.globalBlacklistQueried = True
                self.udp.sendto(res, addr)
                #except:
                
                #printlog('Unexpected error in OpenCA emulator: ' + str(sys.exc_info()[0]))

    def emulateCallStart(self, caller, callee, in_oper, out_oper, redirectnum, starttime = None):

        if self.globalLocked.has_key(caller):
            raise Exception('CALLER NUMBER GLOBALLY LOCKED')
        if self.localLocked.has_key(caller):
            raise Exception('CALLER NUMBER LOCALLY LOCKED')
        if self.globalLocked.has_key(callee):
            raise Exception('CALLEE NUMBER GLOBALLY LOCKED')
        if self.localLocked.has_key(callee):
            raise Exception('CALLEE NUMBER LOCALLY LOCKED')

        self.nextSessionId += 1
        newCallSessionId = self.nextSessionId
        uniqueId = uuid.uuid4().hex[0:16]
        currTime = datetime.now() if starttime is None else starttime
        connectTime = timedelta(seconds = self.defaultConnectTime)

        accttype = 'start' # or 'stop'
        outreplacecdpn = None # = '09503', skip processing

        # test call: calledstationid ~ '^05555', skip processing
        # IF callingstationid = '' THEN callingstationid = 'anonymous'

        self.openCAtoRadSql(accttype, newCallSessionId, uniqueId,
            currTime - connectTime, currTime,
            caller, callee, self.nasipaddress, 0, 3, 3,
            self.operatorRouteNamePrefix % in_oper,
            self.operatorRouteNamePrefix % out_oper, outreplacecdpn,
            redirectnum, None, 0, 0)

        self.currentCalls[newCallSessionId] = {
            'uniqueid': uniqueId,
            'setuptime': currTime - connectTime,
            'connecttime': currTime,
            'caller': caller,
            'callee': callee,
            'in_oper': in_oper,
            'out_oper': out_oper,
            'redirectnum': redirectnum
        }

        return newCallSessionId

    def emulateCallEnd(self, callid, reason, pre_duration = None):
        call = self.currentCalls[callid]

        if pre_duration is None:
            currTime = datetime.now()
            td = currTime - call['connecttime']

            # only Python 2.7+
            # duration = td.total_seconds()
            duration = (td.microseconds + (td.seconds + td.days * 24 * 3600) * 10**6) / 10**6
            # IF duration > 4000000000 THEN duration = 0, reason = 124
        else:
            duration = pre_duration
            currTime = call['connecttime'] + timedelta(seconds = duration)

        accttype = 'stop'
        outreplacecdpn = None # = '09503', skip processing

        self.openCAtoRadSql(accttype, callid, call['uniqueid'], call['setuptime'], call['connecttime'],
            call['caller'], call['callee'], self.nasipaddress, 0, 3, 3,
            self.operatorRouteNamePrefix % call['in_oper'],
            self.operatorRouteNamePrefix % call['out_oper'], outreplacecdpn,
            call['redirectnum'], currTime, reason, duration)

        inserted = []

        connection = avbilling.bind.raw_connection()
        try:
            cursor = connection.cursor()
            cursor.execute("SELECT radacctid FROM radacct_voip_stop WHERE acctuniqueid=%s", (call['uniqueid'],))
            inserted = cursor.fetchall()
            cursor.close()
            connection.commit()
        finally:
            connection.close()

        self.currentCalls.pop(callid, None)

        # Unpack a list of tuples (returned by psycopg2) into a list of values:
        return [radacctid for radacctid, in inserted]

    def generateCall(self, connecttime, duration, caller, callee, caller_op, callee_op, redirectnum = ''):
        callid = self.emulateCallStart(caller, callee, caller_op, callee_op, redirectnum, connecttime)
        return self.emulateCallEnd(callid, 16, duration)

    def openCAtoRadSql(self, accttype, acctsessionid, acctuniqueid, setuptime, connecttime,
                       callingstationid, calledstationid, nasipaddress, acctdelaytime,
                       incd_noa, incg_noa, in_route, out_route, outreplacecdpn,
                       redirectnum, disconnecttime, disconnectcause, acctsessiontime):

        setuptime = setuptime.replace(microsecond = int(setuptime.microsecond / 1000) * 1000)
        connecttime = connecttime.replace(microsecond = int(connecttime.microsecond / 1000) * 1000)
        if disconnecttime:
            disconnecttime = disconnecttime.replace(microsecond = int(disconnecttime.microsecond / 1000) * 1000)

        connection = avbilling.bind.raw_connection()
        try:
            cursor = connection.cursor()
            cursor.execute("""SELECT openca_to_radsql(%s::varchar, %s::bigint, %s::varchar, %s::varchar, %s::varchar,
                                     %s::varchar, %s::varchar, %s::varchar, %s::smallint, %s::smallint, %s::smallint,
                                     %s::varchar, %s::varchar, %s::varchar, %s::varchar, %s::varchar,
                                    %s::smallint, %s::bigint)""",
                                                         (accttype, acctsessionid, acctuniqueid, setuptime,
                                  connecttime, callingstationid, calledstationid, nasipaddress, acctdelaytime,
                                  incd_noa, incg_noa, in_route, out_route, outreplacecdpn,
                                  redirectnum, disconnecttime, disconnectcause, acctsessiontime))
            cursor.close()
            connection.commit()
        finally:
            connection.close()
