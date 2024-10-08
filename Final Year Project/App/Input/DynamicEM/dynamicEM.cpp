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

#include "probs_calc.h"

#include "StringUtil.h"

#include "Version.h"

#include "timestamp.h"

namespace Corp {
  vector<Occ> data;
}

Version version;

#include "process_corpus.h"

#include "process_corpus_frm_archive.h"

void initialize_year_probs(void);


void show_probs(unsigned int it, ostream& s_stats, ostream& w_stats);

void initialize_counts(bool whether_sup);

void do_categorisation(unsigned int it, ostream& cat_dtls);


/* if whether_sup is true then use the setting of the 'hidden' sense 
 * var that is in the data -- so all but one alternative is certain
 * if whether_sup is false then really do the EM thing
 */
double do_em(bool whether_sup, double *sum_log_prob, double *log_prior_prob);

double do_exp(bool whether_sup, double *sum_log_prob);

void do_max(void);

double eval_categorisation(void);

void save_categorisation(unsigned int it, ostream& cat_outcomes);

// all the file streams for logging outputs 
class Outputs {
public:
  ofstream s_stats;  // sense distribs
  ofstream w_stats;  // word stats
  ofstream cat_dtls; // categorisation details
  ofstream cat_outcomes; // categorisation outcomes
  ofstream acc_dtls; // accuracy details
  ofstream param_dtls; // all the param settings
  void close_all(void);
};

void Outputs::close_all() {

  s_stats.close();
  if(em_params.whether_save_word_stats) {
    w_stats.close();
  }
  cat_dtls.close();
  acc_dtls.close();
  if (em_params.whether_save_cat_outcomes) {
    cat_outcomes.close();
  }
  param_dtls.close();
}

Outputs outputs;

void open_an_output(ofstream& strm, string name);
void write_version_info(void);

unsigned int iteration_counter; // just used to pass iteration info to printing


int main(int argc, char **argv) {
  ParList pars;
  pars = parse_command_line(argc, argv);
  ParList saved_pars;
  saved_pars = pars;
  if(check_for_help(pars)) {
    corp_params.exit_with_help(false);
    em_params.exit_with_help(false);
    exit(1);
  }
  pars = corp_params.process_params(pars);
  pars = em_params.process_params(pars);
  if(pars.size() != 0) {
    exit_with_help(pars);
  }


  
  if(corp_params.closed == true) {whether_closed = true;}

  desired_left = corp_params.left;
  desired_right = corp_params.right;
  whether_pad = corp_params.whether_pad;

  // desired_left = 2;
  // desired_right = 2;
  corpus_type = corp_params.corpus_type;
  lowercase = corp_params.lowercase;
  filter_ngram_meta_token = corp_params.filter_ngram_meta_token;
  syn_info = corp_params.syn_token_parts;
  excl_targ_nnp =  corp_params.excl_targ_nnp; // excl_targ_nnp owend by SynGramToken.cpp
  excl_targ_np1 =  corp_params.excl_targ_np1; // excl_targ_np1 owend by CohaToken.cpp
  data_csv_suffix = corp_params.whether_csv_suffix;

  grouping_size = corp_params.grouping_size;
  coha_wlp_field = corp_params.wlp;
  coha_targ_wlp_field = corp_params.targ_wlp;
  filter_coha_at_token = corp_params.filter_coha_at_token; // filter_coha_at_token owned by CohaToken.cpp
    
  years_model = em_params.dynamic; // years_model owned by probs_calc  
  if(em_params.time_stamp) {
    output_suffix = time_stamp();
  }
  else {
    output_suffix = em_params.output_suffix;
  }

  input_suffix = em_params.input_suffix;
  timestamp start_t = get_timestamp();

  if(corp_params.whether_read_archive == false) { // no archive
    if(corp_params.symbol_table_name != "") {
      process_corpus(corp_params.file_list_file,corp_params.symbol_table_name,true,false,corp_params.target_alternates); 
      // Corp::data now contains Occ reps of all data items
    }
    else {
      string newtable_name = "generated_sym_table";
      symbol_table_file_source = em_params.results_location + "/" + add_suffix(newtable_name);
      process_corpus(corp_params.file_list_file,symbol_table_file_source,false,true,corp_params.target_alternates);
    }
  }
  else { // with archive
    process_corpus_frm_archive(corp_params.archive_name, corp_params);
  }
    
  timestamp readata_t = get_timestamp();
  cout << "read all data\n";
  cout << "total lines of extracted data is: " << Corp::data.size() << endl;
  cout << "time taken to load data: " << ((readata_t - start_t) / 1000000.0L) << endl;
  if(corpus_type == 2) {
    cout << "inc each n-grams count potentially total occurrences: " << total_occurrences << endl;
    cout << "prob closer to " << total_occurrences/5 << endl;
  }


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

  // debug

  // for(unsigned int d=0; d < Corp::data.size(); d++) {
  //   Corp::data[d].show();
  // }


  sense_names_for_wordprobs_filename = em_params.sense_names_file;
  sense_names_for_senseprobs_filename = em_params.sense_names_file;

  if(em_params.do_sup) {
    if(em_params.sense_names_file != "") {
      initialize_true_senses(em_params.sense_names_file);
      // true_senses should now have right size and values
    }
    else {
      cout << "need to supply sense_names file for sup\n";
      exit(1);
    }
  }

  if(em_params.do_unsup) {
    initialize_unsup_senses(em_params.num_unsup_senses);
    // senses now has expected size, and each associated with dummy name
  }

  double result_acc;
  double total_prob;

  // set stream to capture various kinds of outputs
  //ofstream s_stats;
  open_an_output(outputs.s_stats, "sense_distribs");

  //vector<string> sn_for_stats(5);
  vector<string> sn_for_stats(8);
  sn_for_stats[0] = "One";
  sn_for_stats[1] = "Two";
  sn_for_stats[2] = "Three";
  sn_for_stats[3] = "Four";
  sn_for_stats[4] = "Five";
    sn_for_stats[5] = "Six";
    sn_for_stats[6] = "Seven";
    sn_for_stats[7] = "Eight";

  outputs.s_stats << "It Year";
  for(unsigned int j=0; j < (em_params.do_unsup ? em_params.num_unsup_senses : true_senses.size()); j++) {
    outputs.s_stats << " " << sn_for_stats[j];
  }

  outputs.s_stats << endl;

  //ofstream w_stats;
  if(em_params.whether_save_word_stats) {
    open_an_output(outputs.w_stats,"word_stats");
  }
  //ofstream cat_dtls;
  open_an_output(outputs.cat_dtls,"cat_details");

  //ofstream cat_outcomes;
  if(em_params.whether_save_cat_outcomes) {
    open_an_output(outputs.cat_outcomes,"cat_outcomes");
    outputs.cat_outcomes << "Year It True Inferred\n";
  }

  //ofstream acc_dtls;
  open_an_output(outputs.acc_dtls,"accuracy_details");

  //ofstream param_dtls;
  open_an_output(outputs.param_dtls,"param_details");
  //saves verbatim params
  show_params(saved_pars,outputs.param_dtls);
  //saves param settings including defaults not explicitly set
  corp_params.show_params(outputs.param_dtls);
  em_params.show_params(outputs.param_dtls);

  if(em_params.record_version) {
    write_version_info();
  }


  /********************/
  /* INITIALIZE PROBS */
  /********************/

  bool verdict;
  string problem;
  
  set_corpus_word_probs(corp_params.corp_wc_min);
  symbol_table_for_wordprobs_filename = symbol_table_file_source;

  UNIFTYPE t;

  if(em_params.do_unsup) {
    /*************************/
    /* UNSUP INITIALISATIONS */
    /*************************/
    // WORD PROBS
    if(em_params.wordprobs_in != "") {
      verdict = initialize_wordprobs_from_file(add_input_suffix(em_params.wordprobs_in),problem);
      if(verdict) {
	//show_word_stats(wordprobs, "probs", 0, cout);
      }
      else {
	cout << problem << endl;
	exit(0);
      }
    }
    else if(em_params.whether_word_rand == true && em_params.words_prior_type == "") {
      // RANDOM
      random_initialize_wordprobs(em_params.set_seed);
    }
    else if(em_params.whether_word_rand == false && em_params.words_prior_type != "") {
      // UNIFORM
      t = convert_to_uniftype(em_params.words_prior_type);
      uniform_initialize_wordprobs(t,false, 0.0);
    }
    else if(em_params.whether_word_rand == true && em_params.words_prior_type != "") {
      // UNIF WITH NOISE
      random_initialize_wordprobs(em_params.set_seed);
      t = convert_to_uniftype(em_params.words_prior_type);
      uniform_initialize_wordprobs(t,em_params.rand_word_mix, em_params.rand_word_mix_level);
    }
    else {
      cout << "no way given to initialize word probs\n";
      em_params.exit_with_help(true);
    }

    // SENSE PROBS

    if(em_params.senseprobs_in != "") {
      // FROM FILE
      verdict = initialize_senseprobs_from_file(add_input_suffix(em_params.senseprobs_in),problem);
      if(verdict) {
	show_sense_probs(0, cout);
      }
      else {
	cout << problem << endl;
	exit(0);
      }
    }
    else if(em_params.whether_sense_rand == true && em_params.senseprobs_string == "") {
      // RANDOM
      random_initialize_senseprobs(em_params.set_seed);
    }
    else if(em_params.whether_sense_rand == false && em_params.senseprobs_string != "") {
      // UNIFORM
      uniform_initialize_senseprobs(em_params.senseprobs_string,false, em_params.rand_mix_level);
    }
    else if(em_params.whether_sense_rand == true && em_params.senseprobs_string != "") {
      // UNIF WITH NOISE
      random_initialize_senseprobs(em_params.set_seed);
      uniform_initialize_senseprobs(em_params.senseprobs_string,em_params.rand_mix, em_params.rand_mix_level);
    }
    else {
      cout << "no way given to initialize sense probs\n";
      em_params.exit_with_help(true);
    }
  }
  else {
    /***********************/
    /* SUP INITIALISATIONS */
    /***********************/

    // set up if supervised
    wordprobs.resize(true_senses.size());// right num of entries, but each entry is empty map
    vector<double> v;
    v.resize(true_senses.size());
    vector<string>::iterator yr_itr;
    for(yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
      senseprobs[*yr_itr] = v;
    }
    // each year gets right number of senses, but no significant info

  }

  // YEAR PROBS
  initialize_year_probs();
  timestamp sense_initialization = get_timestamp();
  cout << "time taken to initialize senses: " << ((sense_initialization - readata_t) / 1000000.0L) << endl;

  // result_acc = do_categorisation();
  // cout << "result accuracy on initial probs: " << result_acc << endl;

  if(em_params.do_sup) {

      double sum_log_prob;
      double log_prior_prob;
      do_em(true, &sum_log_prob, &log_prior_prob); // do supervised training

    cout << "*********starting categorisation on supervised probs *********" << endl;
    cout << "*********calculating result accuracy on supervised probs ******" << endl;
    do_categorisation(0,outputs.cat_dtls);
    result_acc = eval_categorisation();
    outputs.acc_dtls << 0 << " " << result_acc << endl;
    if(em_params.whether_save_cat_outcomes) {
      save_categorisation(0,outputs.cat_outcomes);
    }
    show_probs(0,outputs.s_stats,outputs.w_stats);
  }
  else if(em_params.do_unsup) {
    show_probs(0,outputs.s_stats,outputs.w_stats);
    cout << "*********starting categorisation on initial probs *********" << endl;
    do_categorisation(0,outputs.cat_dtls);
    result_acc = eval_categorisation();
    outputs.acc_dtls << 0 << " " << result_acc << " ";
    if(em_params.whether_save_cat_outcomes) {
      save_categorisation(0,outputs.cat_outcomes);
    }

    if(em_params.save_starts) {
      string aname;
      aname = em_params.results_location + "/unsup_wordprobs_start";
      write_word_stats(wordprobs,add_suffix(aname));
      aname = em_params.results_location + "/unsup_senseprobs_start";
      write_senseprobs(add_suffix(aname));
    }

    cout << "********doing rounds of unsup training ******\n";

    
    for(unsigned int i=1; i <= em_params.max_it; i++) {
      //cout << "********** ITERATION " << i << "  ?*************\n";
      iteration_counter = i;// just used to pass iteration info to printing
      double sum_log_prob;
      double log_prior_prob;
      total_prob = do_em(false, &sum_log_prob, &log_prior_prob);// do unsupervised training
      outputs.acc_dtls << total_prob << " " << sum_log_prob << " " << log_prior_prob << " " << (sum_log_prob + log_prior_prob) << endl;

      //cout << "********* UNSUP PROBS ********\n";
      //show_probs();
      //      cout << "********** at iteration " << i << " calculating result accuracy on unsupervised probs *********" << endl;
      cout << "********** at iteration " << i;
      // << "(log) sum prob " << total_prob <<
      cout << " sum log prob " << sum_log_prob;
      if(em_params.wc_min > 0) {
	cout  << " log prior prob " << log_prior_prob << " total:" << (sum_log_prob + log_prior_prob);
      }
      cout << endl;

      do_categorisation(i,outputs.cat_dtls);
      result_acc = eval_categorisation();
      outputs.acc_dtls << i << " " << result_acc << " ";
      show_probs(i,outputs.s_stats,outputs.w_stats);
      if(em_params.whether_save_cat_outcomes) {
	save_categorisation(i,outputs.cat_outcomes);
      }

    }
    timestamp iterations_t = get_timestamp();
    cout << "time taken for all iterations: " << ((iterations_t - sense_initialization) / 1000000.0L) << endl;
  }


  // to suit older version wordprobs_out might be pathname
  // or just a name with intention to go into results_location dir
  //  if(em_params.wordprobs_out != "") {
  if(true) {   // changed to have this unconditional with em_params having a default 
    string aname;
    if(em_params.wordprobs_out.find('/') != string::npos) {
      aname = em_params.wordprobs_out;
    }
    else {
      aname = em_params.results_location + "/" + em_params.wordprobs_out;
    }
    write_word_stats(wordprobs,add_suffix(aname));
  }

  // if(em_params.senseprobs_out != "") {
  if(true) {
    string aname;
    if(em_params.senseprobs_out.find('/') != string::npos) {
      aname = em_params.senseprobs_out;
    }
    else {
      aname = em_params.results_location + "/" + em_params.senseprobs_out;
    }
    write_senseprobs(add_suffix(aname));
  }

  outputs.close_all();
}

/* poss obsolete, long unused */
void do_categorisation(unsigned int it, ostream& cat_dtls) {
  // apply whatever adopted prob model to get prob of the observed words
  // in occurrence occ given sense S

  // do categorisation

  cat_dtls << "ITERATION: " << it << endl;

  for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {
    SENSE s;
    s = categorise(Corp::data[dn], cat_dtls);
    Corp::data[dn].inferred_sense = s;
  }
}

/*!  if \p whether_sup is true then use the setting of the 'hidden' sense 
  variable that is in the data -- so all but one alternative is certain

  if \p whether_sup is false then really do the EM thing */
double do_em(bool whether_sup, double *sum_log_prob, double *log_prior_prob) {

  double total_prob; 

  total_prob = do_exp(whether_sup, sum_log_prob);
  
  // calc log prior prob before params altered by do_max
  *log_prior_prob = 0.0;
  if(em_params.wc_min > 0) {
    map<string, int>::const_iterator sym_itr;
    for(unsigned int s=0; s < sense_size; s++) {
      for (sym_itr = the_symbols.begin(); sym_itr != the_symbols.end(); sym_itr++) {
	*log_prior_prob += (-1 * em_params.wc_min * log(wordprobs[s][sym_itr->second]));
      }
    }
  }

  do_max();

  // cout << "********* SUPERVISED PROBS ********\n";
  // show_probs();
  return (-1.0 * log(total_prob));
}

vector<vector<double> > saved_exp_sense;
bool old_exp_sense_exists = false;
void compare(vector<vector<double> > &saved_exp_sense,
	     vector<vector<double> > &new_exp_sense);

/*!  for each data item d and each S calculates the cond prob of
  (year,S,words)|d \n

  in unsupervised case this is via first calculating the joint prob of
  (year,S,words) for each S. Summing these also gives prob of
  observed values and these are accumulated in the return value

  in supervised case for one S, the true sense, the cond prob of
  (year,S,words)|d is set 1, and others are set to 0

  in either case, from the per-datum sense-expectations, generate further
  corpus-level expected counts

  wordcounts[i][wn] \n
  sensecounts[y][i]

  note the return value is only meaningful in the unsupervised case
*/
double do_exp(bool whether_sup, double *sum_log_prob) {

  /************************************************************/
  /* 3. (EXP) GO THROUGH ALL DATA ACCUMULATING VIRTUAL COUNTS */
  /************************************************************/

  double total_prob = 0.0;
  *sum_log_prob = 0.0;


  double year_base;
  double sense_g_year;
  double sense_univ;
  double words_g_sense;

  vector<vector<double> > exp_sense;
  // exp_sense[d][S] gives for d-th data the cond prob of (year,S,words)|d
  // in supervised case for one S, the true sense this is set 1, and others
  // are set to 0

  // size up exp_sense appropriately
  exp_sense.resize(Corp::data.size());
  for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {
    exp_sense[dn].resize(sense_size);
  }

  initialize_counts(whether_sup);

  for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {
    Occ d;
    d = Corp::data[dn];

    /* UNSUPERVISED CASE */
    if (whether_sup == false) {

      for (unsigned int s = 0; s < senses.size(); s++) {

	year_base = yearprobs[d.year];

	words_g_sense = calc_words_g_sense(d, s);

	if (years_model) {
	  sense_g_year = senseprobs[d.year][s];
	  //k * P(C^d =i |year^d) * P(words^d|C^d = i)
	  exp_sense[dn][s] = year_base * sense_g_year * words_g_sense;
	} else {
	  sense_univ = senseprobs_univ[s];
	  //k * P(C^d =i) * P(words^d|C^d = i)
	  exp_sense[dn][s] = year_base * sense_univ * words_g_sense;
	}
      }

      //float normalizer = 0.0;
      double normalizer = 0.0;
      for (unsigned int s = 0; s < senses.size(); s++) {
	normalizer += exp_sense[dn][s];
      }

      Corp::data[dn].data_prob = normalizer;
      total_prob += normalizer;
      *sum_log_prob = *sum_log_prob + (-1 * log(normalizer));

      for (unsigned int s = 0; s < senses.size(); s++) {
	exp_sense[dn][s] /= normalizer;
      }
    }
    /* SUPERVISED CASE */
    else { // TODO years vs no years

      for (unsigned int s = 0; s < true_senses.size(); s++) {
	if (d.true_sense == s) {
	  exp_sense[dn][s] = 1.0;
	} else {
	  exp_sense[dn][s] = 0;
	}
      }

    } // end sup case

    // from here on, there is no distinction between the sup and unsup case

    // for each sense S {
    for (unsigned int s = 0; s < sense_size; s++) {

      if (years_model) {
	// update sensecounts for each sense S for current year
	sensecounts[d.year][s] += (exp_sense[dn][s] * d.rpt_cntr);
      } else {
	sensecounts_univ[s] += exp_sense[dn][s];
      }

      // for each word w in d.words {
      for (unsigned int w = 0; w < d.words.size(); w++) {
	if (d.words[w] != 0) {
	  wordcounts[s][d.words[w]] +=
	    (exp_sense[dn][s] * d.rpt_cntr);
	}

      }
    }

  } // end loop on individual data items

  // debug
#if EM_DEBUG
  for(unsigned int dn=0; dn < exp_sense.size(); dn++) {
    for(unsigned int i=0; i < sense_size; i++) {
      cout << "exp for data " << dn << " sense " << i << " " << exp_sense[dn][i] << endl;

    }
  }
#endif

#if EM_DEBUG
  if(old_exp_sense_exists == false) {
    saved_exp_sense = exp_sense;
    old_exp_sense_exists = true;
  }
  else {
    // compare old and current
    compare(saved_exp_sense, exp_sense);
    saved_exp_sense = exp_sense;
  }
#endif

  //uncomment this to show counts before norm
  //show_word_stats(wordcounts, "COUNTS", iteration_counter, outputs.w_stats);

  return total_prob;

}

/*!
  from expected (or actual) counts determined in do_exp 'normalize' to get re-estimate probabilities in \a wordprobs and \a senseprobs 
*/
void do_max() {

  /*********************************************************/
  /* 4. (MAX) RECALC PROBS FROM ACCUMULATED VIRTUAL COUNTS */
  /*********************************************************/

  if (years_model) {
    int year_count;

    // // old style
    // // calc of per-year sense probs
    // //   for each year y {
    // for(unsigned int i=0; i < years.size(); i++) {
    //   string y = years[i];
    //   unsigned int year_count = 0;
    //   //       for each d {
    //   for (unsigned int dn=0; dn < data.size(); dn++) {
    // 	Occ d = data[dn];
    // 	if(d.year == y) {
    // 	  year_count += 1;
    // 	}
    //   }
    //   //     for each sense S {
    //   for(unsigned int s=0; s < senses.size(); s++) {
    // 	senseprobs[y][s] = (sensecounts[y][s]/year_count);
    //   }
    // }
    // new style: above assumes summed sense values for a given year equals
    // amount of data with that year, which is not true if there was any
    // 'add-one' stuff in the derivation of the sensecounts
    vector<double> year_normalizers;
    year_normalizers.resize(years.size());

    for (unsigned int i = 0; i < years.size(); i++) {
      year_normalizers[i] = 0.0;
      string y = years[i];
      for (unsigned int s = 0; s < sensecounts[y].size(); s++) {
	year_normalizers[i] += sensecounts[y][s];
      }
    }

    // calc of per-year sense probs
    //   for each year y {
    for (unsigned int i = 0; i < years.size(); i++) {
      string y = years[i];

      //     for each sense S {
      for (unsigned int s = 0; s < sensecounts[y].size(); s++) {
	senseprobs[y][s] = (sensecounts[y][s] / year_normalizers[i]);
      }
    }
  } else {
    //     for each sense S {
    for (unsigned int s = 0; s < sensecounts_univ.size(); s++) {
      senseprobs_univ[s] = (sensecounts_univ[s] / Corp::data.size());
    }
  }

  // calc for per-sense word probs

  vector<double> word_normalizers; // all set 0
  for (unsigned int s = 0; s < wordcounts.size(); s++) {
    word_normalizers.push_back(0.0);
  }
  map<int, double>::const_iterator w_itr;
  //   for each sense S {
  for (unsigned int s = 0; s < sense_size; s++) {
    // for each w {
    for (w_itr = wordcounts[s].begin(); w_itr != wordcounts[s].end();
	 w_itr++) {
      double count;
      count = w_itr->second;
      word_normalizers[s] += count;
    }
  }


  //for each sense S {
  for (unsigned int s = 0; s < wordcounts.size(); s++) {
    //     for each word w {
    for (w_itr = wordcounts[s].begin(); w_itr != wordcounts[s].end();
	 w_itr++) {
      int w;
      double count;
      w = w_itr->first;
      count = w_itr->second;
      //        wordprobs[S][w] = wordcounts[S][w]/word_normalizers[S];
      wordprobs[s][w] = (count / word_normalizers[s]);
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

// default is cout
void show_probs(unsigned int it, ostream& s_stats, ostream& w_stats) {


  if(em_params.whether_save_word_stats) {
    show_word_stats(wordprobs, " WORD ", it, w_stats);
  }
  //cout << "**** THE SENSE PROBS **************" << endl;

  vector<string>::iterator yr_itr;

  show_sense_probs(it, s_stats);

  // cout << "**** THE YEAR PROBS **************" << endl;

  // for(yr_itr = years.begin(); yr_itr != years.end() ;yr_itr++) {
  //   cout << "YEAR " << *yr_itr << ": " << yearprobs[*(yr_itr)] << endl;
  // }
}

void initialize_counts(bool whether_sup) {


  unsigned int sense_size;
  if (whether_sup == false) {
    sense_size = senses.size();
  } else {
    sense_size = true_senses.size();
  }

  wordcounts.resize(sense_size);

  map<string, int>::const_iterator sym_itr;
  for (unsigned int s = 0; s < wordcounts.size(); s++) {
    for (sym_itr = the_symbols.begin(); sym_itr != the_symbols.end(); sym_itr++) {

      //      wordcounts[s][sym_itr->second] = 0.0;
      wordcounts[s][sym_itr->second] = em_params.wc_min;
    }
  }

  vector<string>::iterator yr_itr;
  vector<double> v;
  for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
    sensecounts[*yr_itr] = v;

    sensecounts[*yr_itr].resize(sense_size);
    sensecounts_univ.resize(sense_size);

    for (unsigned int s = 0; s < sensecounts[*yr_itr].size(); s++) {
      sensecounts[*yr_itr][s] = em_params.sc_min;
      sensecounts_univ[s] = em_params.sc_min;
    }
  }

}

void save_categorisation(unsigned int it, ostream& cat_outcomes) {
  for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {

    Occ d;
    d = Corp::data[dn];
    show_year(cat_outcomes, d.year);
    cat_outcomes << " ";
    cat_outcomes << it << " ";
    cat_outcomes << d.true_sense << " ";
    cat_outcomes << d.inferred_sense << endl;
  }
}

double eval_categorisation() {
  unsigned int num_correct = 0;

  for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {

    if (Corp::data[dn].inferred_sense == Corp::data[dn].true_sense) {
      num_correct++;
      //       cat_dtls << it << " inferred: " << sense_to_string(s) << "same as reference" << sense_to_string(Corp::data[dn].true_sense) << endl;

    }
  }

  // cat_dtls << "num correct " << num_correct << " " << "total " << Corp::data.size() << endl;
  return ((float) num_correct) / Corp::data.size();

}

void compare(vector<vector<double> > &saved_exp_sense,
	     vector<vector<double> > &new_exp_sense) {
  for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {
    for (unsigned int i = 0; i < new_exp_sense[dn].size(); i++) {
      if (saved_exp_sense[dn][i] != new_exp_sense[dn][i]) {
	cout << "diff on " << dn << " sense " << i << "old "
	     << saved_exp_sense[dn][i] << " new "
	     << new_exp_sense[dn][i] << endl;
      }
    }
  }

}

void open_an_output(ofstream& strm, string name) {
  string full_name;
  full_name = em_params.results_location + "/" + name;
  full_name = add_suffix(full_name);
  strm.open(full_name.c_str());
  if (!strm) {
    cout << "prob opening " << full_name << endl;
    exit(1);
  }

}

void write_version_info() {
  string version_info_filename;
  version_info_filename = em_params.results_location + "/" + add_suffix("version_dynamicEM");
  ofstream f;
  f.open(version_info_filename.c_str());
  f << "git_commit_id: " << Version::git_commit_id << endl;
  f << "git_commit_date: " << Version::git_commit_date << endl;
  f << "git_commit_person: " << Version::git_commit_person << endl;
  f.close();

}


