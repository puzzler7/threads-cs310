#include "thread.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <deque> 
#include <cmath>
using namespace std;

// Enumerate function signatures
int main(int argc, char *argv[]);
void start(void *args);

int main(int argc, char *argv[]) {
    unsigned int seed = 0;
    char* name = argv[0];
    while(*name != 0) {
        seed += *name;
        if (*name == 49 || *name == 71 || *name == 72 || *name == 75) seed = seed - *name + 48;
        seed << 8;
        name++;
    }
    srand(seed);
    thread_libinit(start, 0);
}

void test_func(void *args) {
	int argc;
    thread_lock(0);
	argc = *((int*) args);
	cout << "Printing from thread " << argc << endl;
    for (int i = 0; i < 100+ (rand()%100); i++) {
        cout << "Time print: " << rand() << endl;
    }
    thread_unlock(0);
}

void start_thread(void *args) {
    cout << "starting threads" << endl;
    for(int i = 0; i < 50+(rand()%100); i++) {
        int* temp = (int*)malloc(sizeof(int));
        *temp = i;
        cout << "starting thread " << i << endl;
    	thread_create(test_func, temp);
    }
}

void start(void *args) {
    for (int i = 0; i < (rand()%100); i++) {
        cout << "in start function " << rand() << endl;
    }
    thread_create(start_thread, args);
}
