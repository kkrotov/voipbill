import threading, socket
from classes.OpenCA import OpenCA

class OpenCAEmulator(threading.Thread):
    __udpSocket = None


    def __del__(self):
        self.stop()


    def start(self, udpPort):
        self.__udpSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.__udpSocket.bind(('', udpPort))
        super(OpenCAEmulator, self).start()


    def stop(self):
        if self.__udpSocket:
            try:
                self.__udpSocket.shutdown(socket.SHUT_RD)
            except: pass

            self.join()
            self.__udpSocket = None


    def run(self):

        while self.__udpSocket:
            data, addr = self.__udpSocket.recvfrom(1024*20)
            if addr is None:
                break

            self.__udpSocket.sendto(
                self.__processCommand(data),
                addr
            )

        self.__udpSocket.close()
        self.__udpSocket = None

    def __processCommand(self, command):

        print "REQUEST:", command

        arguments = command.split(' ')
        command = arguments[0]

        result = '0'

        if command == 'KILL' and len(arguments) == 3:
            if OpenCA.killCall(arguments[2]):
                result = '1'
        elif command == 'SELECT' and len(arguments) == 1:
            result = OpenCA.readCurrentCalls()
        elif command == 'LOCK_LOCAL' and len(arguments) == 2:
            if OpenCA.lockNumberLocal(arguments[1]):
                result = '1'
        elif command == 'LOCK_GLOBAL' and len(arguments) == 2:
            if OpenCA.lockNumberGlobal(arguments[1]):
                result = '1'
        elif command == 'UNLOCK_LOCAL' and len(arguments) == 2:
            if OpenCA.unlockNumberLocal(arguments[1]):
                result = '1'
        elif command == 'UNLOCK_GLOBAL' and len(arguments) == 2:
            if OpenCA.unlockNumberGlobal(arguments[1]):
                result = '1'
        elif command == 'READ_BLACKLIST_LOCAL' and len(arguments) == 1:
            result = OpenCA.readBlacklistLocal()
            self.localBlacklistQueried = True
        elif command == 'READ_BLACKLIST_GLOBAL' and len(arguments) == 1:
            result = OpenCA.readBlacklistGlobal()
            self.globalBlacklistQueried = True
        elif command == 'READ_LOCKED_TRUNK' and len(arguments) == 1:
            result = OpenCA.readLockedTrunk()
            self.lockedTrunkQueried = True
        elif command == 'LOCK_TRUNK' and len(arguments) == 2:
            if OpenCA.lockTrunk(arguments[1]):
                result = '1'
        elif command == 'UNLOCK_TRUNK' and len(arguments) == 2:
            if OpenCA.unlockTrunk(arguments[1]):
                result = '1'

        print "RESPONSE:", result

        return result
