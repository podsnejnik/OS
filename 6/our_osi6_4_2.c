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


	
	int ThreadNum = 4;
	int GridSize = int (sqrt((double)ThreadNum));
	int BlockSize = 2048/GridSize;
	omp_set_num_threads(ThreadNum);
	#pragma omp parallel
	{
		int ThreadID = omp_get_thread_num();
		int RowIndex = ThreadID/GridSize;
		int ColIndex = ThreadID%GridSize;
		for (int iter=0; iter<GridSize; iter++) {
			for (int i=RowIndex*BlockSize; i<(RowIndex+1)*BlockSize; i++){
				for (int j=ColIndex*BlockSize; j<(ColIndex+1)*BlockSize; j++){
					for (int k=iter*BlockSize; k<(iter+1)*BlockSize; k++){
						data->C[i][j] += data->A[i][k] * data->B[k][j];
					}
				}
			}
		}
	} // pragma omp parallel
			
	
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