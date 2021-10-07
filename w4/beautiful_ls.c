// https://stackoverflow.com/questions/8436841/how-to-recursively-list-directories-in-c-on-linux
//
// https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program/17683417
//
// https://man7.org/linux/man-pages/man2/lstat.2.html
//
// https://github.com/truncs/ls-implementation/blob/master/ls.c

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


#define TEMPLATE_INPUT "halR"
#define TRUE (1)
#define FALSE (0)

typedef struct {
  char all;
  char _long;
  char recursive;
} Flags;

void print_usage(char* str) {
  printf("Show directory.\n\n");
  printf("Usage:\n");
  printf("  %s \n", str);
  printf("  %s [hal]\n", str);
  printf("\n");
  printf("Options:\n");
  printf("  -h --help\t\t Show this screen.\n");
  printf("  -a --all\t\t Do not ignore entries starting with .\n");
  printf("  -l --long\t\t Use a long listing format\n");
  printf("  -R --recursive\t list subdirectories recursively\n");
}

void display_short_content(char* name, int indent) {
  printf("%*s- %s\n", indent, "", name);
}

void display_long_content(char* name, int indent, const char* path) {
  static const char* lookup[] = {"Jan", "Feb", "Mar", "Apr", "May"\
                                 "Jun", "Jul", "Aug", "Sep", "Oct",\
                                 "Nov", "Dec"};

  printf("%*s- ", indent, "");
  struct stat sb;
  struct tm* t;
  char link_read[255];
  ssize_t bytes_read;

  char full_name[1024];
  full_name[0] = '\0';
  if (!strcmp(name, ".")) {
    strncat(full_name, name, 255);
  } else {  
    strncat(full_name, path, 255);
    strncat(full_name, "/", 255);
    strncat(full_name, name, 255);
  }

  lstat(name, &sb);

  int gid = getgid();
  int uid = getuid();
  char* gr_name = getgrgid(gid)->gr_name;
  char* user_name = getpwuid(uid)->pw_name;
  

  printf("%c", S_ISDIR(sb.st_mode)?'d':\
         S_ISFIFO(sb.st_mode)?'p':\
         S_ISLNK(sb.st_mode)?'l':'-');
         
  
  printf("%c", (S_IRUSR & sb.st_mode) ? 'r' : '-');
  printf("%c", (S_IWUSR & sb.st_mode) ? 'w' : '-');
  printf("%c", (S_IXUSR & sb.st_mode) ? 'x' : '-');
  printf("%c", (S_IRGRP & sb.st_mode) ? 'r' : '-');
  printf("%c", (S_IWGRP & sb.st_mode) ? 'w' : '-');
  printf("%c", (S_IXGRP & sb.st_mode) ? 'x' : '-');
  printf("%c", (S_IROTH & sb.st_mode) ? 'r' : '-');
  printf("%c", (sb.st_mode & S_IWOTH) ? 'w' : '-');
  printf("%c  ", (S_IXOTH & sb.st_mode) ? 'x' : '-');
  nlink_t nlink = sb.st_nlink;
  printf("%ld\t", nlink);
  printf("%s\t", user_name);
  printf("%s\t", gr_name);
  printf("%5.0lu ", sb.st_size);
  t = localtime(&sb.st_ctime);
  printf("%s ", lookup[t->tm_mon]);
  printf("%2.0d %2.0d:%2.0d ", t->tm_mday, t->tm_hour, t->tm_min);
  if(S_ISLNK(sb.st_mode)){
    printf("@\033[35m%s ",name );
    printf("\033[37m");
          bytes_read = readlink(name, link_read, 254);
    link_read[bytes_read] = '\0';
    printf("-> %s\n", link_read);
  }
  else if((S_IXUSR & sb.st_mode) || (S_IXGRP & sb.st_mode) || (S_IXOTH & sb.st_mode))
    printf("\033[31m%s\033[37m\n", name);
  else if(S_ISFIFO(sb.st_mode))
    printf("\033[33m%s\033[37m\n", name);
  else 
    printf("%s\n", name);
}


void get_contents(DIR* d, Flags flags, char* directory_name){
  struct dirent* entry;
  int i = 0;
  while ((entry = readdir(d)) != NULL) {
    // only with flag "all" included
    if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
      if (flags.all && flags._long) {
        display_long_content(entry->d_name, 0, directory_name);
      } else if (flags.all) {
        display_short_content(entry->d_name, 0);
      } 
      continue;
    }
    // check long flag
    if (flags._long) {
      display_long_content(entry->d_name, 0, directory_name);
    } else {
      display_short_content(entry->d_name, 0);
    }
  }

}


void recursively_print_listdir(const char* name, Flags flags, int indent) {
  DIR* dir;
  struct dirent* entry;

  if (!(dir = opendir(name)))
    return;

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_DIR) {
      char path[1024];
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      } 
      snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
      printf("%*s[%s]\n", indent, "", entry->d_name);
      recursively_print_listdir(path, flags, indent + 2);
    } else {
      if (flags._long) {
        display_long_content(entry->d_name, indent, name);
      } else {
        display_short_content(entry->d_name, indent);
      }
    }
  }
  closedir(dir);
}

void show_directory(char* directory_name, Flags flags) {
  if (!flags.recursive) {
    DIR* dir = opendir(directory_name);
    get_contents(dir, flags, directory_name);
    closedir(dir);
  } else {
    recursively_print_listdir(directory_name, flags, 0);
  }


}

int main(int argc, char* argv[]) {
  static struct option long_options[] = {
    {"long",     no_argument, 0, 'l'},
    {"recursive",  no_argument, 0, 'R'},
    {"all",  no_argument, 0, 'a'},
    {"directory", required_argument, 0, 'd'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
  };

  Flags flags;

  // reading options
  while (1) {
    int option_index = 0;
    int c = getopt_long(argc, argv, TEMPLATE_INPUT, long_options, &option_index);
    if (c == -1) {
      break;
    }

    switch(c) {
      case 'h':
        print_usage(argv[0]);  // file name
        exit(0);
      case 'a':
        flags.all = TRUE;
        break;
      case 'R':
        flags.recursive = TRUE;
        break;
      case 'l':
        flags._long = TRUE;
        break;
      case '?':
        if (isprint(optopt)) {
          fprintf(stderr, "Unknown option '-%c'.\n", optopt);
        } else {
          fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
        } 
        exit(EXIT_FAILURE);
      default:
        // the task of this garbage is not fully understood
        abort();
    }
  }

  // after reading options try to read directory
  char* directory_name = ".";

  if (argc - optind == 1) {
    directory_name = argv[optind];
  } else if (argc - optind > 1) {
    perror("Too many arguments.\n");
    return -1;
  }

  show_directory(directory_name, flags);

  return 0;
}