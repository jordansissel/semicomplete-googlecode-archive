
#include <iostream>
#include <fstream>
#include <string>
#include <boost/xpressive/xpressive.hpp>

#include "grokpatternset.hpp"
#include "grokregex.hpp"
#include "grokmatch.hpp"
#include "grokconfig.hpp"

using namespace std;
using namespace boost::xpressive;

#define CONFIG_BUFSIZE 4096

int main(int argc, char **argv) {
  GrokConfig config;
  ifstream in(argv[1]);
  string config_data = "";
  char buffer[CONFIG_BUFSIZE];
  int bytes = 0;

  while (!(in.eof() || in.fail())) {
    in.read(buffer, CONFIG_BUFSIZE);
    bytes = in.gcount();
    buffer[bytes] = '\0';
    config_data += buffer;
  }

  config.parse(config_data);

  GrokConfig::watch_file_vector_type::iterator iter;
  GrokConfig::watch_file_vector_type watches;

  watches = config.GetFileEntries();
  map < int, DataInput > inputmap;
  for (iter = watches.begin(); iter != watches.end(); iter++) {
    cout << "Name: " << (*iter).name << endl;
  }

  return 0;
}
