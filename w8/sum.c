// https://stackoverflow.com/questions/4812891/fork-and-pipes-in-c/4812963

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/* Read numbers from the pipe and print sum of them. */
void read_from_pipe (int file) {
  FILE* stream = fdopen(file, "r");
  int number;
  int sum = 0;
  
  while (fscanf(stream, "%d", &number) == 1) { 
    sum += number;
  }

  printf("%d\n", sum);
  fclose(stream);
}

/* Write numbers to the pipe. */
void write_to_pipe (int file) {
  int number;
  FILE* stream = fdopen (file, "w");
  while(scanf("%d", &number) == 1) {
    fprintf(stream, "%d ", number);
  }
  fclose(stream);
}

int main (void) {
  pid_t pid;
  int mypipe[2];

  /* Create the pipe. */
  if (pipe (mypipe)) {
    fprintf (stderr, "Pipe failed.\n");
    return EXIT_FAILURE;
  }

  /* Create the child process. */
  pid = fork ();
  if (pid == (pid_t) 0) {
  /* This is the child process. Close other end first. */
    close(mypipe[1]);
    read_from_pipe(mypipe[0]);
    return EXIT_SUCCESS;
  } else if (pid < (pid_t) 0) {
  /* The fork failed. */
    fprintf (stderr, "Fork failed.\n");
    return EXIT_FAILURE;
  } else {
  /* This is the parent process. Close other end first. */
    close (mypipe[0]);
    write_to_pipe (mypipe[1]);
    return EXIT_SUCCESS;
  }
}
