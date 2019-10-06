#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <omp.h>
#include <vector>
#include <numeric>
#include <time.h>

using namespace std;

#define h  800 
#define w  800

#define input_file  "input.raw"
#define output_file "output.raw"


int main(int argc, char** argv){
    int i;
    FILE *fp;
	
	struct timespec start, stop;
	double time;
	

  	unsigned char a[h][w];
	int count = 30;
	int cluster_mean[4] = {0, 85, 170, 255};

	int eud[4];
	int min_index;
		
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
		vector<vector<unsigned char> > cluster(4, vector<unsigned char>());
		for(int i = 0; i < h; i++)
			for(int j = 0; j < w; j++)
				{
					for(int k = 0; k < 4; k++)
						eud[k] = abs(cluster_mean[k] - a[i][j]);
				
					min_index = min_element(eud, eud + 4) - eud;
					cluster[min_index].push_back(a[i][j]);
				}
		for(int k = 0; k < 4; k++)
		{
		cluster_mean[k] = accumulate(cluster[k].begin(),cluster[k].end(),0)/cluster[k].size();	
		//cout<<"\nFor iteration "<<c<<" the mean of cluster " << k <<" is "<<cluster_mean[k];
		}

		cluster.clear();
		
	}
		
	for(int i = 0; i < h; i++)
			for(int j = 0; j < w; j++)
				{
					for(int k = 0; k < 4; k++)
						eud[k] = abs(cluster_mean[k] - a[i][j]);
				
					min_index = min_element(eud, eud + 4) - eud;
					a[i][j] = cluster_mean[min_index];
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
