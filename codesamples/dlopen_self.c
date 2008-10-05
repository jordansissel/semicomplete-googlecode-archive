#include <stdio.h>
#include <dlfcn.h>

void foo() { printf("Hello\n"); }

int main() {
  void *handle = NULL;
  void (*myfunc)(void) = NULL;

  handle = dlopen(NULL, RTLD_LAZY);
  myfunc = dlsym(handle, "foo");
  if (myfunc == NULL) {
    printf("e: %s\n", dlerror());
  } else {
    myfunc();
  }
  return 0;
}
