#%%
# -*- coding: utf-8 -*-

import pickle
import os
import numpy as np
import matplotlib
from matplotlib import pyplot as plt
from matplotlib import cm
from scipy.stats import linregress

font = {'family': 'normal',
        'weight': 'bold',
        'size': 22}
msize = 20
matplotlib.rc('font', **font)
figsize = (16, 12)

filename = r'efr32bg22IQ_2020-8-17_20-5-55.pickle'
with open(os.path.join(os.getcwd(), '../data', filename), 'rb') as F:
    (I_raw, Q_raw) = pickle.load(F)

n = len(I_raw)
assert n == len(Q_raw)

magnitudes = np.array([i**2 + q**2 for i, q in zip(I_raw, Q_raw)])
start_indices = []
stop_indices = []
thresh = np.mean(magnitudes)
seeking_start = True
for i in range(n):
    if seeking_start and (magnitudes[i] > thresh):
        seeking_start = False
        start_indices.append(i)
    elif not(seeking_start) and (magnitudes[i] < thresh):
        seeking_start = True
        stop_indices.append(i)
if start_indices[-1] > stop_indices[-1]:
    del start_indices[-1]
if stop_indices[0] < start_indices[0]:
    del stop_indices[0]
assert len(start_indices) == len(stop_indices)
max_interval = np.max(np.array([stop-start for start, stop in zip(start_indices, stop_indices)]))
i = 0
while i < len(start_indices):
    if stop_indices[i]-start_indices[i] < .8*max_interval:
        del start_indices[i]
        del stop_indices[i]
    else:
        i += 1



plt.figure(figsize=figsize)
plt.plot(range(n), magnitudes, color='blue', label='Magnitude')
plt.plot(range(n), np.mean(magnitudes)*np.ones(n), color='orange', label='Threshold')
plt.vlines(start_indices, 0, 1.1*np.max(magnitudes[start_indices[0]:stop_indices[-1]]), color='green', label='Rising edges')
plt.vlines(stop_indices, 0, 1.1*np.max(magnitudes[start_indices[0]:stop_indices[-1]]), color='red', label='Falling edges')
plt.xlim([0, n-1])
plt.ylim([0, 1.1*np.max(magnitudes[start_indices[0]:stop_indices[-1]])])
plt.title('Squared magnitude of signal, vs. measurement number')
plt.xlabel('Measurement number')
plt.ylabel('Squared magnitude')
plt.legend()

mean_jump = np.mean((np.mean([s2-s1 for s1, s2 in zip(start_indices[:-1], start_indices[1:])]),
                     np.mean([s2-s1 for s1, s2 in zip(stop_indices[:-1], stop_indices[1:])])))
mean_duration = np.mean([stop-start for start, stop in zip(start_indices, stop_indices)])
midpoints = [start_indices[0]+int(.5*mean_duration)+i*int(mean_jump) for i in range(len(start_indices))]
intervals = []
for midpoint in midpoints:
    intervals.append((midpoint-int(.25*mean_duration), midpoint+int(.25*mean_duration)))


##
interpolate = True
if interpolate:
    on_magnitude = 0
    missed_indices = []
    for interval in intervals:
        on_magnitude += .5*(np.std(I_raw[interval[0]:interval[1]])+np.std(Q_raw[interval[0]:interval[1]]))
        phases = np.unwrap(np.arctan2(Q_raw[interval[0]:interval[1]], I_raw[interval[0]:interval[1]]))
        phase_jumps = [p2-p1 for p1, p2 in zip(phases[:-1], phases[1:])]
        for i in range(len(phases)-1):
            if 1.75 <= (phases[i+1]-phases[i])/np.mean(phase_jumps) <= 2.25:
                missed_indices.append(i+1+interval[0])
    on_magnitude *= np.sqrt(2)
    on_magnitude /= len(intervals)
    ##
    for i in range(len(missed_indices)-1):
        i1 = missed_indices[i]
        i2 = missed_indices[i+1]
        jump = (i2-i1)%39
        if jump == 0:
            jump = 19
        index = i1
        while index < i2:
            interval = None
            for start_index, stop_index in zip(start_indices, stop_indices):
                if start_index <= index < stop_index:
                    interval = [start_index, stop_index]
            if interval == None:
                I_raw = np.insert(I_raw, index, 0)
                Q_raw = np.insert(Q_raw, index, 0)
            else:
                p = np.unwrap(np.arctan2(Q_raw[interval[0]:interval[1]], I_raw[interval[0]:interval[1]]))
                p1 = p[index-1-interval[0]]
                p2 = p[index-interval[0]]
                new_phase = .5*(p1+p2)
                I_raw = np.insert(I_raw, index, np.int16(on_magnitude*np.cos(new_phase)))
                Q_raw = np.insert(Q_raw, index, np.int16(on_magnitude*np.sin(new_phase)))
            for j in range(i+1, len(missed_indices)):
                missed_indices[j] += 1
            for j in range(len(start_indices)):
                if start_indices[j] > index:
                    start_indices[j] += 1
                if stop_indices[j] > index:
                    stop_indices[j] += 1
            index += jump
            jump = 19 if jump == 20 else 20
        assert index == i2
    interval = [start_indices[-1], stop_indices[-1]]
    index = missed_indices[-1]
    p = np.unwrap(np.arctan2(Q_raw[interval[0]:interval[1]], I_raw[interval[0]:interval[1]]))
    p1 = p[index-1-interval[0]]
    p2 = p[index-interval[0]]
    new_phase = .5*(p1+p2)
    I_raw = np.insert(I_raw, index, np.int16(on_magnitude*np.cos(new_phase)))
    Q_raw = np.insert(Q_raw, index, np.int16(on_magnitude*np.sin(new_phase)))
    
    I_raw = I_raw[start_indices[0]-1:stop_indices[-1]+1]
    Q_raw = Q_raw[start_indices[0]-1:stop_indices[-1]+1]
    
    n = len(I_raw)
    assert n == len(Q_raw)
    
    magnitudes = np.array([i**2 + q**2 for i, q in zip(I_raw, Q_raw)])
    start_indices = []
    stop_indices = []
    thresh = np.mean(magnitudes)
    seeking_start = True
    for i in range(n):
        if seeking_start and (magnitudes[i] > thresh):
            seeking_start = False
            start_indices.append(i)
        elif not(seeking_start) and (magnitudes[i] < thresh):
            seeking_start = True
            stop_indices.append(i)
    if start_indices[-1] > stop_indices[-1]:
        del start_indices[-1]
    if stop_indices[0] < start_indices[0]:
        del stop_indices[0]
    assert len(start_indices) == len(stop_indices)
    max_interval = np.max(np.array([stop-start for start, stop in zip(start_indices, stop_indices)]))
    i = 0
    while i < len(start_indices):
        if stop_indices[i]-start_indices[i] < .8*max_interval:
            del start_indices[i]
            del stop_indices[i]
        else:
            i += 1
    
    mean_jump = np.mean((np.mean([s2-s1 for s1, s2 in zip(start_indices[:-1], start_indices[1:])]),
                         np.mean([s2-s1 for s1, s2 in zip(stop_indices[:-1], stop_indices[1:])])))
    mean_duration = np.mean([stop-start for start, stop in zip(start_indices, stop_indices)])
    midpoints = [start_indices[0]+int(.5*mean_duration)+i*int(mean_jump) for i in range(len(start_indices))]
    intervals = []
    for midpoint in midpoints:
        intervals.append((midpoint-int(.25*mean_duration), midpoint+int(.25*mean_duration)))
##


I = []
Q = []
for interval in intervals:
    I.append(I_raw[interval[0]:interval[1]])
    Q.append(Q_raw[interval[0]:interval[1]])

plt.figure(figsize=figsize)
colors = cm.get_cmap('tab20', len(intervals))
colors = [colors(i/(len(intervals))) for i in range(len(intervals))]
lim = 1.1*np.sqrt(np.max(magnitudes[start_indices[0]:stop_indices[-1]]))
for interval, color in zip(intervals, colors):
    plt.axvspan(interval[0], interval[1], alpha=.5, color=color)
plt.plot(range(n), I_raw, '.', markersize=msize, color='red', label='I')
plt.plot(range(n), Q_raw, '.', markersize=msize, color='blue', label='Q')
plt.xlim([np.max((0, start_indices[0]-50)), np.min((n-1, stop_indices[-1]+50))])
plt.ylim([-lim, lim])
plt.title('I/Q values vs. measurement number')
plt.xlabel('Measurement number')
plt.ylabel('I/Q value')
plt.legend()

plt.figure(figsize=figsize)
for i, q, color in zip(I, Q, colors):
    plt.plot(i, q, '.', markersize=msize, color=color)
plt.xlabel('I')
plt.ylabel('Q')
plt.title('Q vs. I')
plt.gca().set_aspect('equal', adjustable='box')
plt.xlim([-lim, lim])
plt.ylim([-lim, lim])

phases = [np.unwrap(np.arctan2(q, i)) for i, q in zip(I, Q)]
lsrls = [(linregress(range(len(phase)), phase)) for phase in phases]
plt.figure(figsize=figsize)
for phase, (m, b, _, _, _), color in zip(phases, lsrls, colors):
    plt.plot(range(len(phase)), phase, '.', markersize=msize, color=color)
    plt.plot(range(len(phase)), m*np.arange(len(phase))+b, '--', color=color)
plt.xlabel('Measurement number')
plt.ylabel('Phase (radians/pi)')
plt.title('Unwrapped phases of IQ measurements, vs. measurement number')

plt.figure(figsize=figsize)
intercepts = [lsrl[1] for lsrl in lsrls]
m = np.mean([lsrl[0] for lsrl in lsrls])
expected = [np.mod(intercepts[0]+m*i*int(mean_jump)+np.pi, 2*np.pi)-np.pi for i in range(len(intercepts))]

plt.plot(range(len(intercepts)), intercepts, '.', color='blue', markersize=msize)
plt.plot(range(len(expected)), expected, '--', color='red')
plt.xlabel('Group of samples')
plt.ylabel('Phase (radians/pi)')
plt.title('Intercept of least-squares regression lines of unwrapped phases')