
#include <string>
#include <iostream>
using namespace std;

void UnescapeString(string &value) {
  string new_value(value);
  string::size_type last_pos = -1;
  string::size_type offset = 0;
  
  while ((last_pos = value.find("\\", last_pos + 1)) != string::npos) {
    string repl;
    string::size_type len = 1; 
    if (value[last_pos + 1] == '%') {
      repl = "%";
      len += 1;
    }

    offset = offset + (len - repl.size());
   
    new_value.replace(last_pos + offset, len, repl);
  } 
  
  value = new_value;
}


int main() {
  string foo("Hello there\\%");
  cout << foo << endl;
  UnescapeString(foo);
  cout << foo << endl;
  return 0;
}
