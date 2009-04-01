#include <stdio.h>
#include <unistd.h>

char *const *environ;
int main(int argc, char **argv) {
  int i;
  char *cmd;
  char *newargv[4];
  printf("argc: %d\n", argc);

  asprintf(&cmd, "%s %s", argv[1], argv[2]);
  newargv[0] = "/bin/sh";
  newargv[1] = "-c";
  newargv[2] = cmd;
  newargv[3] = NULL;
  execve("/bin/sh", newargv, environ);
  return -1;
}
