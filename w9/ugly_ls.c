#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <utime.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>

void display_short_content(char* name) {
  // stub to prevent reading binaries
  if (name[0] != '.' && name[0] != 'a' && name[0] != 'l') {
    printf("%s ", name);
  }
}

void get_contents(DIR* d, char* directory_name){
  struct dirent* entry;
  while ((entry = readdir(d)) != NULL) {
    display_short_content(entry->d_name);
  }
}

void show_directory(char* directory_name) {
  DIR* dir = opendir(directory_name);
  get_contents(dir, directory_name);
  closedir(dir);
}

int main(int argc, char* argv[]) {
  char* directory_name = ".";
  show_directory(directory_name);
  return 0;
}