import ctypes
import numpy as np
import matplotlib.pyplot as plt

# Load shared object files
temp = ctypes.CDLL('./func.so')

# Define function signatures for func.so
temp.toss_coin.argtypes = None
temp.factorial.argtypes = [ctypes.c_int]
temp.factorial.restype = ctypes.c_int
temp.pmf_sim.argtypes = [ctypes.c_int]
temp.pmf_sim.restype = ctypes.c_double
temp.pmf_theory.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_double]
temp.pmf_theory.restype = ctypes.c_double
temp.cdf_sim.argtypes = [ctypes.c_int]
temp.cdf_sim.restype = ctypes.c_double
temp.cdf_theory.argtypes = [ctypes.c_int, ctypes.c_double, ctypes.c_int]
temp.cdf_theory.restype = ctypes.c_double

n = 3
p = 1/2

x = np.array([0, 1, 2, 3])
pmfsim_y = np.array([temp.pmf_sim(i) for i in x])
pmftheory_y = np.array([temp.pmf_theory(n, i, p) for i in x])
cdftheory_y = np.array([temp.cdf_theory(n,p,i) for i in x])
cdfsim_y = np.array([temp.cdf_sim(i) for i in x])

#Plot PMF
plt.figure(figsize=(8, 6))
markerline, stemlines, baseline = plt.stem(x, pmftheory_y, label="theory")
plt.setp(markerline, 'markerfacecolor', 'green')
plt.setp(stemlines, 'color', 'blue')
plt.setp(baseline, 'color', 'gray', 'linewidth', 1)
markerline, stemlines, baseline = plt.stem(x, pmfsim_y, label="sim")
plt.setp(markerline, 'markerfacecolor', 'red')
plt.setp(stemlines, 'color', 'red')
plt.setp(baseline, 'color', 'gray', 'linewidth', 1)
plt.xlabel('x')
plt.ylabel('$p_X(k)$')
plt.legend(loc='best')
plt.grid(True)
plt.savefig("../figs/pmf.png")
plt.show()

#Plot CDF
plt.figure(figsize=(8, 6))
markerline, stemlines, baseline = plt.stem(x, cdftheory_y, label="theory")
plt.setp(markerline, 'markerfacecolor', 'green')
plt.setp(stemlines, 'color', 'blue')
plt.setp(baseline, 'color', 'gray', 'linewidth', 1)
markerline, stemlines, baseline = plt.stem(x, cdfsim_y, label="sim")
plt.setp(markerline, 'markerfacecolor', 'red')
plt.setp(stemlines, 'color', 'red')
plt.setp(baseline, 'color', 'gray', 'linewidth', 1)
plt.xlabel('x')
plt.ylabel('$F_X(k)$')
plt.legend(loc='best')
plt.grid(True)
plt.savefig("../figs/cdf.png")
plt.show()







