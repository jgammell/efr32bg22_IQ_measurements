#%%
# -*- coding: utf-8 -*-

#%% 

def read_output(port):
    num_samples = int(port.readline()[:-1])
    I = []
    Q = []
    for i in range(num_samples):
        sample = port.readline()
        vals = re.findall(r'[+-]?\d+', sample.decode('ascii'))
        I.append(int(vals[0]))
        Q.append(int(vals[1]))
    return (I, Q)   
   
# Code to get IQ measurements at fixed distance, and record mean phase jump between adjacent samples

from matplotlib import pyplot as plt
import numpy as np
from scipy.stats import linregress

J = 0

try:
    TX, RX = None, None
    TX = serial.Serial('COM8')
    RX = serial.Serial('COM9')
    
    TX.write(b't')
    
    #(fig, ax) = plt.subplots(2, 1)
    
    jumps_means = []
    for i in range(25):
        RX.write(b'r')
        I, Q = read_output(RX)
        phase = np.unwrap(np.arctan2(Q, I))
        jumps = [p2-p1 for p1, p2 in zip(phase[:-1], phase[1:])]
        jumps_means.append(np.mean(jumps))
        #averaged_jumps = [np.mean(jumps[i-20:i+20]) for i in range(20, len(jumps)-20)]
        #J = .1*np.mean(jumps) + .9*J
        #print(J)
        #plt.cla()
        #plt.plot(range(len(averaged_jumps)), averaged_jumps)
        #plt.ylim([-2, 2])
        #plt.plot(range(len(averaged_jumps)), averaged_jumps)
        #plt.ylim([-4, 4])
        plt.plot(range(len(phase)), phase)
        #plt.ylim([-200, 200])
        """m, b, _, _, _ = linregress(range(len(phase)), np.unwrap(phase))
        print('m = %1.2f, b %1.2f'%(m, b))
        ax[0].cla()
        ax[1].cla()
        ax[0].plot(range(len(I)), I)
        ax[1].plot(range(len(phase)), np.unwrap(phase), '.', markersize=10)
        ax[1].set_xlim([50, 200])
        ax[1].set_ylim([-200, 200])"""
        #plt.pause(.05)
    print('Mean: %1.3f'%(np.mean(jumps_means)))
    print('StDev: %1.3f'%(np.std(jumps_means)))
finally:
    del TX
    del RX
    
#%%

# Distance measurements taken by hand - in my bedroom/living room

room_values = [.579, .695, .968, 1.033, .983, 1.081, .977, .888, .620]
room_stdev = [.153, .147, .163, .121, .111, .193, .080, .147, .093]
living_room_values = [.753, 1.009, .899, 1.102, 1.015, .966, 1.105, .908, .534]
living_room_stdev = [.161, .119, .127, .107, .268, .127, .101, .192, .079]
distances = [.5, 1, 1.5, 2, 2.5, 2, 1.5, 1, .5]

plt.errorbar(distances[:5], room_values[:5], yerr=room_stdev[:5], color='blue', linestyle='-')
plt.errorbar(distances[4:], room_values[4:], yerr=room_stdev[4:], color='blue', linestyle='-.')
plt.errorbar(distances[:5], living_room_values[:5], yerr=living_room_stdev[:5], color='red', linestyle='-')
plt.errorbar(distances[4:], living_room_values[4:], yerr=living_room_stdev[4:], color='red', linestyle='-.')
plt.legend(['Room 1 Away', 'Room 1 Towards', 'Room 2 Away', 'Room 2 Towards'])

#%% Functions for talking to EFR

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
   


