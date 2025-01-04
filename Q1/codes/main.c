#include <stdio.h>
#include <math.h>
#include "func.c"

int main()
{
	//printf("Enter the initial conditions: ") ;
	double x0 = 0.0, y0 = 2.0 ;
	//scanf("%lf %lf", &x0, &y0 ) ;
	double h = 0.01 ;
	pointGen(x0, y0) ; //call the function from func.c
	//printf("Success\n") ;
	return 0 ;
}
