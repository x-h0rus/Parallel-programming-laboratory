#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include<omp.h>

#define RAND rand() % 100
double execTime = 0.0;

//initializes a lower triangular matrix with random numbers between 0 and 99
void init_mat_inf (int dim, float *M);
//initializes an upper triangular matrix with random numbers between 0 and 99
void init_mat_sup (int dim, float *M);
//performs matrix multiplication
void matmul (float *A, float *B, float *C, int dim);
//optimally multiplies an upper triangular matrix by a lower one
void matmul_sup (float *A, float *B, float *C, int dim);
//optimally multiplies an lower triangular matrix by a upper one
void matmul_inf ();
//write a matrix per screen
void print_mat (float *M, int dim); // TODO



/* Usage: ./matmul <dim> [block_size]*/

int main (int argc, char* argv[])
{
	int block_size = 1, dim;
	float *A, *B, *C;
    int option;

   dim = atoi (argv[1]);
   if (argc == 3) block_size = atoi (argv[2]);

	//we reserve the necessary memory size
	A = (float *) malloc(sizeof(float) * dim * dim);
	B = (float *) malloc(sizeof(float) * dim * dim);
	C = (float *) malloc(sizeof(float) * dim * dim);
	
	//initializes an upper triangular matrix "A" with random numbers between 0 and 99 and dimension "dim"
	init_mat_sup(dim, A);
	//initializes an upper triangular matrix "B" with random numbers between 0 and 99 and dimension "dim"
	init_mat_inf(dim, B);

	do
	{
		printf("What exercise do you want to do? (1 or 2): ");
		scanf("%d", &option);
	} while (option != 1 && option !=2);
	
	printf("Matrix dimension: %d x %d\n", dim, dim);
	printf("PARALLEL EXECUTION\n");

	if (option == 1)
	{
		printf("'matmul' method\n");
		//We multiply A and B and obtain the matrix C
		matmul(A, B, C, dim);
	}
	else
	{
		printf("'matmul_sup' method\n");
		//We multiply A and B in an optimized way and obtain the matrix C
		matmul_sup(A, B, C, dim);
	}
	
	//We print the execution time 
	printf("Computing time: %g \n", execTime);


	exit (0);
}

void init_mat_inf (int dim, float *M)
{
	int i,j,m,n,k;

	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			if (j >= i)
				M[i*dim+j] = 0.0;
			else
				M[i*dim+j] = RAND;
		}
	}
}

void init_mat_sup (int dim, float *M)
{
	int i,j,m,n,k;

	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			if (j <= i)
				M[i*dim+j] = 0.0;
			else
				M[i*dim+j] = RAND;
		}
	}
}


void matmul (float *A, float *B, float *C, int dim)
{
	int i, j, k;

	//We initialize the matrix C, to avoid failures
	for (i=0; i < dim; i++)
		for (j=0; j < dim; j++)
			C[i*dim+j] = 0.0;

	struct timeval t_0, t_1;
	gettimeofday(&t_0, NULL);

	for (i=0; i < dim; i++)
		for (j=0; j < dim; j++)
			for (k=0; k < dim; k++)
				C[i*dim+j] += A[i*dim+k] * B[j+k*dim];	

	gettimeofday(&t_1, NULL);
	execTime = ((t_1.tv_sec - t_0.tv_sec)*1000 + (t_1.tv_usec - t_0.tv_usec)/1000.0) / 1000;			
}

void matmul_sup (float *A, float *B, float *C, int dim)
{
	int i, j, k;

	//We initialize the matrix C, to avoid failures
	for (i=0; i < dim; i++)
		for (j=0; j < dim; j++)
			C[i*dim+j] = 0.0;

	struct timeval t_0, t_1;
	gettimeofday(&t_0, NULL);

	for (i=0; i < dim; i++)
		for (j=0; j < dim; j++)
			for (k=(i+1); k < dim; k++)
				C[i*dim+j] += A[i*dim+k] * B[j+k*dim];
    
	
	gettimeofday(&t_1, NULL);
	execTime = ((t_1.tv_sec - t_0.tv_sec)*1000 + (t_1.tv_usec - t_0.tv_usec)/1000.0) / 1000;
}

void matmul_inf (float *A, float *B, float *C, int dim)
{
	int i, j, k;

	for (i=0; i < dim; i++)
		for (j=0; j < dim; j++)
			C[i*dim+j] = 0.0;

	for (i=0; i < dim; i++)
		for (j=0; j < dim; j++)
			for (k=0; k < i; k++)
				C[i*dim+j] += A[i*dim+k] * B[j+k*dim];
}

void print_mat (float *M, int dim)
{
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			printf("%12f", M[i*dim+j]);
		}
		printf("\n");
	}

	printf("\n");
	printf("\n");
	
}

