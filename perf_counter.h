#ifndef _PERF_COUNTER_H
#define _PERF_COUNTER_H

#include <sys/time.h>
#define PERF_AVG_INTERVAL (1000 * 1000)
#define PERF_TIME_REFRESH (200 * 1000)

typedef struct perf_counter {
	unsigned long long total;
	unsigned long last;
	unsigned long current;
	struct timeval time;
}perf_counter;

typedef struct perf_counter_result {
	unsigned long long total;
	unsigned long avg;
}perf_counter_result;

#define PERF_COUNTER_INIT(c)				\
	do{						\
		(c).total = 0;				\
		(c).last  = 0;				\
		gettimeofday(&((c).time), NULL);	\
	} while(0);					\

void perf_count_inc(perf_counter*);
void perf_count_get(perf_counter_result*,perf_counter*);
#endif
