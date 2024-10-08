#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include <sstream>
#include <algorithm>
#include "Occ.h"
#include "StringUtil.h"
#include "process_corpus.h"
#include <ctype.h>

using namespace std;



#include "sym_table.h"

//#include "prob_tables.h"
#include "years.h"

namespace Corp {
  extern vector<Occ> data;
}
//extern vector<string> years;
//extern vector<SENSE> senses;

extern unsigned int desired_left;
extern unsigned int desired_right;

bool lowercase;

int corpus_type = 1;

bool data_csv_suffix = true;



// relevant for n-grams this includes the rpt_cnt
int total_occurrences = 0;

void set_corpus_word_probs(void);

vector<string> targ_alt_vec;

//void make_targ_vector(string targ_alternates);

void padleft(unsigned int n, vector<string>& pre_words);
void padright(unsigned int n, vector<string>& post_words);
void truncateleft(vector<string>& pre_words);
void truncateright(vector<string>& post_words);
void tokenize(string line, vector<string>& words);
void fix_a_token(string& s);
void sub_divide_token(string token, vector<string>& words);
bool one_char_token(char c);



//void process_single_file(string afile);






/*!
 \param file_list name of a file containing a list of files to process
 \param symbol_table_name name of a file containing a symbol table
 \param whether_read
 \param whether_write
 \param target_alternates slash separate alternates eg blah/BLAH/Blah

 after this function has executed\n
   the global var Corp::data will stores an encoding of the data \n
   there is a symbol table in global var the_symbols relates corpus tokens to ints \n
   the global var years stores the time stamps of the data \n

 if symbol_table_name is empty the symbol table will made on the fly on forgotten

 if symbol_table_name is not empty and whether_read is true it will be read

 if symbol_table_name is not empty and whether_write is true the symbol table will be written to the file

 */
void process_corpus(string file_list,string symbol_table_name, bool whether_read, bool whether_write, string target_alternates) {

  make_targ_vector(target_alternates);

  if(whether_read == true) {
    read_symbol_table(symbol_table_name);
  }
  else {
    initialize_symbol_table();
  }

  total_occurrences = 0;
  cout << "processing corpus\n";
  process_corpus(file_list);

  if(whether_write == true) {
    write_symbol_table(symbol_table_name);
  }

}

/*!
 \param file_list name of a file containing a list of files to process
 */
void process_corpus(string file_list) {

  //  initialize_symbol_table();
  ifstream f;
  f.open(file_list.c_str());
  if(!f) {
    cout << "prob opening " << file_list << endl;
    exit(1);
  }
  string afile;
  int num_processed = 0;
  while(getline(f,afile)) {
    if(corpus_type == 1) {
      process_single_file(afile);
    }
    else if(corpus_type == 2) {
      
      process_single_ngram_file(afile);

    }
    else if(corpus_type == 3) {
      (whole_coha_article ?
       process_single_coha_file_as_single_occ(afile) :
       process_single_coha_file(afile));
    }
    else if(corpus_type == 4) {
      make_syn_info_vector();
      process_single_syn_ngram_file(afile);
    }
    else {
      cout << "unexpected corpus_type\n";

    }
    /* progress info to screen */
    num_processed++;
    if(num_processed % 1000 == 0) {
      cout << "#" << (int)(num_processed/1000) << "k" << endl;
    }
  }

  //debug 
  // vector<string>::iterator yr_itr;
  // for(yr_itr = years.begin(); yr_itr != years.end() ;yr_itr++) {
  //   cout << *(yr_itr) << endl;
  // }
  //debug
  // for(unsigned int i=0; i < Corp::data.size(); i++) {
  //   Corp::data[i].show();
  // }

}


bool make_occ(GoogleOcc gd, Occ& d) {

  //  Occ d;
  d.year = gd.year;
  add_to_years(d.year); // update 

  if(gd.true_sense == -1) {
    d.true_sense = gd.true_sense;
  }
  else { // what is this subtract 1 business
    d.true_sense = ((SENSE)gd.true_sense-1);
  }

  // find left and right of target if possible
  string pre_t, post_t;
  bool made;
  made = make_occ_left_and_right(gd, pre_t, post_t);
  if(!made) { return false; }

  // fill word vector of d from words left and right
  // possibly padding
  make_occ_words(pre_t, post_t, d);

  return true; 

}


bool make_occ_left_and_right(GoogleOcc gd, string& pre_t, string& post_t) {

  string hit_string;
  hit_string = gd.hit_string;
  if(lowercase){ tolower(hit_string); } 

  string url_string;
  url_string = gd.url_string;
  if(lowercase){ tolower(url_string); }

  size_t t_pos;
  string to_tokenize;
  bool found_targ = false;
  string targ;
  // run thru supplied alternatives for target in hit_string
  for(unsigned int i=0; i < targ_alt_vec.size(); i++) {
    string targ_with_em = "<em>";
    targ_with_em += targ_alt_vec[i];
    targ_with_em += "</em>";
    if((t_pos = hit_string.find(targ_with_em)) != string::npos) {
      to_tokenize = hit_string;
      found_targ = true;
      targ = targ_with_em;
      break;
    }
  }

  // run thru supplied alternatives for target in url_string
  if(!found_targ) {
    for(unsigned int i=0; i < targ_alt_vec.size(); i++) {
      string targ_with_em = "<em>";
      targ_with_em += targ_alt_vec[i];
      targ_with_em += "</em>";
      if((t_pos = url_string.find(targ_with_em)) != string::npos) {
	to_tokenize = url_string;
	found_targ = true;
	targ = targ_with_em;
	break;
      }
    }
  }

  if(!found_targ) {
    cout << "failed to find any match in " << hit_string << " OR " << url_string << endl;
    to_tokenize = "";
  }

  if(to_tokenize == "") {
    return false;
  }
      
  // debug
#if EM_DEBUG
  cout << "to_tokenize is " << to_tokenize << endl;
#endif


  pre_t = to_tokenize.substr(0,t_pos);
  post_t = to_tokenize.substr(t_pos + targ.size());

  // debug
#if EM_DEBUG
  cout << "pre_t is " << pre_t << endl;
  cout << "post_t is " << post_t << endl;
#endif

  return true;

}

/*!
 * \param d the members of this Occ object are set from pre_t and post_t \n
 * which are the parts to the left and right of the target
 *
 * d.words will be vector of codes of words to left and right 
 *
 * desired_left, desired_right, whether_pad influence how this vector will look
 */
void make_occ_words(string pre_t, string post_t, Occ& d) {

  vector<string> pre_words;
  tokenize(pre_t,pre_words);
  if(pre_words.size() > desired_left) { // truncate
    truncateleft(pre_words);
    d.num_L = 0;
  }
  else if((whether_pad == true) && (pre_words.size() < desired_left)) {
    d.num_L = desired_left - pre_words.size();
    padleft(d.num_L, pre_words);
  }
  d.size_left = pre_words.size();

  vector<string> post_words;
  tokenize(post_t,post_words);
  if(post_words.size() > desired_right) { // truncate
    truncateright(post_words);
    d.num_R = 0;
  }
  else if((whether_pad == true) && (post_words.size() < desired_right)) {
    d.num_R = desired_right - post_words.size();
    padright(d.num_R, post_words);
  }
  d.size_right = post_words.size();

#if EM_DEBUG
  // debug
  cout << "PRE WORDS: ";
  for(unsigned int i=0; i < pre_words.size(); i++) {
    cout << pre_words[i] << " || ";
  }
  cout << endl;
  cout << "POST WORDS: ";
  for(unsigned int i=0; i < post_words.size(); i++) {
    cout << post_words[i] << " || ";
  }
  cout << endl;
#endif

  d.words.resize(d.size_left + d.size_right);
  string word;
  //    pre_words[0] ... pre_words[4]
  for(unsigned int i=0; i < d.size_left; i++) {
    word = pre_words[i]; 
    d.words[i] = get_code(word);
  }
  //      post_words[0] .. post_words[4]
  for(unsigned int j=0; j < d.size_right; j++) {
    word = post_words[j];
    d.words[d.size_left+j] = get_code(word);
  }

  // debug
#if EM_DEBUG
  cout << "decode of stored words is" << endl;
  for(unsigned int i=0; i < d.size_left; i++) {
    cout << decode_to_symbol(d.words[i]) << " ";
  }
  cout << "||T||";
  for(unsigned int j=0; j < d.size_right; j++) {
    cout << decode_to_symbol(d.words[d.size_left+j]) << " ";
  }
  cout << endl;
#endif
#if CORP_DEBUG
  d.show();
#endif

}

/*!
  \param afile name of a single corpus file to be processed to add to overall Occ vector in Corp::data. Goes by way of intermediate GoogleOcc vector 
 */
void process_single_file(string afile) {

  int num_added = 0; 

  vector<GoogleOcc> google_data;
  Occ d;
  process_single_file(afile, google_data);

  for(unsigned int i=0; i < google_data.size(); i++) {
    Occ d;
    bool made = false;
    made = make_occ(google_data[i],d);
    if(!made) { continue; }
    num_added++;
    Corp::data.push_back(d);
  }

#if CORP_DEBUG  
  cout << "num added by file " << num_added << endl;
#endif

}

/*!
  \param name names a file of data
  \return a string for the year identifier for the file

  assumption is that the filename includes a year \n
  could end this way ....YYYY.csv or just ...YYYY \n
  processes according to val of data_csv_suffix \n
  in n-gram case, files are just YYYY

  if grouping_size is set > 1, then several different year \n
  map to one name eg. with 10 all years in a decade are mapped to first year of that decade
  
  */
string year_from_name(string name) {
 // cheap trick to get year out of filename
  // assumes afile ends this way ....YYYY.csv
  size_t fn_size;
  fn_size = name.size();
  string year;
  //  year = afile.substr(fn_size-9,5);
  if(data_csv_suffix) {
    year = name.substr(fn_size-8,4);
  }
  else {
    year = name.substr(fn_size-4,4);
  }
  if(grouping_size > 1) {
    year = year_to_grouped_year(year);
  }
  return year;

}

string coha_year_from_name(string name) {
 // cheap trick to get year out of filename
  // name ends this way ..._YYYY_....";
  size_t start;
  size_t end;

  for(end = name.size()-1; end >= 0; end--) {
    if(name[end] == '_') { break; }
  }
  // should check for failure
  for(start = end-1; start >= 0; start--) {
    if(name[start] == '_') { break; }
  }
  // should check for failure
  // _ 1 9 9 4 _
  // 3 4 5 6 7 8
  start++;
  string year;
  year = name.substr(start,end-start);

  if(grouping_size > 1) {
    year = year_to_grouped_year(year);
  }
  return year;    

}

// afile should have form: ....YYYY.csv
// eg. bricked_2001.csv

void process_single_file(string afile, vector<GoogleOcc>& google_data) {
  // .. num .. TAB .. sense-id .. TAB .. url-string .. TAB .. hit-string
  // could be empty between two TABS

  ifstream f;
  f.open(afile.c_str());
  if(!f) {
    cout << "prob opening " << afile << endl;
    exit(1);
  }
  else {
#if CORP_DEBUG  
    cout << "processing " << afile << endl;
#endif
  }

  // cheap trick to get year out of filename
  // assumes afile ends this way ....YYYY.csv
  string year;
  year = year_from_name(afile);

  string line = "";
 
  while(getline(f,line)) {
    GoogleOcc gd;
    gd.year = year;

    size_t end_n, end_id, end_hit, end_url;
    end_n = end_id = end_hit = end_url = 0; 
    end_n = line.find('\t',0); // end num
    end_id = line.find('\t',end_n+1); // end sense-id
    //    end_hit = line.find('\t',end_id+1); // end hit string
    end_url = line.find('\t',end_id+1); // end url string

    int num;
    string num_string;
    num_string = line.substr(0,end_n);
    num = stl_to_int(num_string);
    gd.num = num;

    int sense_id;
    string sense_id_str;
    sense_id_str = line.substr(end_n+1, end_id-(end_n+1));
    if(sense_id_str == "") {
      gd.true_sense = -1;
    }
    else {
      sense_id = stl_to_int(sense_id_str);
    // whats this subtract 1 business?
      gd.true_sense = sense_id;
    }

    string url_string;
    url_string = line.substr(end_id+1,end_url-(end_id+1));

  
    string hit_string;
    hit_string = line.substr(end_url+1);  

    gd.url_string = url_string;

    gd.hit_string = hit_string;

    google_data.push_back(gd);
  }


#if CORP_DEBUG
  cout << "num found in file " << afile << " " << google_data.size() << endl;
#endif

#if EM_DEBUG
   for(unsigned int i=0; i < google_data.size(); i++) {
     google_data[i].show(); cout << endl;
   }
#endif

  f.close();
  

}


void process_single_ngram_file(string afile) {
  ifstream f;
  f.open(afile.c_str());
  if(!f) {
    cout << "prob opening " << afile << endl;
    exit(1);
  }
  else {
#if CORP_DEBUG  
    cout << "processing " << afile << endl;
#endif
  }

  // cheap trick to get year out of filename
  // assumes afile ends this way ....YYYY.csv
  string year;
  year = year_from_name(afile);


  int num_added = 0; 

  string ngram_line = "";
 
  while(getline(f,ngram_line)) {
    Occ d;
    d.year = year;
    add_to_years(year);
    // do rest of it
    bool made = false;

    made = make_occ(ngram_line,d);
    if(!made) { continue; }
    num_added++;
    Corp::data.push_back(d);
    total_occurrences += d.rpt_cntr;
  }

#if CORP_DEBUG  
  cout << "num added by file " << num_added << endl;
#endif

}


/*! called by process_single_ngram to
    convert n-gram line to Occ
    \param ngram_line is a single line of n-gram data
    
    \param d the Occ which will contain representation of the data

    \return will be false if nothing in the line matches the target 
*/

bool make_occ(string ngram_line, Occ& d) {

  d.true_sense = -1; // none of n-grams have true sense annotation

  vector<string> pre_t;
  vector<string> post_t;
  bool made;
  // search in first 5 words of line for target
  // set pre_t and pos_t based on that
  made = make_occ_left_and_right(ngram_line, d.year, pre_t, post_t, d.rpt_cntr);
  if(!made) { return false; }

  // make and possibly pad pre_words from pre_t
  // make and possible pad post_words from post_t
  // store codes in d based on pre_words and post_words
  make_occ_words(pre_t,post_t,d);

  return true;


}




/*!
 * \param ngram_line is space separated words, followed by a year (y) 
* and two repetition counts (m and n) \n
* target T should appear in the sequence before the year 
*
* \param pre_t becomes words before target T
*
* \param post_t becomes words after target T 
*
* \param rpt_cntr set to the penultimate repeat counter m \n
* representing how often the n-gram was seen in its year 
*
* \param year in current version year is redundant input as already found from file-name 
*
* \return will be false if the line does not contain the target 
*
* should respect desired_left desired_right ie. not include any more \n
* if not enough this taken care of by later make_occ_words(...) \n
*
* eg 0 1 2 3 4 5 6 7 \n
*    a T f g h y m n 
*
* with desired_left = 4 = desired_right \n
* ==> pre_t = a post_t = f g h \n
* with desired_left = 2 = desired_right \n
* ==> pre_t = a post_t = f g 
*
* eg 0 1 2 3 4 5 6 7 8 9 10 11 12 13 \n
*    a b c d e T f g h i j  yr m  n 
* 
* with desired_left = 2 = desired_right \n
* ==> pre_t = d e  post_t = f g
*
* standard n-gram input has final space in ngram_line \n
* 
*/
bool make_occ_left_and_right(string ngram_line, string year, vector<string>& pre_t, vector<string>& post_t, int& rpt_cntr) {

  size_t end_pos;
  end_pos = ngram_line.size()-1; // final position
  if(ngram_line[end_pos] == ' ') { // this will standardly be true
    ngram_line.erase(end_pos,1); // remove final space
  }

  size_t start = 0;
  size_t spc_pos;
  string word;
  bool found_yr = false;
  size_t t_index;
  bool found_targ = false;
  string targ;
  int spc_count = 0;
  vector<string> parts;
  int avail_left, total_left, start_left;
  int avail_right, total_right, start_right;

  // collect all space separated parts (inc year and repeat counts)
  // (unless they are meta tokens like _START_ and _END_)
  while((spc_pos = ngram_line.find(' ',start)) != string::npos)  {
    word = ngram_line.substr(start,spc_pos-start);
    // deal with word
    //if(!is_ngram_meta_token(word)) {
    if((!filter_ngram_meta_token) || (!is_ngram_meta_token(word))) {

      parts.push_back(word);
      // check if word is target 
      for(unsigned int j=0; j < targ_alt_vec.size(); j++) {
	if(targ_alt_vec[j] == word) {
	  t_index = parts.size()-1;
	  found_targ = true;  
	  targ = targ_alt_vec[j];
	  break;
	}
      }
    }
    // go on to next word
    start = spc_pos + 1;
  }
  word = ngram_line.substr(start); // final item can't be meta token
  parts.push_back(word);
  
  if(parts.size() < 4) {
    cout << "must have last 1 word, year, and 2 freq counters but has less than 4 parts in: " << ngram_line << endl;
    return false;
  }
  

  if(!found_targ) { 
    cout << "failed to find any match in " << ngram_line << endl;
    return false; 
  }

  int yr_index;
  yr_index = parts.size()-3;

  /**************/
  /* make pre_t */
  /**************/
  // eg 0 1 2 3 4 5
  //    _ _ _ _ _ T
  // desired_left = 2
  pre_t.resize(0); // should already be empty really
  avail_left = t_index; // so 5
  if (avail_left <= desired_left) {
    total_left = avail_left; 
    start_left = 0;
  }
  else {
    total_left = desired_left; // so 2
    start_left = avail_left - desired_left; // so 3
  }

  // mar 12 2019 : this had wrong bound and quit too early
  //for(int i=start_left; i < total_left; i++) {
  for(int i=start_left; i < (start_left + total_left); i++) {
    pre_t.push_back(parts[i]);
  }

  /**************/
  /* make post_t */
  /**************/
  // eg 5 6 7 8 9 10 11
  //    T _ _ _ _ _  yr
  // desired_right = 2

  start_right = t_index+1; // so 6
  avail_right = yr_index - start_right; // so 5
  if(avail_right <= desired_right) {
    total_right = avail_right;  
  }
  else {
    total_right = desired_right; // so 2
  }
  
  post_t.resize(0); // should already be empty really
  for(int j=start_right; j < (start_right+total_right); j++) { // so < 8
    post_t.push_back(parts[j]);
  }
  
  
  int cntr_index;
  cntr_index = parts.size()-2;
  string cntr_string;
  cntr_string = parts[cntr_index];
  rpt_cntr = stl_to_int(cntr_string);
  return true;

}


void padleft(unsigned int n, vector<string>& pre_words) {

  vector<string> new_pre;
  for(unsigned int i = 0; i < n; i++) {
    new_pre.push_back("L");
  }
  for(unsigned int j = 0; j < pre_words.size(); j++) {
    new_pre.push_back(pre_words[j]);
  }
  pre_words = new_pre;

}

void padright(unsigned int n, vector<string>& post_words) {

  for(unsigned int i = 0; i < n; i++) {
    post_words.push_back("R");
  }

}

void truncateleft(vector<string>& pre_words) {
    vector<string> short_pre;
    short_pre.resize(desired_left);
    //    pre_words.size()-5 ... pre_words.size()-1
    for(unsigned int i=desired_left; i >= 1; i--) {
      short_pre[desired_left-i] = pre_words[pre_words.size()-i]; 
    }
    pre_words = short_pre;
}

void truncateright(vector<string>& post_words) {
  post_words.resize(desired_right);
}

void process_single_syn_ngram_file(string afile) {
  ifstream f;
  f.open(afile.c_str());
  if(!f) {
    cout << "prob opening " << afile << endl;
    exit(1);
  }
  else {
#if CORP_DEBUG  
    cout << "processing " << afile << endl;
#endif
  }

  // cheap trick to get year out of filename
  // assumes afile ends this way ....YYYY.csv
  string year;
  year = year_from_name(afile);

  int num_added = 0; 

  string syn_ngram_line = "";
 
  while(getline(f,syn_ngram_line)) {
    Occ d;
    d.year = year;
    add_to_years(year);
    // do rest of it
    bool made = false;
    made = make_occ_frm_syn(syn_ngram_line,d);
    if(!made) { continue; }
    num_added++;
    Corp::data.push_back(d);
    total_occurrences += d.rpt_cntr;
  }

#if CORP_DEBUG  
  cout << "num added by file " << num_added << endl;
#endif


}

//       x y a b c d TAB num
//       0 1 2 3 4 5 6
bool make_occ_frm_syn(string syn_ngram_line, Occ& d) {

  d.true_sense = -1; // none of n-grams have true sense annotation


  int t;
  size_t ngram_end_pos;
  ngram_end_pos = syn_ngram_line.find('\t');
  
  string ngram_seq;
  ngram_seq = syn_ngram_line.substr(0,ngram_end_pos);
  //cout << ngram_seq << endl;
  istringstream ss(ngram_seq);
  vector<string> ngram_vec;
  string token;
  while(ss >> token) {
    ngram_vec.push_back(token);
  }
  string cntr_string;
  cntr_string = syn_ngram_line.substr(ngram_end_pos);
  d.rpt_cntr = stl_to_int(cntr_string);

  if((syn_info != "neighb") && (syn_info != "ext_neighb")) {
    vector<SynGramToken> pre_t;
    vector<SynGramToken> post_t;
  
    bool made;
    made = make_occ_left_and_right_frm_syn(ngram_vec, pre_t, post_t);
    if(!made) { return false; }

    // // make and possibly pad pre_words from pre_t
    // // make and possible pad post_words from post_t
    // // store codes in d based on pre_words and post_words
    make_occ_words_frm_syn(pre_t,post_t,d);
    return made;
  }
  else {
    bool made;
    made = make_occ_via_neighbs_frm_syn(ngram_vec, d);
    return made;
  }
  
  //return true;


}

bool make_occ_left_and_right_frm_syn(vector<string> syn_tokens, vector<SynGramToken> &pre_t, vector<SynGramToken>& post_t) {

  SynGramToken t;
  bool found_targ = false;
  bool finished_left = false;
  
  for(int i = 0; i < syn_tokens.size(); i++) {
    t = SynGramToken(syn_tokens[i]);

    if(!finished_left) {
      // check if current 'token' contains target
      for(unsigned int j=0; j < targ_alt_vec.size(); j++) {
	if(targ_alt_vec[j] == t.form) {
	  found_targ = true;  
	  break;
	}
      }
    }

    if(!finished_left && found_targ) { // skips inclusion of target token
      finished_left = true;
      continue;
    }

    if(!finished_left) {
      pre_t.push_back(t);
    }
    else {
      post_t.push_back(t);
    }
    
  }
  return found_targ;
}

// according to command-line paramter make string of relevant parts of token
// and then make symbol from this
// initial version just taken form part
void make_occ_words_frm_syn(vector<SynGramToken> &pre_t, vector<SynGramToken>& post_t, Occ& d) {
  vector<string> pre_words;
  pre_words.resize(pre_t.size());
  for(int i = 0; i < pre_t.size(); i++) {
    pre_words[i] = pre_t[i].filtered();
  }
  vector<string> post_words;
  post_words.resize(post_t.size());
  for(int i = 0; i < post_t.size(); i++) {
    post_words[i] = post_t[i].filtered();
  }

  
  if(pre_words.size() > desired_left) { // truncate
    truncateleft(pre_words);
    d.num_L = 0;
  }
  else if((whether_pad == true) && (pre_words.size() < desired_left)) {
    d.num_L = desired_left - pre_words.size();
    padleft(d.num_L, pre_words);
  }
  d.size_left = pre_words.size();

  if(post_words.size() > desired_right) { // truncate
    truncateright(post_words);
    d.num_R = 0;
  }
  else if((whether_pad == true) && (post_words.size() < desired_right)) {
    d.num_R = desired_right - post_words.size();
    padright(d.num_R, post_words);
  }
  d.size_right = post_words.size();

#if EM_DEBUG
  // debug
  cout << "PRE WORDS: ";
  for(unsigned int i=0; i < pre_words.size(); i++) {
    cout << pre_words[i] << " || ";
  }
  cout << endl;
  cout << "POST WORDS: ";
  for(unsigned int i=0; i < post_words.size(); i++) {
    cout << post_words[i] << " || ";
  }
  cout << endl;
#endif

  d.words.resize(d.size_left + d.size_right);
  string word;
  for(unsigned int i=0; i < d.size_left; i++) {
    word = pre_words[i]; 
    d.words[i] = get_code(word);
  }
  for(unsigned int j=0; j < d.size_right; j++) {
    word = post_words[j];
    d.words[d.size_left+j] = get_code(word);
  }

  // debug
#if EM_DEBUG
  cout << "decode of stored words is" << endl;
  for(unsigned int i=0; i < d.size_left; i++) {
    cout << decode_to_symbol(d.words[i]) << " ";
  }
  cout << "||T||";
  for(unsigned int j=0; j < d.size_right; j++) {
    cout << decode_to_symbol(d.words[d.size_left+j]) << " ";
  }
  cout << endl;
#endif
#if CORP_DEBUG
  d.show();
#endif



}

// basic idea is to process dependencies into and out of target like this (where t is target)
// w -r-> t ==> make ^-r:w feature (exactly one of these)
// t -r-> w ==> make v-r:w feature (>= 0 of these)
//
// but also to effectively collapse 
//   (. -prep-> w -pobj-> .)
//   (. -prep-> w -pcomp-> .)
// to
//   (. -prep_w -> .)
//
// eg. from
//  alteration/NN/nsubj/0 of/IN/prep/1 the/DT/det/4 rock/NN/pobj/2	1
// 
//  with target 'rock'
// ==> ^-prep-of:alteration
//  with target 'alteration'
// ==> v-prep-of:rock
//
// also implements a check whether a target occurrence should be skipped because it is
// tagged 'NNP' -- depending on value of excl_targ_nnp
//
// make SynGramToken sequence, with 'fake' ROOT/?/?/? as item 0
// find target at pos i: (targ/_/f/j)
// find 'effective' head-pos hj and head-rel hf
// by  if (f == pobj && at j:(w',f',j') f' == prep)
//          { hj = j', hf = f'-w'} 
//     else { hj = j , hf = f }
//
//    
// then go thru tokens
// if at head-pos hj
//    ==> make ^-hf:tokens[hj]  (go up from targ via hf to tokens[hj])
//
// else if at dependant of targ with (w/_/f/i) at k
// find dep-pos dk and dep-rel df by
//     if(f == prep && there is at k':(w'/_/pobj/k) {
//             { dk = k', df = f-w }
//	else { dk = k , df = f }
// ==> make v-df:tokens[dk]
//       else ==> v-f:other   (go down from targ via df to tokens[dk])

bool make_occ_via_neighbs_frm_syn(vector<string> syn_tokens, Occ& d) {

  vector<SynGramToken> all;
  SynGramToken t;
  // make fake initial parent of head 
  t.form = "ROOT";
  t.pos = "unk";
  t.dep_rel = "unk";
  t.head_pos = "unk";
  all.push_back(t);
  bool found_targ = false;
  int targ_pos = 0;

  for(int i = 0; i < syn_tokens.size(); i++) {
    t = SynGramToken(syn_tokens[i]);
    all.push_back(t);
    // check if current 'token' contains target
    for(unsigned int j=0; j < targ_alt_vec.size(); j++) {

      // note everything is case-lowered in the raw data
      if((targ_alt_vec[j] == t.form) ) {
	if(excl_targ_nnp && t.pos == "NNP") { // excludes a proper name matches
	  continue;
	}

	found_targ = true;
	targ_pos = i+1;
	break;
      }
    }
  }

  if(!found_targ) { return false; }
  
  int j = stl_to_int(all[targ_pos].head_pos);
  string f = all[targ_pos].dep_rel;
  string hf = f;
  int hj = j; 

  // for extended
  if(syn_info == "ext_neighb" && extendible(f) && all[j].dep_rel == "prep") {
    hj = stl_to_int(all[j].head_pos);
    hf = "prep-";
    hf += all[j].form;
  }
  // end for extended
  
  d.words.clear();
  string as_string;
  
  for(int si = 0; si < all.size(); si++) {
    
    bool added = false;
    if(si == hj) {
      //targ is (targ/f/si) ==> ^f:tokens[si]  (go up via f from targ to tokens[si])
      as_string = "^-";
      as_string += hf;
      as_string += ":";
      as_string += all[hj].form;
      d.words.push_back(get_code(as_string));
      added = true;
    }
    else if(stl_to_int(all[si].head_pos) == targ_pos) {
	//(other/f/i) ==> vf:other   (go down via f to word other)
	int k = si;
	int dk = k;
	string df = all[si].dep_rel;
	// for extended
	if(syn_info == "ext_neighb" && df == "prep") {
	  // find following k':(w'/pobj/si)
	  for(int k1 = k+1; k1 < all.size(); k1++) {
	    if((stl_to_int(all[k1].head_pos) == k) && (extendible(all[k1].dep_rel))) {
	      dk = k1;
	      df += "-";
	      df += all[k].form;
	    }
	  }
	}
	// end for extended
      
      as_string = "v-";
      as_string += df;
      as_string += ":";
      as_string += all[dk].form;
      d.words.push_back(get_code(as_string));
      added = true;
    }

    
// bit of hack to suit Occ::show()
    if(added) {
      if(si <= targ_pos) { d.size_left++ ; }
      else { d.size_right++ ;}
    }
    
  }

  return true;
    // find target position i
// then go thru tokens
// if at targ pos i then
//        (targ/f/j) ==> ^f:tokens[j]  (go up via f to tokens[j])
//    else if at dependant of targ then
//        (other/f/i) ==> vf:other   (go down via f to word other) 

//   vector<SynGramToken> all;
//   SynGramToken t;
//   // make fake initial parent of head 
//   t.form = "ROOT";
//   t.pos = "unk";
//   t.dep_rel = "unk";
//   t.head_pos = "unk";
//   all.push_back(t);
//   bool found_targ = false;
//   int targ_pos = 0;
//   for(int i = 0; i < syn_tokens.size(); i++) {
//     t = SynGramToken(syn_tokens[i]);
//     all.push_back(t);
//     // check if current 'token' contains target
//     for(unsigned int j=0; j < targ_alt_vec.size(); j++) {
//       if(targ_alt_vec[j] == t.form) {
// 	found_targ = true;
// 	targ_pos = i+1;
// 	break;
//       }
//     }
//   }
  
//   d.words.clear();
//   string as_string;
//   for(int si = 1; si < all.size(); si++) {
    
//     bool added = false;
//     if(si == targ_pos) {
//       //(targ/f/j) ==> ^f:tokens[j]  (go up via f to tokens[j])
      
//       as_string = "^-";
//       as_string += all[si].dep_rel;
//       as_string += ":";
//       int sj = stl_to_int(all[si].head_pos);
//       as_string += all[sj].form;
//       d.words.push_back(get_code(as_string));
//       added = true;
//     }
//     else if(stl_to_int(all[si].head_pos) == targ_pos) {
//       //(other/f/i) ==> vf:other   (go down via f to word other)
//       as_string = "v-";
//       as_string += all[si].dep_rel;
//       as_string += ":";
//       as_string += all[si].form;
//       d.words.push_back(get_code(as_string));
//       added = true;
//     }
// // bit of hack to suit Occ::show()
//     if(added) {
//       if(si <= targ_pos) { d.size_left++ ; }
//       else { d.size_right++ ;}
//     }
    
//   }

}
/*  
 processes a 'wlp' file from COHA corpus
 guts of wlp_reader
 prob do not need any intermediate

 */


void process_single_coha_file(string afile) {
  // read through making from each target occurrence
  // a left and right vector
  // call make_occ(left_vec, right_vec, data)
  ifstream f;
  f.open(afile.c_str());
  if(!f) {
    cout << "prob opening " << afile << endl;
    exit(1);
  }
  else {
#if CORP_DEBUG  
    cout << "processing " << afile << endl;
#endif
  }

  string year;
  year = coha_year_from_name(afile);
  
  string line;
  // skip first two lines
  getline(f,line); // skip first
  getline(f,line); // skip second

  // collect all the lines	     	     
  vector<string> doc_lines;
  while(getline(f,line)) {
    doc_lines.push_back(line);
  }

#if CORP_DEBUG
  cout << "number of extracted lines in " << afile << ": " << doc_lines.size() << endl;
#endif
  // loop thru all lines checking for target
  for(int i = 0; i < doc_lines.size(); i++) {
    CohaToken coha(doc_lines[i]);
    string part; // misnamed
    if(coha_targ_wlp_field == '0') { // to match refer to same field for targ as context words
      part = coha.fetch_field(coha_wlp_field);
    }
    else { // to match refer to specific field for targ not nec same as context words
      part = coha.fetch_field(coha_targ_wlp_field);
    }
    // for example could specify 'w' for target, and 'l' in generally
    // so targ 'lead' will not match all things with that lemma
    // but the context words will still be lemmas
    // if(coha_wlp_field == 'w') {
    //   part = coha.word;
    // }
    // else if(coha_wlp_field == 'l') {
    //   part = coha.lemma;
    // }
    // else if(coha_wlp_field == 'p') {
    //   part = coha.pos;
    // }
    // else { // some apt respone
    // }
    
    //coha_lem_from_line(doc_lines[i],lemma);
    //coha_field_from_line(doc_lines[i],part);
    bool found_targ = false;
    // check if part is target

    for(unsigned int j=0; j < targ_alt_vec.size(); j++) {

	if(targ_alt_vec[j] == part) {
	  if(coha_wlp_field == 'l' && excl_targ_np1 && coha.pos == "np1") { // excludes a proper name matches
	    continue;
	  }

	  found_targ = true;  
	  break;
	}

    }

    if(!found_targ) { continue ; }
    
    Occ d;
    d.year = year;
    add_to_years(year);
    d.true_sense = -1; 
    d.rpt_cntr = 1;    
    
    make_occ(doc_lines, i, d);

    Corp::data.push_back(d);

  }

  f.close();  
  
}

void coha_lem_from_line(string line, string& lemma) {
    size_t lem_start;
    lem_start = line.find('\t');
    lem_start++;
    size_t lem_end;
    lem_end = line.find('\t',lem_start);
    // a b TAB
    // 1 2 3
    lemma = line.substr(lem_start,lem_end-lem_start);
    return;
}

// basically move into CohaToken class
// // TODO: depending on setting of coha_wlp_field pull appropriate part
// /*  w for first (word) \n
//   l for second (lemma) \n
//   p for third (pos)*/
// void coha_field_from_line(string line, string& field) {
//   size_t field_start = 0;
//   size_t field_end;
//   field_end = line.find('\t',field_start);
//   // 0 1  2  3 4  5  6 7
//   // a b TAB c d TAB e f
//   if(coha_wlp_field == 'w') {
//     field = line.substr(field_start,field_end-field_start);
//     if(lowercase) { tolower(field) ;}
//     fix_a_coha_token(field); // replaces space with S
//     return;
//   }
//   field_start = field_end+1;
//   field_end = line.find('\t',field_start);
//   if(coha_wlp_field == 'l') {
//     field = line.substr(field_start,field_end-field_start);
//     fix_a_coha_token(field); // replaces space with S
//     return; // lowercase not relevant for lemma
//   }
//   field_start = field_end+1;
//   field_end = line.find(13,field_start); // 'carriage return
//   if(coha_wlp_field == 'p') {
//     field = line.substr(field_start,field_end-field_start);
//     return; // lowercase not relevant for pos
//   }

//   // should only reach here if have non-possible coha_wlp_field
//   cout << "not a possible val for coha_wlp_field\n";
//   return;
// }




bool make_occ(vector<string> &doc_lines, int i, Occ &d) {
  vector<string> pre_t;
  vector<string> post_t;
  make_occ_left_and_right(doc_lines, i, pre_t, post_t);
  make_occ_words(pre_t, post_t,d);

  return true;

}

bool make_occ_left_and_right(vector<string> &doc_lines, int i, vector<string> &pre_t, vector<string>& post_t) {
    int max_i = doc_lines.size()-1;
    int start; // where furthest left will be 
    start = (i < desired_left ? 0 : i-desired_left);
    int end; // where further right will be
    end = (max_i-i > desired_right ? i + desired_right : max_i);
    // collect left part
    for(int left=start; left < i; left++) {
      string part;
      CohaToken coha(doc_lines[left]);
      if(filter_coha_at_token && coha.is_at_token) { continue ; } // make no part 
      part = coha.fetch_field(coha_wlp_field);
      // if(coha_wlp_field == 'w') {
      // 	part = coha.word;
      // }
      // else if(coha_wlp_field == 'l') {
      // 	part = coha.lemma;
      // }
      // else if(coha_wlp_field == 'p') {
      // 	part = coha.pos;
      // }
      // else { // some apt respone
      // }
      //coha_lem_from_line(doc_lines[left],lemma);
      //coha_field_from_line(doc_lines[left],lemma);
      pre_t.push_back(part);
    }
    // collect right part
    for(int right=i+1; right <= end; right++) {
      string part;
      CohaToken coha(doc_lines[right]);
      if(filter_coha_at_token && coha.is_at_token) { continue ; } // make no part 
      part = coha.fetch_field(coha_wlp_field);
      // if(coha_wlp_field == 'w') {
      // 	part = coha.word;
      // }
      // else if(coha_wlp_field == 'l') {
      // 	part = coha.lemma;
      // }
      // else if(coha_wlp_field == 'p') {
      // 	part = coha.pos;
      // }
      // else { // some apt respone
      // }
      //coha_lem_from_line(doc_lines[right],lemma);
      //coha_field_from_line(doc_lines[right],lemma);
      post_t.push_back(part);
    }
    return true; // not sure what to check for
}

void make_occ_words(vector<string>& pre_words, vector<string> &post_words, Occ& d) {

  if((whether_pad == true) && (pre_words.size() < desired_left)) {
    d.num_L = desired_left - pre_words.size();
    padleft(d.num_L, pre_words);
  }
  else {
    d.num_L = 0;
  }
  d.size_left = pre_words.size();

  if((whether_pad == true) && (post_words.size() < desired_right)) {
    d.num_R = desired_right - post_words.size();
    padright(d.num_R, post_words);
  }
  else {
    d.num_R = 0;
  }
  d.size_right = post_words.size();

#if EM_DEBUG
  // debug
  cout << "PRE WORDS: ";
  for(unsigned int i=0; i < pre_words.size(); i++) {
    cout << pre_words[i] << " || ";
  }
  cout << endl;
  cout << "POST WORDS: ";
  for(unsigned int i=0; i < post_words.size(); i++) {
    cout << post_words[i] << " || ";
  }
  cout << endl;
#endif

  d.words.resize(d.size_left + d.size_right);
  string word;
  //    pre_words[0] ... pre_words[4]
  for(unsigned int i=0; i < d.size_left; i++) {
    word = pre_words[i]; 
    d.words[i] = get_code(word);
  }
  //      post_words[0] .. post_words[4]
  for(unsigned int j=0; j < d.size_right; j++) {
    word = post_words[j];
    d.words[d.size_left+j] = get_code(word);
  }

  // debug
#if EM_DEBUG
  cout << "decode of stored words is" << endl;
  for(unsigned int i=0; i < d.size_left; i++) {
    cout << decode_to_symbol(d.words[i]) << " ";
  }
  cout << "||T||";
  for(unsigned int j=0; j < d.size_right; j++) {
    cout << decode_to_symbol(d.words[d.size_left+j]) << " ";
  }
  cout << endl;
#endif
#if CORP_DEBUG
  d.show();
#endif


}

void process_single_coha_file_as_single_occ(string afile) {
  // read through making entire article's contents be a single Occ d
  // and add to Corp::data

  Occ d;
  
  ifstream f;
  f.open(afile.c_str());
  if(!f) {
    cout << "prob opening " << afile << endl;
    exit(1);
  }
  else {
#if CORP_DEBUG  
    cout << "processing " << afile << endl;
#endif
  }

  string year;
  year = coha_year_from_name(afile);
  d.true_sense = -1; 
  d.rpt_cntr = 1;    
  d.year = year;

  add_to_years(year);
    
  string line;
  // skip first two lines
  getline(f,line); // skip first
  getline(f,line); // skip second

  /*************************/
  /* collect all the lines */
  /*************************/
  vector<string> doc_lines;
  while(getline(f,line)) {
    doc_lines.push_back(line);
  }

#if CORP_DEBUG
  cout << "number of extracted lines in " << afile << ": " << doc_lines.size() << endl;
#endif

   f.close();  

  /**********************/
  /* make one giant Occ */
  /**********************/

  vector<string> all;

  // first put relevant field for lines into all
  for(int i=0; i < doc_lines.size(); i++) {
      string part;
      CohaToken coha(doc_lines[i]);
      if(filter_coha_at_token && coha.is_at_token) { continue ; } // make no part 
      part = coha.fetch_field(coha_wlp_field);
      all.push_back(part);
  }


  /* put size of all size_left */
  d.size_left = all.size();
  d.size_right = 0;
  
#if EM_DEBUG
  // debug
  cout << "ALL WORDS: ";
  for(unsigned int i=0; i < all.size(); i++) {
    cout << all[i] << " || ";
  }
  cout << endl;
#endif

  d.words.resize(d.size_left);
  string word;
  //    all[0] ... all[4]
  for(unsigned int i=0; i < d.size_left; i++) {
    word = all[i]; 
    d.words[i] = get_code(word);
  }

  // debug
#if EM_DEBUG
  cout << "decode of stored words is" << endl;
  for(unsigned int i=0; i < d.size_left; i++) {
    cout << decode_to_symbol(d.words[i]) << " ";
  }
  cout << endl;
#endif
#if CORP_DEBUG
  d.show();
#endif


  Corp::data.push_back(d);

}

 
  


void tokenize(string line, vector<string>& words) {
#if EM_DEBUG
  cout << "tokenising:" << line << endl;
#endif

  /* empty the words vector */
  words.clear();

  if(line == "") {
    return;
  }

  /* update the words vector from line */
  string::iterator word_itr, space_itr;
  string token = "";
  word_itr = line.begin();             /* word_itr is beginning of line */

  // add this to get rid of leading white space
  // while(word_itr != line.end() && *word_itr == ' ') {
  //   word_itr++;
  // }

  space_itr = find(word_itr,line.end(),' '); /* find space */
  string sp_dummy = "SPSPSP";
  while(space_itr != line.end()) {
    token = string(word_itr,space_itr);
    fix_a_token(token);
    if(token == "") {
      // words.push_back(sp_dummy);
      // cout << "pushing SPSPSP\n";
    }
    else {
      //      if(corpus_type == 1) {
      if(true) { // TODO: put in command-line arg to switch between alternatives
	sub_divide_token(token, words);
      }
      else {
        words.push_back(token);
      }
    }

    // if(word_itr != space_itr) {
    //   words.push_back(string(word_itr,space_itr));
    // }
    // else {
    //   words.push_back(sp_dummy);
    // }
    word_itr = space_itr+1;
    space_itr = find(word_itr,line.end(),' '); /* find space */

  }

    token = string(word_itr,space_itr);
    fix_a_token(token);
    //  words.push_back(string(word_itr,space_itr)); 
    if(token == "") {
      // words.push_back(sp_dummy);
      // cout << "pushing SPSPSP\n";
    }
    else {
      //      if(corpus_type == 1) {
      if(true) {
	sub_divide_token(token, words);
      }
      else {
        words.push_back(token);
      }
    }
  


  return;
}

bool one_char_token(char c) {

  if(c == '!') {
    return true;
  }
  else if(c == '(') {
    return true;
  }
  else if(c == ')') {
    return true;
  }
  else if(c == '.') {
    return true;
  }
  else if(c == ',') {
    return true;
  }
   else if(c == '\"') {
     return true;
   }
   else if(c == '\'') {
     return true;
   }
  else {
    return false;
  }

}

void sub_divide_token(string token, vector<string>& words) {
  // work thru sequence
  // if current pos is single-char token, generate that, move on
  // else record current pos
  //      scan forward for any next single-char (= end)
  //      and if there is on make token from (pos .. end-1) and move on
  //      if there isn't any, make rest into token and finish
  // eg. 
  // if token is !!g..ab().c
  // divide into 
  // !
  // !
  // g
  // .
  // .
  // ab
  // (
  // )
  // .
  // c
  
  size_t start = 0;
  size_t pos, end;
  pos = 0;
  bool found_next_sing;
  string tok;
  while(pos < token.size()) {
    if(one_char_token(token[pos])) {
      tok = token.substr(pos,1);
      words.push_back(tok);
      //cout << token[pos] << endl;
      pos++;
    }
    else {
      start = pos;
      found_next_sing = false;
      while((!found_next_sing) && pos < token.size()) {
        if(one_char_token(token[pos])) {
          found_next_sing = true;
	}
        else {
          pos++;
	}
      }
      if(found_next_sing) {
	end = pos-1;
        tok = token.substr(start,end-start+1);
      }
      else {
        tok = token.substr(start);
      }
      words.push_back(tok);
      //cout << tok << endl;
    }
  }

}



void fix_a_token(string& s) {
  size_t tab_pos;
  while((tab_pos = s.find('\t')) != string::npos) {
    s[tab_pos] = 'T';
  }

}




void make_targ_vector(string targ_alternates) {

  /* targ_alternates of form
     Num/Num/..../Num
  */

  size_t pos=0;

  // counts the slashes
  while((pos = targ_alternates.find('/',pos)) != string::npos) {
    pos++;
  }

  size_t targ_start, targ_end;
  string one_targ;
  targ_start = 0;
  while((targ_end = targ_alternates.find('/',targ_start)) != string::npos) {
    one_targ = targ_alternates.substr(targ_start,targ_end-targ_start);
    targ_alt_vec.push_back(one_targ);
    targ_start = targ_end+1;
  }

  one_targ = targ_alternates.substr(targ_start);
  targ_alt_vec.push_back(one_targ);

  return;
}

