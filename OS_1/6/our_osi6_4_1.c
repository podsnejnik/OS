#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <fstream>
#include <cmath>
#include <unistd.h>
#include <stdlib.h>
#include <omp.h>


using namespace std;

struct Data {
	int i = 0;
	int ** A = new int *[2048];
	int ** B = new int *[2048];
	int ** C = new int *[2048];
};


void ThreadMatrixMult (Data * data){
	int Size_M = 2048;

	for(int i=0; i<Size_M; ++i)	{
		data->C[i]=new int[Size_M];
	}


	int BlocksNum = 2;
	int ThreadNum = 4;
	omp_set_num_threads(ThreadNum);
	#pragma omp parallel
	{
		int ThreadID = omp_get_thread_num();
		int RowIndex = ThreadID/BlocksNum;
		int ColIndex = ThreadID%BlocksNum;
		int BlockSize = 2048/BlocksNum;
		for (int i=0; i<BlockSize; i++){
			for (int j=0; j<BlockSize; j++){
				for (int k=0; k<2048; k++){
					data->C[(RowIndex*BlockSize+i)][(ColIndex*BlockSize+j)] += data->A[(RowIndex*BlockSize+i)][k] * data->B[k][(ColIndex*BlockSize+j)];
				}
			}
		}
	}
	


	
    // for (int i1=0; i1<2048; i1++){
    // 	for (int j1=0; j1<2048; j1++){
    // 		cout<<data->C[i1][j1]<<" ";
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