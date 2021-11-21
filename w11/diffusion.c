// TODO: read data from command line
// TODO: write data to the file
// TODO: time measurements

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#include <semaphore.h>

#define STUPID_CONST 0.3
#define C 1

#define R_TEMP 0.001 // temperature on the left side
#define L_TEMP 0 // temperature on the right side
#define LEN 100000 // length of kernel

#define NUM_SEGMENTS 500000 // number of segments of the partition
#define TIME 30. // integration time
#define NUM_THREADS 4 // num threads

typedef struct {
  double* lp_cur_temp; // left point current temperature
  double* rp_cur_temp; // right point current temperature
  double* lp_future_temp; // left point future temperature
  double* rp_future_temp; // right point future temperature
  long num_parts; // num points between lp and rp cur temp
  pthread_barrier_t* barrier;
} DataFrame;

void swap_arrays(double** a, double** b) {
    double* tmp = *a;
    *a = *b;
    *b = tmp;
}

void* calc_future_temp_multithread(void* args) {
  DataFrame* df = args;
  // count f[i] = c[i] + stup_const * (c[i+1] - 2*c[i] + c[i-1])
  for (long i = 1; i < df->num_parts - 1; i++) {
    double* cur_temp = df->lp_cur_temp + i;
    double* fut_temp = df->lp_future_temp + i;
    *fut_temp = (*cur_temp) + STUPID_CONST * ((*(cur_temp+1)) - 2 * (*cur_temp) + (*(cur_temp-1)));
  }
  pthread_barrier_wait(df->barrier);
}

void* calc_future_temp_single(void* args) {
  DataFrame* df = args;
  // count f[i] = c[i] + stup_const * (c[i+1] - 2*c[i] + c[i-1])
  for (long i = 1; i < df->num_parts - 1; i++) {
    double* cur_temp = df->lp_cur_temp + i;
    double* fut_temp = df->lp_future_temp + i;
    *fut_temp = (*cur_temp) + STUPID_CONST * ((*(cur_temp+1)) - 2 * (*cur_temp) + (*(cur_temp-1)));
  }
}

// initial conditions:
// tau * const * const / (STEP * STEP) = 0.3
// start temperature eq 0 (except right side)
long calc_num_steps_time(double step_x, long const_c, double stupid_const) {
  double numerator = stupid_const * step_x * step_x;
  double denumerator = const_c * const_c;
  double step = numerator / denumerator;
  return (double)TIME / step;
  return numerator / denumerator;
}

double evaluate_point(double prev_cur, double cur, double next_cur) {
  return cur + STUPID_CONST * (next_cur - 2 * cur + prev_cur);
}

DataFrame* calc_boundary_conditions(DataFrame* data, double* cur_temp, double* future_temp) {
  long num_parts = data->num_parts;

  *(data->lp_future_temp) = (double)L_TEMP;
  *(data[NUM_THREADS - 1].rp_future_temp) = (double)R_TEMP;

  if (NUM_THREADS == 1) {
    return data;
  }

  *(data->rp_future_temp) = evaluate_point(*(data->rp_cur_temp-1),
                                           *(data->rp_cur_temp),
                                           *(data->rp_cur_temp+1)
                            );
  *(data[NUM_THREADS - 1].lp_future_temp) = evaluate_point(*(data[NUM_THREADS - 1].lp_cur_temp-1),
                                                           *(data[NUM_THREADS - 1].lp_cur_temp),
                                                           *(data[NUM_THREADS - 1].lp_cur_temp+1)
                                            );
  
  for (int worker = 1; worker < NUM_THREADS - 1; worker++) {
    *(data[worker].rp_future_temp) = evaluate_point(*(data[worker].rp_cur_temp-1), 
                                                    *(data[worker].rp_cur_temp), 
                                                    *(data[worker].rp_cur_temp+1)
                                     );
    *(data[worker].lp_future_temp) = evaluate_point(*(data[worker].lp_cur_temp-1), 
                                                    *(data[worker].lp_cur_temp), 
                                                    *(data[worker].lp_cur_temp+1)
                                     );
  }
  return data;
}

DataFrame* filling_intervals(DataFrame* data, double* cur_temp, double* future_temp, pthread_barrier_t* barrier) {
  double step_x = (double)LEN / NUM_SEGMENTS;
  long num_parts = NUM_SEGMENTS / NUM_THREADS;
  long used_num_parts = 0;

  // data for first worker
  data->lp_cur_temp = cur_temp;
  data->rp_cur_temp = cur_temp + num_parts - 1;
  data->lp_future_temp = future_temp;
  data->rp_future_temp = future_temp + num_parts - 1;
  data->num_parts = num_parts;
  used_num_parts += num_parts;

  for (int worker = 1; worker < NUM_THREADS; worker++) {
    data[worker].lp_cur_temp = data[worker-1].rp_cur_temp + 1;
    data[worker].lp_future_temp = data[worker-1].rp_future_temp + 1;
    
    if (worker != NUM_THREADS - 1) {
      used_num_parts += num_parts;
      data[worker].num_parts = num_parts;
      data[worker].rp_cur_temp = data[worker].lp_cur_temp + num_parts - 1;
      data[worker].rp_future_temp = data[worker].lp_future_temp + num_parts - 1;
    } else {
      data[NUM_THREADS - 1].num_parts = NUM_SEGMENTS - used_num_parts;
      data[NUM_THREADS - 1].rp_cur_temp = cur_temp + NUM_SEGMENTS - 1;
      data[NUM_THREADS - 1].rp_future_temp = future_temp + NUM_SEGMENTS - 1;
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    data[i].barrier = barrier;
  }

  return data;
}

void print_data(DataFrame* data) {
  for (int i = 0; i < NUM_THREADS; i++) {
    // for (int j = 0; j < data[i].num_parts; j++) {
    for (int j = 0; j < 4; j++) {
      printf("[%f] ", data[i].lp_cur_temp[j]);
    }
    printf("\n");
  }
  printf("---------------------\n");
}

void computation(int is_multithread) {
  sem_t sem;
  pthread_barrier_t barrier;
  pthread_barrier_init(&barrier, NULL, NUM_THREADS);

  pthread_t* threads = (pthread_t*)calloc(NUM_THREADS, sizeof(pthread_t));

  DataFrame* data = (DataFrame*)calloc(NUM_THREADS, sizeof(DataFrame));
  double* cur_temp = (double*)calloc(NUM_SEGMENTS, sizeof(double));
  double* future_temp = (double*)calloc(NUM_SEGMENTS, sizeof(double));

  // begin boundary conditions
  // test
  for (int i = 0; i < NUM_SEGMENTS; i++) {
    cur_temp[i] = i;
  }
  cur_temp[NUM_SEGMENTS - 1] = R_TEMP;

  data = filling_intervals(data, cur_temp, future_temp, &barrier);

  double step_x = (double)LEN / NUM_SEGMENTS;
  long num_steps_time = calc_num_steps_time(step_x, C, STUPID_CONST);
  printf("%ld\n", num_steps_time);
  for (int cur_step = 0; cur_step < num_steps_time; cur_step++) {
    // print_data(data);
    for (int worker = 0; worker < NUM_THREADS; worker++) {
      // computation next step time
      if (is_multithread) {
        int status = pthread_create((threads+worker), NULL, calc_future_temp_multithread, (void*)(data+worker));
        if (status != 0) {
          printf("main error: can't create thread, status = %d\n", status);
          exit(-1);
        }
      } else {      
        calc_future_temp_single((void*)(data+worker));
      }
    }

    swap_arrays(&cur_temp, &future_temp);
    for (int worker = 0; worker < NUM_THREADS; worker++) {
      swap_arrays(&(data[worker].lp_cur_temp), &(data[worker].lp_future_temp));
      swap_arrays(&(data[worker].rp_cur_temp), &(data[worker].rp_future_temp));
    }
    // preparing data to the next step
    calc_boundary_conditions(data, cur_temp, future_temp);
  }

  for (int worker = 0; worker < NUM_THREADS; worker++) {
    pthread_join(threads[worker], NULL);
  }


  print_data(data);

  free(cur_temp);
  free(future_temp);
  free(threads);
  free(data);
}

int main() {
  struct timeval start_time, end_time;
  gettimeofday(&start_time, NULL);
  computation(1);
  gettimeofday(&end_time, NULL);

  double time_spent = 
      (double)(end_time.tv_sec - start_time.tv_sec) + 
      (double)(end_time.tv_usec - start_time.tv_usec) / 10000000;

  printf("[time spent multithread: %f]\n", time_spent);

  gettimeofday(&start_time, NULL);
  computation(0);
  gettimeofday(&end_time, NULL);

  time_spent = 
      (double)(end_time.tv_sec - start_time.tv_sec) + 
      (double)(end_time.tv_usec - start_time.tv_usec) / 10000000;

  printf("[time spent single: %f]\n", time_spent);
  return 0;
}
