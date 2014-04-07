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
#include <sys/mman.h>

using namespace std;

int main(int argc, char* argv[]){ 
	// long long size_buf = 2571215000;//2,452101
	double startTime = (double)clock() / CLOCKS_PER_SEC;
	size_t size_buf = (size_t)6 * (1 << 30);//6гб
	char * my_memory = (char *) malloc(size_buf * sizeof(char));	
	
	long long x;
	asm("rdtsc" : "=A"(x));
	srand (x);	

	int b;
    for(int i = 0; i < 3000000; i++ ) {
    	b = rand() % size_buf;
    	my_memory[b] = 36;
    }
    double endTime = (double)clock() / CLOCKS_PER_SEC;
	printf("Standart: %0.9f\n", endTime - startTime);
    return 0;
}