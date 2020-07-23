#%%
# -*- coding: utf-8 -*-

import serial

class Radio:
    def __init__(self, port):
        self.ser = serial.Serial(port=port)
    def write_char(self, c):
        assert len(c) == 1
        self.ser.write(c)
    def read_available(self):
        return self.ser.in_waiting != 0
    def readline(self):
        return self.ser.readline()
    def __del__(self):
        self.ser.close()

