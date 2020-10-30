# -*- coding: utf-8 -*-
"""
Created on Tue Oct 27 08:48:09 2020

@author: jgamm
"""

from matplotlib import pyplot as plt
import numpy as np
import serial
import pickle

TX = serial.open('COM3')
RX = serial.open('COM8')


while True:
    serial.write(b't\n')