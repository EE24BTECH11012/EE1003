#include <stdio.h>
#include <math.h>
#define h 0.01

double func(double x)
{
	return 1/(x*(x*x - 1)) ;
}

void pointGen(double x, double y)
{
	FILE *ptr ;
	ptr = fopen("values.txt", "w") ;
	for ( double i=2; i<5; i+=h )
	{
		y += h*(func(i) + func(i+h))/2 ;
		fprintf(ptr, "%lf %lf\n", i, y) ;
	}
	printf("The values have been printed.") ;
}
