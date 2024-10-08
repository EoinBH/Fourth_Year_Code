/** \file mainly global structures to store probs and counts */
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include "Occ.h"
#include <algorithm>
#include <math.h>
#include <string>

#include "sym_table.h"
#include "uniftype.h"
#include "compute_distance.h"
#include "distancetype.h"

using namespace std;

/*! \brief 
 years_model indicates whether model uses per-year sense probs (sense_probs) \n
 or rather year-indpt sense probs (senseprobs_univ) */
extern bool years_model;




/*! \brief
 where wn is code for word w \n
 and i is sense value \n
 wordprobs[i][wn] = P(W=w| C=i) */
extern vector<map<int, double> > wordprobs;
extern vector<map<int, double> > wordcounts;

/*! \brief
 word stats indpt of sense    \n
 where wn is code for word w  \n
 wordprobs_corp[wn] = P(W=w) */
extern map<int, double> wordprobs_corp;
extern map<int, double> wordcounts_corp;

/*! \brief
 where y is a year string    \n
 and wn is code for word w \n
 wordprobs_given_year[y][wn] = P(W=w | Year=y) */
extern map<string, map<int, double> > wordcounts_given_year;
extern map<string, map<int, double> > wordprobs_given_year;

/*! \brief
 where y is a year string    \n
 wordtotal_given_year[y] = total amounts of words in data for that year y */
extern map<string, double> wordtotal_given_year;



/*! \brief
 where y is a year string    \n
 and i is a sense indicator  \n
 senseprobs[y][i] = P(C=i | Year=y) */
extern map<string, vector<double> > senseprobs;
extern map<string, vector<double> > sensecounts;

/*! \brief
 where y is a year string   \n
 yearpobs[y] = P(Year = y)  \n
 should be uniform  */
extern map<string, double> yearprobs;

/*! \brief
 where i is a sense indicator \n
 senseprobs_univ[i] = P(C=i) */
extern vector<double> senseprobs_univ;
extern vector<double> sensecounts_univ;

extern string symbol_table_for_wordprobs_filename;

// relating to word probs
bool initialize_wordprobs_from_file(string wordprobs_in, string& problem);

void uniform_initialize_wordprobs(UNIFTYPE unif_type, bool whether_noise,
		double k);
//void uniform_initialize_wordprobs(void); 

void random_initialize_wordprobs(bool set_seed);

void set_corpus_word_probs(void);
// as above but with parameter to set min corpus word count
// effectively same as estimation with Dirich prior with param (.. 1+corp_wc_min ..)
void set_corpus_word_probs(double corp_wc_min);

// relating to sense probs
bool initialize_senseprobs_from_file(string senseprobs_in, string& problem);
//void uniform_initialize_senseprobs(string probs_string); 
void uniform_initialize_senseprobs(string probs_string, bool whether_noise,
		double mix_level);
void random_senseprobs(vector<double>& v);
void random_initialize_senseprobs(bool set_seed);
void conv_to_vector(string prob_string, vector<double>& v);
void random_vec_of_counts(vector<double>& v, unsigned int max_count);
void normalize_vec_of_counts(vector<double>& v);
//void normalize_word_map(map<int, double>& wc, map<int, double>& wp);
double normalize_word_map(map<int, double>& wc, map<int, double>& wp);
void add_eps_then_renormalize_word_map(map<int, double>& ws, map<int, double>& wp, double epsilon);


void write_senseprobs(string senseprobs_out);

// pass probs or counts 
void show_word_stats(vector<map<int, double> >& wordstats, string type,
		unsigned int it, ostream& w_stats);

// these are unrelativised to any other prob 
vector<pair<double, int> > get_one_word_stats(map<int, double>& wordstats, string c);
void show_one_word_stats(map<int, double>& wordstats, string c,
		unsigned int max, string format, ostream& w_stats);

// these are relativised to some other prob
// can specify nature of comparison
vector<pair<double, int> > get_one_word_stats(map<int, double>& wordstats, string c,
					      map<int, double>& wordstats_ref, string c_ref,
					      DISTTYPE d,double epsilon, bool &zero_refs);

void show_one_word_stats(map<int, double>& wordstats, string c,
			 map<int, double>& wordstats_ref, string c_ref,
			 DISTTYPE d,double epsilon, unsigned int max, string format, 
			 ostream& w_stats);

void show_count_zeros_in_word_stats(vector<map<int, double> > word_given_sense_probs, ostream& w_stats);

bool has_zeroes(map<int, double>& wordstats);

// pass probs or counts 
void write_word_stats(vector<map<int, double> >& wordstats,
		string word_stats_out);

void show_sense_probs(unsigned int it, ostream& s_stats);

void show_year(ostream& f, string year);

// possibles from tree-dist cost-table.cpp
/* void print_table(double **table, unsigned int table_size); */
/* void print_total_prob_of_table(double **table, unsigned int table_size); */
/* void write_cost_file(string file_name); */

//bool compare_matrices(double **COST1, unsigned int size1, double **COST2, unsigned int size2);
extern string symbol_table_for_wordprobs_filename;
extern string sense_names_for_wordprobs_filename;
extern string sense_names_for_senseprobs_filename;
