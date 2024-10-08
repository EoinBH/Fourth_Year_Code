#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include "Occ.h"
#include <algorithm>
#include <math.h>

using namespace std;


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

// read a corpus
// save
//   corp_params
//   data 
//   the_symbols  
//   sym_decoder
//   symbol_total
//   years
// into an archive attaching time-stamp to name

namespace Corp {
  vector<Occ> data;
}

extern map<int,string> sym_decoder;

int main(int argc, char **argv) {
  ParList pars;
  pars = parse_command_line(argc, argv);
  ParList saved_pars;
  saved_pars = pars;
  if(check_for_help(pars)) {
    corp_params.exit_with_help(true);
  }
  pars = corp_params.process_params(pars);
  if(pars.size() != 0) {
    exit_with_help(pars);
  }

  corpus_type = corp_params.corpus_type;
  data_csv_suffix = corp_params.whether_csv_suffix;
  grouping_size = corp_params.grouping_size;
  desired_left = corp_params.left;
  desired_right = corp_params.right;
  whether_pad = corp_params.whether_pad;
  lowercase = corp_params.lowercase;
  filter_ngram_meta_token = corp_params.filter_ngram_meta_token;
  syn_info = corp_params.syn_token_parts;
  excl_targ_nnp =  corp_params.excl_targ_nnp; // excl_targ_nnp owend by SynGramToken.cpp
  coha_wlp_field = corp_params.wlp;
  coha_targ_wlp_field = corp_params.targ_wlp;
  excl_targ_np1 =  corp_params.excl_targ_np1; // excl_targ_np1 owend by CohaToken.cpp
  filter_coha_at_token = corp_params.filter_coha_at_token; // filter_coha_at_token owned by CohaToken.cpp
  whole_coha_article = corp_params.whole_coha_article; // set Occ contents to entire coha article
  
  if(corp_params.symbol_table_name != "") {
    cout << "should not be specifying a symbol table file as it will go into the archive\n";
    exit(1);
  }

  process_corpus(corp_params.file_list_file,corp_params.symbol_table_name,false,false,corp_params.target_alternates);

  // should stick time stamp on end of supplied archive name
  string archive_name;
  archive_name = corp_params.archive_name;
  output_suffix = time_stamp();
  archive_name = add_suffix(archive_name);
  
  cout << "data before archive\n";
  cout << "***************\n";

  // corp_params.show_params(cout);
  // for (unsigned int data_id = 0; data_id < Corp::data.size(); data_id++) {
  // 	cout << "data_id " << data_id << endl;
  // 	Corp::data[data_id].show();
  //     }


  // // for (unsigned int y_id = 0; y_id < years.size(); y_id++) {
  // //   cout << years[y_id] << " ";
  // // }
  // // cout << endl;
  
    
  {

    // setting up an archive
    //    ofstream file( "out.bin" );
    ofstream file( archive_name );
    if(!file) { cout << "prob opening " << archive_name << endl; exit(1); }
    
    cereal::BinaryOutputArchive oar( file );
    
    // sending to the archive
    // using << syntax instead of iar( v1_r, v2_r, s1_r );

    oar << corp_params;
    oar <<  Corp::data;
    oar <<  the_symbols;
    oar <<  sym_decoder;
    oar <<  symbol_total;
    oar <<  years;
  }

  // recording params given is file with name
  // D/ceral_param_dtls_TIME
  //
  // where D and TIME should match path of archive as
  // D/...TIME
  
  string param_dtls_name;
  param_dtls_name = corp_params.archive_name;
  string dir_part, final_part;
  dir_and_file(param_dtls_name , dir_part, final_part);
  param_dtls_name = dir_part + "/" + "corpus_cereal_param_details_";
  param_dtls_name += output_suffix; // set to time stamp earlier;
  cout << "param_dtls_name is: " << param_dtls_name << endl;

  ofstream param_dtls;
  param_dtls.open(param_dtls_name.c_str());
  show_params(saved_pars,param_dtls); // record verbatim command-line
  corp_params.show_params(param_dtls); //record corp_params including defaults not explicitly set
  param_dtls.close();
  
  
  // {
  //   // setting up some identifiers
  //   CorpPar corp_params_r;
  //   vector<Occ> data_r;
  //   vector<string> years_r;
    
  //   // setting up an input archive object
  //   //    std::ifstream file( "out.bin" );
  //       std::ifstream file( archive_name );
  //   cereal::BinaryInputArchive iar( file );

  //   iar >> corp_params_r;
  //   iar >> data_r;
  //   //iar >> years_r;
    
  //   cout << "data after archive\n";
  //   cout << "***************\n";

  //     corp_params_r.show_params(cout);
  //   for (unsigned int data_id = 0; data_id < data_r.size(); data_id++) {
  //     cout << "data_id " << data_id << endl;
  //     data_r[data_id].show();
  //   }

  // // for (unsigned int y_id = 0; y_id < years_r.size(); y_id++) {
  // //   cout << years_r[y_id] << " ";
  // // }
  // // cout << endl;


    
  //   // confirming
  //   // if(data == data_r) {
  //   //   cout << "restored is same\n";
  //   // }
  //   // else {
  //   //   cout << "restored is differnt\n";
  //   // }
    
  // }
  
  
}




