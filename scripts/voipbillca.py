#!/usr/bin/python
import sys, re, socket, ConfigParser
from Daemon import Daemon
from commands import *
from datetime import *
from time import *

from NameServices import NameServerWrapper
import NumberAnalysisModule__POA, NumberAnalysisModule, CallControlFunctionModule__POA, CallControlFunctionModule
import CAMMIModule__POA, CAMMIModule

import smtplib
from email.MIMEText import MIMEText

config = ConfigParser.ConfigParser()
config.readfp(open('voipbillca.conf'))
main_pidfile    = config.get('main','pidfile')
main_logfile    = config.get('main','logfile')
main_groupnum   = config.get('main','groupnum')
email_from      = config.get('email','from')
email_to        = config.get('email','to')
email_subject   = config.get('email','subject')
email_body      = config.get('email','body')



def mailsend(mailfrom,mailto,subj,body):
    server = "127.0.0.1"
    port = 25
    msg = MIMEText(body, "", "utf-8")
    msg['Subject'] = subj
    msg['From'] = mailfrom
    msg['To'] = mailto

    s = smtplib.SMTP(server, port)
    s.ehlo()
    s.sendmail(mailfrom, mailto, msg.as_string())
    s.quit()
    return 1


def printlog(text):
    log = open(main_logfile, 'a+')
    time = str(datetime.now())
    log.write(time + ': ' + text + "\n")
    log.close()
    print time,': ',text
    
def complete_short_numbers(param):
    m_num = param.split(',')
    i = 0
    for num in m_num:
        if len(num) == 4:
            m_num[i] = main_groupnum+'.'+num
        i = i + 1
    return m_num


def kill(param):
    class CallBack_i (CallControlFunctionModule__POA.CCFMMICallBack):
        def __init__(self):
            self.exit=0
            self.message=""
            self.retCode=0

        def _print(self, message):
            print message

        def finish(self, message, retCode):
            self.message=message
            self.retCode=retCode
            self.exit=1

    calls=param.split(',')

    ns = NameServerWrapper()
    ccfObj=ns.getObject('TRACE_SPCIO', CallControlFunctionModule.CCFMMIInterface)

    res = True
    for callid in calls:
        cbi = CallBack_i()
        cbo = cbi._this()

        ccfObj.DeleteCall(int(callid), cbo);

        while cbi.exit == 0 :
            #time.sleep(0.1)
            pass
        if (cbi.retCode != 0):
            res = False

        printlog('KILL ' + callid + ' MSG: ' + str(cbi.message) + ' RetCode: ' + str(cbi.retCode))
    return res 

def lock_local(param):
    entries = complete_short_numbers(param)

    ns = NameServerWrapper()
    na = ns.getObject('NA_SPCIO', NumberAnalysisModule.NAMMIInterface)
    result = str(na.AddBWListEntries('Blk_Local', entries)[0])
    
    printlog('LOCK_LOCAL ' + str(entries) + ' ' + result)
    mailsend(email_from,email_to,email_subject,email_body + str("\n") + str(entries) + ' ' + result)
    return (result == 'SUCCEEDED')

def lock_global(param):
    entries = complete_short_numbers(param)

    ns = NameServerWrapper()
    na = ns.getObject('NA_SPCIO', NumberAnalysisModule.NAMMIInterface)
    result = str(na.AddBWListEntries('Blk_Global', entries)[0])
    
    printlog('LOCK_GLOBAL ' + str(entries) + ' ' + result)
    mailsend(email_from,email_to,email_subject,email_body + str("\n") + str(entries) + ' ' + result)
    return (result == 'SUCCEEDED')
    
def lock_trunk(param):
    ns = NameServerWrapper()
    caObj = ns.getObject('CA_SPCIO', CAMMIModule.CAMMIInterface)

    request  = CAMMIModule.Request_t('BLK-SIP-ROUTE', 0, 0, [ CAMMIModule.ParamValue_t("ROUTE",param) ])
    response = caObj.sendRequest(request)

    printlog('LOCK_TRUNK ' + param + ' ' + response.message)

    return response.wasSuccessful

def unlock_local(param):
    entries = complete_short_numbers(param)

    ns = NameServerWrapper()
    na = ns.getObject('NA_SPCIO', NumberAnalysisModule.NAMMIInterface)
    result = str(na.DeleteBWListEntries('Blk_Local', entries)[0])

    printlog('UNLOCK_LOCAL ' + str(entries) + ' ' + result)
    return (result == 'SUCCEEDED')

def unlock_global(param):
    entries = complete_short_numbers(param)

    ns = NameServerWrapper()
    na = ns.getObject('NA_SPCIO', NumberAnalysisModule.NAMMIInterface)
    result = str(na.DeleteBWListEntries('Blk_Global', entries)[0])

    printlog('UNLOCK_GLOBAL ' + str(entries) + ' ' + result)
    return (result == 'SUCCEEDED')
    
def unlock_trunk(param):
    ns = NameServerWrapper()
    caObj = ns.getObject('CA_SPCIO', CAMMIModule.CAMMIInterface)

    request  = CAMMIModule.Request_t('UBL-SIP-ROUTE', 0, 0, [ CAMMIModule.ParamValue_t("ROUTE",param) ])
    response = caObj.sendRequest(request)
    
    printlog('UNLOCK_TRUNK ' + param + ' ' + response.message)
    
    return response.wasSuccessful

def select():
    ns = NameServerWrapper()       
    ccfObj=ns.getObject('TRACE_SPCIO', CallControlFunctionModule.CCFMMIInterface)    

    data=ccfObj.ListCalls(False)
    calls=data.split(',;')
    
    callid=''
    for call in calls:
        m = re.search('A#=(.+?),', call)
        if m:
            if callid:
                callid = callid + ','+m.group(1)
            else:
                callid = m.group(1)
    printlog('SELECT '+ callid)

    return str(callid)
    
def blacklist_local():
    ns = NameServerWrapper()       
    na = ns.getObject('NA_SPCIO', NumberAnalysisModule.NAMMIInterface)
    result = na.RetrieveBWList('Blk_Local')

    res = ''
    if result[0] == NumberAnalysisModule.SUCCEEDED:
        for blk in result[2]:
            if res:
                res = res+','+blk
            else:
                res = blk
    res=res.replace(main_groupnum+'.','')
    printlog('READ_BLACKLIST_LOCAL ' + res)

    return res

    
def blacklist_global():
    ns = NameServerWrapper()       
    na = ns.getObject('NA_SPCIO', NumberAnalysisModule.NAMMIInterface)
    result = na.RetrieveBWList('Blk_Global')

    res = ''
    if result[0] == NumberAnalysisModule.SUCCEEDED:
        for blk in result[2]:
            if res:
                res = res+','+blk
            else:
                res = blk
    res=res.replace(main_groupnum+'.','')
    printlog('READ_BLACKLIST_GLOBAL ' + res)

    return res

    
def read_locked_trunk():
    ns = NameServerWrapper()
    caObj = ns.getObject('CA_SPCIO', CAMMIModule.CAMMIInterface)

    response = caObj.sendRequest(CAMMIModule.Request_t('LST-SIP-ROUTE', 0, 0, []));
    res = ''
    if response.wasSuccessful:
        for row in response.rows:
            if row[5] != 'InService':
                route = row[0]
                if res:
                    res = res+','+route
                else:
                    res = route

    printlog('READ_BLACKLIST_TRUNK ' + res)
    return res


class UdpSrv(Daemon):
    def init(self):
        self.udp = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.udp.bind( ('',58888) )
    
    def run(self):
        udp = self.udp
        while True:
            try:
                data,addr = udp.recvfrom(1024*20)
                res = '0'
                data = data.split(' ')
                cmd = data[0]
                if cmd == 'KILL' and len(data) == 3:
                    if kill( data[2] ):
                        res = '1'
                elif cmd == 'SELECT' and len(data) == 1:
                    res= select()
                elif cmd == 'LOCK_LOCAL' and len(data) == 2:
                    if lock_local( data[1] ):
                        res = '1'
                elif cmd == 'LOCK_GLOBAL' and len(data) == 2:
                    if lock_global( data[1] ):
                        res = '1'
                elif cmd == 'LOCK_TRUNK' and len(data) == 2:
                    if lock_trunk( data[1] ):
                        res = '1'
                elif cmd == 'UNLOCK_LOCAL' and len(data) == 2:
                    if unlock_local( data[1] ):
                        res = '1'
                elif cmd == 'UNLOCK_GLOBAL' and len(data) == 2:
                    if unlock_global( data[1] ):
                        res = '1'
                elif cmd == 'UNLOCK_TRUNK' and len(data) == 2:
                    if unlock_trunk( data[1] ):
                        res = '1'
                elif cmd == 'READ_BLACKLIST_LOCAL' and len(data) == 1:
                        res = blacklist_local()
                elif cmd == 'READ_BLACKLIST_GLOBAL' and len(data) == 1:
                    res = blacklist_global()
                elif cmd == 'READ_LOCKED_TRUNK' and len(data) == 1:
                    res = read_locked_trunk()

                udp.sendto(res, addr)

            except:
                printlog('Unexpected error: '+ cmd + ' :: '  + str(sys.exc_info()[0]))

if __name__ == '__main__':
    getoutput('export LD_LIBRARY_PATH=/opt/OPENca/current/web/omni_lib')
    getoutput('export PYTHONPATH=/opt/OPENca/current/web/ca_server/pyc_files.zip:/opt/OPENca/current/web/ca_server')

    srv = UdpSrv(main_pidfile)
    srv.execd()

