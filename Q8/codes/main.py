import ctypes
import numpy as np
import matplotlib.pyplot as plt

# Load shared object files
temp = ctypes.CDLL('./func.so')

# Define function signatures for func.so
temp.function.argtypes = [ctypes.c_double]
temp.function.restype = ctypes.c_double

temp.pointGen.argtypes = [ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)]
temp.pointGen.restype = None

x0 = -2
num_points = 1000
x = (ctypes.c_double * num_points )()
y = (ctypes.c_double * num_points )()

temp.pointGen(x0, x, y, num_points)

x = np.array(x)
y = np.array(y)

plt.plot(x, y, color='red',linestyle='-', label='$x+y=5$')
plt.plot(x, y, color='blue',linestyle='-.', label ='$2x+2y=10$')
plt.axhline(0, color='black', linewidth=0.5)
plt.axvline(0, color='black', linewidth=0.5)
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.legend()
plt.savefig('../figs/fig.png')
plt.show()

