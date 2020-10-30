#%%
# -*- coding: utf-8 -*-

from radio_interface import Radio
import pickle
import os
import datetime

TX = Radio(r'COM3')
RX = Radio(r'COM8')

TX.configure_transmitter(450, 100, 0)
RX.configure_receiver(450, 5000, 0)
(I, Q) = RX.read_output()

del TX
del RX

dt = datetime.datetime.now()
filename = r'efr32bg22IQ_%d-%d-%d_%d-%d-%d.pickle'\
        %(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second)
with open(os.path.join(os.getcwd(), '../data', filename), 'wb') as F:
    pickle.dump((I, Q), F)
