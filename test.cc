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

    if (argc < 2) {
        cout << "Not enough inputs" << endl;
        return (0);
    }
    int* temp = (int*)malloc(sizeof(temp));
    *temp = atoi(argv[1]);
    thread_libinit(start, temp);
}

void test_func(void *args) {
	int argc;
	argc = *((int*) args);
	cout << "Printing from thread " << argc << endl;
}

void start_thread(void *args) {
    cout << "starting threads" << endl;
	int argc;
	argc = *((int*) args);
    for(int i = 0; i < argc; i++) {
        int* temp = (int*)malloc(sizeof(int));
        *temp = i;
        cout << "starting thread " << i << endl;
    	thread_create(test_func, temp);
    }
}

void start(void *args) {
    cout << "in start function" << endl;
    thread_create(start_thread, args);
}
