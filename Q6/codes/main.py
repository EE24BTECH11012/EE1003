import matplotlib.pyplot as plt
import numpy as np
import ctypes

# Load the shared object file
temp = ctypes.CDLL('./func.so')

# Define argument and return types for C functions
temp.function.argtypes = [ctypes.c_double]
temp.function.restype = ctypes.c_double

temp.derivative.argtypes = [ctypes.c_double]
temp.derivative.restype = ctypes.c_double

temp.pointGen.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)]
temp.gradient_descent.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_double]

# Parameters
start = 0.1
end = 3.0
h = 0.001

x0 = 1
alpha = 0.001

# Correct number of points
num_points = int((end - start) / h)

# Allocate memory for x and y arrays
x = (ctypes.c_double * num_points)()
y = (ctypes.c_double * num_points)()

# Allocate memory for maximum results
absmax = (ctypes.c_double * 2)()

# Generate points for the function plot
temp.pointGen(start, end, x, y)

# Perform Gradient Ascent to find the maximum
temp.gradient_descent(x0, alpha, absmax, end, 2)

# Convert C arrays to NumPy arrays for plotting
x = np.array(x)
y = np.array(y)

# Print the results
print(f"Maximum at (x = {absmax[0]}, f(x) = {absmax[1]})")
# Plot the function and the found extremum points
plt.plot(x, y, color='red', label=r'Objective function $\frac{\log{x}}{x}$')
plt.scatter(absmax[0], absmax[1], color='green', label='Maximum')
plt.xlabel('X-AXIS')
plt.ylabel('Y-AXIS')
plt.grid()
plt.legend()
plt.savefig('../figs/fig.png')
plt.show()

