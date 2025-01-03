#Calling C function in Python
from ctypes import *
import matplotlib.pyplot as plt

#load the shared object file
temp = CDLL('./func.so')

function = temp.function
function.restype = c_double
derivative = temp.derivative
derivative.restype = c_double
pointGen = temp.pointGen

filename = 'values.txt'
x_values = []
y_values = []

with open(filename, 'r') as file :
    data = file.readlines()

for elem in data :
    x_values.append(float(elem.split()[0]))
    y_values.append(float(elem.split()[1]))

plt.plot(x_values, y_values, color='red', linestyle=':', label='$y = e^x$')
plt.xlabel('X-AXIS')
plt.ylabel('Y-AXIS')
plt.grid()
plt.legend()
plt.savefig('fig.png')
plt.show()
