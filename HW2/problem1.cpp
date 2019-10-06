#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <iostream>


typedef struct{
	int id;
	double **A;
	double **B;
	double **C;
	int size;
	int start;
	int end;
} thead_struct;

void* multiply(void * T)
{
	thead_struct* new1 = static_cast<thead_struct*>(T); 

	double **A = new1 -> A;
	double **B = new1 -> B;
	double **C = new1 -> C;
	int size = new1 -> size;

	int start = new1 -> start;
	int end = new1 -> end;


	for(int i = 0; i < size; i++)
			for(int j = start; j < end; j++)
				for(int k = 0; k < size; k++)
					C[i][j] = C[i][j] + (A[i][k] * B[k][j]);


	pthread_exit(NULL);

}



int main(int argc, char** argv){
		int i, j, k;
		struct timespec start, stop; 
		double time;
		int n = 4096; // matrix size is n*n
		int num_of_threads = atoi(argv[1]);

		pthread_t threads[num_of_threads];
   		thead_struct T[num_of_threads];
		

   		int rc;

		double **A = (double**) malloc (sizeof(double*)*n);
		double **B = (double**) malloc (sizeof(double*)*n);
		double **C = (double**) malloc (sizeof(double*)*n);
		for (i=0; i<n; i++) {
			A[i] = (double*) malloc(sizeof(double)*n);
			B[i] = (double*) malloc(sizeof(double)*n);
			C[i] = (double*) malloc(sizeof(double)*n);
		}
		
		for (i=0; i<n; i++){
			for(j=0; j< n; j++){
				A[i][j]=i;
				B[i][j]=i+j;
				C[i][j]=0;			
			}
		}
				
		if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
		
		// Your code goes here //
		// Matrix C = Matrix A * Matrix B //	
		//*******************************//


		for( int i = 0; i < num_of_threads; i++ ) {
      
      		T[i].id = i;
      		T[i].A = A;
      		T[i].B = B;
      		T[i].C = C;
      		T[i].size = n;
      		T[i].start = i*n/num_of_threads;
      		T[i].end = (i+1)*n/num_of_threads - 1;
      		rc = pthread_create(&threads[i], NULL, multiply, (void *)&T[i]);
  		}

  		for(int i = 0; i < num_of_threads ; i++)
  			pthread_join(threads[i], NULL);
		
		//*******************************//
		
		if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
		time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
		
		printf("Number of FLOPs = %lu, Execution time = %f sec,\n%lf MFLOPs per sec\n", 2*n*n*n, time, 1/time/1e6*2*n*n*n);		
		printf("C[100][100]=%f\n", C[100][100]);
		
		// release memory
		for (i=0; i<n; i++) {
			free(A[i]);
			free(B[i]);
			free(C[i]);
		}
		free(A);
		free(B);
		free(C);

		pthread_exit(NULL);

		return 0;
}
		
