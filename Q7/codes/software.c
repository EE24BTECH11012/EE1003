#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <time.h>


void matrix_mult(complex double** A, complex double** B, complex double** C, int n) 
{
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < n; j++) 
        {
            C[i][j] = 0.0;
            for (int k = 0; k < n; k++) 
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void caseCheck(complex double **temp, complex double *eigenvalues, int n) {
	int k=0 ;
        // To handle the special case when some of the off-diagonal elements are not zero.
        // This can typically be the case when the matrix has real elements, but has complex eigenvalues that are conjugates of each other.
        while (k < n) {
            if ((k < n - 1) && (cabs(temp[k + 1][k]) > 1e-10)) { 
               // In such cases, the eigenvalues of the 2 by 2 block matrix becomes the eigenvalues of the matrix, at that position.
                double complex x1 = temp[k][k];
                double complex x2 = temp[k + 1][k];
                double complex y1 = temp[k][k + 1];
                double complex y2 = temp[k + 1][k + 1];

                // Solve quadratic equation: x^2 - (x1 + y2)x + (x1*y2 - x2*y1) = 0
                double complex b = -1.0 * (x1 + y2);
                double complex c = (x1 * y2 - x2 * y1);
                
                eigenvalues[k] = (-1.0*b + csqrt(b*b - 4.0*c)) / 2.0 ;
                eigenvalues[k+1]= (-1.0*b - csqrt(b*b - 4.0*c)) / 2.0 ;
                temp[k + 1][k] = 0;

                // Skip to next block
                k += 2;
            } 
	    else 
	    {
                // For single eigenvalue case (diagonal entry)
		eigenvalues[k] = temp[k][k] ;
                k++;
            }

    }
}

void gram_schmidt(complex double** A, complex double** Q, complex double** R, int n) {
    for (int j = 0; j < n; j++) 
    {
        // extract vector v from the columns of A
        for (int i = 0; i < n; i++) 
        {
            Q[i][j] = A[i][j];
        }

        // Gram-Schmidt process to create orthogonal vectors
        for (int k = 0; k < j; k++) 
        {
            // Find an orthogonal vector to the given vector
            complex double dot_product = 0.0;
            for (int i = 0; i < n; i++) 
            {
                dot_product += conj(Q[i][k]) * Q[i][j];
            }
            R[k][j] = dot_product;

            for (int i = 0; i < n; i++) 
            {
                Q[i][j] -= R[k][j] * Q[i][k];
            }
        }

        // Normalize the j-th column
        R[j][j] = 0.0;
        for (int i = 0; i < n; i++) 
        {
            R[j][j] += creal(Q[i][j])*creal(Q[i][j]) + cimag(Q[i][j])*cimag(Q[i][j]);
        }
        R[j][j] = sqrt(R[j][j]);

        for (int i = 0; i < n; i++) 
        {
            Q[i][j] /= R[j][j];
        }
    }
}

void qr_decomposition(complex double** A, double complex* eigenvalues, int n) 
{
    complex double** Q = (complex double**)malloc(n * sizeof(complex double*));
    complex double** R = (complex double**)malloc(n * sizeof(complex double*));
    complex double** temp = (complex double**)malloc(n * sizeof(complex double*));
    

    for (int i = 0; i < n; i++) 
    {
        Q[i] = (complex double*)malloc(n * sizeof(complex double));
        R[i] = (complex double*)malloc(n * sizeof(complex double));
        temp[i] = (complex double*)malloc(n * sizeof(complex double));
    }
    
    for (int i = 0; i < 1000; i++) 
    {
        gram_schmidt(A, Q, R, n);
        matrix_mult(R, Q, temp, n);
		
       for (int i = 0; i < n; i++) 
       {
          for (int j = 0; j < n; j++) 
          {
              A[i][j] = temp[i][j];
	  }
    }
    }

    caseCheck(A, eigenvalues, n) ;
    for (int i = 0; i < n; i++) {
        free(Q[i]);
        free(R[i]);
        free(temp[i]);
    }
    free(Q);
    free(R);
    free(temp);
}


int main() 
{
    
    int m, n ;
    printf("Enter the size of matrix: ( m n ) ") ;
    scanf("%d %d", &m, &n) ;
    if ( m != n )
    {
      // Eigenvalues can be calculated only for square matrices.
      printf("The matrix entered is not square. Eigenvalues can't be calculated\n") ;
      return 0 ;
    }
    complex double** A = (complex double**)malloc(n * sizeof(complex double*));
    for (int i = 0; i < n; i++) 
    {
        A[i] = (complex double*)malloc(n * sizeof(complex double));
    }
    printf("Enter the elements row-wise ( real imag ): \n") ;
    for ( int i=0; i<n; i++)
    {
        for ( int j=0; j<n; j++)
        {
            double real, imag ;
            fscanf(stdin, "%lf %lf", &real, &imag) ;
            A[i][j] = real + imag*I ;
        }
    }
    clock_t start = clock() ;
    int countS = 0 ;
    for ( int i=0; i<n; i++)
    {
        for ( int j=i; j<n; j++)
        {
            if ( creal(A[i][j]) == creal(A[j][i]) && cimag(A[i][j]) == cimag(A[j][i]))
            {
                countS += 1 ;
            }
        }
    }
    if ( countS == n*(n+1)/2 )
    {
        // Prints if the matrix is symmetric
        printf("The matrix entered is a symmetric matrix.\n") ;
    }

    double complex* eigenvalues = (double complex*)malloc(n * sizeof(double complex));

    qr_decomposition(A, eigenvalues, n);

    printf("Eigenvalues:\n");
    for (int i = 0; i < n; i++) 
    {
        
        printf("%.20lf + %.20lfi\n", creal(eigenvalues[i]), cimag(eigenvalues[i]));
    }
    clock_t end = clock() ;
    printf("Duration of code run: %.10f\n", (double)(end-start)/CLOCKS_PER_SEC ) ;

    // Free allocated memory
    for (int i = 0; i < n; i++) 
    {
        free(A[i]);
    }
    free(A);
    free(eigenvalues);

    return 0;
}

