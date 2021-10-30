#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF_SIZE 2

typedef struct ChunkOfData {
  char* data;
  int length;
  FILE* point_at_the_end;
  int is_last_chunk;
} ChunkOfData;

ChunkOfData read_part_of_data(FILE* stream, int max_length_of_data) {
  long start_point = ftell(stream);
  fseek(stream, 0, SEEK_END);
  long left_to_read = ftell(stream) - start_point;
  fseek(stream, start_point, SEEK_SET);

  char* string = NULL;
  ChunkOfData cod;

  if (left_to_read < max_length_of_data) {
    cod.is_last_chunk = 1;
    cod.length = left_to_read;
    cod.point_at_the_end = NULL;
    cod.data = malloc(left_to_read + 1);
    fread(cod.data, 1, left_to_read, stream);
    cod.data[left_to_read] = 0;
  } else {
    cod.is_last_chunk = 0;
    cod.length = max_length_of_data;
    cod.data = malloc(max_length_of_data + 1);
    fread(cod.data, 1, max_length_of_data, stream);
    cod.data[max_length_of_data] = 0;
    fseek(stream, start_point + max_length_of_data, SEEK_SET);
    cod.point_at_the_end = stream;
  }
  return cod;
}

// read data from pipe and write to dst file
void handle_child_process(int pipe_id, const char* dst) {
  int number_symbols;
  char* data = (char*)malloc((MAX_BUF_SIZE + 1) * sizeof(char));

  remove(dst);
  FILE* dst_stream = fopen(dst, "a");

  while(1) {
    read(pipe_id, &number_symbols, 1);
    read(pipe_id, data, number_symbols);
    fwrite(data, sizeof(char), number_symbols, dst_stream);
    if (number_symbols != MAX_BUF_SIZE) {
      break;
    }
  }

  fclose(dst_stream);
  close(pipe_id);
  free(data);
}

// read data from src file and write to pipe
void handle_parent_process(int pipe_id, const char* src) {
  FILE* src_stream = fopen(src, "r");
  ChunkOfData cod = {NULL, 0, src_stream, 0};

  do {
    cod = read_part_of_data(cod.point_at_the_end, MAX_BUF_SIZE);
    // write len(data) and data to pipe
    write(pipe_id, &cod.length, sizeof(char));
    write(pipe_id, cod.data, cod.length);
  } while(!cod.is_last_chunk);

  fclose(src_stream);
  free(cod.data);
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
