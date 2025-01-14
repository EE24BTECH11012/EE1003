#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define h 0.01
#define tol 1e-6

double function(double x)
{
	return x*x - 3*x + 2 ;
}

double derivative(double x)
{
	return function(x+h)/h - function(x)/h ;
}

double NewtonRaphson(double x0)
{
	double x1 = x0 - function(x0)/derivative(x0) ;
	while (fabs(x1-x0) >= tol )
	{
		x0 = x1 ;
		x1 = x0 - function(x0)/derivative(x0) ;
	}
	return x1 ;
}
