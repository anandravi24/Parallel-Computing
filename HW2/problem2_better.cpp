#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <omp.h>
#include <vector>
#include <numeric>
#include <time.h>


#include <cstdlib>
#include <pthread.h>
#include <mutex>

using namespace std;

#define h  800 
#define w  800

#define input_file  "input.raw"
#define output_file "output.raw"

typedef struct{
	int id;
	vector<vector<vector<unsigned char> > > *cluster;
	unsigned char *a;
	int start;
	int end;
	int *cluster_mean;
} thead_struct;


std::mutex mu;

void* distributed_kmeans(void * T)
{
	thead_struct* new1 = static_cast<thead_struct*>(T);
	int start = new1 -> start;
	int end = new1 -> end;
	int eud[6];
	int min_index;
	unsigned char *a = new1 -> a;
	
	int *cluster_mean = new1->cluster_mean;
	
	auto &cluster  = *new1 -> cluster;
	

	for(int i = 0; i < h; i++)
			for(int j = start; j < end; j++)
				{
					
					for(int k = 0; k < 6; k++)
						eud[k] = abs(cluster_mean[k] - a[i*w+j]);
				
					min_index = min_element(eud, eud + 6) - eud;
					cluster[new1 -> id][min_index].push_back(a[i*w+j]);
				}

	pthread_exit(NULL);
}


int main(int argc, char** argv){

    int i;
    FILE *fp;
	
	struct timespec start, stop;
	double time;
	
	
	unsigned char a[h*w];

	int count = 50;
	int cluster_mean[6] = {0, 65, 100, 125 , 190, 255};

	int eud[6];
	int min_index;
	int num_of_threads = atoi(argv[1]);

	pthread_t threads[num_of_threads];
  	thead_struct *T = new thead_struct[num_of_threads];

  	int rc;

	// the matrix is stored in a linear array in row major fashion
	if (!(fp=fopen(input_file, "rb"))) {
		printf("can not opern file\n");
		return 1;
	}
	fread(a, sizeof(unsigned char), w*h, fp);
	fclose(fp);
    
	// measure the start time here
	if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) { perror( "clock gettime" );}


	//  Your code goes here
		
	for(int c = 0; c < count; c++)
	{
		vector<vector<vector<unsigned char> > > cluster(num_of_threads, vector<vector<unsigned char>>(6, vector<unsigned char> ()));
		for(int i = 0; i < num_of_threads; i++)
		{
			T[i].id = i;
			T[i].start = i*w/num_of_threads;
      		T[i].end = (i+1)*w/num_of_threads - 1;
      		T[i].cluster = &cluster;
      		T[i].cluster_mean = cluster_mean;
      		T[i].a = a;
      		rc = pthread_create(&threads[i], NULL, distributed_kmeans, (void *)&T[i]);

  		}

  		for(int i = 0; i < num_of_threads ; i++)
  			pthread_join(threads[i], NULL);

		//pthread join
		//update k
  		int all_sum;
  		int all_len;
		for(int k = 0; k < 6; k++)
		{
			all_sum = 0;
			all_len = 0;
			for(int j = 0; j < num_of_threads; j++ )
			{
				all_sum += accumulate(cluster[j][k].begin(),cluster[j][k].end(),0);
				all_len += cluster[j][k].size();
			}
		cluster_mean[k] = all_sum/all_len;	

		}

		cluster.clear();
		
		
	}
		

	for(int i = 0; i < 6 ; i++)
		std::cout<<cluster_mean[i]<<std::endl;


	for(int i = 0; i < h; i++)
			for(int j = 0; j < w; j++)
				{
					for(int k = 0; k < 6; k++)
						eud[k] = abs(cluster_mean[k] - a[i*w+j]);
				
					min_index = min_element(eud, eud + 6) - eud;
					a[i*w+j] = cluster_mean[min_index];
				}
	
	
	// measure the end time here
	
	if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror( "clock gettime" );}		
  	time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;

	cout<<"\nTime Taken\n "<<time; 

	
	// print out the execution time here
	
	
	if (!(fp=fopen(output_file,"wb"))) {
		printf("can not opern file\n");
		return 1;
	}	
	fwrite(a, sizeof(unsigned char),w*h, fp);
    fclose(fp);
    
    return 0;
}

