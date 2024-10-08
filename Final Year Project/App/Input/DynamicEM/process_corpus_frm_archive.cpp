#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include "Occ.h"
#include <algorithm>
#include <math.h>




#include "years.h"
#include "sym_table.h"
#include "prob_tables.h"
#include "senses.h"
#include "CmdLineVersions.h"
#include "process_corpus.h"

#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/binary.hpp>
#include "Occ_serialize.h"
#include "CorpPar_serialize.h"

using namespace std;

namespace Corp {
  extern vector<Occ> data;
}

extern map<int,string> sym_decoder;
// others vars should come from headers

bool are_equal(CorpPar& params_in, CorpPar& params_out);
void process_corpus_frm_archive(string archive_name, CorpPar corp_params) {

  std::ifstream file( archive_name );
  if(!file) { cout << "prob opening " << archive_name << endl; exit(1); }
  cereal::BinaryInputArchive iar( file );

  CorpPar corp_params_r;
  
  iar >> corp_params_r;
  iar >>  Corp::data;
  iar >>  the_symbols;
  iar >>  sym_decoder;
  iar >>  symbol_total;
  iar >>  years;

  // do check whether archived and supplied corpus params match
  if(are_equal(corp_params, corp_params_r) == false) {
    cout << "the given and recovered corpus params are not equal\n";
    cout << "so this archive should not be used\n";
    exit(1);
  }
}

// check whether a set of corpus params recovered from an archive
// and set created from program call are equal
// ignore symbol table name
bool are_equal(CorpPar& p, CorpPar& q) {

  if(p.target_alternates == q.target_alternates && 
     p.corpus_type == q.corpus_type && 
     //p.file_list_file == q.file_list_file && 
     p.whether_csv_suffix == q.whether_csv_suffix && 
     p.grouping_size == q.grouping_size && 
     p.left == q.left && 
     p.right == q.right && 
     p.whether_pad == q.whether_pad && 
     p.lowercase == q.lowercase && 
     p.filter_ngram_meta_token == q.filter_ngram_meta_token && 
     p.syn_token_parts == q.syn_token_parts && 
     p.excl_targ_nnp == q.excl_targ_nnp && 
     p.wlp == q.wlp && 
     p.targ_wlp == q.targ_wlp && 
     p.excl_targ_np1 == q.excl_targ_np1 &&
     p.filter_coha_at_token == q.filter_coha_at_token &&
     p.whole_coha_article == q.whole_coha_article &&
     //p.symbol_table_name == q.symbol_table_name && 
     p.closed == q.closed
     ) { return true; }
  else { return false; }
  

}
