#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_TRIALS 10000

char toss_coin()
{
	char outcomes[2] = {'H', 'T'} ;
	return outcomes[rand() %2] ;
}

void prob()
{
	int count_heads = 0 ;	
	for ( int i=0; i < NUM_TRIALS; i++ )
	{
		int count = 0 ;
		char poss[3] = {toss_coin(), toss_coin(), toss_coin()} ;
		for ( int j=0; j<3; j++ )
		{	
			if (poss[i] == 'H')
			{
				count += 1 ;
			}
		}
		if ( count == 0 )
		{
			count_heads += 1 ;
		}
	}
	double probability = (double) count_heads/(8*NUM_TRIALS) ;
	printf("Simulated probability: %.4lf\n", probability) ;
	printf("Theoritical probability: 0.1250\n") ;
	//return probability ;
}
