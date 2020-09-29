#include "interrupt.h"
#include "thread.h"
#include <ucontext.h>

#include <stdio.h>
#include <string.h>
#include <deque> 
#include <cmath>

int thread_libinit(thread_startfunc_t func, void *arg){


	cout << "Thread library exiting.\n";
	exit(0);
}

int thread_create(thread_startfunc_t func, void *arg){

}

int thread_yield(void){

}

/*
int thread_lock(unsigned int lock){

}

int thread_unlock(unsigned int lock){

}

int thread_wait(unsigned int lock, unsigned int cond){

}

int thread_signal(unsigned int lock, unsigned int cond){

}

int thread_broadcast(unsigned int lock, unsigned int cond){

}*/