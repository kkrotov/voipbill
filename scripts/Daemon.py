import sys, os, time, atexit
import logging
from signal import SIGTERM

logging.basicConfig(
    format='%(asctime)s.%(msecs)d %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S',
    level=logging.DEBUG,
    filename='/var/log/syncstat/log.log')

class Daemon:

        def __init__(self, pidfile, stdin='/dev/null', stdout='/var/log/syncstat/stdout.log', stderr='/var/log/syncstat/stderr.log'):
                self.stdin = stdin
                self.stdout = stdout
                self.stderr = stderr
                self.pidfile = pidfile
       
        def daemonize(self):
                try:
                        pid = os.fork()
                        if pid > 0:
                                # exit first parent
                                sys.exit(0)
                except OSError, e:
                        logging.error("fork #1 failed: %d (%s)\n" % (e.errno, e.strerror))
                        sys.exit(1)
       
                # decouple from parent environment
                os.chdir("/")
                os.setsid()
                os.umask(0)
       
                # do second fork
                try:
                        pid = os.fork()
                        if pid > 0:
                                # exit from second parent
                                sys.exit(0)
                except OSError, e:
                        logging.error("fork #2 failed: %d (%s)\n" % (e.errno, e.strerror))
                        sys.exit(1)
       
                # redirect standard file descriptors
                sys.stdout.flush()
                sys.stderr.flush()
                si = file(self.stdin, 'r')
                so = file(self.stdout, 'a+')
                se = file(self.stderr, 'a+', 0)
                os.dup2(si.fileno(), sys.stdin.fileno())
                os.dup2(so.fileno(), sys.stdout.fileno())
                os.dup2(se.fileno(), sys.stderr.fileno())
       
                # write pidfile
                atexit.register(self.delpid)
                pid = str(os.getpid())
                file(self.pidfile,'w+').write("%s\n" % pid)
       
        def delpid(self):
                os.remove(self.pidfile)
 
	def can_i_run(self):
                # Check for a pidfile to see if the daemon already runs
                try:
                        pf = file(self.pidfile,'r')
                        pid = int(pf.read().strip())
                        pf.close()
                except IOError:
                        pid = None

		if pid:
			try:
				os.kill(pid, 0)
			except OSError as (errno, strerr):
				pid = None
       
                if pid:
                        message = "pidfile %s already exist. Daemon already running?\n"
                        print message
                        logging.error(message % self.pidfile)
                        sys.exit(1)        

		
        def startd(self):
                self.can_i_run()
                self.init()
                self.daemonize()
                self.run()
                
        def rund(self):
    		self.can_i_run()
    		self.init()
                self.run()
 
        def stopd(self):
                # Get the pid from the pidfile
                try:
                        pf = file(self.pidfile,'r')
                        pid = int(pf.read().strip())
                        pf.close()
                except IOError:
                        pid = None
       
                if not pid:
                        message = "pidfile %s does not exist. Daemon not running?\n"
                        print message
                        logging.error(message % self.pidfile)
                        return # not an error in a restart
 
                # Try killing the daemon process       
                try:
                        while 1:
                                os.kill(pid, SIGTERM)
                                time.sleep(0.1)
                except OSError, err:
                        err = str(err)
                        if err.find("No such process") > 0:
                                if os.path.exists(self.pidfile):
                                        os.remove(self.pidfile)
                        else:
                                logging.error(str(err))
                                sys.exit(1)
 
        def restartd(self):
                self.stopd()
                self.startd()

	def execd(self):
	    if len(sys.argv) == 2:
	        if 'start' == sys.argv[1]:
        	    self.startd()
	        elif 'stop' == sys.argv[1]:
	            self.stopd()
	        elif 'restart' == sys.argv[1]:
	            self.restartd()
	        elif 'run' == sys.argv[1]:
	            self.rund()
	        else:
	            print "Unknown command"
	            sys.exit(2)
	        sys.exit(0)
	    else:
	        print "usage: %s start|stop|restart|run" % sys.argv[0]
	        sys.exit(2)
 
        def run(self):
                """
                RUN
                """
        def init(self):
		"""
		INIT
		"""    
