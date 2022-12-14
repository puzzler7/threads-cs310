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

int ready = 0;

void test_func(void *args) {
	int argc;
    argc = *((int*) args);
    thread_lock(0);
    if(0 != argc) {
        thread_wait(0, 1);
    }
	cout << "Printing from thread " << argc << endl;
    ready++;
    thread_unlock(0);
    thread_broadcast(0, 1);
}

void start_thread(void *args) {
    cout << "starting threads" << endl;
    for(int i = 0; i < 10; i++) {
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
