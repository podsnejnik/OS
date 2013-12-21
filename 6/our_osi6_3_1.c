#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <fstream>
#include <pthread.h>
#include <stdlib.h>

using namespace std;

volatile int counters[8];


void * multiple_task3(void *k){
	int *local_k = (int *) k;
	for(int i = 0; i < 30000; i++){	
		counters[*local_k] += 1;	
	}
}

int main(int argc, char* argv[]){ 
    double startTime = (double)clock() / CLOCKS_PER_SEC;
	int thread_error;
    pthread_t thread_ids[8];
    for (int thread_num = 0; thread_num < 8; thread_num++){
        cout << "begin to create thread #"<< thread_num << "\n";
        thread_error = pthread_create(&thread_ids[thread_num], NULL, multiple_task3,  (void *)&thread_num);
 		
        if (thread_error!=0){
        	cout<<"error #"<<thread_error<<"\n";
        }
    }
    
    for (int thread_num = 0; thread_num < 8; thread_num++){
        pthread_join(thread_ids[thread_num], NULL);
    }
    double endTime = (double)clock() / CLOCKS_PER_SEC;
    printf("Standart: %0.9f\n", endTime - startTime);
	return 0;
}