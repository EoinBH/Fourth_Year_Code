#include "CmdLineVersions.h"
#include <math.h>
#include <iostream>
#include <utility>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>




CorpPar corp_params;


CorpPar::CorpPar() {
  // default values
  // reading the corpus
  target_alternates = "";
  corpus_type = 1;
  file_list_file = "file_list";
  whether_csv_suffix = true; 
  grouping_size = 1;
  left = 5;
  right = 5;
  whether_pad = true;
  lowercase = false;
  filter_ngram_meta_token = false; // relating to funny tokens in n-grams eg _START_
  syn_token_parts = "0"; // just the word form in syntactic n-gramx
  excl_targ_nnp = true; // exclude target matches tagged as NNP in syntactic n-grams
  wlp = 'l';            // use lemma part of coha data
  targ_wlp = '0';       // no special treatment of target vs context words
  excl_targ_np1 = true; // exclude target matches tagged as np1 in coha
  filter_coha_at_token = false ; // skip the text-blanking @ tokens in coha data
  whole_coha_article = false; // true implies whole article in one Occ
  symbol_table_name = "";
  closed = false;
  corp_wc_min = 0.0;
  whether_archive = false;
  archive_name = "";
  whether_read_archive = false;
}


ParList CorpPar::process_params(ParList l) {

  vector<ArgVal>::iterator p;
  p = l.begin();
  
  char *spill;

  char *arg_val;



  while(p != l.end()) {


    arg_val = p->arg_val;


    if(p->arg_id == "targ") {
      target_alternates = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "corpus_type") {
      corpus_type = strtoul(arg_val,&spill,10);
      p = l.erase(p);
    }
    else if(p->arg_id == "files") {
      file_list_file = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "whether_csv_suffix") {
      whether_csv_suffix = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "group_size") {
      grouping_size = strtoul(arg_val,&spill,10);
      p = l.erase(p);
    } 
    else if(p->arg_id == "left") {
      left = strtoul(arg_val,&spill,10);
      p = l.erase(p);
    }
    else if(p->arg_id == "right") {
      right = strtoul(arg_val,&spill,10);
      p = l.erase(p);
    }
    else if(p->arg_id == "whether_pad") {
      whether_pad = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "lowercase") {
      lowercase = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "filter_ngram_meta_token") {
      filter_ngram_meta_token = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if (p->arg_id == "syn_info") { // for  syntactic n-grams 
      syn_token_parts = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "excl_targ_name") { 
      if(corpus_type == 3) { // coha
	excl_targ_np1 = convert_to_bool(string(arg_val)); 
      }
      else if(corpus_type == 4) { // syntax
	excl_targ_nnp = convert_to_bool(string(arg_val));
      }
      else {
	cout << "ought to have type 3 or 4 with excl_targ_name yes";
	exit_with_help(true);
      }
      p = l.erase(p);
    } 
    else if(p->arg_id == "wlp") { // for coha data
      wlp = arg_val[0];
      p = l.erase(p);
    }
    else if(p->arg_id == "targ_wlp") { // for coha data
      targ_wlp = arg_val[0];
      p = l.erase(p);
    }
    else if(p->arg_id == "filter_coha_at_token") { // for coha data
      filter_coha_at_token = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "whole_coha_articles") {
      whole_coha_article = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "sym") {
      symbol_table_name = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "closed") {
      closed = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if (p->arg_id == "corp_wc_min") {
      corp_wc_min = strtod(arg_val, &spill);
      p = l.erase(p);
    }
    else if(p->arg_id == "whether_archive") {
      whether_archive = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "archive_name") {
      archive_name = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "whether_read_archive") {
      whether_read_archive = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else {
      p++;
    }

  }

  if(target_alternates == "" && !whole_coha_article) {
    cout << "must supply target alternates\n";
    exit_with_help(true);
  }  

  if(corpus_type != 1 && corpus_type != 2 && corpus_type != 3 && corpus_type != 4) {
    cout << "unexpected corpus_type value\n";
    exit_with_help(true);
  }

  if(whether_archive && (archive_name == "")) {
    cout << "must give an archive name\n";
    exit_with_help(true);
  }

  return l;
}


//TODO add newest params to this
void CorpPar::show_params(ostream& f) {
  f << "********************************************\n";
  f << "these are the parameters recovered by interpreting the state of corp_params\nthis may include important defaults which were not explicitly given\n";
  f << "-targ " << target_alternates << endl;
  f << "-corpus_type " << corpus_type << endl;
  f << "-files " << file_list_file << endl;
  f << "-whether_csv_suffix " << bool_to_yes_no(whether_csv_suffix) << endl;
  f << "-group_size " << grouping_size << endl;
  f << "-left " << left << endl;
  f << "-right " << right << endl;
  f << "-whether_pad " << bool_to_yes_no(whether_pad) << endl;
  f << "-lowercase " << bool_to_yes_no(lowercase) << endl;
  f << "-filter_ngram_meta_token " << bool_to_yes_no(filter_ngram_meta_token) << endl;
  f << "-syn_info " << syn_token_parts << endl;
  f <<  "excl_targ_nnp is " << excl_targ_nnp << endl; 
  f <<  "excl_targ_np1 is " << excl_targ_np1 << endl;
  f << "note both above set by -excl_targ_name yes/no" << endl;
  f << "-wlp " <<  wlp << endl;
  f << "-targ_wlp " <<  targ_wlp << endl;
  f << "-filter_coha_at_token " << bool_to_yes_no(filter_coha_at_token) << endl;
  f << "-whole_coha_articles " << bool_to_yes_no(whole_coha_article) << endl;
  

  
  if(symbol_table_name != "") {
    f << "-sym " << symbol_table_name << endl;
    f << "-closed " << bool_to_yes_no(closed) << endl;
  }
  if(corp_wc_min > 0) {
    f << "-corp_wc_min " << corp_wc_min << endl;
  }
}

void CorpPar::exit_with_help(bool wh_exit) {

  cout << 	"Usage: prog_name [options]\n";
  cout << 	"options: only those marked [required] have to be supplied\n";
  cout <<       "-targ Form1/.../FormN [required]\n";
  cout <<       "-corpus_type N: 1 for normal, 2 for n-gram, 3 for coha, 4 for syn-ngram (1)\n"; 
  cout << 	"-files FILENAME: of list of files [required]\n";
  cout <<       "-whether_csv_suffix yes/no: whether data files end .csv (yes)\n";
  cout <<       "-group_size N: rounds year down to multiple of N (1)\n";
  cout <<       "-left N: number of words sought to the left (5) \n";
  cout <<       "-right N: number of words sought to the right (5) \n";
  cout <<       "-whether_pad yes/no: whether to pad out (yes) \n";
  cout <<       "-lowercase yes/no: whether to reduce words to lowercase (no)\n";
  cout <<       "(NGRAM) -filter_ngram_meta_token yes/no: whether to skip meta like _START_ (no)\n";
  cout <<       "(SYNTAX) -syn_info N/../N for parts from token 0:word,1:POS,2:dep-rel,3:head-index (0)\n";
  cout <<       "    can have several eg 0/2 and parts are concatenated \n";
  cout <<       "(SYNTAX) -syn_info neighb: refers only to arcs involving target \n";
  cout <<       "    makes ^f:other and vf:other features \n";
  cout <<       "(SYNTAX) -syn_info ext_neighb: effectively composes certain arcs, mainly involving prepositions\n";
  cout <<       "(COHA) -wlp Field, where Field is w(ord) or l(emma) or p(art-of-speech) \n (l)\n";
  cout <<       "(COHA) -targ_wlp Field, where Field is 0 or w(ord) or l(emma) or p(art-of-speech) \n (l)\n";
  cout <<        "0 no special treatment in searching for a target match whereas other values mean coha token will be filtered according to these in assessing for a target match\n";
  cout <<        "(COHA) -filter_coha_at_token yes/no: whether to read no data from lines whose word part is @\n";
  cout <<        "    (these occur in runs of 10 where content is for copyright reason hidden)\n";
    cout <<      "(COHA) -whole_coha_articles yes/no: whether to treat whole article as single data item (no)\n";
  cout <<     "(COHA and SYNTAX) -excl_targ_name yes/no: skips match to target if tagged as proper-name, a workaround for when relevant part is lower-cased in raw data (yes)\n";
  cout <<       "-sym FILENAME: of pre-existing symbol table\n";
  cout <<       "-closed yes/no: whether this table treated as closed (no)\n";
  cout <<       "-corp_wc_min NUM: quantify for add-one smoothing of corpus probs (these optional used in initialisation. Equates to MAP with Dirich prior with param 1+N (def: 0.0)\n";
  cout <<       "-whether_archive yes/no: whether to create an archive of corpus processing (no)\n";
  cout <<       "-archive_name FILENAME: name for archive (must supply one)\n";
  cout <<       "-whether_read_archive yes/no: whether to read from an archive (no)\n";
  
  if(wh_exit) {
  exit(1);
  }
}

//
dynEMPar em_params;

dynEMPar::dynEMPar() {
  // high level results and algorithm control
  results_location = "../Expts";
  num_unsup_senses = 0;
  do_unsup = true;
  do_sup = false;
  sense_names_file = "";
  dynamic = true;
  //  sense_names_file = "unsup_junk";
  // algorithm initialisation
  wordprobs_in = "";
  senseprobs_in = "";
  senseprobs_string = "";
  whether_sense_rand = false;
  rand_mix = false;
  rand_mix_level = 0.0;
  words_prior_type = "";
  whether_word_rand = false;
  rand_word_mix = false;
  rand_word_mix_level = 0.0;
  set_seed = true;
  // details for running the alg
  max_it = 40; 
  wc_min = 0.0;
  sc_min = 0.0;

  // details for outcomes
  // wordprobs_out = "";
  // senseprobs_out = "";
  senseprobs_out = "em_senseprobs_final"; 
  wordprobs_out = "em_wordprobs_final";
  
  save_starts = false;
  time_stamp = false;
  output_suffix = ""; 
  input_suffix = "";
  record_version = true;
  whether_save_cat_outcomes = false;
  whether_save_word_stats = false;

}


ParList dynEMPar::process_params(ParList l) {

  vector<ArgVal>::iterator p;
  p = l.begin();

  char *spill;

  char *arg_val;

  


  while(p != l.end()) {
    //show_params(cout);
    //cout << "about to process " << p->arg_id << " " << p->arg_val << endl;

    arg_val = p->arg_val;

    if(p->arg_id == "expts") {
      results_location = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "num_senses") {
      num_unsup_senses = strtoul(arg_val,&spill,10);
      p = l.erase(p);
    }
    else if(p->arg_id == "unsup") {
      do_unsup = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "sup") {
      do_sup = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "sense-names") {
      sense_names_file = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "dynamic") {
      dynamic = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "wordprobs") {
      wordprobs_in = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "senseprobs") {
      senseprobs_in = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "senseprobs_string") {
      senseprobs_string = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "sense_rand") {
      whether_sense_rand = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "rand_mix") {
      rand_mix = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "rand_mix_level") {
      rand_mix_level = strtod(arg_val,&spill);
      p = l.erase(p);
    }
    else if(p->arg_id == "words_prior_type") {
      words_prior_type = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "word_rand") {
      whether_word_rand = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "rand_word_mix") {
      rand_word_mix = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "rand_word_mix_level") {
      rand_word_mix_level = strtod(arg_val,&spill);
      p = l.erase(p);
    }
    else if(p->arg_id == "set_seed") {
      set_seed = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "max_it") {
      max_it = strtoul(arg_val,&spill,10);
      p = l.erase(p);
    }
    else if(p->arg_id == "wc_min") {
      wc_min = strtod(arg_val,&spill);
      p = l.erase(p);
    } 
    else if(p->arg_id == "sc_min") {
      sc_min = strtod(arg_val,&spill);
      p = l.erase(p);
    }
    else if(p->arg_id == "wordprobs_out") {
      wordprobs_out = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "senseprobs_out") {
      senseprobs_out = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "save_starts") {
      save_starts = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } 
    else if(p->arg_id == "time_stamp") {
      time_stamp = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "output_suffix") {
      output_suffix = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "input_suffix") {
      input_suffix = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "record_version") {
      record_version = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "whether_save_cat_outcomes") {
      whether_save_cat_outcomes = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "whether_save_word_stats") {
      whether_save_word_stats = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else {
      //if(p != l.end()) { p++; }
      p++;
    }

  
  }
  //cout << "finished processing\n";  
  if(do_sup) { do_unsup = false; }
  if(do_unsup) { do_sup = false; }

  return l;
}



void dynEMPar::show_params(ostream& f) {

  f << "********************************************\n";
  f << "these are the parameters recovered by interpreting the state of params\nthis may include important defaults which were not explicitly given\n";
  f << "-expts " << results_location << endl;
  f << "-num_senses " << num_unsup_senses << endl;
  f << "-unsup " << bool_to_yes_no(do_unsup) << endl;
  f << "-sup " << bool_to_yes_no(do_sup) << endl;
  if(sense_names_file != "") {
   f << "-sense-names " << sense_names_file << endl;
  }
  f << "-dynamic " << bool_to_yes_no(dynamic) << endl; 
  if(wordprobs_in != "") {
    f << "-wordprobs " << wordprobs_in << endl; 
  }
  if(senseprobs_in != "") { 
    f << "-senseprobs " << senseprobs_in << endl;
  }

  if(senseprobs_string != "") {
    f << "-senseprobs_string " << senseprobs_string << endl;
  }
  f << "-sense_rand " << bool_to_yes_no(whether_sense_rand) << endl;
  f << "-rand_mix " << bool_to_yes_no(rand_mix) << endl;
  if(rand_mix) {
    f << "-rand_mix_level " << rand_mix_level << endl;
  }
  if(words_prior_type != "") {
    f << "-words_prior_type " << words_prior_type << endl;
  }
  f << "-word_rand " << bool_to_yes_no(whether_word_rand) << endl;
  f << "-rand_word_mix " << bool_to_yes_no(rand_word_mix) << endl; 
  if(rand_word_mix) { 
    f << "-rand_word_mix_level " << rand_word_mix_level << endl;
  }
  f << "-set_seed " << bool_to_yes_no(set_seed) << endl;
  f << "-max_it " << max_it << endl;
  if(wc_min > 0) {
    f << "-wc_min " << wc_min << endl;
  }
  if(sc_min > 0) {
    f << "-sc_min " << sc_min << endl;
  }

  if(wordprobs_out != "") {
    f << "-wordprobs_out " << wordprobs_out << endl;
  }
  if(senseprobs_out != "") {
    f << "-senseprobs_out " << senseprobs_out << endl;
  }   
  if(save_starts != false) {
    f << "-save_starts " << bool_to_yes_no(save_starts) << endl;
  }
  if(time_stamp) {
    f << "-time_stamp " << bool_to_yes_no(time_stamp) << endl;
  }
  if(output_suffix != "") {
    f << "-output_suffix " << output_suffix << endl;
  }
  if(input_suffix != "") {
    f << "-input_suffix " << input_suffix << endl;
  }
  f << "-record_version " << bool_to_yes_no(record_version) << endl;
  f << "-whether_save_cat_outcomes " << bool_to_yes_no(whether_save_cat_outcomes) << endl;
  f << "-whether_save_word_stats " << bool_to_yes_no(whether_save_word_stats) << endl;

}

void dynEMPar::exit_with_help(bool wh_exit) {
  cout << 	"Usage: prog_name [options]\n";
  cout << 	"options: only those marked [required] have to be supplied\n";
  cout << 	"-expts DIRNAME: dir for recording results [required]\n";
  cout <<       "-num_senses N: number of unsup senses [required]\n";
  cout <<       "-unsup yes/no: whether to do unsupervised learning (yes)\n"; 
  cout <<       "-sup yes/no: whether to do supervised learning (no)\n";
  cout <<       "-sense-names FILENAME: of sense names file -- relevant only in sup case\n";
  cout <<       "-dynamic yes/no: per-year or year-independent sense probs (yes)\n";
  cout <<       "-wordprobs FILENAME: of initial word prob file\n"; 
  cout <<       "-senseprobs FILENAME: of initial sense probs\n"; 
  cout <<       "-senseprobs_string N/N/N.../N: initial sense probs, to be shared across years\n"; 
  cout << 	"-sense_rand yes/no: whether to generate random sense probs (no)\n";
  cout <<       "-rand_mix yes/no: whether to add random noise to sense probs initialisations (no) \n";
  cout <<       "-rand_mix_level N: 0 means no noise; 1.0 means all noise (0.0)\n";

  cout <<       "-words_prior_type: unif or corpus\n";
  cout << 	"-word_rand yes/no: whether to generate random word probs for each word (no)\n";
  cout <<       "-rand_word_mix yes/no; whether to add random noise to word prob initialisations (no)\n";
  cout <<       "-rand_word_mix_level N: 0 means no noise; 1.0 means all noise (0.0)\n";
  cout << 	"-set_seed yes/no: whether to set seed to 0 (reproducible) or not (yes)\n";
  cout <<       "-max_it NUM: number of iterations of EM (40)\n";
  cout <<       "-wc_min NUM: quantity for add-one smooothing of word probs. Equates to MAP-EM with Dirich prior with param 1+N (def: 0.0)\n"; 
  cout <<       "-sc_min NUM: quantify for add-one smoothing of sense probs. Equates to MAP-EM with Dirich prior with param 1+N (def: 0.0)\n";


  cout <<       "-wordprobs_out: place to write final word prob matrix (defaults to em_wordprobs_final)\n";
  cout <<       "-senseprobs_out: place to write final sense prob matric (defaults to em_senseprobs_final\n";


  cout <<       "-save_starts yes/no: whether to keep record of start probs\n";
  cout <<       "-time_stamp yes/no: whether to add a time-stamp to output filenames (no)\n" ;
  cout <<       "-output_suffix STRING: STRING will added as suffix to outputs\n"; 
  cout <<       "-record_vesion yes/no: whether to include program version info in output (yes)\n";
  cout <<       "-whether_save_cat_outcomes yes/no: whether make file recording (at each iteration) for each data item its current most prob sense (no)\n";
  cout <<       "-whether_save_word_stats yes/no: whether make file recording (at each iteration) giving all word probs for all senses (tends to be very large) (no)\n";

  if(wh_exit) {
    exit(1);
  }

}

gibbsPar gs_params;

gibbsPar::gibbsPar() {

  results_location = "./";
  num_unsup_senses = 2; //TODO in dynEMPar to 0 as default, find out what checks done for 0 downstream
  init_via_dirichlet = false;
  wordprobs_in = "";
  senseprobs_in = "";
  senseprobs_string = "";
  whether_sense_rand = false;
  rand_mix = false;
  rand_mix_level = 0.0;
  words_prior_type = "";
  whether_word_rand = false;
  rand_word_mix = false;
  rand_word_mix_level = 0.0;
  set_seed = true; // same as default in EM case
  set_sampling_seed = true; // for sampling
  gamma_theta = 1.0;
  gamma_phi = 1.0;
  // no_senses = 2;


  burn_itr = 500;
  max_itr = 5000;
  convert_em = true;


  sampled_senseprobs = "gibbs_sampled_sense_probs";
  sampled_wordprobs = "gibbs_sampled_word_probs";
  senseprobs_out = "gibbs_senseprobs_final"; 
  wordprobs_out = "gibbs_wordprobs_final";
  time_stamp = false;
  output_suffix = ""; 
  input_suffix = "";
  record_version = true;
}

ParList gibbsPar::process_params(ParList l) {
  
  vector<ArgVal>::iterator p;
  p = l.begin();

  char *spill;

  char *arg_val;

  while(p != l.end()) {
    //show_params(cout);
    //cout << "about to process " << p->arg_id << " " << p->arg_val << endl;

    arg_val = p->arg_val;
  
    if (p->arg_id == "expts") {
      results_location = string(arg_val);
      p = l.erase(p);
    }
    else if (p->arg_id == "num_senses") { //DONE migrate 
      num_unsup_senses = atoi(arg_val);
      p = l.erase(p);
    }
    else if (p->arg_id == "init_via_dirichlet") {
      init_via_dirichlet = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if (p->arg_id == "wordprobs") { // DONE migrate name
      wordprobs_in = string(arg_val);
      p = l.erase(p);
    }
    else if (p->arg_id == "senseprobs") {
      senseprobs_in = string(arg_val);
      p = l.erase(p);
    }
    else if (p->arg_id == "senseprobs_string") {
      senseprobs_string = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "sense_rand") {
      whether_sense_rand = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if (p->arg_id == "rand_mix") {
      rand_mix = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if (p->arg_id == "rand_mix_level") {
      rand_mix_level = strtod(arg_val, &spill);
      p = l.erase(p);
    }
    else if (p->arg_id == "words_prior_type") {
      words_prior_type = string(arg_val);
      p = l.erase(p);
    }
   else if(p->arg_id == "word_rand") {
      whether_word_rand = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "rand_word_mix") {
      rand_word_mix = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "rand_word_mix_level") {
      rand_word_mix_level = strtod(arg_val,&spill);
      p = l.erase(p);
    } 
    else if (p->arg_id == "set_seed") {
      set_seed = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if (p->arg_id == "set_sampling_seed") {
      set_sampling_seed = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if (p->arg_id == "burn_itr") {
      burn_itr = atoi(arg_val);
      p = l.erase(p);
    }
    else if (p->arg_id == "max_itr") {
      max_itr = atoi(arg_val);
      p = l.erase(p);
    }
    else if (p->arg_id == "gtheta") {
      gamma_theta = strtod(arg_val, &spill);
      p = l.erase(p);
    }
    else if (p->arg_id == "gphi") {
      gamma_phi = strtod(arg_val, &spill);
      p = l.erase(p);
    }
    else if (p->arg_id == "sampled_wordprobs") { // DONE migrate name
      sampled_wordprobs = string(arg_val);
      p = l.erase(p);
    }
    else if (p->arg_id == "sampled_senseprobs") { // DONE migrate name
      sampled_senseprobs = string(arg_val);
      p = l.erase(p);
    }
    else if (p->arg_id == "wordprobs_out") { // DONE migrate name
      wordprobs_out = string(arg_val);
      p = l.erase(p);
    }
    else if (p->arg_id == "senseprobs_out") { // DONE migrate name
      senseprobs_out = string(arg_val);
      p = l.erase(p);
    }
    else if (p->arg_id == "convert_to_EM") {
      convert_em = convert_to_bool(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "time_stamp") {
      time_stamp = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if(p->arg_id == "output_suffix") {
      output_suffix = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "input_suffix") {
      input_suffix = string(arg_val);
      p = l.erase(p);
    }
    else if(p->arg_id == "record_version") {
      record_version = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else {
      //if(p != l.end()) { p++; }
      p++;
    }


  }

  return l;
}

// TODO
void gibbsPar::show_params(ostream& f) {
  f << "********************************************\n";
  f << "these are the parameters recovered by interpreting the state of gs_params\nthis may include important defaults which were not explicitly given\n";
  f << "-expts " << results_location << endl;
  f << "-num_senses " << num_unsup_senses << endl;
  f << "-init_via_dirichlet " << bool_to_yes_no(init_via_dirichlet) << endl;
  if(wordprobs_in != "") {
    f << "-wordprobs " << wordprobs_in << endl; 
  }
  if(senseprobs_in != "") { 
    f << "-senseprobs " << senseprobs_in << endl;
  }
  if(senseprobs_string != "") {
    f << "-senseprobs_string " << senseprobs_string << endl;
  }
  f << "-sense_rand " << bool_to_yes_no(whether_sense_rand) << endl;
  f << "-rand_mix " << bool_to_yes_no(rand_mix) << endl;
  if(rand_mix) {
    f << "-rand_mix_level " << rand_mix_level << endl;
  }
  if(words_prior_type != "") {
    f << "-words_prior_type " << words_prior_type << endl;
  }
  f << "-sense_rand " << bool_to_yes_no(whether_sense_rand) << endl;
  f << "-rand_mix " << bool_to_yes_no(rand_mix) << endl;
  if(rand_mix) {
    f << "-rand_mix_level " << rand_mix_level << endl;
  }
  f << "-set_seed " << bool_to_yes_no(set_seed) << endl;
  f << "-set_sampling_seed " << bool_to_yes_no(set_sampling_seed) << endl;
  f << "-burn_itr " << burn_itr << endl;
  f << "-max_itr " << max_itr << endl;
  f << "-gtheta " <<  gamma_theta << endl;
  f << "-gphi " << gamma_phi << endl;
  f << "-sampled_wordprobs " << sampled_wordprobs << endl;
  f << "-sampled_senseprobs " << sampled_senseprobs << endl;
  f << "-convert_to_EM " << bool_to_yes_no(convert_em) << endl;
  if(convert_em) {
     f << "-wordprobs_out " << wordprobs_out << endl;
     f << "-senseprobs_out " << senseprobs_out << endl;
  }
  if(time_stamp) {
    f << "-time_stamp " << bool_to_yes_no(time_stamp) << endl;
  }
  if(output_suffix != "") {
    f << "-output_suffix " << output_suffix << endl;
  }
  if(input_suffix != "") {
    f << "-input_suffix " << input_suffix << endl;
  }
  f << "-record_version " << bool_to_yes_no(record_version) << endl;


}


// DONE
void gibbsPar::exit_with_help(bool whether_ex) {
  cout << "Usage: prog_name [options]\n";
  cout << 	"options: only those marked [required] have to be supplied\n";
  cout << "-expts DIRNAME: dir for recording results [required]\n";
  cout <<       "-num_senses N: number of unsup senses [required]\n";
  cout << "-init_via_dirichlet yes/no: init word and sense probs set by sampling from dirichlets (no)";
  cout << "-wordprobs FILENAME: of initial word probabilities "
          "(theta) in dynamicEM acceptable format\n";
  cout << "-senseprobs FILENAME: of initial sense probabilities "
    "(phi) in dynamicEM acceptable format\n";
  cout << "-senseprobs_string N/N/N.../N: initial sense probs, to be shared across years\n";
  cout << 	"-sense_rand yes/no: whether to generate random sense probs (no)\n";
  cout <<       "-rand_mix yes/no: whether to add random noise to sense probs initialisations (no) \n";
  cout <<       "-rand_mix_level N: 0 means no noise; 1.0 means all noise (0.0)\n";

  cout <<       "-words_prior_type: unif or corpus\n";
  cout << 	"-word_rand yes/no: whether to generate random word probs for each word (no)\n";
  cout <<       "-rand_word_mix yes/no; whether to add random noise to word prob initialisations (no)\n";
  cout <<       "-rand_word_mix_level N: 0 means no noise; 1.0 means all noise (0.0)\n";
  cout << "-words_prior_type [corpus|unif] \n";
  cout << "-set_seed yes/no: whether to set seed for random initialisations to 0 (reproducible) or not (yes)\n";
  cout << "-set_sampling_seed yes/no: whether to let sampler take default seed (reproducible) or not (yes)\n";
  cout << "-gtheta Gamma-theta(prior) value for the words\n";
  cout << "-gphi Gamma-phi(prior) value for the yearsn\n";
  cout << "-max_itr INTEGER: maximum number of iterations for the "
    "Gibbs sampler \n";
  cout << "-burn_itr INTEGER: number of burn-in iterations \n";

  cout << "-sampled_senseprobs FILENAME: to save the sampled sense probabilities "
          "(phi) [defaults to gibbs_sampled_sense_probs]\n";
  cout << "-sampled_wordprobs FILENAME: to save the sampled word probabilities "
          "(theta) [defaults to gibbs_sampled_word_probs]\n";

  cout << "-convert_to_EM [yes|no]: will decide whether to create final files recording means over samples (defaults to no)"
    "having same format as final results files from dynamicEM \n";
  cout << "-wordprobs_out FILENAME: name of file to record final means computed from the samples for word probabilities"
    "same format as final results file from dynamicEM [defaults to gibbs_wordprobs_final]\n";
  cout << "-senseprobs_out FILENAME: name of file to record final means computed from the samples for sense probabilities"
    "same format as final results file from dynamicEM [defaults to gibbs_senseprobs_final\n";
  cout << "-time_stamp yes/no: whether to add a time-stamp to output filenames (no)\n" ;
  cout <<       "-output_suffix STRING: STRING will added as suffix to outputs\n"; 
  cout <<       "-record_vesion yes/no: whether to include program version info in output (yes)\n";

  if(whether_ex) {
    exit(1);
  }
}





InspPar insp_params;

InspPar::InspPar() {
  num_unsup_senses = 0; // duplicate frm dynEMPar
  sense_names_file = "";// duplicate frm dynEMPar
  //corp_wc_min = 0.0;    // duplicate frm dynEMPar
  wordprobs_in = "";   // duplicate frm dynEMPar
  senseprobs_in = "";   // duplicate frm dynEMPar
  w_g_s = false;
  w_g_ss = false;
  w_g_s_old = false;
  first_sense = 0;
  second_sense = 0;
  w_g_y = false;
  w_g_y_old = false;
  first_year = "junk";
  second_year = "junk";

  sampled_wordprobs = ""; // duplicate from gibbsPar
  sample_w_g_s = false; // for looking at w-given-sense probs from particular sample
  which_it = 0;
  which_sense = 0;
  
  dtype = RATIO; 
  interp_equiv = 0.001;
  max_show = 30;
  show_format = "horiz";
  words_to_track = "";
  per_year_totals = false;
  show_vocab_stats = false;
  show_all_data = false;
  show_zeroes = false;
  make_data_prob = false;
  uniq_words = false;

  distance = false;
  symmetric = true;

  show_examples = false;
  num_examples = 0;
  example_year = "all";
  example_sort = "prob";
  no_duplicates = true;

  enter_loop = true;
}


ParList InspPar::process_params(ParList l) {

  vector<ArgVal>::iterator p;
  p = l.begin();

  char *spill;

  char *arg_val;

  


  while(p != l.end()) {
    // show_params(cout);
    // cout << "about to process " << p->arg_id << " " << p->arg_val << endl;

    arg_val = p->arg_val;

    /*************************************/
    /* 'sig' words for a sense (or year) */
    /*************************************/
     if(p->arg_id == "num_senses") {
      num_unsup_senses = strtoul(arg_val,&spill,10);
      p = l.erase(p);
    } else if(p->arg_id == "sense-names") {
      sense_names_file = string(arg_val);
      p = l.erase(p);
    } else if(p->arg_id == "wordprobs") {
      wordprobs_in = string(arg_val);
      p = l.erase(p);
    } else if(p->arg_id == "senseprobs") {
      senseprobs_in = string(arg_val);
      p = l.erase(p);
    } else if (p->arg_id == "w_g_s") {
      w_g_s = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "w_g_ss") {
      w_g_ss = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "first") {
      first_sense = strtoul(arg_val, &spill, 10);
      p = l.erase(p);
    } else if (p->arg_id == "second") {
      second_sense = strtoul(arg_val, &spill, 10);
      p = l.erase(p);
    }  else if (p->arg_id == "w_g_y") {
      w_g_y = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "first_year") {
      first_year = string(arg_val);
      p = l.erase(p);
    } else if (p->arg_id == "second_year") {
      second_year = string(arg_val);
      p = l.erase(p);
    } else if(p->arg_id == "sampled_wordprobs") {
      sampled_wordprobs = string(arg_val);
      p = l.erase(p);
    } else if (p->arg_id == "sample_w_g_s") {
      sample_w_g_s = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "which_it") {
      which_it = strtoul(arg_val, &spill, 10);
      p = l.erase(p);
    } else if (p->arg_id == "which_sense") {
      which_sense = strtoul(arg_val, &spill, 10);
      p = l.erase(p);
    } else if (p->arg_id == "ranking") { // freq to_other to_corpus
      ranking = string(arg_val);
      p = l.erase(p);
    } else if (p->arg_id == "dtype") { // RATIO vs KL_LR vs KL_RL vs KL_SYM
      string dtype_s = string(arg_val);
      if(dtype_s == "ratio") { dtype = RATIO;     p = l.erase(p);}
      else if(dtype_s == "kl_lr") { dtype = KL_LR;     p = l.erase(p);}
      else if(dtype_s == "kl_rl") { dtype = KL_RL;     p = l.erase(p);}
      else if(dtype_s == "kl_sym") { dtype = KL_SYM;     p = l.erase(p);}
      else {
	cout << "unexpected dtype value\n";
	exit_with_help(true);
      }
    } else if (p->arg_id == "interp_equiv") {
      interp_equiv = strtod(arg_val, &spill);
      p = l.erase(p);
    } else if (p->arg_id == "interp_lam") {
      interp_lam = strtod(arg_val, &spill); // not used yet
      p = l.erase(p);
    } else if (p->arg_id == "max_show") {
      max_show = strtoul(arg_val, &spill, 10);
      p = l.erase(p);
    } else if (p->arg_id == "show_format") {
      show_format  = string(arg_val);
      p = l.erase(p);
    }
    /*************************************/
    /* tracking particular words         */
    /*************************************/
    else if (p->arg_id == "track_words") {
      words_to_track = string(arg_val);
      p = l.erase(p);
    }
    /*************************************/
    /* entire corpus                     */
    /*************************************/
    else if (p->arg_id == "per_year_totals") {
      per_year_totals = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if (p->arg_id == "show_vocab_stats") {
      show_vocab_stats = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else if (p->arg_id == "show_all_data") {
      show_all_data = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "show_zeroes") {
      show_zeroes = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "data_prob") {
      make_data_prob = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "uniq_words") {
      uniq_words = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "distance") {
      distance = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "symmetric") {
      symmetric = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "show_examples") {
      show_examples = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "num_examples") {
      num_examples  = strtoul(arg_val, &spill, 10);
      p = l.erase(p);
    } else if (p->arg_id == "example_year") {
      example_year = string(arg_val);
      p = l.erase(p);
    } else if (p->arg_id == "example_sort") {
      example_sort = string(arg_val);
      p = l.erase(p);
    } else if (p->arg_id == "no_duplicates") {
      no_duplicates = convert_to_bool(string(arg_val));
      p = l.erase(p);
    } else if (p->arg_id == "enter_loop") {
      enter_loop = convert_to_bool(string(arg_val));
      p = l.erase(p);
    }
    else {
      p++; 
    }
  
  }
  //cout << "finished processing\n";  

  if(sample_w_g_s && (ranking == "to_other")) {
    cout << "-ranking to_other is not possible with -sample_w_g_s yes\n";
    exit_with_help(false); // ie. have another go
  }
  return l;
}


// don't currently use this for inspector
void InspPar::show_params(ostream& f) {

  f << "********************************************\n";
  f << "these are the parameters recovered by interpreting the state of params\nthis may include important defaults which were not explicitly given\n";
  f << "-num_senses " << num_unsup_senses << endl;
  if(sense_names_file != "") {
   f << "-sense-names " << sense_names_file << endl;
  }
  if(wordprobs_in != "") {
    f << "-wordprobs " << wordprobs_in << endl; 
  }
  if(senseprobs_in != "") {
    f << "-senseprobs " << senseprobs_in << endl;
  }
}

void InspPar::exit_with_help(bool wh_exit) {
  cout << 	"Usage: prog_name [options]\n";
  cout << 	"options: only those marked [required] have to be supplied\n";
  cout <<       "-num_senses N: number of unsup senses [required]\n";
  cout <<       "-sense-names FILENAME: of sense names file -- relevant only in sup case\n";
  cout <<       "-wordprobs FILENAME: of word probs\n";
  cout <<       "-senseprobs FILENAME: of sense probs\n"; 
  cout << 	"-first N: a sense number when comparing 2 senses\n";
  cout << 	"-second N: a sense number when comparing 2 senses\n";
  cout <<       "-w_g_s yes/no: whether to show significant words for a sense (no)\n";
  cout <<       "-w_g_ss yes/no: whether to show significant words for all senses (no)\n";
  cout <<       "-w_g_y yes/no: whether to show significant words for a year (no)\n";
  cout << 	"-first_year N: a year when comparing 2 years\n";
  cout << 	"-second_year N: a year when comparing 2 years\n";
  cout <<       "-ranking freq/to_other/to_corpus: sorting by freq/by comparison to other sense/by comparison to corpus \n"; // has no real default leading to error if none specified so forces being explicit
    cout <<       "-dtype ratio/kl_lr/kl_rl/kl_sym: 'distance type' between two distribs used in sorting to find sig words for a sense (ratio)\n";
  cout <<       " where p,q are word probs from two distribs ratio: p/q, kl_lr: p(log(p/q), kl_rl: q(log(q/p)), kl_sym: avge of 2 preceding\n";
  cout <<       "-interp_equiv EPS: added to p and q before 'distance' calc, equivalent to an interpolation with uniform prob (0.001)\n";
  cout << "\n\
    if interp_equiv is e, and interp_lam is l and u is uniform prob\n\
     e = ((1-l)/l) u \n\
     l = 1/(1 + e/u) \n\
     and the following ratios are equal \n\
      (p1 + e)/(p2 + e)   vs   (lp1 + (1-l)u)/(lp2 + (1-l2)u) \n";
  cout <<       "-interp_lam Lam: interpolation weight with a uniform prob, see above\n";
  cout <<       "-max_show N: number of significant words to show (30)\n";
  cout <<       "-show_format F: horiz or vert (vert)\n";
  cout <<       "-track_words W1/.../WN: make table of per-year probs for these words, useful to track time trend of words thought esp associated with a particular sense \n";
  cout <<       "-per_year_totals yes/no: whether to total frequencies for each year(false)\n";
  cout <<       "-show_vocab_stats yes/no: whether to show #types #tokens(false)\n";
  cout <<       "-show_all_data yes/no: whether to show the extracted corpus data, can be used to illuminate effect of corpus-extraction related params (false)\n";
  cout <<       "-show_zeroes yes/no: whether show count of words with zero prob in each sense\n";
  cout <<       "-data_prob yes/no: show total log prob of data (false)\n";
  cout <<       "-uniq_words yes/no: show data having unique words (false)\n";
  cout << "-show_examples yes/no: whether to show examples for each sense \n";
  cout << "-num_examples NUMBER: of examples for each sense (0)\n";
  cout << "-example_year [NUMBER|all] (all)\n";
  cout << "-example_sort [prob|diff]: how to sort egs whose best is S\n";
    cout <<  "prob: sort by S|words; diff: sort by S|words - S'|words, where S' is second best (prob)n";
  cout << "-no_duplicates yes/no: whether duplicates of an example type get suppressed (yes) \n";
  cout << "  note with n-grams (at full size) and with no year grouping, duplication can't arise but with grouping it can\n";
  cout << "  with Coha corpus and wide context duplication is very unlikely\n" << endl;
  cout << "-enter_loop yes/no: go into round of further inspection (yes)\n";
  //TODO explain others

  if(wh_exit) {
  exit(1);
  }
  
}

compPar comp_params;

compPar::compPar() {
  first_word = "";
  second_word = "";
  interp_equiv = 0.001;
}

ParList compPar::process_params(ParList l) {

  vector<ArgVal>::iterator p;
  p = l.begin();

  char *spill;

  char *arg_val;

  


  while(p != l.end()) {
    // show_params(cout);
    // cout << "about to process " << p->arg_id << " " << p->arg_val << endl;

    arg_val = p->arg_val;
  

    if (p->arg_id == "files1") {
      first_file_list_file = string(arg_val);
      p = l.erase(p);
    } else if (p->arg_id == "files2") {
      second_file_list_file = string(arg_val);
      p = l.erase(p);
    } else if (p->arg_id == "word1") {
      first_word = string(arg_val);
      p = l.erase(p);
    } else if (p->arg_id == "word2") {
      second_word = string(arg_val);
      p = l.erase(p);
    }
    // else if (p->arg_id == "dtype") { // RATIO vs KL_LR vs KL_RL vs KL_SYM
    //   string dtype_s = string(arg_val);
    //   if(dtype_s == "ratio") { dtype = RATIO;     p = l.erase(p);}
    //   else if(dtype_s == "kl_lr") { dtype = KL_LR;     p = l.erase(p);}
    //   else if(dtype_s == "kl_rl") { dtype = KL_RL;     p = l.erase(p);}
    //   else if(dtype_s == "kl_sym") { dtype = KL_SYM;     p = l.erase(p);}
    //   else {
    // 	cout << "unexpected dtype value\n";
    // 	exit_with_help(true);
    //   }
    // }
    else if (p->arg_id == "interp_equiv") {
       interp_equiv = strtod(arg_val, &spill);
       p = l.erase(p);
    }
    // else if (p->arg_id == "interp_lam") {
    //   interp_lam = strtod(arg_val, &spill); // not used yet
    //   p = l.erase(p);
    // } else if (p->arg_id == "max_show") {
    else {
      p++; 
    }
  }

  return l;
  
}

// don't currently use this for compare
void compPar::show_params(ostream& f) {

  f << "********************************************\n";
  f << "these are the parameters recovered by interpreting the state of params\nthis may include important defaults which were not explicitly given\n";
}

void compPar::exit_with_help(bool wh_exit) {
  cout << 	"Usage: prog_name [options]\n";
  cout << 	"options: only those marked [required] have to be supplied\n";
  cout <<       "-files1 FILENAME: file with list of files for first word [required] \n";
  cout <<       "-files2 FILENAME: file with list of files for second word [required] \n";
	
  cout <<       "-word1 W: first word in the comparison \n";
  cout <<       "-word2 W: second word in the comparison \n";

  cout <<       "-interp_equiv EPS: added to p and q before 'distance' calc, equivalent to an interpolation with uniform prob (0.001)\n";
  if(wh_exit) {
  exit(1);
  }
  
}



ExamplesPar eg_params;

ExamplesPar::ExamplesPar() {
  no_sents = 0;
  year = "all";
  sort_by = "prob";
  
}

ParList ExamplesPar::process_params(ParList l) {
  vector<ArgVal>::iterator p;
  p = l.begin();

  char *spill;

  char *arg_val;

  


  while(p != l.end()) {
    // show_params(cout);
    // cout << "about to process " << p->arg_id << " " << p->arg_val << endl;

    arg_val = p->arg_val;
  

    if (p->arg_id == "no_sents") {
      no_sents = strtoul(arg_val, &spill, 10);
      p = l.erase(p);
    } else if (p->arg_id == "year") {
      year = string(arg_val);
      p = l.erase(p);
    } else if (p->arg_id == "sort_by") {
      sort_by = string(arg_val);
      p = l.erase(p);
    }
        else {
      p++; 
    }
  }

  
  return l;

}

void ExamplesPar::show_params(ostream& f) {

}

void ExamplesPar::exit_with_help(bool wh_exit) {

  cout << "Usage: getSenseExamples [options]\n";
  cout << "options:\n";
  cout << "-no_sents NUMBER: of sentences for each sense \n";
  cout << "-year [NUMBER|all] \n";
  cout
    << "-sort_by [prob|diff] prob: gets the maximum probability value; diff: gets the maximum difference value"
    << endl;

  if(wh_exit) {
    exit(1);
  }


}





