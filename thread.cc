#include "interrupt.h"
#include "thread.h"
#include <ucontext.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <deque> 
#include <cmath>

using namespace std;

deque<ucontext_t*> waiting;
ucontext_t* running = (ucontext_t*) malloc(sizeof(ucontext_t));

int thread_libinit(thread_startfunc_t func, void *arg){
	getcontext(running);

	char *stack = new char [STACK_SIZE];
	running->uc_stack.ss_sp = stack;
	running->uc_stack.ss_size = STACK_SIZE;
	running->uc_stack.ss_flags = 0;
	running->uc_link = NULL;

	makecontext(running, (void (*)()) func, 1, arg);

	setcontext(running);

	cout << "Thread library exiting.\n";
	exit(0);
}

int thread_create(thread_startfunc_t func, void *arg){
	ucontext_t* newthread = (ucontext_t*)malloc(sizeof(newthread));

	getcontext(newthread);
	char *stack = new char [STACK_SIZE];
	newthread->uc_stack.ss_sp = stack;
	newthread->uc_stack.ss_size = STACK_SIZE;
	newthread->uc_stack.ss_flags = 0;
	newthread->uc_link = NULL;

	makecontext(newthread, (void (*)()) func, 1, arg);
	waiting.push_back(newthread);
	return 0;
}

int thread_yield(void){
	ucontext_t* next = waiting.front();
	waiting.pop_front();
	waiting.push_back(running);
	ucontext_t* oldrun = running;
	running = next;
	swapcontext(oldrun, next);
	return 0;
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