#include "thread.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <deque> 
#include <cmath>
using namespace std;

#define DEBUG

// Enumerate function signatures
int main(int argc, char *argv[]);
void start(void *args);

typedef struct cashierData {
	char fname[50];
	int num;
} cdata;

typedef struct Sandwich {
	int sw;
	int cash;
} sandwich;

unsigned int num_cashiers;
unsigned int live_cashiers;
unsigned int board_size;
deque<Sandwich*> corkboard;
deque<int> cash_to_kill;
deque<deque<int>* > orders;

int main(int argc, char *argv[]) {


    board_size = 3;
    num_cashiers = 5;
    live_cashiers = num_cashiers;

    srand(0);
    for (int i = 0; i < num_cashiers; i++) {
    	deque<int> *temp = new deque<int>;
    	temp->push_back(rand()%1000);
    	temp->push_back(rand()%1000);
    	orders.push_back(temp);
    }

    for (int i = 0; i < orders.size(); i++) {
    	cout << "Orders " << i << ": ";
    	for (int j = 0; j < orders.at(i)->size(); j++) {
    		cout << orders.at(i)->at(j) << ", ";
    	}
    	cout << endl;
    }

    thread_libinit(start, 0);
    //start_preemptions(false, true, 1234);
}

unsigned int getMaxSize() {
	return live_cashiers < board_size ? live_cashiers : board_size;
}

bool readyToMake() {
	return corkboard.size() >= getMaxSize();
}

bool boardContainsCash(int cash) {
	for (int i = 0; i < corkboard.size(); i++) {
		if (corkboard.at(i)->cash == cash) return true;
	}
	return false;
}

int findNextSandwich(int last) {
	int min_dist = 12341234;
	int next = -1;
	for(int i = 0; i < corkboard.size(); i++) {
		int dist = abs(corkboard.at(i)->sw-last);
		if (dist < min_dist) {
			min_dist = dist;
			next = i;
		}
	}
	return next;
}

void printBoard() {
	#ifndef DEBUG
	return;
	#endif
	sandwich* sw;
	for (int i = 0; i < corkboard.size(); i++) {
		sw = corkboard.at(i);
		cout << sw->cash << ":" << sw->sw << " ";
	}
	cout << endl << "killboard: ";
	for (int i = 0; i < cash_to_kill.size(); i++) {
		cout << cash_to_kill.at(i) << " ";
	}
	cout << endl;
	cout << "max size: " << getMaxSize() << endl;
	cout << "live cash: " << live_cashiers << endl;
}

bool killContains(int c) {
	for (int i = 0; i < cash_to_kill.size(); i++) {
		if (cash_to_kill.at(i) == c) return true;
	}
	return false;
}

void killErase(int c) {
	for (int i = 0; i < cash_to_kill.size(); i++) {
		if (cash_to_kill.at(i) == c){
			cash_to_kill.erase(cash_to_kill.begin()+i);
			return;
		} 
	}
}

void maker(void* arg) {

	int last = -1;
	int index;
	thread_lock(1234);
	
	
	while (live_cashiers > 0 ||  corkboard.size() > 0) {
		
		while(!readyToMake()) {
			#ifdef DEBUG
			cout << "maker wait" << endl;
			#endif
			thread_wait(1234, 0);
			//thread_yield();
			#ifdef DEBUG
			cout << "maker done wait" << endl;
			#endif
		}		
		
		index = findNextSandwich(last); //`
		sandwich* sw = corkboard.at(index); //`
		last = sw->sw;
		#ifdef DEBUG
		cout << "cork size: " << corkboard.size() << endl;
		#endif
		cout << "READY: cashier " << sw->cash << " sandwich " << sw->sw << endl; 
		
		
		if (killContains(sw->cash)) { //`
			killErase(sw->cash); //`
			live_cashiers--;
			//thread_broadcast(1234, (sw->cash+5)*100);
		}
		corkboard.erase(corkboard.begin()+index); //`
		printBoard(); //`
		
		//thread_signal(sw->cash, sw->sw);
		thread_broadcast(1234, 1015);
		//thread_yield();
		#ifdef DEBUG
		cout << "signal board open" << endl;
		#endif
		free(sw);
	}
	thread_unlock(1234);
	#ifdef DEBUG
	cout << "maker end" <<endl;
	#endif
}

int getorder(int cashnum, int* res) {
	deque<int>* list = orders.at(cashnum);
	if (list->size() <= 0) {
		return 0;
	}
	int ret = list->front();
	list->pop_front();
	*res = ret;
	return 1;
}

void cashier(void* arg) {
	cdata* data = (cdata*) arg;
	ifstream order_file(data->fname);

	string line;
	int sw_num;
	#ifdef DEBUG
	cout << "cash " << data->num << " at lock" << endl;
	#endif
	thread_lock(1234);
	#ifdef DEBUG
	cout << "cash " << data->num << " after lock" << endl;
	#endif
	while(getorder(data->num, &sw_num)) {
		if (readyToMake()) { //`
			//thread_yield();
			thread_broadcast(1234,0);
			#ifdef DEBUG
			printf("cash %d signaling to make\n", data->num);
			#endif
			//thread_yield();
		}
		while (corkboard.size() >= board_size || boardContainsCash(data->num)) { //`
			#ifdef DEBUG
			printf("cashier %d wait\n", data->num);
			#endif
			thread_wait(1234, 1015);
			#ifdef DEBUG
			printf("cashier %d done wait\n", data->num);
			#endif
		}
		cout << "POSTED: cashier " << data->num << " sandwich " << sw_num << endl;
		sandwich* sw = (sandwich*)malloc(sizeof(sandwich));
		sw->sw = sw_num;
		sw->cash = data->num;
		corkboard.push_back(sw); //`
		printBoard(); //`
	}

	cash_to_kill.push_back(data->num); //`
	if (readyToMake()) { //`
		//thread_yield();
		thread_broadcast(1234,0);
		#ifdef DEBUG
		printf("cash %d signaling to make\n", data->num);
		#endif
		//thread_yield();
	}
	thread_unlock(1234);
	#ifdef DEBUG
	cout << "cashier end " << data->num << endl;
	#endif
	free(data);
	//thread_yield();
	//thread_unlock(1);
	
}


void start_thread(void *args) {
	#ifdef DEBUG
	cout << "in start thread" << endl;
	#endif
	thread_create(maker, NULL);
	#ifdef DEBUG
	cout << "making maker" << endl;
	#endif
    for(int i = 0; i < num_cashiers; i++) {
    	cdata* data = (cdata*)malloc(sizeof(cdata));
    	data->num = i;
    	#ifdef DEBUG
    	cout << "making cash " << i << endl;
    	#endif
    	thread_create(cashier, data);
    }
}

void start(void *args) {
	#ifdef DEBUG
	cout << "in start" << endl;
	#endif
    thread_create(start_thread, args);
}
