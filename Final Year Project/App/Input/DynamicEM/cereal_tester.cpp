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
#include <cereal/archives/binary.hpp>

// read a corpus
// make some kind of print out
// export to an archive

// also possibly read an arcive to set an archive
// generate print out again

vector<Occ> data; 

// serialise a single Occ object
template<class Archive> 
void serialize(Archive &ar, Occ &obj) { 
  ar( obj.words ); 
  ar( obj.true_sense );
  ar( obj.inferred_sense );
  ar( obj.year );
  ar( obj.data_prob );
  ar( obj.max_prob );
  ar( obj.diff_to_max );
  ar( obj.rpt_cntr ); 
  ar( obj.size_left );
  ar( obj.size_right ); 
  ar( obj.num_L ); 
  ar( obj.num_R );
}


main(int argc, char **argv) {
  ParList pars;
  pars = parse_command_line(argc, argv);
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

  if(corp_params.symbol_table_name == "") {
    cout << "have to specify name of symbol table file\n";
    exit(1);
  }

  process_corpus(corp_params.file_list_file,corp_params.symbol_table_name,false,true,corp_params.target_alternates);

  cout << "data before archive\n";
  cout << "***************\n";
  
  for (unsigned int data_id = 0; data_id < data.size(); data_id++) {
	cout << "data_id " << data_id << endl;
	data[data_id].show();
      }

  for (unsigned int y_id = 0; y_id < years.size(); y_id++) {
    cout << years[y_id] << " ";
  }
  cout << endl;
    
  {

    // setting up an archive
    ofstream file( "out.bin" );
    cereal::BinaryOutputArchive oar( file );
    
    // sending to the archive
    // using << syntax instead of iar( v1_r, v2_r, s1_r );
    oar << data;
    oar << years;
  }

  {
    // setting up some identifiers
    vector<Occ> data_r;
    vector<string> years_r;
    
    // setting up an input archive object
    std::ifstream file( "out.bin" );
    cereal::BinaryInputArchive iar( file );

    iar >> data_r;
    iar >> years_r;
    
    cout << "data after archive\n";
    cout << "***************\n";

    for (unsigned int data_id = 0; data_id < data_r.size(); data_id++) {
      cout << "data_id " << data_id << endl;
      data_r[data_id].show();
    }

  for (unsigned int y_id = 0; y_id < years_r.size(); y_id++) {
    cout << years_r[y_id] << " ";
  }
  cout << endl;


    
    // confirming
    // if(data == data_r) {
    //   cout << "restored is same\n";
    // }
    // else {
    //   cout << "restored is differnt\n";
    // }
    
  }
  
  
}




