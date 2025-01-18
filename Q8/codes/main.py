import ctypes
import numpy as np
import matplotlib.pyplot as plt

# Load shared object files
temp = ctypes.CDLL('./func.so')

# Define function signatures for func.so
temp.function.argtypes = [ctypes.c_double]
temp.function.restype = ctypes.c_double

temp.pointGen.argtypes = [ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)]
temp.pointGen.restype = None

temp.lu_decomposition.argtypes = [
    ctypes.POINTER(ctypes.c_double),  # A
    ctypes.POINTER(ctypes.c_double),  # L
    ctypes.POINTER(ctypes.c_double),  # U
    ctypes.c_int                      # n
]

def lu_decomposition_py(A):
    n = A.shape[0]
    A_flat = A.flatten().astype(np.double)
    L = np.zeros((n, n), dtype=np.double)
    U = np.zeros((n, n), dtype=np.double)

    # Convert numpy arrays to C pointers
    A_ptr = A_flat.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
    L_ptr = L.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
    U_ptr = U.ctypes.data_as(ctypes.POINTER(ctypes.c_double))

    # Call the C function
    temp.lu_decomposition(A_ptr, L_ptr, U_ptr, n)

    return L, U

# Example usage
if __name__ == "__main__":
    A = np.array([[1, 1], [2, 2]], dtype=np.double)
    L, U = lu_decomposition_py(A)
    print("L:\n", L)
    print("U:\n", U)
    # Solving AX = B
    B = np.array([[5], [10]], dtype=np.double)
    Y = np.linalg.solve(L, B)  # Forward substitution
    X = np.linalg.solve(U, Y)  # Backward substitution
    print("Solution X:\n", X)

print(P)
x0 = -2
num_points = 1000
x = (ctypes.c_double * num_points )()
y = (ctypes.c_double * num_points )()

temp.pointGen(x0, x, y, num_points)

x = np.array(x)
y = np.array(y)

plt.plot(x, y, color='red',linestyle='-', label='$x+y=5$')
plt.plot(x, y, color='blue',linestyle='-.', label ='$2x+2y=10$')
plt.axhline(0, color='black', linewidth=0.5)
plt.axvline(0, color='black', linewidth=0.5)
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.legend()
plt.savefig('../figs/fig.png')
plt.show()

