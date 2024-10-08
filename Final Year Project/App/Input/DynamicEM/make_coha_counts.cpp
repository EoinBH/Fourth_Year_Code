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
#include <ctype.h>
using namespace std;

#include "years.h"

#include "sym_table.h"

#include "prob_tables.h"

#include "senses.h"      

#include "CmdLineVersions.h"      

#include "probs_calc.h"

#include "StringUtil.h"

#include "process_corpus.h"

#include "process_corpus_frm_archive.h"

vector<Occ> data;

void initialize_year_probs();

int coha_count_min = 400;

void print_corpus_counts(int min);
void print_wordcounts_given_year(vector<pair<double, int> > &wp_list);
ParList coha_counts_further_params(ParList l);

int main(int argc, char **argv) {
  ParList pars;
  pars = parse_command_line(argc, argv);
  ParList saved_pars;
  saved_pars = pars;
  if(check_for_help(pars)) {
    corp_params.exit_with_help(false);
    insp_params.exit_with_help(false);
    exit(1);
  }

  pars = corp_params.process_params(pars);
  pars = insp_params.process_params(pars);
  pars = coha_counts_further_params(pars);
  
  if(pars.size() != 0) {
    exit_with_help(pars);
  }
  
  corpus_type = 3;
  if (corp_params.closed == true) {
    whether_closed = true;
  }
  
  grouping_size = corp_params.grouping_size;
  
  coha_wlp_field = corp_params.wlp;
  coha_targ_wlp_field = corp_params.targ_wlp;
  excl_targ_np1 =  corp_params.excl_targ_np1; // excl_targ_np1 owend by CohaToken.cpp
  filter_coha_at_token = corp_params.filter_coha_at_token; // filter_coha_at_token owned by CohaToken.cpp
  whole_coha_article = corp_params.whole_coha_article; // set Occ contents to entire coha article

  if(corp_params.whether_read_archive == false) { // no archive
    
    if (corp_params.symbol_table_name != "") {
      process_corpus(corp_params.file_list_file, corp_params.symbol_table_name, true,
		     false, corp_params.target_alternates);
      // data now contains Occ reps of all data items
    } else {
      process_corpus(corp_params.file_list_file, corp_params.symbol_table_name, false,
		     false, corp_params.target_alternates);
    }
  }
  else { // with archive
    process_corpus_frm_archive(corp_params.archive_name, corp_params);
  }

  cout << "read all data\n";
  cout << "total amount of extracted data is: " << data.size() << endl;

  cout << "doing counting ...\n";  
  set_corpus_word_probs(0);

  //frm get_one_word_stats(map<int, double>& wordstats,string c)

  cout << "doing sorting and writing ...\n";
  //print_corpus_counts(400);
  print_corpus_counts(coha_count_min);



  
}

bool is_quote(char c) {

  if(c == '\'') { return true; }
  else if(c == '\"') { return true; }
  else { return false; }
}
bool is_normal(int code) {
  string s;
  s = decode_to_symbol(code);
  for(int i = 0; i < s.length(); i++) {
    //if(!isalnum(s[i])) { return false; }
    if(is_quote(s[i])) { return false; }
  }
  return true;
}

void print_corpus_counts(int min) {
  
  vector<pair<double, int> > wp_list;
  map<int, double>::const_iterator sym_itr;
  for (sym_itr = wordcounts_corp.begin(); sym_itr != wordcounts_corp.end(); sym_itr++) {
    pair<double, int> p; // prob then code
    p.first = sym_itr->second;
    p.second = sym_itr->first;
    if(p.first > min && is_normal(p.second)) {
      wp_list.push_back(p);
    }

  }
  sort(wp_list.begin(), wp_list.end());
  reverse(wp_list.begin(), wp_list.end());

  cout << "finished sort\n";
  
  /* wp_list contains sorted version of counts */

  ofstream cnts;
  cnts.open("/tmp/cnts");
  
  for (unsigned int j = 0; j < wp_list.size(); j++) {

      cnts << decode_to_symbol(wp_list[j].second) << ": " << wp_list[j].first << endl;

  }

  cnts.close();
  cout << "finished first\n";
  print_wordcounts_given_year(wp_list);

}

void print_wordcounts_given_year(vector<pair<double, int> > &wp_list) {
  ofstream cnts;
  cnts.open("/tmp/per_year_cnts");


  
  cnts << "WORD ";
  vector<string>::iterator yr_itr;
  cnts << "ALL ";
  for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
    cnts << *yr_itr;
    if(yr_itr+1 != years.end()) {
       cnts << " ";
    }
  }
  cnts << endl;


  for (unsigned int j = 0; j < wp_list.size(); j++) {
    cnts << decode_to_symbol(wp_list[j].second) << " ";
    cnts << wordcounts_corp[wp_list[j].second] << " ";
    for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
      cnts << wordcounts_given_year[*yr_itr][wp_list[j].second];
      if(yr_itr+1 != years.end()) {
       cnts << " ";
      }
    }
    cnts << endl;
  }

  cnts.close();
    cout << "finished second\n";
}


ParList coha_counts_further_params(ParList l) {
  vector<ArgVal>::iterator p;
  p = l.begin();
  
  char *spill;

  char *arg_val;

  while(p != l.end()) {

    arg_val = p->arg_val;
    
    if(p->arg_id == "coha_count_min") {
      coha_count_min = strtoul(arg_val,&spill,10);
      p = l.erase(p);
    }

  }

  return l;

}
