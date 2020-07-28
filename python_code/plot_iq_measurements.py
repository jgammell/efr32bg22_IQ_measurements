#%%
# -*- coding: utf-8 -*-

from scipy.stats import linregress
import numpy as np
import matplotlib
from matplotlib import pyplot as plt
from matplotlib import cm
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

#data_filename = r'efr32bg22IQ_2020-7-15_27-38-38.pickle'
with open(os.path.join(os.getcwd(), 'data', data_filename), 'rb') as F:
    Data = pickle.load(F)

I_colors = cm.get_cmap('cool', len(Data))
I_colors = [I_colors(i/len(Data)) for i in range(len(Data))]
Q_colors = cm.get_cmap('Wistia', len(Data))
Q_colors = [Q_colors(i/len(Data)) for i in range(len(Data))]

# Plot IQ vs. 'time'
plt.figure(figsize=figsize)
for trial in range(len(Data)):
    I = Data[trial][0]
    Q = Data[trial][1]
    plt.plot(range(len(I)), I, '.', color=I_colors[trial], markersize=msize)
    plt.plot(range(len(Q)), Q, '.', color=Q_colors[trial], markersize=msize)
plt.title('IQ data')
plt.xlabel('Measurement number')
plt.ylabel('I/Q')

intercepts = []
# Plot unwrapped phase vs. 'time'
plt.figure(figsize=figsize)
for trial in range(len(Data)):
    I = Data[trial][0][50:]
    Q = Data[trial][1][50:]
    phase = np.unwrap(np.arctan2(Q, I))
    plt.plot(range(len(phase)), phase, '.', color=I_colors[trial], markersize=msize, label='Trial %d'%(trial+1))
    plt.legend()
    m, b, r, _, _ = linregress(range(len(phase)), phase)
    intercepts.append(b)
    print('Trial %d LSRL for unwrapped phase:'%(trial+1))
    print('\tEquation: phase=%.3fn+%.3f'%(m, b))
    print('\tr-squared=%.6f'%(r**2))
plt.title('Unwrapped IQ phase')
plt.ylabel('Radians')
plt.xlabel('Measurement number')

# Plot y-intercept of LSRL for each trial
plt.figure(figsize=figsize)
plt.plot(range(len(intercepts)), intercepts)
plt.xlabel('Trial')
plt.ylabel('Radians')
plt.title('Y-intercept of LSRL of unwrapped phase, vs. trial number')

# Plot difference between adjacent phase measurements, vs. 'time'
plt.figure(figsize=figsize)
for trial in range(len(Data)):
    I = Data[trial][0]
    Q = Data[trial][1]
    phase = np.unwrap(np.arctan2(Q, I))
    phase_jump = np.array([p1-p0 for p0, p1 in zip(phase[:-1], phase[1:])])
    plt.plot(range(len(phase_jump)), phase_jump, '.', color=I_colors[trial], markersize=msize, label='Trial %d'%(trial+1))
    plt.legend()
plt.title('Phase jump between samples')
plt.ylabel('Radians')
plt.xlabel('Measurement number')


# Plot Q vs. I
plt.figure(figsize=figsize)
for trial in range(len(Data)):
    I = Data[trial][0]
    Q = Data[trial][1]
    plt.plot(I, Q, '.', color=I_colors[trial], markersize=msize, label='Trial %d'%(trial+1))
    plt.legend()
plt.title('Normalized IQ data')
plt.xlabel('I')
plt.ylabel('Q')
plt.gca().set_aspect('equal', adjustable='box')

