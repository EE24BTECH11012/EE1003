#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define h 0.01

double function(double x)
{
	return 5-x ;
}

void pointGen(double x0, double *x, double *y, int num)
{
	for ( int i=0; i<num; i++ )
	{
		x[i] = x0 ;
		y[i] = function(x[i]) ;
		x0 += h ;
	}
}

#include <stdio.h>
#include <stdlib.h>

// LU Decomposition function
void lu_decomposition(double* A, double* L, double* U, int n) {
    // Initialize L and U with zeros
    for (int i = 0; i < n * n; i++) {
        L[i] = 0.0;
        U[i] = 0.0;
    }

    for (int i = 0; i < n; i++) {
        // Upper Triangular
        for (int k = i; k < n; k++) {
            double sum = 0.0;
            for (int j = 0; j < i; j++) {
                sum += L[i * n + j] * U[j * n + k];
            }
            U[i * n + k] = A[i * n + k] - sum;
        }

        // Lower Triangular
        for (int k = i; k < n; k++) {
            if (i == k) {
                L[i * n + i] = 1.0;  // Diagonal as 1
            } else {
                double sum = 0.0;
                for (int j = 0; j < i; j++) {
                    sum += L[k * n + j] * U[j * n + i];
                }
                L[k * n + i] = (A[k * n + i] - sum) / U[i * n + i];
            }
        }
    }
}

