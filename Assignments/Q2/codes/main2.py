import numpy as np
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D  # For custom legend entries

# Define the grid for x and y
x = np.linspace(-1.1, 1.1, 500)
y = np.linspace(-0.5, 0.5, 500)
X, Y = np.meshgrid(x, y)

# Define the function: x^2 - y^2 - (x^2 + y^2)^2
Z = X**2 - Y**2 - (X**2 + Y**2)**2

# Plot the contour where the function equals zero
plt.figure(figsize=(8, 8))
contour = plt.contour(
    X, Y, Z, levels=[0], colors='green', linewidths=1.5, linestyles='-.'
)

# Add labels
plt.xlabel("X-axis")
plt.ylabel("Y-axis")

# Create a custom legend entry with linestyle
custom_legend = [
    Line2D([0], [0], color='green', linestyle='-.', linewidth=1.5, label="theory")
]
plt.legend(handles=custom_legend, loc="upper right")

# Add grid and style
plt.axhline(0, color='black', linewidth=0.5, linestyle='--')
plt.axvline(0, color='black', linewidth=0.5, linestyle='--')
plt.grid(alpha=0.3)
plt.gca().set_aspect('equal', adjustable='box')

# Display the plot
plt.show()

