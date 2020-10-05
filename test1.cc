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
    thread_libinit(start, 0);
    thread_libinit(start, 0);
}

void test_func(void *args) {
	int argc;
    thread_lock(0);
	argc = *((int*) args);
	cout << "Printing from thread " << argc << endl;
    thread_lock(0);
}

void start_thread(void *args) {
    cout << "starting threads" << endl;
    for(int i = 0; i < 10; i++) {
        int* temp = (int*)malloc(sizeof(int));
        *temp = i;
        cout << "starting thread " << i << endl;
    	thread_create(test_func, temp);
        char* memwaste = new char[1024*1024];
    }
}

void start(void *args) {
    cout << "in start function" << endl;
    thread_create(start_thread, args);
}
