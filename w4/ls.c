// обрабатывается лишь с текущей директории

#include <stdio.h>
#include <string.h>
#include <assert.h> // assert()
#include <stdlib.h> // exit()
#include <errno.h>  // errno
#include <unistd.h>
#include <ctype.h>

#include <dirent.h>

#include <ftw.h>

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <grp.h>


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <unistd.h>


#define TEMPLATE_INPUT "halR"

#define FLAG_a 0
#define FLAG_l 0
#define FLAG_R 0

void print_usage(char* str) {
  printf("usage: ./a.out -h \n\n");
  printf("options:\n");
  printf("\t \t List information about the FILEs\n");
  printf("\t-a \t do not ignore entries starting with .\n");
  printf("\t-l \t use a long listing format\n");
  printf("\t-R \t list subdirectories recursively\n");
}

// https://stackoverflow.com/questions/8436841/how-to-recursively-list-directories-in-c-on-linux
void recursively_print_listdir(const char *name, int indent) {
  DIR *dir;
  struct dirent *entry;

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

static const char * lookup[] = {"Jan", "Feb", "Mar", "Apr", "May"\
                                "Jun", "Jul", "Aug", "Sep", "Oct",\
                                "Nov", "Dec"};

void display_contents(char * name) {
  struct stat sb;
  struct tm * t;
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

void get_contents(DIR *d){

  struct dirent *entry;
  int i =0;
  while((entry = readdir(d)) != NULL){
    if(i < 2 ){
      i++;
      continue;
    } 
    display_contents(entry->d_name);
  }

}

int main(int argc, char* argv[]) {
  static struct option long_options[] = {
    {"long",     no_argument, 0, 'l'},
    {"recursive",  no_argument, 0, 'R'},
    {"all",  no_argument, 0, 'a'},
    {0, 0, 0, 0}
  };


  // TODO: читать реальную директорию, а не текущую
  char start_dirrectory[] = ".";

  int c;
  int index;

  opterr = 0;

  DIR *d;
  struct dirent *dir;

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

  int option_index = 0;
  // https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
  while ((c = getopt_long(argc, argv, TEMPLATE_INPUT, long_options, &option_index)) != -1) {
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
      case 'R':
    		recursively_print_listdir(start_dirrectory, 0);
        break;
      case '?':
        if (optopt == 'f') {
          fprintf(stderr, "Option -%c requires an argumen.\n", optopt);
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

  // заготовка под директорию
  char* ls_dir = argv[optind];
  printf("%s", ls_dir);


  for (index = optind; index < argc; index++) {
    printf("Non-option argument %s\n", argv[index]);
  }

  return 0;
}
