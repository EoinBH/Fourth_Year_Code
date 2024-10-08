#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <algorithm>

using namespace std;

// implements 2 kinds of behaviour
// one is an expanding table, adding new non-zero codes for new unseen things
// other is a non-expanding table, returning zero for unseen things

unsigned int symbol_total = 0;

bool whether_closed = false; // default is to be open

string symbol_table_file_source = "";

// stores the mapping from strings to ints
// should probably add a visibility confiner to this
map<string,int> the_symbols;

map<int,string> sym_decoder;

unsigned int get_code(string word) {
  map<string,int>::const_iterator symbol_table_itr;
  symbol_table_itr = the_symbols.find(word);
  if(symbol_table_itr != the_symbols.end()) {
    return symbol_table_itr->second;
  }
  else if(whether_closed == false) {
    symbol_total++;
    the_symbols[word] = symbol_total;
    sym_decoder[symbol_total] = word;
    return symbol_total;
  }
  else { 
    return 0;
  }

}

unsigned int check_code(string word) {
  map<string,int>::const_iterator symbol_table_itr;
  symbol_table_itr = the_symbols.find(word);
  if(symbol_table_itr != the_symbols.end()) {
    return symbol_table_itr->second;
  }
  else { 
    return 0;
  }
}


string decode_to_symbol(int code) {
  if(code > 0 && code <= symbol_total) {
    return sym_decoder[code];
  }
  else { 
    return "none";
  }
}

void decode_print(int code) {
  string print_symbol;
  print_symbol = decode_to_symbol(code);
  printf("'%s'",print_symbol.c_str());
}

void read_symbol_table(string symbol_table_file) {

  string symbol;
  int code;
  ifstream file;
  file.open(symbol_table_file.c_str());
  if(!file) {
    cout << "problem opening symbol table " <<  symbol_table_file << endl;
    exit(1);
  }

  // debugging additions
  unsigned int max_code = 0;
  while(file >> symbol >> code) {

    the_symbols[symbol] = code;
    if(code > max_code) {
      max_code = code;
    }
    // DEBUG
    // cout << symbol << " " << code << " " << max_code << endl;
    
    if(sym_decoder.find(code) == sym_decoder.end()) {
     sym_decoder[code] = symbol;
     if(sym_decoder.size() != max_code) {
       cout << "sym_decoder.size()" << sym_decoder.size() << " and max_code" << max_code << " are not equal\n";
     }
    }
  }
  //symbol_total = the_symbols.size();
  symbol_total = sym_decoder.size();
  whether_closed = true;
  symbol_table_file_source = symbol_table_file;
}

void write_symbol_table(string symbol_table_file) {
  ofstream file;
  file.open(symbol_table_file.c_str());
  if(!file) {
    cout << "problem opening symbol table " <<  symbol_table_file << endl;
    exit(1);
  }
  vector<pair<int, string> > table_vec;
  table_vec.clear();

  map<string,int>::const_iterator symbol_table_itr;
  for(symbol_table_itr = the_symbols.begin(); symbol_table_itr != the_symbols.end(); symbol_table_itr++) {
    pair<int, string> p;
    p.first = symbol_table_itr->second;
    p.second = symbol_table_itr->first;
    table_vec.push_back(p);
    //file << symbol_table_itr->first << " " <<  << endl;
  }
  sort(table_vec.begin(),table_vec.end());
  for(unsigned int i=0; i < table_vec.size(); i++) {
    file << table_vec[i].second << " " << table_vec[i].first << endl;
  }
  file.close();
}

void initialize_symbol_table() {
  the_symbols.clear();
  symbol_total = 0;
}
