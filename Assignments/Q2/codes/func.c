#include <stdio.h>
#include <math.h>
#define E 2.718281828459045
#define H 0.01

double derivative(double x, double y)
{
	//Definition from first principle
	return  (pow(x,3) - 3*x*y*y)/(pow(y,3) - 3*x*x*y) ;
}

void pointGen(double x, double y, int k )
{
	FILE *ptr ;
	if ( k == 1 ) {
	ptr = fopen("values.txt", "w") ; //Opens the file in write mode
	if ( ptr == NULL )
	{
		printf("File doesn't exist") ;
		return ;
	}
	for ( int i=0; i<=2000; i++ )
	{
		fprintf(ptr, "%lf %lf\n", x, y ) ; //prints in the file
		x += H ;
		y += derivative(x, y)*H ;
	}


	fclose(ptr) ; //closes file
	}
	else
	{
		ptr = fopen("values1.txt", "w") ; //Opens the file in write mode
	if ( ptr == NULL )
	{
		printf("File doesn't exist") ;
		return ;
	}
	for ( int i=0; i<=2000; i++ )
	{
		fprintf(ptr, "%lf %lf\n", x, y ) ; //prints in the file
		x += H ;
		y += derivative(x, y)*H ;
	}


	fclose(ptr) ; //closes file
	}
	printf("The values have been printed in the file\n") ;
}


