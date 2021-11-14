#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <math.h>

#include <fcntl.h>

#include "calculator.h"

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
    info_about_intervals[i].num = i;
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

double integrate_func(long num_segments, int num_threads) {
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

  free(threads);
  free(info_about_intervals);

  return my_sum;
}

double find_avg_time_and_avg_res(long num_segments, long num_threads, double* avg_res) {
  double accumulated_res = 0.;
  double accumulated_time = 0;
  for (int i = 0; i < NUM_SAMPLES_TO_AVG; i++) {
    struct timeval start_time, end_time;
    
    gettimeofday(&start_time, NULL);
    accumulated_res += integrate_func(num_segments, num_threads);
    gettimeofday(&end_time, NULL);
    
    double required_time = 
        (end_time.tv_sec + (double)end_time.tv_usec / USEC_IN_SEC) -
        (start_time.tv_sec + (double)start_time.tv_usec / USEC_IN_SEC);

    accumulated_time += required_time;
  }

  if (avg_res != NULL) {
    *avg_res = accumulated_res / NUM_SAMPLES_TO_AVG;
  }

  return accumulated_time / NUM_SAMPLES_TO_AVG;
}



// int main(int argc, char* argv[]) {
//   // we expect to receive: [num_segments] [num_threads]
//   if (argc != 3) {
//     printf(
//         "Please, change your arguments format! \n"
//         "This program cannot work with this data format!\n"
//         "Expected format is: ./a.out [num_segments] [num_threads]\n");
//     exit(0);
//   } else {
//     long num_segments = atoi(argv[1]);
//     long num_threads = atoi(argv[2]);
//     double avg_res;
//     double avg_time = find_avg_time_and_avg_res(num_segments, num_threads, &avg_res);
//     printf(" avg time: %f\n num_segments: %ld\n num_threads: %ld\n", avg_time, num_segments, num_threads);

//     double pi = 3.1415926535897932384626433832795;
//     printf("integrate sum is:  %.20f\n", avg_res);
//     printf("correct answer is: %.20f\n", pi);
//     printf("the different is:  %.20f\n", fabs(avg_res - pi));

//   }
//   return 0;
// }