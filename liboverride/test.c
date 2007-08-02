#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>

int bind(int s, const struct sockaddr *addr, socklen_t addrlen) {
  void *handle = NULL;
  int (*real_func)(int, const struct sockaddr*, socklen_t) = NULL;
  handle = dlopen("/lib/libc.so.6", RTLD_LAZY);

  if (handle == NULL) {
    fprintf(stderr, "Failed to dlopen libc\n");
    return -1;
  }
  real_func = dlsym(handle, "bind");

  printf("bind called on socket %d\n", s);
  return real_func(s, addr, addrlen);
}
