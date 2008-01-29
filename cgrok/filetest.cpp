
#include <string>
using namespace std;

#include "fileobserver.hpp"

int main() {
  FileObserver fo;
  string messages("/var/log/messages");
  string auth("/var/log/auth.log");
  string lscmd("ls /var/log/*.log");
  string tailcmd("tail -f /var/log/auth.log");
  fo.AddFile(messages);
  //fo.AddFile(auth);
  //fo.AddFileCommand(lscmd);
  //fo.AddCommand(tailcmd);

  FileObserver::data_input_vector_type input_vector;
  FileObserver::data_pair_type input_pair;
  FileObserver::data_input_vector_type::iterator iter;

  fo.OpenAll();
  fo.ReadLines(30, input_vector);
  for (iter = input_vector.begin(); iter != input_vector.end(); iter++) {
    input_pair = *iter;
    cout << "(" << input_pair.first.data << ") " << input_pair.second << endl;
  }

  return 0;
}
