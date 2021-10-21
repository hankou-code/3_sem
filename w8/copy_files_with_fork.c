#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct DataFrame {
  char* data;
  int length;
} DataFrame;

// https://stackoverflow.com/questions/14002954/c-programming-how-to-read-the-whole-file-contents-into-a-buffer
DataFrame read_data(FILE* stream) {
  fseek(stream, 0, SEEK_END);
  long stream_size = ftell(stream);
  fseek(stream, 0, SEEK_SET);  /* same as rewind(stream); */
  
  char *string = malloc(stream_size + 1);
  fread(string, 1, stream_size, stream);
  string[stream_size] = 0;

  DataFrame df;
  df.data = string;
  df.length = stream_size;

  return df;
}

// read data from pipe and write to dst file
void handle_child_process(int pipe_id, const char* dst) {
  // read data from pipe
  int number_symbols;
  read(pipe_id, &number_symbols, sizeof(int));
  char* data = (char*)malloc(number_symbols * sizeof(char));
  read(pipe_id, data, number_symbols);

  // write data to dst
  FILE* dst_stream = fopen(dst, "w");
  fwrite(data, sizeof(char), number_symbols, dst_stream);

  // common part
  fclose(dst_stream);
  close(pipe_id);
  free(data);
}

// read data from src file and write to pipe
void handle_parent_process(int pipe_id, const char* src) {
  // read data
  FILE* src_stream = fopen(src, "r");
  DataFrame df = read_data(src_stream);

  // write number symbols and string to pipe
  write(pipe_id, &df.length, sizeof(int));
  write(pipe_id, df.data, df.length);

  // common part
  fclose(src_stream);
  free(df.data);
  close(pipe_id);
}


int main (void) {
  // mypipe[0] -- for reading
  // mypipe[1] -- for writing
  int mypipe[2];

  /* Create the pipe. */
  if (pipe (mypipe)) {
    fprintf (stderr, "Pipe failed.\n");
    return EXIT_FAILURE;
  }

  /* Create the child process. */
  pid_t pid;
  pid = fork ();
  if (pid == (pid_t) 0) {
  /* This is the child process. Close other end first. */
    close(mypipe[1]);
    handle_child_process(mypipe[0], "text.out");
    return EXIT_SUCCESS;
  } else if (pid < (pid_t) 0) {
  /* The fork failed. */
    fprintf(stderr, "Fork failed.\n");
    return EXIT_FAILURE;
  } else {
  /* This is the parent process. Close other end first. */
    close(mypipe[0]);
    handle_parent_process(mypipe[1], "text.in");
    return EXIT_SUCCESS;
  }
}