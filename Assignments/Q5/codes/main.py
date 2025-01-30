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
temp.gradient_descent.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_double, ctypes.c_int]

# Parameters
start = -2.0
end = 2.0
h = 0.01

x0 = 10
alpha = 0.01

# Correct number of points
num_points = int((end - start) / h)

# Allocate memory for x and y arrays
x = (ctypes.c_double * num_points)()
y = (ctypes.c_double * num_points)()

# Allocate memory for minimum and maximum results
absmin = (ctypes.c_double * 2)()
absmax = (ctypes.c_double * 2)()

# Generate points for the function plot
temp.pointGen(start, end, x, y)

# Perform Gradient Descent to find the minimum
temp.gradient_descent(x0, alpha, absmin, start, 1)

# Perform Gradient Ascent to find the maximum
temp.gradient_descent(x0, -alpha, absmax, end, 2)

# Convert C arrays to NumPy arrays for plotting
x = np.array(x)
y = np.array(y)

# Print the results
print(f"Minimum at (x = {absmin[0]}, f(x) = {absmin[1]})")
print(f"Maximum at (x = {absmax[0]}, f(x) = {absmax[1]})")

# Plot the function and the found extremum points
plt.plot(x, y, color='red', label='Objective function $f(x) = x^3$')
plt.scatter(absmin[0], absmin[1], color='blue', label='Minimum')
plt.scatter(absmax[0], absmax[1], color='green', label='Maximum')
plt.xlabel('X-AXIS')
plt.ylabel('Y-AXIS')
plt.grid()
plt.legend()
plt.savefig('../figs/fig.png')
plt.show()

