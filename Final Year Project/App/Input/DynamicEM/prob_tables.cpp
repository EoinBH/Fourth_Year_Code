#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include "Occ.h"  // really needed?
#include <algorithm>
#include <math.h>
#include <string>
#include <sstream>
#include "sym_table.h"
#include "senses.h"
#include "prob_tables.h"
#include "StringUtil.h"
#include "years.h"

using namespace std;

bool years_model = true;



void threshold(map<int, double>& wp, double eps);
 

// where wn is code for word w
// and i is sense value
// wordprobs[i][wn] = P(W=w| C=i)
vector<map<int, double> > wordprobs;
vector<map<int, double> > wordcounts;

// word stats indpt of sense
// where wn is code for word w
// wordprobs_corp[wn] = P(W=w)
map<int, double> wordprobs_corp;
map<int, double> wordcounts_corp;

//
//  where y is a year string    
//  and wn is code for word w 
//  wordprobs_given_year[y][wn] = P(W=w | Year=y) 
map<string, map<int, double> > wordcounts_given_year;
map<string, map<int, double> > wordprobs_given_year;

// wordtotal_given_year[y] = total amounts of words in data for that year y 
map<string, double> wordtotal_given_year;

// where y is a year string
// and i is a sense indicator
// senseprobs[y][i] = P(C=i | Year=y)
map<string, vector<double> > senseprobs;
map<string, vector<double> > sensecounts;

// where y is a year string
// yearpobs[y] = P(Year = y)  // should be uniform
map<string, double> yearprobs;

// where i is a sense indicator
// senseprobs_univ[i] = P(C=i)
vector<double> senseprobs_univ;
vector<double> sensecounts_univ;

string symbol_table_for_wordprobs_filename;
string sense_names_for_wordprobs_filename;
string sense_names_for_senseprobs_filename;

namespace Corp {
  extern vector<Occ> data;
}
extern vector<string> years;
extern bool years_model;

//void show_year(ostream& f, string year); 

//void random_vec_of_counts(vector<double>& v, unsigned int max_count);
//void normalize_vec_of_counts(vector<double>& v);

//void normalize_word_map(map<int,double>& wc, map<int,double>& wp);

// file format is 
// S W
// SymTableName
// SenseNamesFile
// s_1 w_1_p w_2_p ..... w_W_p
// s_2  ditto
// s_S  ditto
// so for each sense full line of entries, including words with zero prob
// have to assume position in this line is same as symbol table code
bool initialize_wordprobs_from_file(string wordprobs_in, string& problem) {

  ifstream wordprobs_file;
  wordprobs_file.open(wordprobs_in.c_str());
  if (!wordprobs_file) {
    problem = "prob opening: " + wordprobs_in;
    return false;
  }
  cout << "successfully opened " << wordprobs_in << endl;

  unsigned int num_senses;
  unsigned int num_words;
  string top_line;
  getline(wordprobs_file, top_line);
  istringstream ss(top_line);
  ss >> num_senses >> num_words;
  cout << "table num_senses is " << num_senses << endl;
  // assumes sense_size been set somehow from either senses.size or true_senses.size()
  if (num_senses != sense_size) {
    problem = "table num_senses does not match expected";
    return false;
  }

  cout << "table num_words  is " << num_words << endl;
  // should be same symbol_total
  if (num_words != symbol_total) {
    problem = "table num_words does not matched expected";
    return false;
  }

  getline(wordprobs_file, top_line);
  symbol_table_for_wordprobs_filename = top_line;

  cout << "this prob  table's words is to be interpreted wrt to "
       << symbol_table_for_wordprobs_filename << endl;

  getline(wordprobs_file, top_line);
  sense_names_for_wordprobs_filename = top_line;
  if (sense_names_for_wordprobs_filename != "") {
    cout << "this prob table's senses is to be interpreted wrt to "
	 << sense_names_for_wordprobs_filename << endl;
  } else {
    cout
      << "this prob table is for hidden senses with no a priori interpretation"
      << endl;
  }

  wordprobs.resize(num_senses);
  map<string, int>::const_iterator sym_itr;

  // initializing all to zero. necessary?
  for (sym_itr = the_symbols.begin(); sym_itr != the_symbols.end(); sym_itr++) {
    for (unsigned int s = 0; s < wordprobs.size(); s++) {
      wordprobs[s][sym_itr->second] = 0.0;
    }
  }

  double prob;
  unsigned int i;
  i = 0;
  unsigned int sense_id;
  string prob_line;

  while (getline(wordprobs_file, prob_line)) {
    istringstream ss(prob_line);
    ss >> sense_id; // should be same as i
    unsigned int j = 0;
    for (unsigned int j = 1; j <= num_words; j++) {
      ss >> prob;
      if (!(ss.fail())) {
	wordprobs[i][j] = prob;
      } else {
	problem = "prob reading a prob table entries";
	return false;
      }
    }
    i++;
  }

  // display the prob table
  //  for (i=0; i<=num_words; i++) {
  //      for(unsigned int j = 0;  j<=num_words; j++) {
  //        cout << PROB[i][j] << ' ';
  //      }
  //      cout << endl;
  //    }
  //    cout << endl;

  return true;
}

// TODO
// FLAT: for a sense, all words will have same prob
// CORPUS: for a sense, each word gets its corpus prob
void uniform_initialize_wordprobs(UNIFTYPE unif_type, bool whether_noise,
				  double k) {
  //
  wordprobs.resize(senses.size());

  map<string, int>::const_iterator sym_itr;
  double unif_val;
  unsigned int s;

  for (sym_itr = the_symbols.begin(); sym_itr != the_symbols.end(); sym_itr++) {
    for (s = 0; s < senses.size(); s++) {
      (unif_type == FLAT ? unif_val = 1.0 / symbol_total : unif_val =
       wordprobs_corp[sym_itr->second]);
      if (whether_noise == false) {
	wordprobs[s][sym_itr->second] = unif_val;
      } else {
	// assume wordprobs is already set to something random
	// combine random r with suggested u according to
	// (1 - k)u + kr : so k small means small noise
	wordprobs[s][sym_itr->second] = ((1 - k) * unif_val)
	  + (k * wordprobs[s][sym_itr->second]);
      }
    }
  }

  //  symbol_table_for_wordprobs_filename = symbol_table_file_source;

}

// OLD VERSION
// // assumes interested in unsup case
// void uniform_initialize_wordprobs() {

//   /*
//     wordprobs[i][w] : uniform
//     senseprobs[y][i] : WALLS->0.35, PHONE->0.55, OTHER=>0.1
//     yearprobs[y]  : uniform 
//   */

//   wordprobs.resize(senses.size());
//   map<string,int>::const_iterator sym_itr;

//   for(sym_itr = the_symbols.begin(); sym_itr != the_symbols.end() ; sym_itr++) {
//     for(unsigned int s=0; s < senses.size(); s++) {
//       wordprobs[s][sym_itr->second] = 1.0/symbol_total;
//     }
//   }

//   symbol_table_for_wordprobs_filename = symbol_table_file_source;
// }

void random_initialize_wordprobs(bool set_seed) {

  wordprobs.resize(senses.size()); // right num of entries, but each entry is empty
  // uniform_initialize_wordprobs(); // gets everything to have right dimensions

  if (set_seed) {
    srand(0);
  } else {
    srand(time(NULL));
  }

  map<string, int>::const_iterator sym_itr;

  vector<double> v(symbol_total);
  for (unsigned int s = 0; s < senses.size(); s++) {
    random_vec_of_counts(v, 1000000);
    normalize_vec_of_counts(v);

    unsigned int vi;
    for (sym_itr = the_symbols.begin(), vi = 0;
	 sym_itr != the_symbols.end(), vi < v.size(); sym_itr++, vi++) {

      wordprobs[s][sym_itr->second] = v[vi];

    }
  }
}

// file format is 
// Y S
// SenseNamesFile
// y_1 s_1_p ..... s_S_p
// y_2  ditto
// :
// y_Y  ditto
// so for each year full line of sense entries
// positions in this line correspond to senses vector
bool initialize_senseprobs_from_file(string senseprobs_in, string& problem) {

  problem = "";
  ifstream senseprobs_file;
  senseprobs_file.open(senseprobs_in.c_str());
  if (!senseprobs_file) {
    problem = "prob opening: " + senseprobs_in;
    return false;
  }
  cout << "successfully opened " << senseprobs_in << endl;

  unsigned int num_years;
  unsigned int num_senses;

  string top_line;
  getline(senseprobs_file, top_line);
  istringstream ss(top_line);
  ss >> num_years >> num_senses;

  cout << "table num_years is " << num_years << endl;
  // should be same as years.size()
  if (num_years != years.size()) {
    problem = "table num_years does not matched expected";
    return false;
  }

  cout << "table num_senses is " << num_senses << endl;
  // assumes sense_size has been set from senses.size() or true_senses.size()
  if (num_senses != sense_size) {
    problem = "table num_senses does not match expected";
    return false;
  }

  getline(senseprobs_file, top_line);
  sense_names_for_senseprobs_filename = top_line;
  if (top_line != "") {
    cout << "this prob table's senses is to be interpreted wrt to "
	 << sense_names_for_senseprobs_filename << endl;
  } else {
    cout
      << "this prob table is for hidden senses with no a priori interpretation"
      << endl;
  }

  double prob;
  unsigned int i;
  i = 0;
  string year;
  string prob_line;
  vector<double> v;
  while (getline(senseprobs_file, prob_line)) {
    istringstream ss(prob_line);
    ss >> year; // should be same as years[i]
    senseprobs[year] = v;
    senseprobs[year].resize(num_senses);
    for (unsigned int j = 0; j < num_senses; j++) {
      ss >> prob;
      if (!(ss.fail())) {

	senseprobs[year][j] = prob;
	// trial jun25 2013
	//        senseprobs[year][j] = (0.01 * prob) + (0.99 * 1.0/3.0);
	// senseprobs[year][j] = 1.0/5.0;
      } else {
	problem = "prob reading a prob table entries";
	return false;
      }
    }
    i++;
  }

  return true;
}

void (string senseprobs_out) {
  ofstream senseprobs_file;
  senseprobs_file.open(senseprobs_out.c_str());
  if (!senseprobs_file) {
    cout << "prob opening: " << senseprobs_out << endl;
    exit(1);
  }
  cout << "successfully opened " << senseprobs_out << endl;

  unsigned int num_years;
  unsigned int num_senses;

  num_years = years.size();
  num_senses = senseprobs[years[0]].size();

  senseprobs_file << num_years << " " << num_senses << endl;
  senseprobs_file << sense_names_for_senseprobs_filename << endl;

  string year;
  for (unsigned int y = 0; y < num_years; y++) {
    senseprobs_file << years[y] << " ";

    for (unsigned int s = 0; s < num_senses; s++) {
      if (years_model) {
	year = years[y];
	senseprobs_file << senseprobs[year][s];
      } else {
	senseprobs_file << senseprobs_univ[s];
      }
      if (s < num_senses - 1) {
	senseprobs_file << " ";
      }
    }
    senseprobs_file << endl;
  }

  senseprobs_file.close();

}

void conv_to_vector(string prob_string, vector<double>& v) {

  /* prob_string of form
     Num/Num/..../Num
  */

  unsigned int ns = 0;
  size_t pos = 0;

  // counts the slashes
  while ((pos = prob_string.find('/', pos)) != string::npos) {
    pos++;
    ns++;
  }
  ns++;

  size_t num_start, num_end;
  string one_num;
  char *spill;
  num_start = 0;
  while ((num_end = prob_string.find('/', num_start)) != string::npos) {
    one_num = prob_string.substr(num_start, num_end - num_start);
    v.push_back(strtod(one_num.c_str(), &spill));
    num_start = num_end + 1;
  }

  one_num = prob_string.substr(num_start);
  v.push_back(strtod(one_num.c_str(), &spill));

  if (v.size() != ns) {
    cout << "prob with number of senses\n";
    exit(1);
  }

  if (v.size() != senses.size()) {
    cout << "number of supplied probs does not match expected num\n";
    exit(1);
  }

  return;
}

// WALLS->0.35, PHONE->0.55, OTHER=>0.1
void uniform_initialize_senseprobs(string probs_string, bool whether_noise,
				   double k) {
  //void uniform_initialize_senseprobs(string prob_string) {

  vector<double> v;
  conv_to_vector(probs_string, v);

  vector<string>::iterator yr_itr;

  if (whether_noise == false) {
    for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
      senseprobs[*yr_itr] = v;

      // senseprobs[*yr_itr][0] = 5.0/16;
      // senseprobs[*yr_itr][1] = 10.0/16;
      // senseprobs[*yr_itr][2] = 1.0/16;
    }
  } else {
    // assume senseprobs is already set to something random
    // combine random r with suggested u according to
    // (1 - k)u + kr : so k small means small noise

    for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
      for (unsigned int s = 0; s < senseprobs[*yr_itr].size(); s++) {
	senseprobs[*yr_itr][s] = ((1 - k) * v[s])
	  + (k * senseprobs[*yr_itr][s]);
      }

      // senseprobs[*yr_itr][0] = 5.0/16;
      // senseprobs[*yr_itr][1] = 10.0/16;
      // senseprobs[*yr_itr][2] = 1.0/16;
    }
  }

  senseprobs_univ = v;
  return;
}

//void normalize_word_map(map<int, double>& wc, map<int, double>& wp) {
//besides normalising first map into second, returns sum of first
double normalize_word_map(map<int, double>& wc, map<int, double>& wp) {
  map<int, double>::const_iterator w_itr;
  double total_count = 0.0;
  for (w_itr = wc.begin(); w_itr != wc.end(); w_itr++) {
    total_count += w_itr->second;
  }
  for (w_itr = wc.begin(); w_itr != wc.end(); w_itr++) {
    int w;
    double count;
    w = w_itr->first;
    count = w_itr->second;
    wp[w] = (count / total_count);
  }
  return total_count;
}

void add_eps_then_renormalize_word_map(map<int, double>& ws, map<int, double>& wp, double epsilon) {

    // add eps to ws
  map<int, double>::const_iterator sym_itr;
  map<int, double> ws_interp;
  for (sym_itr = ws.begin(); sym_itr != ws.end(); sym_itr++) {
    double p;
    p = ws[sym_itr->first];
    ws_interp[sym_itr->first] = p + epsilon;

  }
  normalize_word_map(ws_interp, wp);
}

void normalize_vec_of_counts(vector<double>& v) {
  double sum;
  sum = 0;
  for (unsigned int s = 0; s < v.size(); s++) {
    sum += v[s];
  }
  for (unsigned int s = 0; s < v.size(); s++) {
    v[s] = v[s] / sum;
  }
  return;
}

void random_vec_of_counts(vector<double>& v, unsigned int max_count) {
  for (unsigned int s = 0; s < v.size(); s++) {
    v[s] = rand() % max_count + 1;
  }
  return;
}

// same as random_vec_of_counts(v,1000000); normalize_vec_of_counts(v);
void random_senseprobs(vector<double>& v) {

  double sum;
  sum = 0;
  for (unsigned int s = 0; s < v.size(); s++) {
    v[s] = rand() % 1000000 + 1;
    sum += v[s];
  }
  for (unsigned int s = 0; s < v.size(); s++) {
    v[s] = v[s] / sum;
  }
  return;

}

//void random_initialize_senseprobs() {
void random_initialize_senseprobs(bool set_seed) {
  if (set_seed) {
    srand(0);
  } else {
    srand(time(NULL));
  }

  /*
    senseprobs[y][i] : WALLS->0.35, PHONE->0.55, OTHER=>0.1

  */
  vector<string>::iterator yr_itr;
  vector<double> v;

  for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
    v.resize(senses.size());

    random_senseprobs(v);
    senseprobs[*yr_itr] = v;

    // senseprobs[*yr_itr][0] = 5.0/16;
    // senseprobs[*yr_itr][1] = 10.0/16;
    // senseprobs[*yr_itr][2] = 1.0/16;
  }

  // for debugging
  cout << "random sense choice\n";
  show_sense_probs(0, cout);

  random_senseprobs(v);
  senseprobs_univ = v;
}

//void show_sense_probs(unsigned int it, ostream& f) {
void show_sense_probs(unsigned int it, ostream& s_stats) {
  //cout << "Year One Two Three" << endl;
  if (years_model) {
    vector<string>::iterator yr_itr;
    for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
      s_stats << it << " ";
      show_year(s_stats, *yr_itr);
      s_stats << " ";
      for (unsigned int s = 0; s < senseprobs[*yr_itr].size(); s++) {
	//      cout << "SENSE " << s << " given year " << *yr_itr << ": " << senseprobs[*yr_itr][s] << endl;
	s_stats << senseprobs[*yr_itr][s];
	if (s < senseprobs[*yr_itr].size()-1) {
	  s_stats << " ";
	} else {
	  s_stats << endl;
	}
      }
    }
  } else {
    vector<string>::iterator yr_itr;
    s_stats << it << " ";
    for (unsigned int s = 0; s < senseprobs_univ.size(); s++) {
      //      cout << "SENSE " << s << " given year " << *yr_itr << ": " << senseprobs[*yr_itr][s] << endl;
      s_stats << senseprobs_univ[s];
      if (s < senseprobs_univ.size()-1) {
	s_stats << " ";
      } else {
	s_stats << endl;
      }
    }

  }
}

void show_year(ostream& f, string year) {
  // year.substr(0,2);
  // f << "20" << year.substr(0,2);
  f << year;

}

void show_word_stats(vector<map<int, double> >& wordstats, string type,
		     unsigned int it, ostream& w_stats) {

  map<string, int>::const_iterator sym_itr;

  w_stats << "**** IT " << it << " THE WORD " << type << " **************"
	  << endl;

  for (unsigned int s = 0; s < wordstats.size(); s++) {
    vector<pair<double, int> > wp_list;

    map<int, double>::const_iterator sym_itr;
    for (sym_itr = wordstats[s].begin(); sym_itr != wordstats[s].end(); sym_itr++) {
      pair<double, int> p;
      p.first = sym_itr->second;
      // // begin temp: might try this to show ratios between two senses
      // if(s==0) {
      // 	p.first = (p.first/(wordstats[1][sym_itr->first]));
      // }
      // else if(s==1) {
      // 	p.first = (p.first/(wordstats[0][sym_itr->first]));
      // }
      // // end temp
      p.second = sym_itr->first;
      wp_list.push_back(p);

    }
    sort(wp_list.begin(), wp_list.end());
    reverse(wp_list.begin(), wp_list.end());

    // // BEGIN block of code to sort differently
    // // have this in inspector.cpp instead which runs on file of file probs
    // // so produced after EM run
    // vector<pair<double,int> > wp_list_short;
    // for(unsigned int j=0; j < 100; j++) {
    //   wp_list_short.push_back(wp_list[j]);
    // }

    // vector<pair<double,int> > ratio_list;
    // for(unsigned int j=0; j < 100; j++) {
    //   pair<double,int> p;
    //   p = wp_list_short[j];
    //   pair<double,int> rp;
    //   rp.second = p.second;
    //   if(s==0) {
    //    	rp.first = (p.first/(wordstats[1][p.second]));
    //   }
    //   else if(s==1) {
    // 	rp.first = (p.first/(wordstats[0][p.second]));
    //   }
    //   ratio_list.push_back(rp);
    // }

    // wp_list = ratio_list;
    // sort(wp_list.begin(),wp_list.end());
    // reverse(wp_list.begin(),wp_list.end());
    // END block of code to sort differently
    unsigned int num_gone_to_zero = 0;
    for (unsigned int j = 0; j < wp_list.size(); j++) {
      if (wp_list[j].first > 0) {
	w_stats << decode_to_symbol(wp_list[j].second)
		<< " given SENSE " << s << ": " << wp_list[j].first
		<< endl;
      } else {
	num_gone_to_zero++;
      }
    }
    cout << "NUM GONE TO ZERO FOR SENSE " << s << ": " << num_gone_to_zero
	 << endl;
  }

}

vector<pair<double, int> > get_one_word_stats(map<int, double>& wordstats,
					      string c) {
  vector<pair<double, int> > wp_list;
  map<int, double>::const_iterator sym_itr;
  for (sym_itr = wordstats.begin(); sym_itr != wordstats.end(); sym_itr++) {
    pair<double, int> p;
    p.first = sym_itr->second;
    p.second = sym_itr->first;
    wp_list.push_back(p);

  }
  sort(wp_list.begin(), wp_list.end());
  reverse(wp_list.begin(), wp_list.end());
  return wp_list;
}

// shows first max items of sorted version of wordstats 
void show_one_word_stats(map<int, double>& wordstats, string c,
			 unsigned int max, string format,  ostream& w_stats) {

  vector<pair<double, int> > wp_list = get_one_word_stats(wordstats, c);

  //  unsigned int num_gone_to_zero = 0;
  if(format == "horiz") {
    w_stats << c;
  }

    unsigned int limit;
  if(max < wp_list.size()) { limit = max; } else { limit = wp_list.size(); }
  for (unsigned int j = 0; j < limit; j++) {
    //if (wp_list[j].first > 0) {
    if(format == "horiz") {
      w_stats << decode_to_symbol(wp_list[j].second);
      if(j < limit-1) { w_stats << ","; }
      else { w_stats << endl; }
    }
    else {
      w_stats << decode_to_symbol(wp_list[j].second) << " given " << c
	      << ": " << wp_list[j].first << endl;
      //} else {
      //num_gone_to_zero++;
      //}
    }
  }
  // cout << "NUM GONE TO ZERO FOR " << c << ": " << num_gone_to_zero << endl;

}

// DISTYPE d param determines whether straight ratio p/q or one of the log variants of that
// that are used in KL distance calcs eg. p(log(p/q))
//
// zero_refs will get to false if a distrib function as denominator has zeroes, meaning results are meaningless
// 
// 'epsilon' gets passed from inspector code as interp_equiv
// because adding this equivalent to recalibrating the contributing 
// probs to a particular interpolation with a uniform prob u (=1/size_of_vocab))
// with interpol weight lam = 1/(1 + eps/u) then following hold
//  - renormalised (p + eps) is same as (lam p) + (1 - lam)u
//  - if p',q' are p,q interpolated with u at above defined lam
//    then p'/q' = (p + eps)/(q + eps)
//    (so for the ratio don't need to renormalise)
// for the KL variants using p(log(p/q)) for the *ranking* of the KL-contributions
// can work with just p+eps, q+eps and not renormalise, but the absolute values will
// then be slightly misleading
// 
// if contributing probs are already 'smoothed' then the passed epsilon
// should be zero 

vector<pair<double, int> > get_one_word_stats(map<int, double>& wordstats, string c,
					      map<int, double>& wordstats_ref, string c_ref,
					      DISTTYPE d,double epsilon, bool &zero_refs) {
  vector<pair<double, int> > wp_list;

  if(epsilon == 0.0) {
    if((d == RATIO || d == KL_LR) && has_zeroes(wordstats_ref)) {
      zero_refs = true;
      return wp_list;
    }
    else if ((d == KL_RL) && has_zeroes(wordstats)) {
      zero_refs = true;
      return wp_list;
    }
    else if ((d == KL_SYM) && (has_zeroes(wordstats_ref) || has_zeroes(wordstats))) {
      zero_refs = true;
      return wp_list;
    }
  }

  /* modify the input distribs to p_alt q_alt according to epsilon */
  
  map<int, double> p_alt, q_alt;
  map<int, double>::const_iterator sym_itr;
  
  if((epsilon != 0) && (d == KL_LR || d == KL_RL || d == KL_SYM)) { // add eps then normalize
    add_eps_then_renormalize_word_map(wordstats, p_alt, epsilon);
    add_eps_then_renormalize_word_map(wordstats_ref, q_alt, epsilon);
  }
  else if (epsilon != 0 && d == RATIO) { // just add eps 

    p_alt = wordstats;     q_alt = wordstats_ref;

    for (sym_itr = p_alt.begin(); sym_itr != p_alt.end(); sym_itr++) {
      double p = p_alt[sym_itr->first];
      p_alt[sym_itr->first] = p + epsilon;
      double q = q_alt[sym_itr->first];
      q_alt[sym_itr->first] = q + epsilon;
    }
  }
  else {
    // some apt response here
        p_alt = wordstats;     q_alt = wordstats_ref;
  }

  /* calc the comparison scores and store in wp_list */
  for (sym_itr = wordstats.begin(); sym_itr != wordstats.end(); sym_itr++) {
    pair<double, int> pr;
    double p, q;
    p = p_alt[sym_itr->first];
    q = q_alt[sym_itr->first];

    if(d == RATIO) {
      pr.first = p / q;
    }
    else if(d == KL_LR) {
      pr.first = computeKL_contrib(p, q);
    }
    else if(d == KL_RL) {
      pr.first = computeKL_contrib(q, p);
    }
    else if(d == KL_SYM) {
      pr.first = 0.5 * (computeKL_contrib(p, q) + computeKL_contrib(q, p));
    }
    else {
      // some apt response here
    }
    
    pr.second = sym_itr->first;
    wp_list.push_back(pr);
  }
  
  sort(wp_list.begin(), wp_list.end());
  reverse(wp_list.begin(), wp_list.end());
  return wp_list;
}

// shows first max items of sorted version of wordstats, sorting by ratio to 
// value in wordstats_ref (eg.  wordstats of another sense, or the corpus probs)
// if format is vert shows vertically with numerical value
// if format is horix shows as comma sep list without numerical values
void show_one_word_stats(map<int, double>& wordstats, string c,
			 map<int, double>& wordstats_ref, string c_ref,
			 DISTTYPE d,double epsilon, unsigned int max, string format, 
			 ostream& w_stats) {
  bool zero_refs = false;
  vector<pair<double, int> > wp_list = get_one_word_stats(wordstats, c, wordstats_ref, c_ref,
							  d, epsilon, zero_refs);

  if(zero_refs) {
    cout << "a prob distribs used as denominator contains zeroes\n";
    cout << "need to pass a non-zero epsilon (eg. -interp_equiv)\n";
    return;
  }

  if(format == "horiz") {
    w_stats << c << ": ";
  }

  unsigned int limit;
  if(max < wp_list.size()) { limit = max; } else { limit = wp_list.size(); }
  for (unsigned int j = 0; j < limit ; j++) {
    if(format == "horiz") {
      w_stats << decode_to_symbol(wp_list[j].second);
      if(j < limit-1) { w_stats << ","; }
      else { w_stats << endl; }
    }
    else {
      w_stats << decode_to_symbol(wp_list[j].second) << " given " << c
	      << ": " << wp_list[j].first << endl;
    }
  }
  //cout << "NUM GONE TO ZERO FOR " << c << ": " << num_gone_to_zero << endl;

}

void show_count_zeros_in_word_stats(vector<map<int, double> > word_given_sense_probs, ostream& w_stats) {
  for(int s=0; s < word_given_sense_probs.size(); s++) {
    w_stats << "NUM GONE TO ZERO FOR " << s << ": ";
    map<int, double>::const_iterator sym_itr;
    int num_to_zero = 0;
    for (sym_itr = word_given_sense_probs[s].begin(); sym_itr != word_given_sense_probs[s].end(); sym_itr++) {
      if(sym_itr->second == 0.0) { num_to_zero++; }
    }
    w_stats << num_to_zero << endl;
  }
}

bool has_zeroes(map<int, double>& wordstats) {

  map<int, double>::const_iterator sym_itr;
  for (sym_itr = wordstats.begin(); sym_itr != wordstats.end(); sym_itr++) {

    if(sym_itr->second == 0.0) {
      cout << "0 prob for: " << sym_itr->first << " " << decode_to_symbol(sym_itr->first) << "(probably others too)" << endl;
      return true;
    }
  }
  return false;
}


// pass probs or counts 
void write_word_stats(vector<map<int, double> >& wordstats,
		      string word_stats_out) {
  // file format is 
  // S W
  // SymTableName
  // SenseNamesFile
  // s_1 w_1_p w_2_p ..... w_W_p
  // s_2  ditto
  // s_S  ditto
  // so for each sense full line of entries, including words with zero prob
  // have to assume position in this line is same as symbol table code

  ofstream word_stats_str;
  word_stats_str.open(word_stats_out.c_str());
  if (!word_stats_str) {
    cout << "prob opening " << word_stats_out << endl;
    exit(1);
  } else {
    cout << "successfully opened " << word_stats_out << endl;

  }

  //  word_stats_str << senses.size() << " " << symbol_total << endl;
  word_stats_str << sense_size << " " << symbol_total << endl;
  word_stats_str << symbol_table_for_wordprobs_filename << endl;
  word_stats_str << sense_names_for_wordprobs_filename << endl;

  for (unsigned int s = 0; s < wordstats.size(); s++) {
    word_stats_str << s << " ";
    map<int, double>::const_iterator sym_itr;
    for (sym_itr = wordstats[s].begin(); sym_itr != wordstats[s].end(); sym_itr++) {
      if (sym_itr != wordstats[s].begin()) {
	word_stats_str << " ";
      }
      word_stats_str << sym_itr->second;
    }
    // end loop of words
    word_stats_str << endl;
  } //  loop over senses

}

// bool compare_matrices(double **COST1, unsigned int size1, double **COST2, unsigned int size2) {

//   for(unsigned int i = 0; i <= size1; i++) {
//     for(unsigned int j = 0; j <= size1; j++) {
//       if(COST1[i][j] != COST2[i][j]) {

// 	cout << i << " " << j << endl;
// 	cout << COST1[i][j] << " " << COST2[i][j] << endl;
// 	return false;
//       }
//     }
//   }
//   return true;
// }

void set_corpus_word_probs(void) {

  map<string, int>::const_iterator sym_itr;
  // set all to zero
  for (sym_itr = the_symbols.begin(); sym_itr != the_symbols.end(); sym_itr++) {
    wordcounts_corp[sym_itr->second] = 0.0;
  }
  vector<string>::iterator yr_itr;
  for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
    wordcounts_given_year[*yr_itr] = wordcounts_corp;
  }

  // traverse Corp::data updaing wordcounts
  for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {
    Occ d;
    d = Corp::data[dn];
    // for each word w in d.words {
    for (unsigned int w = 0; w < d.words.size(); w++) {
      if (d.words[w] != 0) {
	wordcounts_corp[d.words[w]] += (1 * d.rpt_cntr);
	wordcounts_given_year[d.year][d.words[w]] += (1 * d.rpt_cntr);
      }
    }
  }

  //begin temp debug sep 21 2015
  // cout << "begin low count words\n";
  // int low_ct = 0;
  // for (sym_itr = the_symbols.begin(); sym_itr != the_symbols.end(); sym_itr++) {
  //   if(wordcounts_corp[sym_itr->second] <= 140.0) {
  //     cout << sym_itr->first << " " << wordcounts_corp[sym_itr->second] << endl;
  //     low_ct += 1;
  //   }
  // }
  // cout << "entire num low count words: " << low_ct << endl;
  //end temp debug sep 21 2015


  // normalize into probs
  normalize_word_map(wordcounts_corp, wordprobs_corp);

  for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
    map<int, double> cnts;
    map<int, double> probs;
    double total;
    cnts = wordcounts_given_year[*yr_itr];
    probs = cnts;
    total = normalize_word_map(cnts, probs);
    wordprobs_given_year[*yr_itr] = probs;
    wordtotal_given_year[*yr_itr] = total;
  }
}

// note this is exact same style of implementation as used for wc_min in
// dynamicEM.cpp
void set_corpus_word_probs(double corp_wc_min) {
  map<string, int>::const_iterator sym_itr;
  // set all to zero
  for (sym_itr = the_symbols.begin(); sym_itr != the_symbols.end(); sym_itr++) {
    wordcounts_corp[sym_itr->second] = corp_wc_min;
  }
  vector<string>::iterator yr_itr;
  for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
    wordcounts_given_year[*yr_itr] = wordcounts_corp;
  }

  // traverse Corp::data updaing wordcounts
  for (unsigned int dn = 0; dn < Corp::data.size(); dn++) {
    Occ d;
    d = Corp::data[dn];
    // for each word w in d.words {
    for (unsigned int w = 0; w < d.words.size(); w++) {
      if (d.words[w] != 0) {
	wordcounts_corp[d.words[w]] += (1 * d.rpt_cntr);
	wordcounts_given_year[d.year][d.words[w]] += (1 * d.rpt_cntr);
      }
    }
  }



  // normalize into probs
  normalize_word_map(wordcounts_corp, wordprobs_corp);
  wordtotal_given_year.clear();
  for (yr_itr = years.begin(); yr_itr != years.end(); yr_itr++) {
    map<int, double> cnts;
    map<int, double> probs;
    double total;
    cnts = wordcounts_given_year[*yr_itr];
    probs = cnts;
    total = normalize_word_map(cnts, probs);
    // threshold hack
    // threshold(probs,0.0001);
    wordprobs_given_year[*yr_itr] = probs;
    wordtotal_given_year[*yr_itr] = total;
  }

}



void threshold(map<int, double>& wp, double eps) {
  map<int, double>::const_iterator w_itr;

  for (w_itr = wp.begin(); w_itr != wp.end(); w_itr++) {
    int w;
    double prob;
    w = w_itr->first;
    prob = w_itr->second;
    if(prob < eps) {
      wp[w] = 0.0;
    }

  }
}
