import uuid
from config.config import *

class OpenCA:

    __currentCalls = {}
    __lockedNumbersLocal = {}
    __lockedNumbersGlobal = {}
    __lastSessionId = cfg.openca_lastSessionId

    @staticmethod
    def generateSessionId():
        OpenCA.__lastSessionId += 1
        return OpenCA.__lastSessionId

    @staticmethod
    def generateUniqueId():
        return uuid.uuid4().hex[0:16]

    @staticmethod
    def readCurrentCalls():
        return ','.join([str(key) for key in OpenCA.__currentCalls.keys()])

    @staticmethod
    def killCall(sessionId):
        OpenCA.__currentCalls.pop(sessionId, None)

    @staticmethod
    def readBlacklistLocal():
        return ','.join(OpenCA.__lockedNumbersLocal.keys())

    @staticmethod
    def readBlacklistGlobal():
        return ','.join(OpenCA.__lockedNumbersGlobal.keys())

    @staticmethod
    def lockNumberLocal(number):
        OpenCA.__lockedNumbersLocal[number] = True

    @staticmethod
    def lockNumberGlobal(number):
        OpenCA.__lockedNumbersGlobal[number] = True

    @staticmethod
    def unlockNumberLocal(number):
        OpenCA.__lockedNumbersLocal.pop(number, None)

    @staticmethod
    def unlockNumberGlobal(number):
        OpenCA.__lockedNumbersGlobal.pop(number, None)
