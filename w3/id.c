#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

int main() {
  int gid = getgid();
  int uid = getuid();
  char* gr_name = getgrgid(gid)->gr_name;
  char* user_name = getpwuid(uid)->pw_name;

  printf("uid=%d(%s) ", uid, user_name);
  printf("gid=%d(%s) ", gid, gr_name);
  printf("groups=%d(%s)", gid, gr_name);
  char* current_user_name = getpwuid(uid)->pw_name;  

  struct group* cur_group = NULL;
  int group_include_user_name = FALSE;

  while((cur_group = getgrent()) != NULL) {
    char** group_members = cur_group->gr_mem;
    int i = 0;
    char* member = NULL;
    for (int i = 0; (member = group_members[i]) != NULL; i++) {
      if (!strcmp(member, current_user_name)) {
	group_include_user_name = TRUE;
      }
    }
    if (group_include_user_name) {
      printf(",%d(%s)", cur_group->gr_gid, cur_group->gr_name);
      group_include_user_name = FALSE;
    }
  }

  return 0;
}
