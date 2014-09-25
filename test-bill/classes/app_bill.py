#!/usr/bin/python
# -*- coding: utf-8 -*-

import threading

from config.config import *


class AppBill(threading.Thread):
    def __init__(self, configFilePath = cfg.billing_app_config, params = None):
        threading.Thread.__init__(self)

        self.finished = False

        self.proc = None
    
        # Initialize config file with current params
        self.writeConfigParams(configFilePath, **cfg.__dict__)

        import subprocess
        args = {}
        args[ 'shell' ] = True
        args[ 'stdin' ] = subprocess.PIPE
        args[ 'stdout' ] = subprocess.PIPE
        args[ 'stderr' ] = subprocess.STDOUT

        outStr = ""
        self.proc = subprocess.Popen( cfg.billing_app_path + ' --c ' + configFilePath + ((' ' + params) if params else ''), **args)

        self.start()

    def __del__(self):
        self.stop()

    def writeConfigParams(self, dstPath, **configSubst):
        dst = open(dstPath, 'w+')
        with open(cfg.billing_app_config_template, 'r') as src:
            strConfig = src.readlines()
            for line in strConfig:
                dst.write(line % configSubst)
        dst.close()

    def stop(self):
        if self.proc: 
            if not self.finished:
                self.proc.kill()
            self.join()

    def run(self):
        out, err = self.proc.communicate()
        self.finished = True
        if self.proc.returncode != 0:
            errDesc = out if out else '' + err if err else ''
            print(errDesc)

