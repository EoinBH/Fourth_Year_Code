#include <string>
#include <vector>

using namespace std;

extern string syn_info;
extern vector<int> syn_info_vec;
extern void make_syn_info_vector(void);
extern bool extendible(string dep_rel);
extern bool excl_targ_nnp; // used to eliminate matches where lower-cased from a name b
#if !defined SYNGRAMTOKEN_H
#define SYNGRAMTOKEN_H

class SynGramToken {
public:
  SynGramToken();
  SynGramToken(string token_string);
  //??
  string form;
  string pos;
  string dep_rel;
  string head_pos;
  string filtered(void);
  void show(void);
};


#endif
