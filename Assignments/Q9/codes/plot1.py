import numpy as np
import matplotlib.pyplot as plt

def pmf(x) :
    if x == 0 :
        return 0.125
    elif x == 1 :
        return 0.375
    elif x == 2 :
        return 0.375
    elif x == 3 :
        return 0.125
    else :
        return 0.0

x = [-0.5,0,0.5,1,1.5,2,2.5,3,3.5]
y = [pmf(i) for i in x]

x = np.array(x)
y = np.array(y)

plt.ylim(-0.01,1)
plt.stem(x, y, basefmt=" ", linefmt='b-', markerfmt='bo', use_line_collection=True)
plt.xlabel('Random variable')
plt.ylabel('Probability')
plt.grid()
plt.savefig('../figs/fig1.png')
plt.show()
