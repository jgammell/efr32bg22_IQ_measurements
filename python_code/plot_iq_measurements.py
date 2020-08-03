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

#data_filename = r'efr32bg22IQ_2020-8-11_3-35-37.pickle'
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
processed_phases = []
plt.figure(figsize=figsize)
for trial in range(len(Data)):
    I = Data[trial][0][50:]
    Q = Data[trial][1][50:]
    phases = np.unwrap(np.arctan2(Q, I))
    # Process phases
    phase_differences = [p2-p1 for p1, p2 in zip(phases[:-1], phases[1:])]
    def num_skip(p):
        p /= np.mean(phase_differences)
        if .75 <= p < 1.25:
            return 0
        if 1.75 <= p < 2.25:
            return 1
        return -1
    
    processed_phase = [phases[0]]
    for p1, p2 in zip(phases[:-1], phases[1:]):
        if num_skip(p2-p1) == -1:
            continue
        elif num_skip(p2-p1) == 0:
            processed_phase.append(p2)
        elif num_skip(p2-p1) == 1:
            processed_phase.append(.5*(p1+p2))
            processed_phase.append(p2)
        elif num_skip(p2-p1) == 2:
            processed_phase.append(p1+(1/3)*(p2-p1))
            processed_phase.append(p1+(2/3)*(p2-p1))
            processed_phase.append(p2)
    processed_phases.append(processed_phase)
    
    plt.plot(range(len(processed_phase)), processed_phase, '.', color=I_colors[trial], markersize=msize, label='Trial %d'%(trial+1))
    plt.legend()
    m, b, r, _, _ = linregress(range(len(processed_phase)), processed_phase)
    intercepts.append(b)
    print('Trial %d LSRL for unwrapped phase:'%(trial+1))
    print('\tEquation: phase=%.3fn+%.3f'%(m, b))
    print('\tr-squared=%.6f'%(r**2))
plt.title('Unwrapped IQ phase')
plt.ylabel('Radians')
plt.xlabel('Measurement number')

# Plot y-intercept of LSRL for each trial
[fig, ax] = plt.subplots(2, 1, sharex=True, figsize=figsize)
m, b, r, _, _ = linregress(range(len(intercepts)), np.unwrap(intercepts))
residuals = np.array([i-(m*x+b) for i, x in zip(np.unwrap(intercepts), range(len(intercepts)))])
ax[0].plot(range(len(intercepts)), np.unwrap(intercepts), '.', markersize=2*msize, color='blue')
ax[0].plot(np.linspace(0, len(intercepts)-1, 1000), m*np.linspace(0, len(intercepts)-1, 1000)+b, color='blue')
ax[1].plot(range(len(residuals)), residuals, '.', markersize=2*msize, color='red')
ax[1].set_ylabel('Residual (Radians)')
ax[0].set_ylabel('Y-intercept (Radians)')
ax[1].set_xlabel('Measurement number')
fig.suptitle('Y-intercept of LSRL of unwrapped phase, vs. trial number')
print('Error predicting y-intercept: %.06f'%(np.mean(np.square(residuals))))

# Plot difference between adjacent phase measurements, vs. 'time'
plt.figure(figsize=figsize)
for processed_phase, trial in zip(processed_phases, range(len(processed_phases))):
    differences = [p2-p1 for p1, p2 in zip(processed_phase[:-1], processed_phase[1:])]
    plt.plot(range(len(differences)), differences, '.', color=I_colors[trial], markersize=msize, label='Trial %d'%(trial+1))
plt.legend()
plt.title('Phase jump between samples')
plt.xlabel('Measurement number')
plt.ylabel('Jump (Radians)')


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

