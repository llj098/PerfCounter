#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h>

#include "perf_counter.h"

void test_counter(int interval)
{
	int i,j;
	perf_counter pc; 
	PERF_COUNTER_INIT(pc);
	perf_counter_result pr = { 0, 0 };


	for(j =0; j < 10; j++) {
		for(i =0; i < 10000; i++) {
			perf_count_inc(&pc);
			usleep(interval);
		}
	
		perf_count_get(&pr, &pc);
		printf("result :total %llu,avg %ld\n",pr.total, pr.avg);
	}
}

int main()
{
	int i ;

	for(i= 10; i< 1000 * 1000; i = i *10) {
		printf("## interval %d\n", i);
		test_counter(i);
	}

	return 0;
}
