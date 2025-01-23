import ctypes
import numpy as np
import matplotlib.pyplot as plt

# Load shared object files
temp = ctypes.CDLL('./func.so')

# Define function signatures for func.so
temp.toss_coin.argtypes = None

temp.prob.argtypes = None

temp.prob()


