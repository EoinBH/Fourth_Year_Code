#include "StringUtil.h"
#include <sstream>
#include <stdlib.h>
#include <time.h>

string output_suffix;
string input_suffix;

// converts an STL string to an int
int stl_to_int(string s) {
  int i;
  i = strtol((char *)s.c_str(),NULL,10);
  return i;
}

// converts an int to an STL string
string int_to_stl(int i) {
  string s;
  ostringstream ss;
  ss << i;
  s = ss.str();
  return s;
}

void tolower(string& s) {
  // switched Jul 30
  for(size_t i=0; i < s.size(); i++) {
    s[i] = tolower(s[i]);
  }

}

string time_stamp(void)
{
  time_t  cal_time;
  
  cal_time = time(NULL);
  char * time_ptr;
  time_ptr = ctime(&cal_time); 
  string time_string;
  time_string = string(time_ptr);
  size_t spc_pos=0;
  while((spc_pos = time_string.find(' ',spc_pos)) != string::npos) {
    time_string[spc_pos] = '_';
    spc_pos++;
  }
  // to get rid of newline at end
  time_string = time_string.substr(0,time_string.size()-1);
  return time_string;
}

string add_suffix(string name_in) {
  string name_out;
  name_out = name_in;
  if(output_suffix != "") {
    name_out += "_";
    name_out += output_suffix;
  }
  return name_out;

}

string add_input_suffix(string name_in) {
  string name_out;
  name_out = name_in;
  if(input_suffix != "") {
    name_out += "_";
    name_out += input_suffix;
  }
  return name_out;

}

// eg. path = A/B/C/d
// --> dir = A/B/C,  file = d
// note dir part does not incl final /
void dir_and_file(string path, string& dir, string& file) {

  size_t old_pos = 0;
  size_t  new_pos;
  while((new_pos = path.find('/',old_pos)) != string::npos) {
    old_pos = new_pos+1; 
  }

  file = path.substr(old_pos);
  dir = path.substr(0,old_pos-1);
  // cout << "dir: " << dir << " and file " << file << endl;
}
