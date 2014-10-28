import threading, subprocess
from config.config import *

class AppBill(threading.Thread):

    __configPath = cfg.billing_app_config
    __process = None


    def __del__(self):
        self.stop()

    def start(self):
        self.__initializeConfig()
        self.__process = self.__createProcess()
        super(AppBill, self).start()


    def stop(self):
        if self.__process:
            self.__process.kill()
            self.join()


    def run(self):
        self.__process.communicate()
        self.__process = None

    def __initializeConfig(self):
        configFile = open(self.__configPath, 'w+')
        templateFile = open(cfg.billing_app_config_template, 'r')

        for line in templateFile.readlines():
            configFile.write(line % cfg.__dict__)

        templateFile.close()
        configFile.close()


    def __createProcess(self):

        arguments = [
            cfg.billing_app_path,
            '--c',
            self.__configPath
        ]

        return subprocess.Popen(arguments)

