#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "senses.h"
#include "StringUtil.h"

using namespace std;

vector<string> senses;
vector<string> true_senses;
unsigned int sense_size = 0;
string sense_to_string(SENSE S) {
  if((S < 0) || (S > senses.size()-1)) {
    cout << "unexpected sense value" << endl;
    return "UNEXP";
  }
  else {
    return true_senses[S];
  }
}

string unsup_sense_to_string(SENSE S) {
  if((S < 0) || (S > senses.size()-1)) {
    cout << "unexpected sense value" << endl;
    return "UNEXP";
  }
  else {

    //return int_to_stl(S);
    return senses[S];
  }
}

void initialize_true_senses(string sense_file) {

  string sense_name;
  ifstream file;
  file.open(sense_file.c_str());
  if(!file) {
    cout << "problem opening sense file " <<  sense_file << endl;
    exit(1);
  }

  while(file >> sense_name) {
    true_senses.push_back(sense_name);
  }
  sense_size = true_senses.size();
}

void initialize_unsup_senses(unsigned int num_senses) {
  if(num_senses == 0) {
    cout << "requested number of unsup senses is zero which makes no sense\n";
    exit(1);
  }
  senses.resize(num_senses);
  for(SENSE S=0; S < senses.size(); S++) {
    senses[S] = "unsup" + int_to_stl(S); 
  }
  sense_size = senses.size();
}
