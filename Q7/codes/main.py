import ctypes
import numpy as np
import matplotlib.pyplot as plt

# Load shared object files
temp = ctypes.CDLL('./func.so')

# Define function signatures for func.so
temp.function.argtypes = [ctypes.c_double]
temp.function.restype = ctypes.c_double

temp.derivative.argtypes = [ctypes.c_double]
temp.derivative.restype = ctypes.c_double

temp.NewtonRaphson.argtypes = [ctypes.c_double]
temp.NewtonRaphson.restype = ctypes.c_double

# Newton-Raphson Method
initGuess1 = float(input('Guess an initial value for root 1: '))
root1 = temp.NewtonRaphson(initGuess1)
print(f"Root 1: {root1}")

initGuess2 = float(input('Guess an initial value for root 2: '))
root2 = temp.NewtonRaphson(initGuess2)
print(f"Root 2: {root2}")

# Plotting function
x = []
y = []
i = 0
h = 0.01
while i <= 3:
    x.append(i)
    y.append(temp.function(i))
    i += h

plt.plot(x, y, color='red', label='$y = x^2 - 3x + 2$')
plt.scatter(root1, temp.function(root1), color='blue', label=f'Root 1 at x={root1:.2f}')
plt.scatter(root2, temp.function(root2), color='green', label=f'Root 2 at x={root2:.2f}')
plt.axhline(0, color='black', linewidth=0.5)
plt.axvline(0, color='black', linewidth=0.5)
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.legend()
plt.title("Newton-Raphson Root Finding")
plt.savefig('../figs/fig.png')
plt.show()

