#include <stdio.h>
#include <string.h>
#include <stdlib.h> // exit()

#include <assert.h> // assert()
#include <errno.h>  // errno
#include <unistd.h>
#include <ctype.h>

#include <dirent.h>
#include <time.h>

#include <ftw.h>
#include <pwd.h>
#include <grp.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <getopt.h>

#define TEMPLATE_INPUT "hald:R"

void print_usage(char* str) {
  printf("Usage:\n");
  printf("  %s \n", str);
  printf("  %s [halR]\n", str);
  printf("  %s [d] <path>\n", str);
  printf("\n");
  printf("Options:\n");
  printf("  -h --help\t\t Show this screen.\n");
  printf("  -a --all\t\t Do not ignore entries starting with .\n");
  printf("  -l --long\t\t Use a long listing format\n");
  printf("  -d --directory\t Set directory [default: .]\n");
  printf("  -R --recursive\t list subdirectories recursively\n");
}

// https://stackoverflow.com/questions/8436841/how-to-recursively-list-directories-in-c-on-linux
void recursively_print_listdir(const char* name, int indent) {
  DIR* dir;
  struct dirent* entry;

  if (!(dir = opendir(name)))
    return;

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_DIR) {
      char path[1024];
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;
      snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
      printf("%*s[%s]\n", indent, "", entry->d_name);
      recursively_print_listdir(path, indent + 2);
    } else {
      printf("%*s- %s\n", indent, "", entry->d_name);
    }
  }
  closedir(dir);
}

static const char* lookup[] = {"Jan", "Feb", "Mar", "Apr", "May"\
                                "Jun", "Jul", "Aug", "Sep", "Oct",\
                                "Nov", "Dec"};

void display_contents(char* name) {
  struct stat sb;
  struct tm* t;
  char link_read[255];
  ssize_t bytes_read;
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

void get_contents(DIR* d){

  struct dirent* entry;
  int i = 0;
  while ((entry = readdir(d)) != NULL) {
    if (i < 2) {
      i++;
      continue;
    } 
    display_contents(entry->d_name);
  }

}

int main(int argc, char* argv[]) {
  char* start_dirrectory = ".";

  opterr = 0;

  DIR* d;
  struct dirent* dir;

  if (argc == 1) {
    d = opendir(start_dirrectory);
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] != '.') {
          printf("%s\n", dir->d_name);
        }
      }
    closedir(d);
    }
  }
  
  static struct option long_options[] = {
    {"long",     no_argument, 0, 'l'},
    {"recursive",  no_argument, 0, 'R'},
    {"all",  no_argument, 0, 'a'},
    {"directory", required_argument, 0, 'd'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
  };


  // https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
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
        // https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program/17683417
        d = opendir(start_dirrectory);
        if (d) {
          while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
          }
        closedir(d);
        }
        break;
      case 'l':
        // https://man7.org/linux/man-pages/man2/lstat.2.html
        //
      	// https://github.com/truncs/ls-implementation/blob/master/ls.c
        d = opendir(start_dirrectory);
        get_contents(d);
        break;
      case 'd':
        start_dirrectory = optarg;
        break;
      case 'R':
        recursively_print_listdir(start_dirrectory, 0);
        break;
      case '?':
        if (optopt == 'd') {
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        } else if (isprint(optopt)) {
          fprintf(stderr, "Unknown option '-%c'.\n", optopt);
        } else {
          fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
        } 
        return 1;
      default:
        abort();
    }
  }

  for (int index = optind; index < argc; index++) {
    printf("Non-option argument %s\n", argv[index]);
  }

  return 0;
}
