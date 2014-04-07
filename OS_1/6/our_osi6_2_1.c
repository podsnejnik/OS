#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <fstream>
#include <cmath>
#include <pthread.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char* argv[]){ 
	double startTime = (double)clock() / CLOCKS_PER_SEC;	
	// long long size_buf = 2571215000;//2,452101	
	size_t size_buf = (size_t)6 * (1 << 30);//6гб
	char * my_memory = (char *) malloc(size_buf * sizeof(char));	
		
	int k;
    for(int i = 0; i < 3000000; i++) {
    	k =  (i + 1) % size_buf;
        my_memory[k] = 36;
    }
    double endTime = (double)clock() / CLOCKS_PER_SEC;
	printf("Standart: %0.9f\n", endTime - startTime);
	
	return 0;
}