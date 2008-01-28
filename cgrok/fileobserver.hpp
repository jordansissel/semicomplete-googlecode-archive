#ifndef __FILEOBSERVER_HPP
#define __FILEOBSERVER_HPP

#include <vector>
#include <string>
#include <iostream>

using namespace std;

#include <stdio.h>
#include <errno.h>
#include <string.h>

struct DataInput {
  FILE *fd;
  string data;

  bool is_command; /* is the 'filename' actually a command? */
  string shell;    /* only set if is_command == true */
  bool follow;     /* Only valid on files (when is_command == false) */
};

class FileObserver {
  public:
    FileObserver() : inputs() { }

    void AddCommand(string command, string shell) {
      DataInput di;
      di.data = command;
      di.is_command = true;
      di.shell = shell;

      /* commands not quite supported yet */
    }

    void AddFile(string filename) {
      DataInput di;
      di.data = filename;
      di.fd = NULL;//fopen(filename.c_str(), "r");

      this->inputs.push_back(di);
    }

    void OpenAll() {
      vector<DataInput>::iterator iter;
      for (iter = this->inputs.begin(); iter != this->inputs.end(); iter++) {
        DataInput di = *iter;
        if (di.is_command) {
          cerr << "Commands not yet supported" << endl;
        } else {
          this->OpenFile(di);
        }
      }
    }

    void OpenFile(DataInput &di) {
      di.fd = fopen(di.data.c_str(), "r");
      if (di.fd == NULL) {
        cerr << "OpenFile failed on file: '" << di.data << "'" << endl;
        cerr << "Error was: " << strerror(errno) << endl;
        return;
      }

      cerr << "OpenFile success on file: '" << di.data << "'" << endl;
    }

  private:
    vector<DataInput> inputs;
};

#endif /* ifdef __FILEOBSERVER_HPP */
