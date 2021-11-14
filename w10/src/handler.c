#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>
#include <stdlib.h>

#include "handler.h"
#include "calculator.h"
#include "config.h"

void write_data_to_the_file(int num_threads, int flags) {
  char path_to_file[100];
  path_to_file[0] = '\0';
  strcat(path_to_file, "graphs_data/");
  strcat(path_to_file, "data");
  
  char num_threads_str[100];
  sprintf(num_threads_str, "%d", num_threads);
  
  strcat(path_to_file, num_threads_str);
  strcat(path_to_file, ".temp");

  int file = open(path_to_file, O_CREAT|O_RDWR|O_TRUNC, RWX_PERMISSION);
  if (file == -1) {
    printf(" Value of errno: %d\n ; %s\n", errno, path_to_file);
    exit(0);
  }

  dup2(file, 1);

  if (flags & 1) {
    for (long num_segments = MIN_SEGMENTS; num_segments <= MAX_SEGMENTS; num_segments += SEGMENT_ADD_STEP) {
      printf("%ld %f\n", num_segments, find_avg_time_and_avg_res(num_segments, num_threads, NULL));
    }
  }
  if (flags & 2) {
    for (long num_segments = MIN_SEGMENTS; num_segments <= MAX_SEGMENTS; num_segments *= SEGMENT_MULTIPLIER) {
      printf("%ld %f\n", num_segments, find_avg_time_and_avg_res(num_segments, num_threads, NULL));
    }
  }

  dup2(1, file);
  close(file);
}
