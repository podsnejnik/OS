#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <fstream>
#include <cmath>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


using namespace std;

struct Data {
	
	int i = 0;
	int ** A = new int *[2048];
	int ** B = new int *[2048];
	int ** C = new int *[2048];
};

void *Mult(void *data){
	Data *local_data = (Data*)data;
	int Size_M = 2048;
	int count = (local_data->i)+512;
	cout<<"count = "<<count<<"\n";
	int i = local_data->i;
	cout<<"i = "<<i<<"\n";
	for (i; i<count; i++){ 
		for (int j = 0; j < Size_M; j++) {
		    local_data->C[i][j] = 0;
		    for (int k = 0; k < Size_M; k++) {
				local_data->C[i][j] += local_data->A[i][k] * local_data->B[k][j];
			}
		}
	}
	
}

void ThreadMatrixMult (Data * data){
	int Size_M = 2048;

	for(int i=0; i<Size_M; ++i)	{
		data->C[i]=new int[Size_M];
	}

	pthread_t thread_ids[4];
	int thread_error;
	int thread_num = 0;

	for (data->i = 0; data->i<2048; data->i+=512){
		cout<<"this is for = "<<data->i<<"\n";
		thread_error = pthread_create(&thread_ids[thread_num], NULL, Mult, (void *)data);
		sleep(1);
		thread_num++;
	}


    for (int j = 0; j<4; j++){
        pthread_join(thread_ids[j], NULL);
    }

    

    // for (int i=0; i<2048; i++){
    // 	for (int j=0; j<2048; j++){
    // 		cout<<data->C[i][j]<<" ";
    // 	}
    // 	cout<<"\n";
    // }

}


int main(int argc, char* argv[]){ 
	int Size_M = 2048;
	Data *data = new Data;

	for(int i=0; i<Size_M; ++i)	{
		data->A[i]=new int[Size_M];
		data->B[i]=new int[Size_M];
	}

	for (int i = 0; i<Size_M; i++){
		for (int j = 0; j<Size_M; j++){
			data->A[i][j] = 10;
			data->B[i][j] = 10;
		}
	}
	double startTime = (double)clock() / CLOCKS_PER_SEC;
	ThreadMatrixMult(data);
	double endTime = (double)clock() / CLOCKS_PER_SEC;
    printf("Standart: %0.9f\n", endTime - startTime);
	return 0;

}