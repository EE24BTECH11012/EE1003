import numpy as np
import matplotlib.pyplot as plt

# Define the CDF function
def cdf(x):
    if x < 0:
        return 0
    elif 0 <= x < 1:
        return 0.125
    elif 1 <= x < 2:
        return 0.5
    elif 2 <= x < 3:
        return 0.875
    else:
        return 1

# Generate x values from -0.5 to 3.5 with step size 0.1
x = []
j = -0.5
while j <= 3.5:
    x.append(j)
    j += 0.001

x = np.array(x)
y = np.array([cdf(i) for i in x])  # Corrected function call

# Plotting the CDF
plt.plot(x, y, color='red', linestyle='-', label='CDF')
plt.xlabel('Random variable')
plt.ylabel('Probability')
plt.grid()
plt.savefig('../figs/plot2.png')
plt.legend()
plt.show()

