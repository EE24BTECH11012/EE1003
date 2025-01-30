#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUM_TRIALS 100000
#define no_of_tosses 3

// Function to simulate a fair coin toss (returns 'H' or 'T')
char toss_coin() {
    char outcomes[2] = {'H', 'T'};
    return outcomes[rand() % 2];
}

int factorial(int n)
{
	if ( n < 0 )
	{
		printf("Invalid\n") ;
		return -1 ;
	}
	else if ( n == 0 || n == 1 )
	{
		return 1 ;
	}
	int prod = 1 ;
	for ( int i=n; i>1; i-- )
	{
		prod *= i ;
	}
	return prod ;
}

double pmf_theory(int n, int k, double p)
{
	return factorial(n)/(factorial(n-k)*factorial(k))*pow(1-p, n-k)*pow(p,k) ;
}

double pmf_sim(int no_of_heads) {
    srand(time(NULL));  // Seed the random number generator
    int count_req = 0 ;
    for ( int i=0; i< NUM_TRIALS; i++ )
    {
       char poss[no_of_tosses] = {toss_coin(), toss_coin(), toss_coin()} ;
       int count = 0 ;
       for ( int j=0; j<no_of_tosses; j++ )
       {
	       if ( poss[j] == 'H' )
	       {
		       count += 1 ;
	       }
       }
       if ( count == no_of_heads )
       {
	       count_req += 1 ;
       }
    }
    double probability = (double) count_req/NUM_TRIALS ;
    return probability ;
    //printf("%lf\n", probability) ;

}

double cdf_sim(int no_of_heads)
{
	double prob = 0 ;
	for ( int i=0; i<= no_of_heads; i++ )
	{
		prob += pmf_sim(i) ;
	}
	return prob ;
	//printf("%lf\n", prob) ;
}

double cdf_theory(int n, double p, int no_of_heads)
{
	double prob = 0.0 ;
	for ( int i=0; i<=no_of_heads; i++ )
	{
		prob += pmf_theory(n, i, p) ;
	}
	return prob ;
	//printf("%lf\n", prob) ;
}

