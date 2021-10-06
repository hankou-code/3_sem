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

#define TEMPLATE_INPUT "hvifp"
#define TRUE (1)
#define FALSE (0)
#define CONFIRMATION "yes"
#define CANCELLATION "no"
#define PERMISSION_DENIED_CODE 13

extern int errno;

typedef struct {
  char verbose;
  char interactive;
  char force;
  char preserve;
} Flags;

typedef struct DataFrame {
  char* data;
  int length;
} DataFrame;


void print_usage(char* str) {
  printf("Copying data from source_file_name to destination_file_name\n\n");
  printf("Usage:\n");
  printf("  %s source_file_name destination_file_name \n", str);
  printf("  %s [hvifp] source_file_name destination_file_name \n", str);
  printf("\n");
  printf("Options:\n");
  printf("  -h --help\t\t show this screen.\n");
  printf("  -v --verbose\t\t show what was done.\n");
  printf("  -i --interactive\t\t copying with confirmation.\n");
  printf("  -f --force\t forced copy.\n");
  printf("  -p --preserve\t copying metadata\n");
}

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

void write_data(FILE* stream, DataFrame df) {
  fwrite(df.data, sizeof(char), df.length, stream);
}

void print_verbose(char* src, char* dst) {
  printf("[%s] has been copied to [%s]\n", src, dst);
}


// looks like cute shit
void confirmation_of_copying() {
  printf("Are you sure? [yes/no]\n");
  char buf[1000];
  while (1) {
    scanf("%s", buf);
    if (!strcmp(buf, CONFIRMATION)) {
      return;
    } else if (!strcmp(buf, CANCELLATION)) {
      // cancel copying
      printf("Copying has been canceled.\n");
      exit(0);
    }
  }
}

void print_metadata(char* str) {
  struct stat sb;
  stat(str, &sb);

  // printf("File type:                ");

  // switch (sb.st_mode & S_IFMT) {
  //   case S_IFBLK:  printf("block device\n");            break;
  //   case S_IFCHR:  printf("character device\n");        break;
  //   case S_IFDIR:  printf("directory\n");               break;
  //   case S_IFIFO:  printf("FIFO/pipe\n");               break;
  //   case S_IFLNK:  printf("symlink\n");                 break;
  //   case S_IFREG:  printf("regular file\n");            break;
  //   case S_IFSOCK: printf("socket\n");                  break;
  //   default:       printf("unknown?\n");                break;
  // }

  // printf("I-node number:            %ld\n", (long) sb.st_ino);
  // printf("Mode:                     %lo (octal)\n", (unsigned long) sb.st_mode);
  // printf("Link count:               %ld\n", (long) sb.st_nlink);
  // printf("Ownership:                UID=%ld   GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);

  // printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
  // printf("File size:                %lld bytes\n", (long long) sb.st_size);
  // printf("Blocks allocated:         %lld\n", (long long) sb.st_blocks);

  // printf("Last status change:       %s", ctime(&sb.st_ctime));
  printf("Last file access:         %s", ctime(&sb.st_atime));
  printf("Last file modification:   %s", ctime(&sb.st_mtime));
}

void copy_metadata_from_file_to_file(char* src, char* dst) {
  struct stat sb;
  if (stat(src, &sb) == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  }

  // https://linux.die.net/man/2/fstat
  // receiving data
  mode_t st_mode = sb.st_mode;
  uid_t st_uid = sb.st_uid;
  gid_t st_gid = sb.st_gid;
  time_t last_access_time = sb.st_atime;
  time_t last_mode_time = sb.st_mtime;
  
  struct utimbuf src_time = {
    .actime = last_access_time, 
    .modtime = last_mode_time
  };


  // set data at destination
  chmod(dst, st_mode);
  chown(dst, st_uid, st_gid);
  utime(dst, &src_time);
}

// TODO: refactoring with switch/case
void copy_data_from_file_to_file(char* src, char* dst, Flags flags) {
  // first of all check if confirmation was added
  if (flags.interactive) {
    confirmation_of_copying();
  }

  FILE* src_stream = fopen(src, "r");
  DataFrame df = read_data(src_stream);

  FILE* dst_stream = fopen(dst, "w");

  if (flags.force && errno == PERMISSION_DENIED_CODE && dst_stream == NULL) {
    remove(dst);
    dst_stream = fopen(dst, "w");
  } else if (src_stream == NULL || dst_stream == NULL) {
    perror("Something went wrong when opening files.\n");
    exit(-1);
  }

  write_data(dst_stream, df);

  if (flags.preserve) {
    printf("-------------------was----------------------\n");
    print_metadata(src);
    printf("--------------------------------------------\n");
    print_metadata(dst);
    copy_metadata_from_file_to_file(src, dst);
    printf("--------------------------------------------\n\n");
    printf("-------------------is-----------------------\n");
    print_metadata(src);
    printf("--------------------------------------------\n");
    print_metadata(dst);
    printf("--------------------------------------------\n");
  }

  // print what was done if it is required
  if (flags.verbose) {
    print_verbose(src, dst);
  }

  fclose(src_stream);
  fclose(dst_stream);
  free(df.data);
}

int main(int argc, char* argv[]) {
  static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"verbose", no_argument, 0, 'v'},
    {"interactive", no_argument, 0, 'i'},
    {"force", no_argument, 0, 'f'},
    {"preserve", no_argument, 0, 'p'},
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
      case 'v':
        flags.verbose = TRUE;
        break;
      case 'i':
        flags.interactive = TRUE;
        break;
      case 'f':
        flags.force = TRUE;
        break;
      case 'p':
        flags.preserve = TRUE;
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

  // after reading options try to read filenames
  
  // check if this (2) file names was given
  if (argc - optind < 2) {
    printf("Too few arguments.\n");
    return -1;
  } else if (argc - optind > 2) {
    printf("Too many arguments.\n");
    return -2;
  }

  // reading file names
  char* src_file_name = argv[optind];
  char* dst_file_name = argv[optind + 1];

  copy_data_from_file_to_file(src_file_name, dst_file_name, flags);

  return 0;
}