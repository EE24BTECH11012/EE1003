#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUM_TRIALS 10000

// Function to simulate a fair coin toss (returns 'H' or 'T')
char toss_coin() {
    char outcomes[2] = {'H', 'T'};
    return outcomes[rand() % 2];
}

void prob() {
    FILE *ptr = fopen("values.dat", "w");
    if (ptr == NULL) {
        printf("Error: Unable to open values.dat\n");
        return;
    }

    srand(time(NULL));  // Seed the random number generator

    int k = 1;
    while (k <= NUM_TRIALS) {  // Ensure last value is written at MAX_ITER
        int count_all_tails = 0;

        for (int i = 0; i < k; i++) {
            int count = 0;
            char poss[3] = {toss_coin(), toss_coin(), toss_coin()};

            for (int j = 0; j < 3; j++) {
                if (poss[j] == 'H') {
                    count++;
                }
            }

            if (count == 0) {  // Count trials with zero heads
                count_all_tails++;
            }
        }

        double probability = (double)count_all_tails /k;
        fprintf(ptr, "%d %.6lf\n", k, probability);  // Ensure consistent formatting

        k += 5;
    }

    fclose(ptr);  // Close the file properly
    printf("Values are successfully written to values.dat\n");
}

