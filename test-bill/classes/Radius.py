from classes.OpenCA import *
from classes.DbAvbilling import *
from datetime import datetime, timedelta
from config.config import *

class Radius:

    @staticmethod
    def startCall(
            caller, callee, inOper, outOper, connectTime = None,
            redirectNum = None, sessionId = None, uniqueId = None
    ):
        Radius.rawInsertCall(
            recordType = 'start',
            caller = caller, callee = callee,
            inOper = inOper, outOper = outOper,
            connectTime = connectTime, redirectNum = redirectNum,
            sessionId = sessionId, uniqueId = uniqueId,
        )

    @staticmethod
    def stopCall(
            caller, callee, duration, inOper, outOper, connectTime = None,
            redirectNum = None, sessionId = None, uniqueId = None, disconnectCause = None
    ):
        if connectTime is None:
            connectTime = datetime.now()

        disconnectTime = connectTime - timedelta(seconds = duration)

        if disconnectCause is None:
            disconnectCause = 16

        Radius.rawInsertCall(
            recordType = 'stop',
            caller = caller, callee = callee, duration = duration,
            inOper = inOper, outOper = outOper,
            connectTime = connectTime, redirectNum = redirectNum,
            sessionId = sessionId, uniqueId = uniqueId,
            disconnectTime = disconnectTime, disconnectCause = disconnectCause
        )


    @staticmethod
    def rawInsertCall(
            recordType, caller, callee, duration, inOper, outOper, connectTime = None,
            redirectNum = None, sessionId = None, uniqueId = None, disconnectTime = None, disconnectCause = None
    ):
        if sessionId is None:
            sessionId = OpenCA.generateSessionId()

        if uniqueId is None:
            uniqueId = OpenCA.generateUniqueId()

        if connectTime is None:
            connectTime = datetime.now()

        inRoute = cfg.openca_operatorRouteNamePrefix % inOper
        outRoute = cfg.openca_operatorRouteNamePrefix % outOper
        nasipAddress = cfg.openca_nasipaddress
        acctDelayTime = 0
        incdNoa = 3
        incgNoa = 3
        outReplaceCdpn = None

        connection = DbAvbilling.connection().bind.raw_connection()
        try:
            cursor = connection.cursor()
            cursor.execute("""
                SELECT openca_to_radsql(
                    %s::varchar, %s::bigint, %s::varchar, %s::varchar, %s::varchar,
                    %s::varchar, %s::varchar, %s::varchar, %s::smallint,
                    %s::smallint, %s::smallint, %s::varchar, %s::varchar, %s::varchar,
                    %s::varchar, %s::varchar, %s::smallint, %s::bigint
                )""", (
                    recordType, sessionId, uniqueId, connectTime, connectTime,
                    caller, callee, nasipAddress, acctDelayTime,
                    incdNoa, incgNoa, inRoute, outRoute, outReplaceCdpn,
                    redirectNum, disconnectTime, disconnectCause, duration
                ))
            cursor.close()
            connection.commit()
        finally:
            connection.close()