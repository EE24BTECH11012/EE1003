#Calling C function in Python
from ctypes import *
import matplotlib.pyplot as plt
import numpy as np

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
    #reads from the file
    data = file.readlines()

for elem in data :
    #append values to the list
    x_values.append(float(elem.split()[0]))
    y_values.append(float(elem.split()[1]))

#Plotting the theoritical solution using the function obtained from Laplace transform
x_func = [0.0]
y_func = [2.0]
h = 0.01
for i in range(1000) :
    x_func.append(x_func[i] + h)
    y_func.append(y_func[i] + (np.e**x_func[i])*h)

#Plot using z-transform
x_z = [0.0]
y_z = [2.0]
for i in range(1000) :
    x_z.append(x_z[i] + h)
    y_z.append(1 + (1+h)**i)

plt.plot(x_z, y_z, color='red', linestyle = ':', label='sim2') #Using z-transform
plt.plot(x_values, y_values, color='blue', linestyle='-', label='sim1') #Computational graph
plt.plot(x_func, y_func, color='green', linestyle='-.', label='theory') #Theoritical graph
plt.xlabel('X-AXIS')
plt.ylabel('Y-AXIS')
plt.grid()
plt.legend()
#plt.savefig('fig.png')
plt.show()
