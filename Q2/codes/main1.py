#Calling C function in Python
from ctypes import *
import matplotlib.pyplot as plt
import numpy as np

#load the shared object file
temp = CDLL('./func.so')

#function = temp.function
#function.restype = c_double
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

filename = 'values1.txt'
x_values1 = []
y_values1 = []

with open(filename, 'r') as file :
    #reads from the file
    data = file.readlines()

for elem in data :
    #append values to the list
    x_values1.append(float(elem.split()[0]))
    y_values1.append(float(elem.split()[1]))


#Plotting the theoritical solution using the function obtained from Laplace transform
x_func = [-10.0]
y_func = [10.0]
h = 0.01
for i in range(2000) :
    x_func.append(x_func[i] + h)
    y_func.append(y_func[i] + ((x_func[i]**3 - 3*x_func[i]*y_func[i]*y_func[i])/(y_func[i]**3 - 3*x_func[i]*x_func[i]*y_func[i]))*h)

x_func2 = [-10.0]
y_func2 = [-10.0]
h = 0.01
for i in range(2000) :
    x_func2.append(x_func2[i] + h)
    y_func2.append(y_func2[i] + ((x_func2[i]**3 - 3*x_func2[i]*y_func2[i]*y_func2[i])/(y_func2[i]**3 - 3*x_func2[i]*x_func2[i]*y_func2[i]))*h)
    
plt.plot(x_values, y_values, color='blue', linestyle='-', label='sim') #Computational graph
plt.plot(x_values1, y_values1, color='blue', linestyle='-') 
plt.plot(x_func, y_func, color='green', linestyle='-.', label='theory') #Theoritical graph
plt.plot(x_func2, y_func2, color='green', linestyle='-.')
plt.xlabel('X-AXIS')
plt.ylabel('Y-AXIS')
plt.grid()
plt.legend()
#plt.savefig('fig.png')
plt.show()
