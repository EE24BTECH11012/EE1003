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
