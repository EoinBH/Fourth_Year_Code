#include "uniftype.h"
#include <iostream>
#include <stdlib.h>

UNIFTYPE convert_to_uniftype(string s) {
  UNIFTYPE t;
  if(s == "flat") { 
    t = FLAT;
  }
  else if(s == "corpus") {
    t = CORPUS;
  }
  else {
    cout << "unknown words_prior_type\n";
    exit(0);
  }
  return t;
}

