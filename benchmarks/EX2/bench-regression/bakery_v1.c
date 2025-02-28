/* Copyright (C) 2018
 * This benchmark is part of SWSC
 */

/* Adapted from:
 * Lamport's Bakery critical section algorithm, implemented with fences.
 *
 * Adapted from the benchmark in the URL:
 *   https://www.geeksforgeeks.org/operating-system-bakery-algorithm/
 */

#include <assert.h>
#include <stdint.h>
#include <pthread.h>

#define LOOP 5
#define THREAD_COUNT 2

int tickets[THREAD_COUNT];
int choosing[THREAD_COUNT];
int resource;

int lock(int thread) {
  	int ok = 0;
  	int max_ticket = 0;

	choosing[thread] = 1;

  	// 修改: 通过将最大票据值的计算移到循环外部，提高性能
  	for (int i = 0; i < THREAD_COUNT; ++i) {
    	max_ticket = max_ticket > tickets[i] ? max_ticket : tickets[i];
  	}
  
	tickets[thread] = max_ticket + 1;
	choosing[thread] = 0;
  
  	// 修改: 在此加入一个短路优化，避免无意义的循环
  	for (int other = 0; other < THREAD_COUNT; ++other) {
    	ok = 0;
    	for (int i = 0; i < LOOP; i++) {
      		if (!choosing[other]) {
        		ok = 1;
        		break;
      		}
    	}
    
    	if (ok == 0) return 0; // 修改：避免重复执行后续检查
    	else {
      		ok = 0;
      		for (int i = 0; i < LOOP; i++) {
        		int other_ticket = tickets[other];
        		int thread_ticket = tickets[thread];
        		if (!(other_ticket != 0 &&
              		(other_ticket < thread_ticket || (other_ticket == thread_ticket && other < thread)))) {
                	ok = 1;
                	break;
              	}
      		}
      
			if (ok == 0) return 0; // 只在失败时退出
    	}
  	}
  	return 1;
}


void unlock(int thread) {
  	tickets[thread] = 0;
}

void use_resource(int thread) {
  	if (resource != 0) {
    	assert(0);
  	}
  	resource = thread;
  	resource = 0;
}

void *thread_body(void *arg) {
  	int thread = *((int *)arg);
  	if (lock(thread)) {
    	use_resource(thread);
    	unlock(thread);
  	}
	return NULL;
}

int main() {
  	pthread_t threads[THREAD_COUNT];

  	for (int i=0; i<THREAD_COUNT; i++) {
    	tickets[i] = 0;
    	choosing[i] = 0;
  	}
	
  	resource = 0;
  
  	for (int i = 0; i < THREAD_COUNT; ++i) {
    	pthread_create(&threads[i], NULL, thread_body, &i);
  	}
  
  	for (int i = 0; i < THREAD_COUNT; ++i) {
    	pthread_join(threads[i], NULL);
  	}
  
  	return 0;
}
