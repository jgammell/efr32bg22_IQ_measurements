#%%
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib
from matplotlib import pyplot as plt
import pickle
import os

font = {'family' : 'normal',
        'weight' : 'bold',
        'size'   : 22}
msize = 10

matplotlib.rc('font', **font)
figsize = (16, 12)

def normalize_data(data):
    std = np.std(data)
    data = np.array([d/(np.sqrt(2)*std) for d in data])
    return data
def phase_lsrl(data, invfn):
    return np.polyfit(range(len(data)), np.unwrap(invfn(data)), 1)

data_filename = r'efr32bg22IQ_2020-7-11_23-6-45.pickle'
with open(os.path.join(os.getcwd(), 'data', data_filename), 'rb') as F:
    Data = pickle.load(F)
I = normalize_data(Data[0])
Q = normalize_data(Data[1])

plt.figure(figsize=figsize)
plt.plot(range(len(I)), I, 'r.', markersize=msize, label='I')
plt.plot(range(len(Q)), Q, 'b.', markersize=msize, label='Q')
plt.legend()
plt.title('Normalized IQ data')
plt.xlabel('Measurement number')
plt.ylabel('I/Q')
plt.ylim([-1.1, 1.1])
plt.figure(figsize=figsize)
phase = np.unwrap(np.arctan2(Q, I))
plt.plot(range(len(phase)), phase)
plt.title('Unwrapped IQ phase')
plt.ylabel('Radians')
plt.xlabel('Measurement number')

plt.figure(figsize=figsize)
plt.plot(I, Q, 'b.', markersize=msize)
plt.title('Normalized IQ data')
plt.xlabel('I')
plt.ylabel('Q')
plt.xlim([-1.1, 1.1])
plt.ylim([-1.1, 1.1])
plt.gca().set_aspect('equal', adjustable='box')

