import ctypes
import matplotlib.pyplot as plt
import numpy as np

# Load the shared object file
link = ctypes.CDLL('./func.so')

link.area.argtypes = [ctypes.c_double]
link.area.restype = ctypes.c_double
link.pointGen.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_int]

# Function to calculate area
def calculate_Area(n):
    if n <= 1:
        return False
    return link.area(n)

link.pointGen(0, 0, 1)
link.pointGen(0, 0, 2)
a = calculate_Area(float(input('Enter a value of n: ')))  # Takes input of an n value
print(a)  # Prints area

# Read data from the first file
filename = 'values1.txt'
x_1 = []
y_1 = []

with open(filename, 'r') as file:
    # Reads from the file
    data = file.readlines()

for elem in data:
    # Append values to the list
    x_1.append(float(elem.split()[0]))
    y_1.append(float(elem.split()[1]))

# Read data from the second file
filename = 'values2.txt'
x_2 = []
y_2 = []

with open(filename, 'r') as file:
    # Reads from the file
    data = file.readlines()

for elem in data:
    # Append values to the list
    x_2.append(float(elem.split()[0]))
    y_2.append(float(elem.split()[1]))

x_1 = np.array(x_1)
x_2 = np.array(x_2)
y_1 = np.array(y_1)
y_2 = np.array(y_2)
# Plot the curves
plt.plot(x_1, y_1, color='red', linestyle='-', label='$y=x^2$')
plt.plot(x_2, y_2, color='blue', linestyle='-', label='$y=x$')

# Shade the area between the curves
plt.fill_between(x_1, y_1, y_2, where=(y_1 < y_2), color='gray', alpha=0.5, label='Shaded Area')

plt.axhline(0, color='black', linewidth=0.5)  # Thicker x-axis
plt.axvline(0, color='black', linewidth=0.5)  # Thicker y-axis
# Add legend and labels
plt.legend()
plt.xlabel('X-AXIS')
plt.ylabel('Y-AXIS')
plt.grid(True)

# Save and show the plot
plt.savefig('../figs/fig.png')
plt.show()

