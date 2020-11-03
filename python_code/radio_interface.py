#%%
# -*- coding: utf-8 -*-

#%% 

from matplotlib import pyplot as plt
import numpy as np
from scipy.stats import linregress
import serial
import pickle
import datetime
import os
import re
import time


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
try:
    TX = serial.Serial('COM8')
    RX = serial.Serial('COM9')
    TX.write(b't')
    RX.write(b'r')
    I_RX, Q_RX = read_output(RX)
    time.sleep(.1)
    TX.write(b't')
    time.sleep(.1)
    RX.write(b't')
    time.sleep(.1)
    TX.write(b'r')
    time.sleep(.1)
    RX.write(b't')
    I_TX, Q_TX = read_output(TX)
    
    dt = datetime.datetime.now()
    filename =  r'efr32bg22_%d-%d-%d_%d-%d-%d.pickle'\
        %(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second)
    with open(os.path.join(r'C:\Users\jgamm\Desktop\2020-10-29', filename), 'wb') as F:
        pickle.dump((I_TX, Q_TX, I_RX, Q_RX), F)
finally:
    TX.close()
    RX.close()
    
#%%

import pickle
import os
from matplotlib import pyplot as plt
import numpy as np
from scipy.stats import linregress

with open(os.path.join(r'C:/Users/jgamm/Desktop/2020-10-29/', filename), 'rb') as F:
    (I_TX, Q_TX, I_RX, Q_RX) = pickle.load(F)
assert len(I_TX) == len(Q_TX)
assert len(I_RX) == len(Q_RX)
sample_indices_TX = np.arange(0, len(I_TX))
sample_indices_RX = np.arange(0, len(I_RX))
phase_TX = np.unwrap(np.arctan2(Q_TX, I_TX))
phase_RX = np.unwrap(np.arctan2(Q_RX, I_RX))

(fig, axes) = plt.subplots(1, 2)
axes[0].plot(I_TX, Q_TX, '.', color='blue')
axes[1].plot(I_RX, Q_RX, '.', color='blue')
axes[0].set_xlabel('I')
axes[0].set_ylabel('Q')
axes[1].set_xlabel('I')
axes[0].set_title('Transmit side')
axes[1].set_title('Receive side')
fig.suptitle('Q vs I for samples')

(fig, axes) = plt.subplots(1, 2)
axes[0].plot(sample_indices_TX, phase_TX, color='blue')
axes[1].plot(sample_indices_RX, phase_RX, color='blue')
axes[0].set_xlabel('Sample index')
axes[1].set_xlabel('Sample index')
axes[0].set_ylabel('Unwrapped phase')
axes[0].set_title('Transmit side')
axes[1].set_title('Receive side')
fig.suptitle('Unwrapped phase')
    
    
    
    
    
    
    
#%%
r"""
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
        m, b, _, _, _ = linregress(range(len(phase)), np.unwrap(phase))
        print('m = %1.2f, b %1.2f'%(m, b))
        ax[0].cla()
        ax[1].cla()
        ax[0].plot(range(len(I)), I)
        ax[1].plot(range(len(phase)), np.unwrap(phase), '.', markersize=10)
        ax[1].set_xlim([50, 200])
        ax[1].set_ylim([-200, 200])
        #plt.pause(.05)
    print('Mean: %1.3f'%(np.mean(jumps_means)))
    print('StDev: %1.3f'%(np.std(jumps_means)))
finally:
    del TX
    del RX
""";
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
   


