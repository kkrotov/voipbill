#!/usr/bin/python
from time import sleep
from classes.OpenCAEmulator import OpenCAEmulator

if __name__ == "__main__":
    openCA = OpenCAEmulator()
    openCA.start(58888)
    while True:
        sleep(5)
