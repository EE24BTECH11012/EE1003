#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define h 0.01       // Step size for derivative and point generation
#define tol 1e-6     // Tolerance for gradient descent convergence

// Function: f(x) = x^3
double function(double x) {
    return x * x * x;
}

// Numerical derivative: f'(x)
double derivative(double x) {
    return (function(x + h) - function(x)) / h;
}

// Generates points for plotting f(x)
void pointGen(double start, double end, double* x, double* y) {
    int i = 0;
    for (double val = start; val <= end; val += h, i++) {
        x[i] = val;           // Store x-values
        y[i] = function(x[i]); // Store corresponding y-values
    }
}

// Gradient Descent/Ascent to find local minima or maxima
void gradient_descent(double x0, double alpha, double* extremum, double par, int num) {
    double x1 = x0 - alpha * derivative(x0);  // Correct sign for descent
    while (fabs(x1 - x0) >= tol) {
        x0 = x1;
        x1 = x0 - alpha * derivative(x0);
    }
    if ( num == 1 )
    {
	extremum[0] = function(par) < function(x1) ? par : x1 ;    
    }
    else if ( num == 2 )
    {
	extremum[0] = function(par) < function(x1) ? par : x1 ;    
    }
    extremum[1] = function(extremum[0]) ;
    
}

