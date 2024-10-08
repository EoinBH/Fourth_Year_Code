#include "SynGramToken.h"
#include "StringUtil.h"
using namespace std;

string syn_info = "0";
vector<int> syn_info_vec;
bool excl_targ_nnp = true; // used to eliminate matches where lower-cased from a name 
SynGramToken::SynGramToken(){}

// need scan backwards for '/' in  word/pos-tag/dep-label/head-index
// a b c / d e /
// 0 1 2 3 4 5 6
SynGramToken::SynGramToken(string token){

  size_t head_start_pos;
  head_start_pos = token.rfind('/',string::npos);
  head_pos = token.substr(head_start_pos+1);
  
  size_t dep_rel_start_pos;
  dep_rel_start_pos = token.rfind('/',head_start_pos-1);
  dep_rel = token.substr(dep_rel_start_pos+1,head_start_pos - dep_rel_start_pos -1);

  size_t pos_start_pos;
  pos_start_pos = token.rfind('/',dep_rel_start_pos-1);
  pos = token.substr(pos_start_pos+1,dep_rel_start_pos - pos_start_pos - 1);

  form = token.substr(0,pos_start_pos);
  
}

// make vector of parts
// pick from this according to contents of   syn_info_vec
string SynGramToken::filtered() {
  vector<string> as_vec;
  as_vec.push_back(form);
  as_vec.push_back(pos);
  as_vec.push_back(dep_rel);
  as_vec.push_back(head_pos);
  string result = "";
  int which;
  for(int i = 0; i < syn_info_vec.size(); i++) {
    which = syn_info_vec[i];
    result += as_vec[which];
    if(i != syn_info_vec.size()-1) {
      result += "-";
    }
  }
  return result;
}

void SynGramToken::show() {

}

void make_syn_info_vector() {
  if((syn_info == "neighb") || (syn_info == "ext_neighb")) { return; }
  
  /* syn_info of form
     Num/Num/..../Num
  */
  syn_info_vec.clear();
  
  size_t inf_start, inf_end;
  string one_inf;
  inf_start = 0;
  while((inf_end = syn_info.find('/',inf_start)) != string::npos) {
    one_inf = syn_info.substr(inf_start,inf_end-inf_start);
    syn_info_vec.push_back(stl_to_int(one_inf));
    inf_start = inf_end+1;
  }

  one_inf = syn_info.substr(inf_start);
  syn_info_vec.push_back(stl_to_int(one_inf));

  return;
}


// used in deciding whethr to extend a relation when syn_info == ext_neigh
bool extendible(string dep_rel) {
  if((dep_rel == "pobj") || (dep_rel == "pcomp")) {
    return true;
  }
  else {
    return false;
  }


}
