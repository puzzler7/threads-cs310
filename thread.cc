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
#include <map>
#include <assert.h>

using namespace std;

deque<ucontext_t*> waiting;
ucontext_t* running;
ucontext_t* dead;
ucontext_t* to_kill = NULL;
bool initialized = false;
map <int, bool> locks;
map <int, deque<ucontext_t*> > locked_threads;

int swap_thread(ucontext_t* curr, ucontext_t* next) {
	int ret = swapcontext(curr, next);
	interrupt_disable();
	return ret;
}

void runNext(bool del, bool slp) {
	//cout << "in run next" << endl;
	if (to_kill != NULL) {
		free(to_kill->uc_stack.ss_sp);
		free(to_kill);
		to_kill = NULL;
	}
	if (del) {
		to_kill = running;
	} else if (slp) {
	} else {
		waiting.push_back(running);
	}
	if (waiting.size() <= 0) {
		exit(0);
	}
	ucontext_t* next = waiting.front();
	waiting.pop_front();
	ucontext_t* oldrun = running;
	running = next;
	if (del) {
		//cout << "delswap" << endl;
		swap_thread(dead, next);
	} else {
		//cout << "else swap" << endl;
		swap_thread(oldrun, next);
	}
	//cout << "run next ret" << endl;
}

void stub(void* fn, void* arg) {
	//cout << "in stub" << endl;
	interrupt_enable();
	//cout << "after stub enable" << endl;
	((void (*)(void*))fn)(arg);
	interrupt_disable();
	runNext(true, false);
}

int thread_libinit(thread_startfunc_t func, void *arg){
	interrupt_disable();
	if (initialized) {
		return -1;
	}
	try {
		dead = (ucontext_t*) malloc(sizeof(ucontext_t));
		running = (ucontext_t*) malloc(sizeof(ucontext_t));
		if (dead == NULL || running == NULL) {
			throw 0;
		}
		getcontext(running);

		//cout << "preinit" << endl;
		char *stack = new char [STACK_SIZE];
		if (stack == NULL) {
			throw 0;
		}
		//cout << "postinit" << endl;
		running->uc_stack.ss_sp = stack;
		running->uc_stack.ss_size = STACK_SIZE;
		running->uc_stack.ss_flags = 0;
		running->uc_link = NULL;

		makecontext(running, (void (*)()) stub, 2, func, arg);
		initialized = true;
		setcontext(running);

		cout << "Thread library exiting.\n";
		exit(0);
	} catch (...){
		//cout << "thread_libinit failed" << endl;
		return -1;
	}
}

int thread_create(thread_startfunc_t func, void *arg){
	interrupt_disable();
	if(!initialized) {
		return -1;
	}
	try {
		ucontext_t* newthread = (ucontext_t*)malloc(sizeof(ucontext_t));

		getcontext(newthread);
		//cout << "precreate" << endl;
		char *stack = new char [STACK_SIZE];
		//cout << "postcreate" << endl;
		if (stack == NULL) {
			return -1;
		}
		newthread->uc_stack.ss_sp = stack;
		newthread->uc_stack.ss_size = STACK_SIZE;
		newthread->uc_stack.ss_flags = 0;
		newthread->uc_link = NULL;
	

		makecontext(newthread, (void (*)()) stub, 2, func, arg);
		waiting.push_back(newthread);
		interrupt_enable();
		return 0;
	} catch (...) {
		//cout << "thread_create failed" << endl;
		return -1;
	}
}

int thread_yield(void){
	interrupt_disable();
	if(!initialized) {
		return -1;
	}
	runNext(false, false);
	interrupt_enable();
	return 0;
}


int thread_lock(unsigned int lock){
	interrupt_disable();
	if(!initialized) {
		return -1;
	}

	if (!locks.count(lock) || !locks[lock]) {
		locks[lock] = true;
	} else {
		locked_threads[lock].push_back(running);
		runNext(false, true);
	}

	interrupt_enable();
	return 0;
}

int thread_unlock(unsigned int lock){
	interrupt_disable();
	if(!initialized) {
		return -1;
	}
	if (!locked_threads.count(lock) || !locks.count(lock)) {
		return -1;
	}

	locks[lock] = false;
	if (locked_threads[lock].size() > 0) {
		locks[lock] = true;
		ucontext_t* ready = locked_threads[lock].front();
		locked_threads[lock].pop_front();
		waiting.push_back(ready);
	}


	interrupt_enable();
	return 0;
}

/*int thread_wait(unsigned int lock, unsigned int cond){
	if(!initialized) {
		return -1;
	}
}

int thread_signal(unsigned int lock, unsigned int cond){
	if(!initialized) {
		return -1;
	}
}

int thread_broadcast(unsigned int lock, unsigned int cond){
	if(!initialized) {
		return -1;
	}
}*/