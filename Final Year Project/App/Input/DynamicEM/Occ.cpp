#include "Occ.h"
#include <iostream>

extern string decode_to_symbol(int code);

unsigned int desired_left;
unsigned int desired_right;
bool whether_pad = true;
bool whole_coha_article = false;

Occ::Occ() {
  true_sense = -1;
  inferred_sense = -1;
  rpt_cntr = 1;
  num_L = 0;
  num_R = 0;
  size_left = 0;
  size_right = 0;
}

void Occ::show() {

  cout << "year: " << year << " SENSE: " << sense_to_string(true_sense) << endl;
  cout << "WORDS: ";
  for(unsigned int i=0; i < size_left; i++) {
    cout << decode_to_symbol(words[i]) << " ";
  }
  cout << "||T|| ";
  for(unsigned int j=0; j < size_right; j++) {
    cout << decode_to_symbol(words[size_left+j]) << " ";
  }
  //cout << " year: " << year;
  cout << endl;
  unsigned int how_many = 0;
  for(unsigned w=0; w < words.size(); w++) {
    if(words[w] == 0) { how_many++; }
  }
  cout << "num unk: " << how_many << endl;

  if(rpt_cntr > 0) {
    cout << "repeated: " << rpt_cntr << endl;
  }
}

void Occ::show_inf() {

  cout << "year: " << year << " INF SENSE: " << sense_to_string(inferred_sense) << endl;
  cout << "WORDS: ";
  for(unsigned int i=0; i < size_left; i++) {
    cout << decode_to_symbol(words[i]) << " ";
  }
  cout << "||T|| ";
  for(unsigned int j=0; j < size_right; j++) {
    cout << decode_to_symbol(words[size_left+j]) << " ";
  }
  cout << endl;
}

string Occ::make_string() {
	string word = "";
	for (unsigned int i = 0; i < size_left; i++) {
		word += decode_to_symbol(this->words[i]) + " ";
	}
	word += "||T|| ";
	for (unsigned int j = 0; j < size_right; j++) {
		word += decode_to_symbol(this->words[size_left + j]) + " ";
	}
	return word;
}

