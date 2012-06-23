#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#include <unistd.h>
#include "perf_counter.h"

/*
 * A simple performace counter, lijin liu<llj098@gmail.com>
 *
 * This counter could count 'Total Count' and 'Count per second'
 * For 'per second' counting, there are two ways to measure the time
 * 1. gettimeofday, it is accurate but it slow, about 1us time each call
 * 2. background thread maintaining, measuring the time without system call, this helps a lot, about 50X faster
 * You can define the 'bg_time' to enable the backgroud time measuring
 * 
 */


struct timeval __perf_c_usec = { 0 , 0 };

void* perf_bg_getusec(void *arg)
{
	while(1) {
		if(__perf_c_usec.tv_sec <= 0) {
			gettimeofday(&__perf_c_usec,NULL);
		}
		else {
			__perf_c_usec.tv_usec += PERF_TIME_REFRESH;
			struct timeval val;
			gettimeofday(&val,NULL);

			if(__perf_c_usec.tv_usec >= 1000 * 1000) {
				__perf_c_usec.tv_sec ++;
				__perf_c_usec.tv_usec -= 1000 * 1000;
			}
		}
		usleep(PERF_TIME_REFRESH);
	}
	return NULL;
}

void perf_start_bgtime()
{
	pthread_t t;
	pthread_attr_t attr;
	int stack_size = 16384,s;
	
	s = pthread_attr_init(&attr);
	if(s != 0) {
		perror("attr init");
		return;
	}

	s = pthread_attr_setstacksize(&attr,stack_size);
	if(s != 0) {
		perror("set stack");
		return;
	}

	s = pthread_create(&t, &attr, perf_bg_getusec, NULL);
	if(s != 0) {
		perror("error pthread create ");
		return ;
	}
}

void perf_count_inc(perf_counter *c)
{
	struct timeval val;
	unsigned long long l;

#if bg_time
	if(__perf_c_usec.tv_sec == 0) {
		perf_start_bgtime();
		sleep(1);
	}
	val = __perf_c_usec;
#else 
	if(gettimeofday(&val, NULL) < 0) {
		return;
	}
#endif

	l = (val.tv_sec - c->time.tv_sec) * 1000 * 1000;
	l += val.tv_usec - c->time.tv_usec;

	if(l > PERF_AVG_INTERVAL) {
		
		if(l >= 2 * PERF_AVG_INTERVAL) {
			c->last = 0;
		}
		else {
			c->last = c->current;
		}
		c->current = 1;
		c->time = val;
	}
	else {
		c->current++;
	}

	c->total++;
}

void 
perf_count_get(perf_counter_result *result,perf_counter *c)
{
	struct timeval val;
	long l;

#if bg_time
	val = __perf_c_usec;
#else
	gettimeofday(&val, NULL);
#endif

	l = (val.tv_sec - c->time.tv_sec) * 1000 * 1000;
	l += val.tv_usec - c->time.tv_usec;

	if(l > PERF_AVG_INTERVAL) {
		if(l > 2 * PERF_AVG_INTERVAL) {
			result->avg = 0;
		}
		else{
			result->avg = c->current * ((float)PERF_AVG_INTERVAL / l);
		}
	}
	else {
		long r  = c->current + c->last;
		long t = l + PERF_AVG_INTERVAL;
		result->avg = r * ((float)PERF_AVG_INTERVAL / t);
	}

	result->total = c->total;
}
