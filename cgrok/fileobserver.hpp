#ifndef __FILEOBSERVER_HPP
#define __FILEOBSERVER_HPP

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>

#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct DataInput {
  FILE *fd;
  string data;

  bool is_command; /* is the 'filename' actually a command? */
  string shell;    /* only set if is_command == true */
  bool follow;     /* Only valid on files (when is_command == false) */
};

class FileObserver {
  public:
    typedef pair < DataInput, string > data_pair_type;
    typedef vector < data_pair_type > data_input_vector_type;

    FileObserver() : inputs(), old_buffer() { }

    void AddCommand(string command) {
      DataInput di;
      di.data = command;
      di.is_command = true;
      di.shell = "/bin/sh";

      cout << "Adding cmd: " << command << endl;
      this->inputs.push_back(di);
    }

    void AddFile(string filename) {
      DataInput di;
      di.data = filename;
      di.fd = NULL;
      di.is_command = false;
      di.follow = true;
      cout << "Adding file: " << filename << endl;

      this->inputs.push_back(di);
    }

    void AddFileCommand(string command) {
      FILE *fd = popen(command.c_str(), "r");
      char buf[4096];

      while (fgets(buf, 4096, fd) != NULL) {
        buf[ strlen(buf) - 1 ] = '\0';
        string filename(buf);
        this->AddFile(buf);
      }
    }

    void Merge(FileObserver fo) {
      vector<DataInput>::iterator di_iter;
      for (di_iter = fo.inputs.begin(); di_iter != fo.inputs.end(); di_iter++) {
        this->inputs.push_back(*di_iter);
      }

      map <int, string>::const_iterator buffer_iter);
      for (buffer_iter = fo.old_buffers.begin(); 
           buffer_iter != fo.old_buffers.end();
           buffer_iter++) {
        this->old_buffers[ (*buffer_iter).first ] = (*buffer_iter).second;
      }
      
    }

    void OpenAll() {
      vector<DataInput>::iterator iter;
      vector<DataInput> inputs_copy = this->inputs;
      this->inputs.clear();
      for (iter = inputs_copy.begin(); iter != inputs_copy.end(); iter++) {
        DataInput di = *iter;
        if (di.is_command) {
          this->OpenCommand(di);
        } else {
          this->OpenFile(di);
        }
        cout << "openall fd: " << di.fd << endl;
        /* Hack to store the modified DataInput object back */
        this->inputs.push_back(di);
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

    void OpenCommand(DataInput &di) {
      di.fd = popen(di.data.c_str(), "r");
      if (di.fd == NULL) {
        cerr << "OpenCommand failed on command: '" << di.data << "'" << endl;
        cerr << "Error was: " << strerror(errno) << endl;
        return;
      }

      cerr << "OpenCommand success on command: '" << di.data << "'" << endl;
    }

    void ReadLines(float timeout, data_input_vector_type &data) {
      int ret;
      vector<DataInput>::iterator iter;
      struct timeval tv;
      fd_set in_fdset;
      fd_set err_fdset;

      tv.tv_sec = (long)timeout;
      tv.tv_usec = (long)(timeout - (tv.tv_sec)) * 1000000L;

      /* Come up with a descriptor set that includes all of the inputs */
      FD_ZERO(&in_fdset);
      FD_ZERO(&err_fdset);
      for (iter = this->inputs.begin(); iter != this->inputs.end(); iter++) {
        cout << "File: " << (*iter).fd << endl;
        if ( (*iter).fd != NULL ) {
          FD_SET(fileno( (*iter).fd ), &in_fdset);
          FD_SET(fileno( (*iter).fd ), &err_fdset);
        } else {
          cout << "Skipping file (no open fd): " << (*iter).data << endl;
        }
      }

      ret = select(FD_SETSIZE, &in_fdset, NULL, &err_fdset, &tv);
      if (ret == -1) {
        cerr << "Error in select() call" << endl;
        return;
      } else if (ret == 0) {
        return; /* Nothing to read */
      }
      cout << "Select returned " << ret << endl;

      /* else, ret > 0, read data from the inputs */
      for (iter = this->inputs.begin(); iter != this->inputs.end(); iter++) {
        DataInput di = *iter;
        if (FD_ISSET(fileno(di.fd), &in_fdset))
          ReadLinesFromInput(data, di);
        else if (FD_ISSET(fileno(di.fd), &err_fdset))
          cout << "Error condition on " << di.data << endl;
      }
    }

    void ReadLinesFromInput(data_input_vector_type &data, DataInput &di) {
      fd_set fdset;
      bool done;
      struct timeval tv;
      int ret;
      string buffer = this->old_buffers[fileno(di.fd)];
      char readbuf[4096];

      cout << "Reading from: " << di.data << endl;

      while (!done) {
        FD_ZERO(&fdset);
        FD_SET(fileno(di.fd), &fdset);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        ret = select(FD_SETSIZE, &fdset, NULL, &fdset, &tv);

        /* XXX: Handle '< 0' and '== 0' cases separately */
        if (ret <= 0) {
          done = true;
          continue;
        }

        int bytes;

        bytes = fread(readbuf, 1, 4096, di.fd);
        if (bytes == 0)
          done = true;
        else
          buffer.append(readbuf, bytes);

        /* Cut this short if the buffer is big enough */
        if (buffer.size() > 5<<10)
          done = true;
      }

      /* Look for full lines of text, push them into the data vector */
      int pos = 0;
      int last_pos = 0;
      while ((pos = buffer.find("\n", last_pos)) != string::npos) {
        data_pair_type p;
        p.first = di;
        p.second = buffer.substr(last_pos, (pos - last_pos));
        //cout << "Found line: " << p.second.size() << endl;
        //cout << "data: " << last_pos << " / " << buffer.length() << endl;
        data.push_back(p);
        last_pos = pos + 1;
      }
      
      string remainder;
      if (last_pos <= buffer.size() - 1)
         remainder = buffer.substr(last_pos, buffer.size() - last_pos);
      old_buffers[fileno(di.fd)] = remainder;
    }

  protected:
    vector<DataInput> inputs;
    map <int, string> old_buffers;
};

#endif /* ifdef __FILEOBSERVER_HPP */
