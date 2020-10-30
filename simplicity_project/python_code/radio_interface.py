#%%
# -*- coding: utf-8 -*-

import serial
import re
import time

class Radio:
    def __init__(self, port, timeout=.1):
        self.ser     = serial.Serial(port=port)
        self.timeout = timeout
        self.ser.flush()
    def configure_transmitter(self, period_us, time_trans_us, channel):
        assert self.ser.in_waiting == 0
        assert type(period_us) == int
        assert 300 <= period_us <= 0xFFFF
        assert type(time_trans_us) == int
        assert 100 <= time_trans_us <= 13000
        assert type(channel) == int
        assert 0 <= channel <= 39
        s = b't%c%c%c%c%c\n'%(period_us       & 0xFF, period_us     >> 8,
                                time_trans_us & 0xFF, time_trans_us >> 8,
                                channel)
        self.ser.write(s)
        t_0 = time.time()
        while self.ser.in_waiting == 0:
            assert time.time()-t_0 < self.timeout
        assert self.ser.read() == b'\n'
    def configure_receiver(self, delay_us, duration_us, channel):
        assert self.ser.in_waiting == 0
        assert type(delay_us) == int
        assert 0 <= delay_us <= 0xFFFF
        assert type(duration_us) == int
        assert 0 <= duration_us <= 0xFFFF
        assert type(channel) == int
        assert 0 <= channel <= 39
        s = b'r%c%c%c%c%c\n'%(delay_us    & 0xFF, delay_us    >> 8,
                              duration_us & 0xFF, duration_us >> 8,
                              channel)
        self.ser.write(s)
        t_0 = time.time()
        while self.ser.in_waiting == 0:
            assert time.time()-t_0 < self.timeout
        assert self.ser.read() == b'\n'
    def read_output(self):
        num_samples = int(self.ser.readline()[:-1])
        I = []
        Q = []
        for i in range(num_samples):
            sample = self.ser.readline()
            vals = re.findall(r'[+-]?\d+', sample.decode('ascii'))
            I.append(int(vals[0]))
            Q.append(int(vals[1]))
        return (I, Q)
