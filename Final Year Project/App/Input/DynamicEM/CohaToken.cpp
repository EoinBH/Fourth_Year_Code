#include "CohaToken.h"
#include "StringUtil.h"
#include <iostream>
using namespace std;

extern bool lowercase;
char coha_wlp_field = 'l'; // for lemma which is middle of wlp line
char coha_targ_wlp_field = '0'; // in checking for target match filter acccording to coha_wlp_field
bool excl_targ_np1 = true;  // used to eliminate lemma target matches was originally upper-case name
bool filter_coha_at_token = false; // used to skip @ tokens default set anal to filter_ngram_meta_token

CohaToken::CohaToken(){}

// format is
// <WORD><TAB><LEMMA><TAB><POS><CARRIAGE-RETURN>
// the final char shows as ^M and is prob due to bad windows->unix copy
// ascii code of this 13
// 0 1  2  3 4  5  6 7
// w w TAB l l TAB p p \r
CohaToken::CohaToken(string coha_line){

  size_t field_start = 0;
  size_t field_end;
  is_at_token = false;
  
  /* find word part */
  // v
  // 0 1  2  3 4  5  6 7
  // W W TAB l l TAB p p \r
  field_end = coha_line.find('\t',field_start);
  word = coha_line.substr(field_start,field_end-field_start);
  if(word == "@") { is_at_token = true; }
  if(lowercase) { tolower(word) ;}
  fix_a_coha_token(word); // replaces space with S
  
  field_start = field_end+1;
  /* find lemma part */
  //         v
  // 0 1  2  3 4  5  6 7
  // w w TAB L L TAB p p \r
  field_end = coha_line.find('\t',field_start);
  lemma = coha_line.substr(field_start,field_end-field_start);
  fix_a_coha_token(lemma); // replaces space with S
  // lowercase not relevant for lemma

  field_start = field_end+1;
  /* find pos part */
  //                 v
  // 0 1  2  3 4  5  6 7
  // w w TAB l l TAB P P \r
  field_end = coha_line.find(13,field_start); // 'carriage return'
  pos = coha_line.substr(field_start,field_end-field_start);
  // lowercase not relevant for pos

}


// replaces space chars that are in token (which really shouldn't be) with S
// so other functions can rely on space as a separator
void CohaToken::fix_a_coha_token(string& s) {
  size_t spc_pos;
  while((spc_pos = s.find(' ')) != string::npos) {
    s[spc_pos] = 'S';
  }
}

string CohaToken::fetch_field(char wlp) {

    if(wlp == 'w') {
      return word;
    }
    else if(wlp == 'l') {
      return lemma;
    }
    else if(wlp == 'p') {
      return pos;
    }
    else {
      cout << "unexpected field spec -- should w l or p\n";
      return "junk";
    }


}
