#include "CmdLine.h"
#include <iostream>
#include <utility>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

using namespace std;



ArgVal::ArgVal(string id, char *v) {
  arg_id = id;
  arg_val = v;
}

void ArgVal::show() {
  cout << "arg: " << arg_id << " val: " << arg_val << endl;
}

vector<ArgVal> parse_command_line(int argc, char **argv) {


  vector<ArgVal> all_params;
  string arg_id;
  char *arg_val;
  // check that argc-1 is even ?
  if(((argc-1) % 2) != 0) {
    cout << "not pairing off argument identifiers with argument values\n";
    exit(1);
  }

  unsigned int i = 1;
  while(i < argc) {
    ArgVal av_pair(string(argv[i]),argv[i+1]);
    if(av_pair.arg_id[0] != '-') {
      cout << "argument identifiers should begin with -\n";
      cout << av_pair.arg_id << " does not\n";
      exit(1);
    }
    else {
      av_pair.arg_id = av_pair.arg_id.substr(1);
      all_params.push_back(av_pair); 
      i = i + 2;
    }

  }

  // for(int i=0; i < all_params.size(); i++) {
  //   all_params[i].show();

  // }

  return all_params; 

}

vector<string> params_as_seq;

vector<ArgVal> parse_fresh_params(string new_params_line, bool &ok) {

  vector<ArgVal> all_params;
  ok = true;
  // make sequence of strings from new_params_line, just segementing on space
  //vector<string> params_as_seq;
  params_as_seq.clear();
  string::iterator word_itr, space_itr;
  string token = "";
  word_itr = new_params_line.begin();             /* word_itr is beginning of new_params_line */
  space_itr = find(word_itr,new_params_line.end(),' '); /* find space */
  while(space_itr != new_params_line.end()) {
    token = string(word_itr,space_itr);
    params_as_seq.push_back(token);
    word_itr = space_itr+1;
    space_itr = find(word_itr,new_params_line.end(),' '); /* find space */
  }
  token = string(word_itr,space_itr);
  params_as_seq.push_back(token);

  if(params_as_seq.size() % 2 != 0) {
    cout << "not pairing off argument identifiers with argument values\n";
    ok = false;
    return all_params;
  }

  // turn params_as_seq into a vector<ArgVal> sequence instead
  

  string arg_id;
  char *arg_val;

  unsigned int i = 0;
  while(i < params_as_seq.size()) {
  arg_id = params_as_seq[i];
  arg_val = (char *)(params_as_seq[i+1].c_str());
  ArgVal av_pair(arg_id,arg_val);
    if(av_pair.arg_id[0] != '-') {
      cout << "argument identifiers should begin with -\n";
      cout << av_pair.arg_id << " does not\n";
      ok = false; return all_params;
    }
    else {
      av_pair.arg_id = av_pair.arg_id.substr(1);
      all_params.push_back(av_pair); 
      i = i + 2;
    }

  }
		 
  return all_params; 

}


bool check_for_help(ParList l) {
  for(int i=0; i < l.size(); i++) {
    if((l[i].arg_id == "help") && (string(l[i].arg_val) == "yes")) {
      return true;
    }
  }
  return false;
}

// this shows/saves verbatim the command-line params
void show_params(ParList l, ostream& f) {
  f << "this is the literal command-line args that were passed:\n";
  f << "********************************************\n";

  for(int i=0; i < l.size(); i++) {
    f << "-" << l[i].arg_id << " " << l[i].arg_val;
    if(i != (l.size()-1)) { f << " \\" << endl; } // don't put continuation on last line
    else { f <<  endl; } // put return after last line
  }
}


// this called when uninterpreted left-overs
void exit_with_help(ParList l) { 
  cout << "there were the following uninterpreted:\n";
  for(int i=0; i < l.size(); i++) {
    cout << "-" << l[i].arg_id << " " << l[i].arg_val << endl;
  }
  exit(1);
}

bool convert_to_bool(string option) {
  if(option == "yes") {
    return true;
  }
  else if(option == "no") {
    return false;
  }
  else {  
    cout << "unexpected value " << option << endl;
  }
  exit(1);
}

string bool_to_yes_no(bool b) {
  if(b) {return "yes"; } 
  else { return "no"; }
}

