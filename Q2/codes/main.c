#include <stdio.h>
#include <math.h>
#include "func.c"

int main()
{
	//printf("Enter the initial conditions: ") ;
	double x0 = -10.0, y0 = -10.0 ;
	//scanf("%lf %lf", &x0, &y0 ) ;
	double h = 0.01 ;
	pointGen(x0, y0,1) ; //call the function from func.c
	//printf("Success\n") ;	
	x0 = -10.0, y0 = 10.0 ;
	pointGen(x0, y0, 0) ;
	return 0 ;
}
