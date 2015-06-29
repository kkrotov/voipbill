#!/usr/bin/python
import time, sys, psycopg2, ConfigParser, ftplib, os, xml.etree.ElementTree, hashlib, syslog

from Daemon import Daemon

config = ConfigParser.ConfigParser()
config.readfp(open('CdrParser.conf'))
main_pidfile    = config.get('main','pidfile')
main_nasip      = config.get('main','nasip')
db_host         = config.get('db','host')
db_port         = config.get('db','port')
db_user         = config.get('db','user')
db_password     = config.get('db','password')
db_database     = config.get('db','database')
ftp_host        = config.get('ftp','host')
ftp_user        = config.get('ftp','user')
ftp_password    = config.get('ftp','password')


class CdrParser(Daemon):
    def init(self):
        self.db = psycopg2.connect(host=db_host, port=db_port, user=db_user, password=db_password, database=db_database)
        self.db.set_isolation_level(0)

    def getLastDbFileName(self):
        cur = self.db.cursor()
        cur.execute("select file_name from calls_cdr.cdr_file order by file_name desc")
        lastFileName = cur.fetchone()
        cur.close()
        if lastFileName is not None:
            return lastFileName[0]
        else:
            return None

    def isFileParsed(self, fileName):
        cur = self.db.cursor()
        cur.execute("select file_name from calls_cdr.cdr_file where file_name='%s' " % fileName)
        lastFileName = cur.fetchone()
        cur.close()
        return lastFileName is not None

    def getFileNameForParse(self):
        ftp = ftplib.FTP(ftp_host)
        ftp.login(ftp_user, ftp_password)
        files = ftp.nlst()
        ftp.close()

        lastNotParsedFileName = None
        n = len(files) - 1
        while n >= 0:
            fileName = files[n]
            if self.isFileParsed(fileName):
                break
            else:
                lastNotParsedFileName = fileName
            n = n - 1
        return lastNotParsedFileName

    def getFileData(self, fileName):

        tmpFileName = '/tmp/' + fileName
        tmpFile = None

        ftp = ftplib.FTP(ftp_host)
        try:
            ftp.login(ftp_user, ftp_password)
            tmpFile = open(tmpFileName, 'wb')
            ftp.retrbinary('RETR %s' % fileName, tmpFile.write)
            #tmpFile.write('</OT_CDR_File>')
        finally:
            ftp.quit()
            if tmpFile is not None:
                tmpFile.close()

        tree = xml.etree.ElementTree.parse(tmpFileName)

        os.unlink(tmpFileName)

        return tree.getroot().findall('OT_Call_CDR')

    def getValue(self, cdr, name, canBeEmpty = False):
        value = cdr.find(name)
        if value is not None or canBeEmpty == False:
            value = value.text
        return value

    def isCallFinished(self, cdr):
        return cdr.find('CallFinished').text == 'Yes'

    def parseCallId(self, cdr):
        return cdr.find('CallReference').text

    def parseTime(self, cdr, name, canBeEmpty = False):
        value = self.getValue(cdr, name, canBeEmpty)
        if canBeEmpty and value is None:
            return None

        return value[:4] + '-' + value[4:6] + '-' + value[6:8] + ' ' + value[8:10] + ':' + value[10:12] + ':' + value[12:14] + '.' + value[15:21] + '+00'

    def parseSrcNumber(self, cdr, name, canBeEmpty = False):
        value = self.getValue(cdr, name, canBeEmpty)
        if canBeEmpty and value is None:
            return ['anonymous', '3']
        return value.split(' ')

    def parseDstNumber(self, cdr, name):
        value = self.getValue(cdr, name)
        return value.split(' ')

    def parseSessionTime(self, cdr, name, canBeEmpty = False):
        value = self.getValue(cdr, name, canBeEmpty)
        if canBeEmpty and value is None:
            return 0
        value = value.split(':')[0]
        return int(value) + 1

    def parseDisconnectCause(self, cdr, name, canBeEmpty = False):
        value = self.getValue(cdr, name, canBeEmpty)
        if canBeEmpty and value is None:
            return 0
        value = value.split(' ')[0]
        value = value.split(':')[1]
        return int(value)

    def parseCall(self, cdr):
        call_id             = self.parseCallId(cdr)
        src_number, src_noa = self.parseSrcNumber(cdr, 'IncomingCallingAddress', True)
        dst_number, dst_noa = self.parseDstNumber(cdr, 'IncomingCalledAddress')
        redirect_number, x  = self.parseSrcNumber(cdr, 'IncomingRedirectingAddress', True)
        setup_time          = self.parseTime(cdr, 'StartTimeStamp')
        connect_time        = self.parseTime(cdr, 'AnswerTimeStamp', True)
        disconnect_time     = self.parseTime(cdr, 'ReleaseTimeStamp')
        session_time        = self.parseSessionTime(cdr, 'CallDuration', True)
        src_route           = self.getValue(cdr, 'IncomingRouteId')
        dst_route           = self.getValue(cdr, 'OutgoingRouteId', True)
        disconnect_cause    = self.parseDisconnectCause(cdr, 'CauseIndicator', True)
        setup_time_raw      = self.getValue(cdr, 'StartTimeStamp')

        hash = call_id + setup_time_raw[:14]
        hash = hashlib.md5(hash).hexdigest()

        return {
            'call_id': call_id,
            'src_number': src_number[:32],
            'dst_number': dst_number[:32],
            'redirect_number': redirect_number[:32],
            'setup_time': setup_time,
            'connect_time': connect_time,
            'disconnect_time': disconnect_time,
            'session_time': session_time,
            'src_route': src_route,
            'dst_route': dst_route,
            'src_noa': src_noa,
            'dst_noa': dst_noa,
            'disconnect_cause': disconnect_cause,
            'hash': hash,
        }

    def isCallNotExists(self, cur, hash):
        cur.execute("select hash from calls_cdr.cdr where hash=%s limit 1", (hash,))
        row = cur.fetchone()
        return row is None

    def insertCall(self, cur, call):
        cur.execute(
            "select public.insert_cdr(%s,%s,%s,%s,%s,'%s',%s,%s,%s,'%s',%s,%s,%s,%s)",
            (call['call_id'], main_nasip, call['src_number'], call['dst_number'], call['redirect_number'], call['session_time'], call['setup_time'], call['connect_time'], call['disconnect_time'], call['disconnect_cause'], call['src_route'], call['dst_route'], call['src_noa'], call['dst_noa'],)
        )

    def insertCdrFile(self, cur, fileName, totalCount, insertCount, errorCount, errorIds):
        errorIds = ','.join(errorIds)
        cur.execute(
            "insert into calls_cdr.cdr_file(file_name,total_count,insert_count,error_count,error_ids)values(%s,%s,%s,%s,%s)",
            (fileName, totalCount, insertCount, errorCount, errorIds,)
        )

    def parseCycle(self):
        self.init()

        while True:
            fileName = self.getFileNameForParse()
            if fileName is None:
                break
            print fileName

            cdrList = self.getFileData(fileName)
            totalCount = 0
            insertCount = 0
            errorCount = 0
            errorIds = []

            cur = self.db.cursor()

            for cdr in cdrList:
                totalCount = totalCount + 1
                #print totalCount

                if self.isCallFinished(cdr) == False:
                    continue


                try:
                    call = self.parseCall(cdr)
                except:
                    errorCount = errorCount + 1
                    errorIds.append(self.getValue(cdr, 'CallReference'))
                    continue

                if self.isCallNotExists(cur, call['hash']):
                    self.insertCall(cur, call)
                    #print call
                    insertCount = insertCount + 1


            self.insertCdrFile(cur, fileName, totalCount, insertCount, errorCount, errorIds)

            cur.close()

            message = 'CdrParser: parsed: ' + fileName + ', total: ' + str(totalCount) + ', insert: ' + str(insertCount) + ', error:' + str(errorCount) + ', errorIds: ' + ','.join(errorIds)
            syslog.syslog(syslog.LOG_INFO, message)
            print message



    def run(self):
        while True:
            try:
                self.parseCycle()

                time.sleep(60*5)
                continue
            except KeyboardInterrupt:
                break
            except:
                message = 'CdrParser: error: ' + str(sys.exc_info()[1])
                print message
                syslog.syslog(syslog.LOG_ERR, message)
                time.sleep(60*30)




if __name__ == '__main__':
    syslog.openlog('CdrParser', syslog.LOG_PID, syslog.LOG_LOCAL0)
    CdrParser(main_pidfile).execd()

