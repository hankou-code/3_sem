#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "explorer.h"
#include "calculator.h"
#include "config.h"

// https://coderoad.ru/822323/%D0%9A%D0%B0%D0%BA-%D1%81%D0%B3%D0%B5%D0%BD%D0%B5%D1%80%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D1%82%D1%8C-random-int-%D0%B2-C
int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Supporting larger values for n would requires an even more
    // elaborate implementation that combines multiple calls to rand()
    assert (n <= RAND_MAX);

    // Chop off all of the values that would cause skew...
    int end = RAND_MAX / n; // truncate skew
    assert (end > 0);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}


// create someone similiar to the: "0xFFFFFF"
void create_random_color(char* data) {
  data[0] = '\"';
  data[1] = '0';
  data[2] = 'x';

  for(int i = 0; i < 6; i++) {
    data[i + 3] = randint(6) + 65;
  }

  data[9] = '\"';
  data[10] = '\0';
}

void build_graph() {
  char path_to_files[1000];
  generate_several_lines_plot_command(path_to_files);

  char * commandsForGnuplot[] = {"set title \"TITLEEEEE\"", path_to_files};
  FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
  for (int i = 0; i < NUM_COMMANDS; i++) {
    fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
  }  
  for (int i = 0; i < NUM_COMMANDS; i++) {
    fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
  }
}

void generate_plot_command_of_one_line(char* data, int num_threads) {
  char path_to_file[100];
  path_to_file[0] = '\0';
  strcat(path_to_file, "\'graphs_data/");
  strcat(path_to_file, "data");
  char num_threads_str[100];
  sprintf(num_threads_str, "%d", num_threads);
  strcat(path_to_file, num_threads_str);
  strcat(path_to_file, ".temp\' ");

  strcat(path_to_file, " ti \'");
  strcat(path_to_file, num_threads_str);
  strcat(path_to_file, "\' ");

  char plot_commands[BUF_SIZE] = "\0";

  char color[11];
  create_random_color(color);

  char line_properties[BUF_SIZE] = "lt rgb ";
  strcat(line_properties, color);

  strcat(plot_commands, path_to_file);
  strcat(plot_commands, line_properties);

  data[0] = '\0';
  strcat(data, plot_commands);
}

// plot "ls.dat" using 2 title 'Removed' with lines, \
//    "ls.dat" using 3 title 'Added' with lines, \
//    "ls.dat" using 4 title 'Modified' with lines
void generate_several_lines_plot_command(char* path_to_files) {
  path_to_files[0] = '\0';
  strcat(path_to_files, "plot ");
  for (int i = MIN_THREADS; i <= MAX_THREADS; i++) {
    char path_to_file[100];
    generate_plot_command_of_one_line(path_to_file, i);
    strcat(path_to_files, path_to_file);
    if (i != MAX_THREADS) {
      strcat(path_to_files, ",");
    }
  }
}