#include <stdio.h>
#include <math.h>
#define h 0.01

double area(double n)
{
	double sum = 0.0 ;
	for ( int i=1; i < n; i++ )
	{
		sum += i - (i*i)/n ;
	}
	sum /= n*n/2 ;
	return sum ;
}

void pointGen(double x, double y, int k)
{
	FILE *ptr ;
	if ( k == 1 )
	{
		ptr = fopen("values1.txt", "w") ;
		for ( double i=-1; i<2; i+=h )
		{
			fprintf(ptr, "%lf %lf\n", i, i*i) ;
		}
		fclose(ptr) ;
	}
	else if ( k == 2 )
	{
		ptr = fopen("values2.txt", "w") ;
		for ( double i=-1; i<2; i+=h )
		{
			fprintf(ptr, "%lf %lf\n", i, i) ;
		}
		fclose(ptr) ;
	}
	else
	{
		printf("Invalid indexing") ;
	}
}

