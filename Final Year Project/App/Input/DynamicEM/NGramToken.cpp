#include "NGramToken.h"

bool filter_ngram_meta_token = false;

// things like _START_ _END_

bool is_ngram_meta_token(string word) {
  if((word == "_START_") || (word == "_END_")) {
    return true;
  }
  else {
    return false;
  }

}
