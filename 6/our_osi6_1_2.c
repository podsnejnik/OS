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

int probability(int a, int b){
	long long x;
	asm("rdtsc" : "=A"(x));
	srand (x);
	int p = rand() % a + b;
	return p;
}

void * multiple_task1(void * arg){
	int size_buf = 0;
	char * buffer = NULL;
	for(int i = 0; i < 20000; i++){
		int p = probability(3, 1);
		if (p == 1){			
			size_buf = probability(4 *256, 256);
			buffer = (char *) malloc(size_buf * sizeof(char));
		}
		if (p == 2 && buffer){
			free(buffer);
			buffer = NULL;
		}
		if (p == 3 && buffer){
			memset(buffer, '1', size_buf);
			buffer = NULL;

		}		
	}
}


int main(int argc, char* argv[]){ 
	double startTime = (double)clock() / CLOCKS_PER_SEC;	
	int thread_error;
   pthread_t thread_ids[20];
   for (int thread_num = 0; thread_num < 20; thread_num++){

       cout << "begin to create thread #"<< thread_num << "\n";
       thread_error = pthread_create(&thread_ids[thread_num], NULL, multiple_task1, NULL);

       if (thread_error!=0){
       	cout<<"error #"<<thread_error<<"\n";
       }
   }

   for (int thread_num = 0; thread_num < 20; thread_num++){
       pthread_join(thread_ids[thread_num], NULL);
   }
   double endTime = (double)clock() / CLOCKS_PER_SEC;
	printf("Standart: %0.9f\n", endTime - startTime);
	return 0;
}