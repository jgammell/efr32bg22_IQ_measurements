#%%
# -*- coding: utf-8 -*-

from scipy.stats import linregress
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

data_filename = r'efr32bg22IQ_2020-7-12_24-46-59.pickle'
with open(os.path.join(os.getcwd(), 'data', data_filename), 'rb') as F:
    Data = pickle.load(F)
I = Data[0]
Q = Data[1]

plt.figure(figsize=figsize)
plt.plot(range(len(I)), I, 'r.', markersize=msize, label='I')
plt.plot(range(len(Q)), Q, 'b.', markersize=msize, label='Q')
plt.legend()
plt.title('Normalized IQ data')
plt.xlabel('Measurement number')
plt.ylabel('I/Q')
plt.figure(figsize=figsize)
phase = np.unwrap(np.arctan2(Q, I))
plt.plot(range(len(phase)), phase)
plt.title('Unwrapped IQ phase')
plt.ylabel('Radians')
plt.xlabel('Measurement number')
m, b, r, _, _ = linregress(range(len(phase)), phase)
print('LSRL for unwrapped phase:')
print('\tEquation: phase=%.3fn+%.3f'%(m, b))
print('\tr-squared=%.6f'%(r**2))

plt.figure(figsize=figsize)
plt.plot(I, Q, 'b.', markersize=msize)
plt.title('Normalized IQ data')
plt.xlabel('I')
plt.ylabel('Q')
plt.gca().set_aspect('equal', adjustable='box')

