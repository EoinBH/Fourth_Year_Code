#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include "Occ.h"
#include <algorithm>
#include <math.h>
#include <iomanip>

using namespace std;

#include "years.h"

#include "sym_table.h"

#include "prob_tables.h"

#include "senses.h"      

#include "CmdLineVersions.h"      

#include "probs_calc.h"

#include "StringUtil.h"

#include "process_corpus.h"

#include "compute_distance.h"

//#include "../GibbsSampling/EM_compatibility.h"

namespace Corp {
  vector<Occ> data;
}

// not in header yet
//void process_corpus_frm_archive(string archive_name, CorpPar corp_params);
#include "process_corpus_frm_archive.h"

// find how many data (which data) have a word which is unique to it
unsigned int count_elsewhere(int word_code, unsigned data_id);
void make_a_vector(string word_alternates, vector<string>& word_alt_vec);

// help stuff for example case
void get_top_sentences(unsigned int senses, int no_sents, string sort_by,
		       string year, bool no_dup);

void initialize_year_probs();
void update_prob_and_index_vec(vector<pair<double, int> > &prob_and_index_vec, int no_sents, pair<double, int> p, bool no_dup);

int main(int argc, char **argv) {


  // note going to use CorpPar, dynEMPar, InspPar
  // though only minutely need dynEMPar and most of its params have no impact
  ParList pars;
  pars = parse_command_line(argc, argv);
  ParList saved_pars;
  saved_pars = pars;
  if(check_for_help(pars)) {
    corp_params.exit_with_help(false);
    //em_params.exit_with_help(false);
    insp_params.exit_with_help(false);
    exit(1);
  }
  pars = corp_params.process_params(pars);
  //pars = em_params.process_params(pars);
  pars = insp_params.process_params(pars);
  if(pars.size() != 0) {
    exit_with_help(pars);
  }

  
  if((corp_params.corp_wc_min > 0.0) && (insp_params.interp_equiv > 0.0)) {
    cout << "you prob don't want to set non-zero of both corp_wc_min and interp_equiv\n";
    insp_params.exit_with_help(false);
    exit(1);
  }

  
  corpus_type = corp_params.corpus_type;
  if (corp_params.closed == true) {
    whether_closed = true;
  }

  desired_left = corp_params.left;
  desired_right = corp_params.right;
  whether_pad = corp_params.whether_pad;
  lowercase = corp_params.lowercase;
  filter_ngram_meta_token = corp_params.filter_ngram_meta_token;
  syn_info = corp_params.syn_token_parts;
  excl_targ_nnp =  corp_params.excl_targ_nnp; // excl_targ_nnp owned by SynGramToken.cpp
  excl_targ_np1 =  corp_params.excl_targ_np1; // excl_targ_np1 owned by CohaToken.cpp
  data_csv_suffix = corp_params.whether_csv_suffix;
  grouping_size = corp_params.grouping_size;
  coha_wlp_field = corp_params.wlp;
  coha_targ_wlp_field = corp_params.targ_wlp;
  filter_coha_at_token = corp_params.filter_coha_at_token; // filter_coha_at_token owned by CohaToken.cpp
  whole_coha_article = corp_params.whole_coha_article; // set Occ contents to entire coha article

  if(corp_params.whether_read_archive == false) { // no archive
    
    if (corp_params.symbol_table_name != "") {
      process_corpus(corp_params.file_list_file, corp_params.symbol_table_name, true,
		     false, corp_params.target_alternates);
      // Corp::data now contains Occ reps of all data items
    } else {
      process_corpus(corp_params.file_list_file, corp_params.symbol_table_name, false,
		     false, corp_params.target_alternates);
    }
  }
  else { // with archive
    process_corpus_frm_archive(corp_params.archive_name, corp_params);
  }
  
  cout << "read all data\n";
  cout << "total amount of extracted data is: " << Corp::data.size() << endl;
  if (corpus_type == 2) {
    cout << "inc each n-grams count potentially total occurrences: "
	 << total_occurrences << endl;
    cout << "prob closer to " << total_occurrences / 5 << endl;
  }

  if (insp_params.sense_names_file != "") {
    initialize_true_senses(insp_params.sense_names_file); // not sure if anything currently uses this
    // true_senses should now have right size and values
  } else {
    initialize_unsup_senses(insp_params.num_unsup_senses);
    // senses now has expected size, and each associated with dummy name
  }

  /******************************************** */
  /* do set up for options that need word probs */
  /**********************************************/
  if (insp_params.w_g_s || insp_params.w_g_ss || insp_params.make_data_prob || insp_params.show_examples) { 
    if (insp_params.wordprobs_in == "") {
      cout << "need to specify word probs file\n";
      exit(1);
    }
    bool verdict;
    string problem;
    verdict = initialize_wordprobs_from_file(insp_params.wordprobs_in, problem);
    if (!verdict) {
      cout << problem << endl;
      exit(0);
    }
  }

  /***********************************************/
  /* do set up for options that need sense probs */
  /***********************************************/
  if (insp_params.make_data_prob || insp_params.show_examples) { 
    if (insp_params.senseprobs_in == "") {
      cout << "need to specify sense probs file\n";
      exit(1);
    }
    bool verdict;
    string problem;
    verdict = initialize_senseprobs_from_file(insp_params.senseprobs_in,problem);
    if (!verdict) {
      cout << problem << endl;
      exit(0);
    }
  }

  bool go_again = true;

  do {
    /**************************************/
    /* start processing options from here */
    /**************************************/

  
    if (insp_params.w_g_s) {

      if (insp_params.first_sense == insp_params.second_sense) {
	cout << "need to which pair of senses to contrast\n";
	exit(1);
      }

      // ranking options: freq to_other to_corpus

      string firstname, secondname;
      if (insp_params.ranking == "to_other") {

	if (insp_params.sense_names_file == "") {
	  firstname = "SENSE " + int_to_stl(insp_params.first_sense);
	  secondname = "SENSE " + int_to_stl(insp_params.second_sense);
	} else {
	  firstname = sense_to_string(insp_params.first_sense);
	  secondname = sense_to_string(insp_params.second_sense);
	}

	show_one_word_stats(wordprobs[insp_params.first_sense], firstname,
			    wordprobs[insp_params.second_sense], secondname,
			    insp_params.dtype, insp_params.interp_equiv, insp_params.max_show, insp_params.show_format,
			    cout);
	show_one_word_stats(wordprobs[insp_params.second_sense], secondname,
			    wordprobs[insp_params.first_sense], firstname,
			    insp_params.dtype, insp_params.interp_equiv,insp_params.max_show, insp_params.show_format,
			    cout);
      } else if (insp_params.ranking == "to_corpus") {
	set_corpus_word_probs(corp_params.corp_wc_min);
	//firstname = sense_to_string(params.first_sense);
	firstname = "SENSE " + int_to_stl(insp_params.first_sense);
	secondname = "corpus";
	show_one_word_stats(wordprobs[insp_params.first_sense], firstname,
			    wordprobs_corp, secondname,
			    insp_params.dtype, insp_params.interp_equiv, insp_params.max_show,insp_params.show_format,
			    cout);
	firstname = "SENSE " + int_to_stl(insp_params.second_sense);
	show_one_word_stats(wordprobs[insp_params.second_sense], firstname,
			    wordprobs_corp, secondname,
			    insp_params.dtype, insp_params.interp_equiv, insp_params.max_show, insp_params.show_format,
			    cout);
      } else if (insp_params.ranking == "freq") {
	firstname = "SENSE " + int_to_stl(insp_params.first_sense);
	show_one_word_stats(wordprobs[insp_params.first_sense], firstname,
			    insp_params.max_show, insp_params.show_format,
			    cout);
	secondname = "SENSE " + int_to_stl(insp_params.second_sense);
	show_one_word_stats(wordprobs[insp_params.second_sense], secondname,
			    insp_params.max_show, insp_params.show_format,
			    cout);
      } else {
	cout << "unknown ranking type";
	insp_params.exit_with_help(true);
      }

    }

    if (insp_params.w_g_ss) {
      string firstname, secondname;
      set_corpus_word_probs(corp_params.corp_wc_min);
      
      for(int i = 0; i < insp_params.num_unsup_senses; i++) {
	firstname = "SENSE " + int_to_stl(i);
	secondname = "corpus";
	show_one_word_stats(wordprobs[i], firstname,
			    wordprobs_corp, secondname,
			    insp_params.dtype, insp_params.interp_equiv, insp_params.max_show, insp_params.show_format,
			    cout);
      }
    }

    if (insp_params.w_g_y) {
      set_corpus_word_probs(corp_params.corp_wc_min);

      if (insp_params.first_year == insp_params.second_year) {
	cout << "need to which pair of years to contrast\n";
	exit(1);
      }

      if (insp_params.ranking == "to_other") {
	show_one_word_stats(wordprobs_given_year[insp_params.first_year],
			    insp_params.first_year, wordprobs_given_year[insp_params.second_year],
			    insp_params.second_year, insp_params.dtype, insp_params.interp_equiv,insp_params.max_show, insp_params.show_format,
			    cout);
	show_one_word_stats(wordprobs_given_year[insp_params.second_year],
			    insp_params.second_year, wordprobs_given_year[insp_params.first_year],
			    insp_params.first_year, insp_params.dtype, insp_params.interp_equiv,insp_params.max_show, insp_params.show_format,
			    cout);

      } else if (insp_params.ranking == "to_corpus") {
	show_one_word_stats(wordprobs_given_year[insp_params.first_year],
			    insp_params.first_year, wordprobs_corp, "corpus",
			    insp_params.dtype,insp_params.interp_equiv,insp_params.max_show, insp_params.show_format,
			    cout);
	show_one_word_stats(wordprobs_given_year[insp_params.second_year],
			    insp_params.second_year, wordprobs_corp, "corpus",
			    insp_params.dtype,insp_params.interp_equiv,insp_params.max_show, insp_params.show_format,
			    cout);

      } else if (insp_params.ranking == "freq") {
	show_one_word_stats(wordprobs_given_year[insp_params.first_year],
			    insp_params.first_year, insp_params.max_show, insp_params.show_format,
			    cout);
	show_one_word_stats(wordprobs_given_year[insp_params.second_year],
			    insp_params.second_year, insp_params.max_show, insp_params.show_format,
			    cout);

      } else {
	cout << "unknown ranking type";
	insp_params.exit_with_help(true);
      }

      
    }

    if(insp_params.sample_w_g_s) {
      cout << "temp withdrawn option relating to Gibbs Sampling\n";
      // set_corpus_word_probs(corp_params.corp_wc_min);
      // // ranking options: freq to_corpus
      // map<int, double> one_sense_wordprobs; // to store the request w-given-sense probs (EM-style)
      // read_word_probs_from_sampled_wordprobs_file(insp_params.sampled_wordprobs, insp_params.num_unsup_senses, insp_params.which_it, insp_params.which_sense, one_sense_wordprobs);

      // string firstname, secondname;
      // if (insp_params.ranking == "to_corpus") {



      // 	firstname = "SENSE " + int_to_stl(insp_params.which_sense);
      // 	secondname = "corpus";
      // 	show_one_word_stats(one_sense_wordprobs, firstname,
      // 			    wordprobs_corp, secondname,
      // 			    insp_params.dtype, insp_params.interp_equiv, insp_params.max_show,insp_params.show_format,
      // 			    cout);

      // } else if (insp_params.ranking == "freq") {
      // 	firstname = "SENSE " + int_to_stl(insp_params.which_sense);
      // 	show_one_word_stats(one_sense_wordprobs, firstname,
      // 			    insp_params.max_show, insp_params.show_format,
      // 			    cout);
      // 	// for debugging, so show what the corpus probs were	
      // 	secondname = "corpus";
      //   show_one_word_stats(wordprobs_corp, secondname,
      // 			    insp_params.max_show, insp_params.show_format,
      // 			    cout);
      // 	// end debugging
  	
      // } else {
      // 	cout << "unknown ranking type";
      // 	insp_params.exit_with_help(true);
      // }

    }
    
    if (insp_params.per_year_totals) {
      cout << "total amounts per time period\n";
      string year_id;
      for(int i=0; i < years.size(); i++) {
	year_id = years[i];
	int amount = 0;
	for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {
	  Occ d;
	  d = Corp::data[dn];
	  if(d.year == year_id) {
	    amount += d.rpt_cntr;
	  }
	}
	cout << year_id << " " << amount << endl;
      }
    }

    
    if (insp_params.show_vocab_stats) {
      cout << "#types in total:" << symbol_total << endl;
      int tokens = 0;
      for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {
	Occ d;
	d = Corp::data[dn];
	for (unsigned int i = 0; i < d.words.size(); i++) {
	  double word_contrib = 0.0;
	  if (d.words[i] != 0) { // 0 is out-of-table
	    tokens += (1 * d.rpt_cntr);
	  } 
	}
      }
      cout << "#token in total:" << tokens << endl;      
    }

    
    
    if (insp_params.show_all_data) {

      for (unsigned int data_id = 0; data_id < Corp::data.size(); data_id++) {
	cout << "data_id " << data_id << endl;
	Corp::data[data_id].show();
      }

    }

    if (insp_params.make_data_prob) {
      // show_sense_probs(0, cout);
      // go thru every data point accumulating log of joint prob
      double total_prob = 0.0;
      double year_base;
      double words_g_sense;
      double sense_g_year;
      year_base = 1.0/years.size();
      for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {
	Occ d;
	d = Corp::data[dn];
	for (unsigned int s = 0; s < senses.size(); s++) {
	  words_g_sense = calc_words_g_sense(d, s);
	  sense_g_year = senseprobs[d.year][s];
	  //k * P(C^d =i |year^d) * P(words^d|C^d = i)
	  total_prob += (year_base * sense_g_year * words_g_sense);
	}
      }
      cout << "log data prob: " << -1.0 * log(total_prob) << endl;
    }

    if(insp_params.show_zeroes) {
       show_count_zeros_in_word_stats(wordprobs, cout);
    }

    // find how many data (which data) have a word which is unique to it
    //bool unique_words = false;
    if (insp_params.uniq_words) {
      vector<int> wds;
      for (unsigned int data_id = 0; data_id < Corp::data.size(); data_id++) {
	wds = Corp::data[data_id].words;
	vector<string> uniq;
	for (unsigned int j = 0; j < wds.size(); j++) {

	  unsigned int elsewhere = 0;
	  elsewhere = count_elsewhere(wds[j], data_id);
	  if (elsewhere == 0) {
	    uniq.push_back(decode_to_symbol(wds[j]));

	  }
	}

	if (uniq.size() > 0) {
	  cout << data_id << " has unique words: ";
	  for (unsigned i = 0; i < uniq.size(); i++) {
	    cout << uniq[i] << " ";
	  }
	  cout << endl;
	} else {
	  cout << data_id << " has no unique words\n";
	}

      }
    }

    if (insp_params.words_to_track != "") {
      set_corpus_word_probs(corp_params.corp_wc_min);
      vector<string> word_alt_vec;
      make_a_vector(insp_params.words_to_track, word_alt_vec);
      cout << "year total ";
      for (int i = 0; i < word_alt_vec.size(); i++) {
	cout << word_alt_vec[i];
	if (i != word_alt_vec.size() - 1) {
	  cout << " ";
	}
      }
      cout << endl;
      vector<string>::iterator yr_itr;
      for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
	cout << *yr_itr << " ";
	cout << wordtotal_given_year[*yr_itr] << " ";
	for (int i = 0; i < word_alt_vec.size(); i++) {
	  cout << wordprobs_given_year[*yr_itr][get_code(word_alt_vec[i])];
	  // temp mod used to see low counts for track words with mouse in coha corpus
	  //	cout << wordcounts_given_year[*yr_itr][get_code(word_alt_vec[i])];
	  if (i != word_alt_vec.size() - 1) {
	    cout << " ";
	  }
	}
	cout << endl;
      }
    }

    if (insp_params.show_examples) {
      initialize_year_probs();
      // verbatim from getSenseExamples
      for (unsigned int i = 0; i < Corp::data.size(); ++i) {
	vector<double> joint_probs;
	double data_prob = 0.0, max_prob = 0.0;
	SENSE S;
	// // just for debuggin
	// if ((Corp::data[i].words[0] == 639) && (Corp::data[i].words[1] == 579)) {
	//   cout << "looking at consilium pompeii\n";
	// }
	categorise_by_data_with_eps(Corp::data[i], data_prob, max_prob, joint_probs, S, insp_params.interp_equiv);
	Corp::data[i].data_prob = data_prob;
	Corp::data[i].max_prob = max_prob / data_prob;
	Corp::data[i].diff_to_max = (joint_probs[0] / data_prob)
	  - (joint_probs[1] / data_prob);
	Corp::data[i].inferred_sense = S;

#if DEBUG
	cout << i << Corp::data[i].inferred_sense << "::" << Corp::data[i].data_prob << "::"
	     << Corp::data[i].max_prob << "::" << Corp::data[i].diff_to_max << endl;
#endif
      }
      get_top_sentences(insp_params.num_unsup_senses, insp_params.num_examples, insp_params.example_sort, insp_params.example_year, insp_params.no_duplicates);

    }


    if(!insp_params.enter_loop) {
      go_again = false;
    }
    else {
      string line;
      cout << "quit or continue\n";
      getline(cin,line);
      if(line == "quit") { go_again = false;}
      else {
	bool got_some_params = false;
	while(!got_some_params) {
	  string some_alternative_params;
	  cout << "enter some new params\n";
	  getline(cin,some_alternative_params);
	  pars = parse_fresh_params(some_alternative_params,got_some_params);
	  if(check_for_help(pars)) {
	    corp_params.exit_with_help(false);
	    insp_params.exit_with_help(false);
	  }
	  pars = insp_params.process_params(pars);
	}
        go_again = true;      
      }
    }
    
  } while (go_again);
  
return 0;
}

unsigned int count_elsewhere(int word_code, unsigned data_id) {
  unsigned int num_elsewhere = 0;
  vector<int> wds;
  for (unsigned int i = 0; i < data_id; i++) {
    wds = Corp::data[i].words;
    for (unsigned int j = 0; j < wds.size(); j++) {
      if (wds[j] == word_code) {
	num_elsewhere++;
      }
    }
  }

  for (unsigned int i = data_id + 1; i < Corp::data.size(); i++) {
    wds = Corp::data[i].words;
    for (unsigned int j = 0; j < wds.size(); j++) {
      if (wds[j] == word_code) {
	num_elsewhere++;
      }
    }
  }

  return num_elsewhere;
}


void make_a_vector(string word_alternates, vector<string>& word_alt_vec) {

  /* word_alternates of form
     word1/word2/..../wordN
  */

  size_t pos = 0;

  // counts the slashes
  while ((pos = word_alternates.find('/', pos)) != string::npos) {
    pos++;
  }

  size_t targ_start, targ_end;
  string one_targ;
  targ_start = 0;
  while ((targ_end = word_alternates.find('/', targ_start)) != string::npos) {
    one_targ = word_alternates.substr(targ_start, targ_end - targ_start);
    word_alt_vec.push_back(one_targ);
    targ_start = targ_end + 1;
  }

  one_targ = word_alternates.substr(targ_start);
  word_alt_vec.push_back(one_targ);

  return;
}

void get_top_sentences(unsigned int senses, int no_sents, string sort_by, string year, bool no_dup) {
  map<SENSE, vector<pair<double, int> > > top_N_sents;
  // set prop empty vector for each sense
  for (unsigned int i = 0; i < senses; ++i) {
    vector<pair<double, int> > prob_and_index_vec;
    top_N_sents[i] = prob_and_index_vec;
  }
  cout << "\n\n" << endl;
  double sort_val;
  for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {

    SENSE key = Corp::data[dn].inferred_sense;

    if (sort_by == "diff")
      sort_val = Corp::data[dn].diff_to_max;
    else
      sort_val = Corp::data[dn].max_prob;

#if DEBUG
    if (key == 1 && sort_val > 0.5)
      cout << sort_val << "::" << key << endl;
#endif


    if (top_N_sents[key].size() < no_sents) { 
      if ((year == "all") ||  (Corp::data[dn].year == year)) {
	update_prob_and_index_vec(top_N_sents[key], no_sents, make_pair(sort_val, dn), no_dup);
      }
    }
    else if (sort_val > top_N_sents[key].back().first) {
      if((year == "all") ||  (Corp::data[dn].year == year)) {
	update_prob_and_index_vec(top_N_sents[key], no_sents, make_pair(sort_val, dn), no_dup);
	// top_N_sents[key].pop_back();
	// top_N_sents[key].push_back(make_pair(sort_val, dn));
#if DEBUG
	cout << "Printing DEBUG for SENSE" << key << endl;
	for (int d = 0; d < top_N_sents[key].size(); ++d)
	  cout << top_N_sents[key][d].first << "::"
	       << top_N_sents[key][d].second << endl;
#endif
      }
    }
    else {
      continue;
    }
    sort(top_N_sents[key].rbegin(), top_N_sents[key].rend());
    // debug
    // cout << "size of top_N_sets for sense " << key << ": " << top_N_sents[key].size() << endl;
  }

  /**
   * prints sentences in descending order of probability
   * for each sense
   */
  cout << "for each sense k, top_N from cases where k is best sense " << endl;
  for (unsigned int i = 0; i < senses; ++i) {
    cout << "SENSE_" << i << " TOP " << no_sents << " SENTENCES (sorted by ";
        if(sort_by == "prob") { cout << "SCORE = S|w"; }
	    else { cout << "SCORE = S|w - S'|w, where S' is second best";}
    cout << " )\n";
    cout << "*************************" << endl;
    cout << "SCORE" << "\t" << "SENTENCE" << "\t\t\t" << "YEAR\n";
    if (top_N_sents[i].size() <= 0)
      cout << "Nothing found" << endl;
    else {
      for (int j = 0; j < top_N_sents[i].size(); ++j) {
	//	cout << setprecision(10) << top_N_sents[i][j].first << "\t"
	//	  cout << setprecision(10) << Corp::data[top_N_sents[i][j].second].max_prob << "\t"
	cout << setprecision(10) << top_N_sents[i][j].first << "\t"
	     << Corp::data[top_N_sents[i][j].second].make_string() << "\t"
	     << Corp::data[top_N_sents[i][j].second].year << endl;
      }
      cout << endl;
    }
  }
}

void initialize_year_probs() {
  vector<string>::iterator yr_itr;

  float yr_norm;
  yr_norm = years.size();

  for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
    yearprobs[*(yr_itr)] = 1 / yr_norm;
  }
}

// if less than desired size just add
// if at desired size, pop and push
// if no_dup is true, never adds a duplicate
void update_prob_and_index_vec(vector<pair<double, int> > &prob_and_index_vec, int no_sents, pair<double, int> p, bool no_dup) {

  // check if there
  bool there = false;
  if(no_dup) {
    for(int i = 0; i < prob_and_index_vec.size(); i++) {
      pair<double, int> other_p;
      other_p = prob_and_index_vec[i];
      if(Corp::data[p.second].words == Corp::data[other_p.second].words) {
	there = true;
	break;
      }
    }
  }
  
  if(there) { return;}
  
  if(prob_and_index_vec.size() < no_sents) {
    prob_and_index_vec.push_back(p);
  }
  else {
    prob_and_index_vec.pop_back();
    prob_and_index_vec.push_back(p);
  }
  
}
