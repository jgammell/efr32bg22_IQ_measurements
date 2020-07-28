#%%
# -*- coding: utf-8 -*-

import serial
import re
import time

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
    def run_trials(self, initial_delay_us, num_trials, delay_us):
        assert type(initial_delay_us) == int
        assert type(num_trials) == int
        assert type(delay_us) == int
        assert 0 <= initial_delay_us <= 100000
        assert 0 <= num_trials <= 10
        assert 0 <= delay_us <= 100000
        self.ser.write(b'ri%dn%dd%d\n'%(initial_delay_us, num_trials, delay_us))
        output = []
        while not self.read_available(): pass
        for trial in range(num_trials):
            output.append([[], []])
            num_samples = int(self.readline()[:-1])
            print('Trial %d: saving %d samples'%(trial+1, num_samples))
            for i in range(num_samples):
                sample = self.readline()
                vals = re.findall(r'[+-]?\d+', sample.decode('ascii'))
                output[-1][0].append(int(vals[0]))
                output[-1][1].append(int(vals[1]))
        return output
            
    def __del__(self):
        self.ser.close()

