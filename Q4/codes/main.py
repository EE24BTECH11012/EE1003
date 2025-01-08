import numpy as np
import matplotlib.pyplot as plt
import ctypes
#Load the shared object file
link = ctypes.CDLL('./func.so')

link.pointGen.argtypes = [ctypes.c_double, ctypes.c_double]
link.pointGen(2.0,0.0)

x0 = [2]
y0 = [0]
h = 0.01
xt = [2]
yt = [0]

filename = 'values.txt'



with open(filename, 'r') as file :
    data = file.readlines()

for i in range(len(data)-1) :
    xt.append(float(data[i].split()[0]))
    yt.append(float(data[i].split()[1]))
    #xt.append(xt[i] + h)
    #yt.append(yt[i] + (xt[i+1] - xt[i])*(func(xt[i]) + func(xt[i+1]))/2)

for i in range(len(data)-1) :
    x0.append(x0[i] + h)
    y0.append(np.log(2*np.sqrt(x0[i]*x0[i] - 1)/(np.sqrt(3)*x0[i])))


plt.plot(x0, y0, color='red', linestyle='-', label='theory')
plt.plot(xt, yt, color='blue', linestyle='-', label='sim')
# Add legend and labels
plt.legend()
plt.xlabel('X-AXIS')
plt.ylabel('Y-AXIS')
plt.grid(True)

# Save and show the plot
plt.savefig('../figs/fig.png')
plt.show()
