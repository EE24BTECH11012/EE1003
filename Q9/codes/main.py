import ctypes
import numpy as np
import matplotlib.pyplot as plt

# Load shared object files
temp = ctypes.CDLL('./func.so')

# Define function signatures for func.so
temp.toss_coin.argtypes = None

temp.prob.argtypes = None


#temp.rand_gen()
temp.prob()

x = []
y = []
with open("values.dat", "r") as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 2:  # Ensure valid rows with 2 columns
            x.append(float(parts[0]))
            y.append(float(parts[1]))
        else:
            print(f"Skipping malformed line: {line.strip()}")

x = np.array(x)
y = np.array(y)

xt = [i for i in range(10000)]
yt = [1/8 for i in range(10000)]
plt.ylim(0,1)
plt.plot(x,y, label="Sim", lw=0.5)
plt.plot(xt, yt, color='red', linestyle='-')
plt.xlabel('No of iterations')
plt.ylabel('Probability')
plt.legend()
plt.savefig('../figs/sim.png')
plt.show()




