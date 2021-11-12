#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BEGINNING_INTERVAL 0
#define ENDING_INTERVAL 2

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define BAD_MESSAGE         -13
#define SUCCESS               0

typedef struct {
		double start_interval;
		double end_interval;
		double step;
		double sum;
} Data;

double func(double x) {
	if ((int)x >= 2) {
		return 0;
	} else {
		return sqrt(4 - x * x);
	}
}

void filling_info_about_intervals(long num_segments, int num_threads, Data* info_about_intervals) {
	// |                          interval                         |
	// |interval_size |interval_size |interval_size |interval_size |
	// |step|step|step|step|step|step|step|step|step|step|step|step|
	double interval_size = (double)(ENDING_INTERVAL - BEGINNING_INTERVAL) / num_threads;
	double step = (double)(ENDING_INTERVAL - BEGINNING_INTERVAL) / num_segments;

	for (int i = 0; i < num_threads; i++) {
		info_about_intervals[i].start_interval = interval_size * i;
		info_about_intervals[i].end_interval = interval_size * (i + 1);
		info_about_intervals[i].step = step;
		info_about_intervals[i].sum = 0.;
	}
}

void* sum_at_subinterval(void* args) {
	Data* info = args;
	double sum = 0.;
	double current_x = info->start_interval;
	while(current_x < info->end_interval) {
		sum += (info->step) * func(current_x);
		current_x += info->step;
	}
	info->sum = sum;
	return NULL;
}

void integrate_func(long num_segments, int num_threads) {
	pthread_t* threads = (pthread_t*)calloc(num_threads, sizeof(pthread_t));
	int status;

	Data* info_about_intervals = (Data*)calloc(num_threads, sizeof(Data));
	filling_info_about_intervals(num_segments, num_threads, info_about_intervals);

	// first step at finding resulting sum
	double my_sum = (info_about_intervals->step) * (func(BEGINNING_INTERVAL) + func(ENDING_INTERVAL)) / 2;

	for (int i = 0; i < num_threads; i++) {
		status = pthread_create((threads+i), NULL, sum_at_subinterval, (void*)(info_about_intervals+i));
		if (status != 0) {
			printf("main error: can't create thread, status = %d\n", status);
			exit(ERROR_CREATE_THREAD);
		}
	}

	for (int i = 0; i < num_threads; i++) {
		status = pthread_join(threads[i], NULL);
		if (status != SUCCESS) {
			printf("main error: can't join thread, status = %d\n", status);
			exit(ERROR_JOIN_THREAD);
		}
	}

	for (int i = 0; i < num_threads; i++) {
		my_sum += info_about_intervals[i].sum;
	}

	double pi = 3.1415926535897932384626433832795;
	printf("integrate sum is:  %.20f\n", my_sum);
	printf("correct answer is: %.20f\n", pi);
	printf("the different is:  %.20f\n", fabs(my_sum - pi));

	free(threads);
	free(info_about_intervals);
}

int main(void) {
	clock_t begin = clock();
	integrate_func(100000000, 4);
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("time was spent: %f", time_spent);
	return 0;
}