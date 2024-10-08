#include "CmdLine.h"
#include "distancetype.h"
#include <vector>
#include <string>
#include <fstream>
#include "Version.h"
#include "StringUtil.h"

using namespace std;


#if !defined CORP_CMD_H
#define CORP_CMD_H

/*! \brief class for representing information from the command-line that is relevant to how a given corpus is going to be read \n
 processing the command-line parameters will set members of this class \n
 other code then sets values of various global vars based on these
 */
class CorpPar {
 public:
  CorpPar(void);
  ParList process_params(ParList l); /*!< \brief interprets and removes corpus-relevant pairs and returns whats left */
  void show_params(ostream& f); /*!< \brief shows vals of all the members \n
				  will typically include things not stated explicitly in command-line */
  /* relating to reading in data from a corpus */
  string target_alternates;
  int corpus_type;
  string file_list_file;
  bool whether_csv_suffix;
  int grouping_size;
  unsigned int left;
  unsigned int right;
  bool whether_pad;
  bool lowercase;
  bool filter_ngram_meta_token; // relating to funny tokens in n-grams eg _START_
  string syn_token_parts;   // relating to syntactic n-grams
  bool excl_targ_nnp;       // relating to syntactic n-grams
  char wlp;                 // relating to coha data
  char targ_wlp;            // relating to coha data
  bool excl_targ_np1;       // relating to coha data
  bool filter_coha_at_token; // relating to coha data
  bool whole_coha_article; // relating to coha data
  string symbol_table_name;
  bool closed;
  double corp_wc_min; // used in 'add-one' smoothing of corpus word probs
  bool whether_archive;
  string archive_name;
  bool whether_read_archive;
  void exit_with_help(bool whether_ex);
};

extern CorpPar corp_params;

#endif

#if !defined EM_CMD_H
#define EM_CMD_H

/*! \brief class for representing information from the command-line that is relevant to how the EM param estimation will run \n
 processing the command-line parameters will set members of this class \n
 other code then sets values of various global vars based on these \n
 note that running the dynamicEM code uses also the CorpPar class
 */
class dynEMPar {
 public:
  dynEMPar(void);
  ParList process_params(ParList l); /*!< \brief interprets and removes em-relevant pairs and returns whats left */
  void show_params(ostream& f); /*!< \brief shows vals of all the members \n
				  will typically include things not stated explicitly in command-line */
  /* relating to running the learning and saving results */
  string results_location;
  unsigned int num_unsup_senses;
  bool do_unsup;
  bool do_sup;
  string sense_names_file;
  bool dynamic;
  /* relating to initialisation */
  string wordprobs_in;
  string senseprobs_in;
  string senseprobs_string;
  bool whether_sense_rand;
  bool rand_mix;
  double rand_mix_level;
  string words_prior_type;
  bool whether_word_rand;
  bool rand_word_mix;
  double rand_word_mix_level;
  bool set_seed;
  /* relating to fine-tuning the learning */
  unsigned int max_it;
  double wc_min; // used in add-one smoothing of word probs
  double sc_min; // used in add-one smoothing of sense probs


  /* relating to saved outcomes */
  string wordprobs_out;
  string senseprobs_out;
  bool save_starts;
  bool time_stamp; // if yes time will be a suffix
  string output_suffix; // use to pass in some pre-defined suffix
  string input_suffix; // use to pass in some pre-defined suffix
  bool record_version; // send git commit id made at compile time
  bool whether_save_cat_outcomes;
  bool whether_save_word_stats;
  void exit_with_help(bool whether_ex);
};

extern dynEMPar em_params;

#endif

#if !defined GIBBS_CMD_H
#define GIBBS_CMD_H

/*! \brief class for representing information from the command-line that is relevant to how the Gibbs param estimation will run \n
 processing the command-line parameters will set members of this class \n
 other code then sets values of various global vars based on these \n
 note that running the gibbs samplong code uses also the CorpPar class
 */
class gibbsPar {
 public:
  gibbsPar(void);
  ParList process_params(ParList l); /*!< \brief interprets and removes Gibbs-relevant pairs and returns whats left */
  void show_params(ostream& f); /*!< \brief shows vals of all the members \n
				  will typically include things not stated explicitly in command-line */

  /* relating to running the learning and saving results */
  string results_location;
  //int no_senses;
  int num_unsup_senses; // TODO is unsigned in dynEMPar

  /* relating to initialisation */
  bool init_via_dirichlet;
  string wordprobs_in;
  string senseprobs_in;
  string senseprobs_string;
  bool whether_sense_rand;
  bool rand_mix;
  double rand_mix_level;
  string words_prior_type;
  bool whether_word_rand;
  bool rand_word_mix;
  double rand_word_mix_level;
  bool set_seed; // for random initialisations
  bool set_sampling_seed; // for sampling
  /* relating to fine-tuning the learning */
  unsigned int max_itr, burn_itr;
  double gamma_theta;
  double gamma_phi;

  /* relating to saved outcomes */
  string sampled_wordprobs;   // contains all the word prob samples
  string sampled_senseprobs;  // contains all the sense prob samples
  string wordprobs_out;       // contains the means from the samples
  string senseprobs_out;      // contains the means from the samples
  bool convert_em;
  bool time_stamp; // if yes time will be a suffix
  string output_suffix; // use to pass in some pre-defined suffix
  string input_suffix; // use to pass in some pre-defined suffix
  bool record_version; // send git commit id made at compile time

  //NOT USING this string load_symbol_table_file;

  void exit_with_help(bool whether_ex);
};

extern gibbsPar gs_params;

#endif 

#if !defined INSP_CMD_H
#define INSP_CMD_H

/*! \brief class for representing information from the command-line that is relevant to how the inspector program runs \n
 processing the command-line parameters will set members of this class \n
 other code then sets values of various global vars based on these \n
 note that running the inspector program also uses the CorpPar class
 */
class InspPar {
 public:
  InspPar(void);
  ParList process_params(ParList l); /*!< \brief interprets and removes inspector-relevant pairs and returns whats left */
  void show_params(ostream& f);
  unsigned int num_unsup_senses; // duplicate frm dynEMPar
  string sense_names_file; // duplicate frm dynEMPar
  //  double corp_wc_min; // duplicate from dynEMPar
  string wordprobs_in;// duplicate from dynEMPar
  string senseprobs_in;// duplicate from dynEMPar
  bool w_g_s;
  bool w_g_ss; // above is for specific sense pair, this for all senses
  bool w_g_s_old;
  unsigned int first_sense;
  unsigned int second_sense;
  bool w_g_y;
  bool w_g_y_old;
  string first_year;
  string second_year;

  string sampled_wordprobs; // duplicate from gibbsPar
  bool sample_w_g_s; // for looking at w-given-sense probs from particular sample
  unsigned int which_it;
  unsigned int which_sense;
  
  string ranking; // freq to_other to_corpus
  DISTTYPE dtype; // RATIO vs KL_LR vs KL_RL vs KL_SYM 
  double interp_equiv; // added to probs before taking ratio
  double interp_lam; // interpolation weighting between a prob and uniform before taking a ratio
  /* if interp_equiv is e, and interp_lam is l and u is uniform prob
     e = ((1-l)/l) u
     l = 1/(1 + e/u)
     and the following ratios are equal
      (p1 + e)/(p2 + e)   vs   (lp1 + (1-l)u)/(lp2 + (1-l2)u)
  */
  unsigned int max_show;
  string show_format; // vert vs horiz
  bool per_year_totals;
  bool show_vocab_stats;
  bool show_all_data;
  bool show_zeroes;
  bool make_data_prob;
  bool uniq_words;
  string words_to_track;
  bool distance;
  bool symmetric;
  /* relating to examples */
  bool show_examples;
  int num_examples;
  string example_year;
  string example_sort;
  bool no_duplicates;
  bool enter_loop;
  void exit_with_help(bool whether_ex);
};

extern InspPar insp_params;

class compPar {
 public:
  compPar(void);
  ParList process_params(ParList l);
  void show_params(ostream& f);
  void exit_with_help(bool whether_ex);
  string first_file_list_file;
  string second_file_list_file;
  string first_word;
  string second_word;
  // copied from InsPar
  double interp_equiv; // added to probs before taking ratio
  double interp_lam; // interpolation weighting between a prob and uniform before taking a ratio
  /* if interp_equiv is e, and interp_lam is l and u is uniform prob
     e = ((1-l)/l) u
     l = 1/(1 + e/u)
     and the following ratios are equal
      (p1 + e)/(p2 + e)   vs   (lp1 + (1-l)u)/(lp2 + (1-l2)u)
  */

};

extern compPar comp_params;



class ExamplesPar {
 public:
  ExamplesPar(void);
  ParList process_params(ParList l);
  void show_params(ostream& f);
  void exit_with_help(bool whether_ex);
  int no_sents;
  string year;
  string sort_by;
};

extern ExamplesPar eg_params;

#endif
