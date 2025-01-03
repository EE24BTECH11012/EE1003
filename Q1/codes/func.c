#include <stdio.h>
#include <math.h>
#define E 2.718281828459045
#define H 0.01

double function(double x)
{
	return pow(E,x) + 1 ;
}

double derivative(double x)
{
	return function(x+H)/H - function(x)/H ;
}

void pointGen(double x, double y )
{
	FILE *ptr ;
	ptr = fopen("values.txt", "w") ;
	if ( ptr == NULL )
	{
		printf("File doesn't exist") ;
		return ;
	}
	for ( int i=0; i<=1000; i++ )
	{
		fprintf(ptr, "%lf %lf\n", x, y ) ;
		x += H ;
		y += derivative(x)*H ;
	}


	fclose(ptr) ;
	printf("The values have been printed in the file\n") ;
}


