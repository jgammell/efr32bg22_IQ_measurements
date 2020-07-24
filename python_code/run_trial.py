#%%
# -*- coding: utf-8 -*-

from radio_interface import Radio
import re
import pickle
import datetime
import os

def set_state_tone(radio):
    radio.write_char(b't')
def set_state_idle(radio):
    radio.write_char(b'i')
def set_state_receive(radio):
    radio.write_char(b'r')
def read_iq_data(radio):
    while not radio.read_available(): pass
    num_samples = int(radio.readline()[:-1])
    sample_buffer = []
    for i in range(num_samples):
        sample = radio.readline()
        sample_buffer.append(sample)
    return sample_buffer
def parse_raw_output(output):
    I = []
    Q = []
    for s, i in zip(output, range(len(output))):
        vals = re.findall(r'[+-]?\d+', s.decode('ascii'))
        I.append(int(vals[0]))
        Q.append(int(vals[1]))
    return (I, Q)
def save_data(data):
    dt = datetime.datetime.now()
    filename = r'efr32bg22IQ_%d-%d-%d_%d-%d-%d.pickle'\
                %(dt.year, dt.month, dt.hour, dt.day, dt.minute, dt.second)
    with open(os.path.join(os.getcwd(), 'data', filename), 'wb') as F:
        pickle.dump(data, F)
    return filename
    

try:
    tx = Radio(r'COM8')
    rx = Radio(r'COM3')
    set_state_tone(tx)
    set_state_receive(rx)
    raw_output = read_iq_data(rx)
    set_state_idle(tx)
    parsed_data = parse_raw_output(raw_output)
    data_filename = save_data(parsed_data)
finally:
    del tx
    del rx