/*
Задание 3: 0.5 балла. Создать пайп и родить процесс. С помощью
функции exec (или execvp, execl и т.д.) заменить ребенка программой
myls (простой, без флагов) и то, что она должна вывести на экран, вы-
вести в пайп (поменять место вывода с помощью dup2). Родительский
процесс должен считать из пайпа имена всех файлов, выведенных с по-
мощью ls, и вывести все их содержимое на экран.
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define MAX_BUF_SIZE 1000

void cat(const char* filename) {
  int ch;
  FILE *file = fopen(filename, "r");
  while (!feof(file) && !ferror(file)) {
    ch = getc(file);
    if (ch != EOF) {
      putchar(ch);
    }
  }
  fclose(file);
  putchar('\n');
}

// write name files to pipe
void handle_child_process(int pipe_id) {
  char *args[]={"./ls", NULL};
  dup2(pipe_id, 1);
  execv(args[0], args);
}

// read name files from pipe
void handle_parent_process(int pipe_id) {
  int number_symbols = MAX_BUF_SIZE;
  char* data = (char*)calloc((MAX_BUF_SIZE + 1), sizeof(char));
  read(pipe_id, data, number_symbols);

  char name_of_file[MAX_BUF_SIZE];

  int shift = 0;
  while(sscanf(data + shift, "%s", name_of_file) == 1) {
    shift += strlen(name_of_file) + 1;
    printf("----------------------------------\n");
    printf("[%s]\n", name_of_file);
    printf("----------------------------------\n");
    cat(name_of_file);
  }

  close(pipe_id);
  free(data);
}


int main (void) {
  // my_pipe[0] -- for reading
  // my_pipe[1] -- for writing
  int my_pipe[2];

  /* Create the pipe. */
  if (pipe (my_pipe)) {
    fprintf (stderr, "Pipe failed.\n");
    return EXIT_FAILURE;
  }

  /* Create the child process. */
  pid_t pid;
  pid = fork ();
  if (pid == (pid_t) 0) {
  /* This is the child process. Close other end first. */
    close(my_pipe[0]);
    handle_child_process(my_pipe[1]);
    return EXIT_SUCCESS;
  } else if (pid < (pid_t) 0) {
  /* The fork failed. */
    fprintf(stderr, "Fork failed.\n");
    return EXIT_FAILURE;
  } else {
  /* This is the parent process. Close other end first. */
    close(my_pipe[1]);
    handle_parent_process(my_pipe[0]);
    return EXIT_SUCCESS;
  }
}
