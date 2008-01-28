
#include <string>
using namespace std;

#include "fileobserver.hpp"

int main() {
  FileObserver fo;
  string foo("/var/log/messages");
  fo.AddFile(foo);
  fo.OpenAll();

  return 0;
}
