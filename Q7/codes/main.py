import ctypes
import numpy as np
import matplotlib.pyplot as plt

#Load the shared object file
temp = ctypes.CDLL('./func.so')

temp.function.argtypes = [ctypes.c_double]
temp.function.restype = ctypes.c_double

temp.derivative.argtypes = [ctypes.c_double]
temp.derivative.restype = ctypes.c_double

temp.NewtonRaphson.argtypes = [ctypes.c_double]
temp.NewtonRaphson.restype = ctypes.c_double

initGuess1 = float(input('Guess an initial value: '))
root1 = temp.NewtonRaphson(initGuess1)
print(root1)

initGuess2 = float(input('Guess an initial value: '))
root2 = temp.NewtonRaphson(initGuess2)
print(root2)

x = [0]
y = [2]
j = 0
i = 0
h = 0.01
while x[j] < 3 :
    x.append(i)
    y.append(temp.function(i))
    i += h
    j += 1

plt.plot(x, y, color='red', label='$y = x^2 - 3x + 2$')
plt.scatter(root1, temp.function(root1), label='Root1 at x=1')
plt.scatter(root2, temp.function(root2), label='Root2 ar x=2')
plt.axhline(0, color='black', linewidth=0.2)
plt.axvline(0, color='black', linewidth=0.2)
plt.legend()
plt.xlabel('X-AXIS')
plt.ylabel('Y-AXIS')
plt.savefig('../figs/fig.png')
plt.show()


