#include "GoogleOcc.h"
#include "Occ.h"
#include "NGramToken.h"
#include "SynGramToken.h"
#include "CohaToken.h"

//! entry function to process the corpus and generate corresponding Occ vector in data
void process_corpus(string file_list,string symbol_table_name, bool whether_read, bool whether_write, string target_alternates); 

//! called by above
void process_corpus(string file_list);

//!
extern bool lowercase;


/*! \brief
  1 indicates standard corpus, one line is one occurrence \n
  2 indicates n-gram corpus, each line contains a count \n
  3 indicates coha corpus of word tab lemma tab pos lines \n
  4 indicates syntactic n-gram corpus, each line contains a count
 */
extern int corpus_type;

extern bool data_csv_suffix;

/*! \brief
    called by above for standard corpus \n
    adds to the overall Occ vector in data \n
    updates years \n 
    goes by way of intermediate GoogleOcc vector */



void process_single_file(string afile);

/*! \brief
   called by above \n
   builds a single GoogleOcc vector for the single file */
void process_single_file(string afile, vector<GoogleOcc>& google_data);

/*! called by process_single_file after GoogleOcc's are made to 
    convert to Occ
    may fail if nothing in the GoogleOcc matches the target */
bool make_occ(GoogleOcc gd, Occ& d);
bool make_occ_left_and_right(GoogleOcc gd, string& pre_t, string& post_t);
void make_occ_words(string pre_t, string post_t, Occ& d);

/*! \brief
   called by process_corpus for n-gram corpus\n */
void process_single_ngram_file(string afile);

/*! \brief called by process_single_ngram to convert n-gram line to Occ \n
    may fail if nothing in the line matches the target */
bool make_occ(string ngram_line, Occ& d);
//bool make_occ_left_and_right(string ngram_line, string year, string& pre_t, string& post_t, int& rpt_cntr);
bool make_occ_left_and_right(string ngram_line, string year, vector<string>& pre_t, vector<string>& post_t, int& rpt_cntr);

// void make_occ(string pre_t, string post_t, Occ& d) is same for n-gram case

/*! \brief
   called by process_corpus for coha corpus (wlp) version\n */
void process_single_coha_file(string afile);
bool make_occ(vector<string> &doc_lines, int i, Occ &d);
bool make_occ_left_and_right(vector<string> &doc_lines, int i, vector<string> &pre_t, vector<string>& post_t);
void make_occ_words(vector<string>& pre_t, vector<string> &post_t, Occ& d);
void coha_lem_from_line(string line, string& lemma);
void coha_field_from_line(string line, string& field);

void process_single_coha_file_as_single_occ(string afile);

/*! \brief
   called by process_corpus for syntactic n-gram corpus \n */
void process_single_syn_ngram_file(string afile);
bool make_occ_frm_syn(string syn_ngram_line, Occ& d);
bool make_occ_left_and_right_frm_syn(vector<string> syn_tokens, vector<SynGramToken> &pre_t, vector<SynGramToken>& post_t);
void make_occ_words_frm_syn(vector<SynGramToken> &pre_t, vector<SynGramToken>& post_t, Occ& d);
bool make_occ_via_neighbs_frm_syn(vector<string> syn_tokens, Occ& d);


void make_targ_vector(string targ_alternates);

// for n-grams this is not the same as data.size()
extern int total_occurrences;

