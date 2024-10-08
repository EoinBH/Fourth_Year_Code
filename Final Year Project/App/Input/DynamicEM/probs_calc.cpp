#include <vector>
#include <map>
#include <string>
#include <fstream>

#include <utility>

#include <algorithm>
#include <math.h>

using namespace std;

#include "sym_table.h"

#include "prob_tables.h"
#include "probs_calc.h"      

//TODO: may well underflow
double calc_words_g_sense(Occ occ, unsigned int s) {
	double p = 1.0;

	for (unsigned int i = 0; i < occ.words.size(); i++) {
		double word_contrib = 0.0;
		if (occ.words[i] == 0) { // out-of-table
			word_contrib = 1.0;
		} else {
			word_contrib = wordprobs[s][occ.words[i]];
#if CAT_DEBUG
			cout << "prob of " << decode_to_symbol(occ.words[i]) << " in sense " << s << ": " << word_contrib << endl;
#endif
		}
		p = p * word_contrib;

	}
	return p;
}

/*! epsilon parameter comes from interp_equiv input parameter
 * is equivalent to a certain interpolation of the prob with a uniform prob
 * has smoothing effect
 * and if wc_min was set to non-zero in training then epsilon should be zero
 */
double calc_words_g_sense_with_eps(Occ occ, unsigned int s, double epsilon) {
	double p = 1.0;

	for (unsigned int i = 0; i < occ.words.size(); i++) {
		double word_contrib = 0.0;
		if (occ.words[i] == 0) { // out-of-table
			word_contrib = 1.0;
		} else {
		  word_contrib = wordprobs[s][occ.words[i]] + epsilon;
#if CAT_DEBUG
			cout << "prob of " << decode_to_symbol(occ.words[i]) << " in sense " << s << ": " << word_contrib << endl;
#endif
		}
		p = p * word_contrib;

	}
	return p;

}

void categorise_by_data_with_eps(Occ &d, double &data_prob, double &max_prob,
				 vector<double> &joint_probs, SENSE &S, double epsilon) {
	int alts_at_0 = 0;
	for (unsigned int s = 0; s < senses.size(); s++) {
		double year_base, sense_g_year, words_g_sense;
		double sense_univ;
		double joint_prob;
		words_g_sense = calc_words_g_sense_with_eps(d, s, epsilon);
		year_base = yearprobs[d.year];

		if (years_model) {
			sense_g_year = senseprobs[d.year][s];
			//k * P(C^d =i |year^d) * P(words^d|C^d = i)
			joint_prob = year_base * sense_g_year * words_g_sense;
			if (joint_prob == 0.0) {
				alts_at_0++;
			}
			joint_probs.push_back(joint_prob);
#if CAT_DEBUG
			cout << "for sense " << s << "year_base " << year_base;
			cout << " sense_g_year " << sense_g_year;
			cout << " words_g_sense " << words_g_sense;
			cout << endl << "joint " << joint_prob << endl;
#endif
		} else {
			sense_univ = senseprobs_univ[s];
			//k * P(C^d =i) * P(words^d|C^d = i)
			joint_prob = year_base * sense_univ * words_g_sense;
#if CAT_DEBUG
			cout << "for sense " << s << "year_base " << year_base;
			cout << " sense_univ " << sense_univ;
			cout << " words_g_sense " << words_g_sense;
			cout << endl << "joint " << joint_prob << endl;
#endif
		}
		data_prob += joint_prob;
		if (joint_prob > max_prob) {
			S = s;
			max_prob = joint_prob;
		}
	}
	sort(joint_probs.rbegin(), joint_probs.rend());
#if CAT_DEBUG
	cout << "chosen " << chosen << "prob " << max_prob << endl;
	cout << "num of alts at 0: " << alts_at_0 << endl;
#endif

}

// seldom used
SENSE categorise(Occ d, ostream& cat_dtls) {

	double max_prob = 0.0;
	SENSE chosen = 0;
#if CAT_DEBUG
	d.show();
#endif
	int alts_at_0 = 0;
	for (unsigned int s = 0; s < senses.size(); s++) {
		double year_base, sense_g_year, words_g_sense;
		double sense_univ;
		double joint_prob;
		words_g_sense = calc_words_g_sense(d, s);
		year_base = yearprobs[d.year];

		if (years_model) {
			sense_g_year = senseprobs[d.year][s];
			//k * P(C^d =i |year^d) * P(words^d|C^d = i)
			joint_prob = year_base * sense_g_year * words_g_sense;
			if (joint_prob == 0.0) {
				alts_at_0++;
			}
#if CAT_DEBUG
			cat_dtls << "for sense " << s << "year_base " << year_base;
			cat_dtls << " sense_g_year " << sense_g_year;
			cat_dtls << " words_g_sense " << words_g_sense;
			cat_dtls << endl << "joint " << joint_prob << endl;
#endif
		} else {
			sense_univ = senseprobs_univ[s];
			//k * P(C^d =i) * P(words^d|C^d = i)
			joint_prob = year_base * sense_univ * words_g_sense;
#if CAT_DEBUG
			cat_dtls << "for sense " << s << "year_base " << year_base;
			cat_dtls << " sense_univ " << sense_univ;
			cat_dtls << " words_g_sense " << words_g_sense;
			cat_dtls << endl << "joint " << joint_prob << endl;
#endif
		}

		if (joint_prob > max_prob) {
			chosen = s;
			max_prob = joint_prob;
		}
	}

#if CAT_DEBUG
	cat_dtls << "chosen " << chosen << "prob " << max_prob << endl;
	cat_dtls << "num of alts at 0: " << alts_at_0 << endl;
#endif

	return chosen;
}

// seldom used 
void categorise_by_data(Occ &d, double &data_prob, double &max_prob,
		vector<double> &joint_probs, SENSE &S) {
	int alts_at_0 = 0;
	for (unsigned int s = 0; s < senses.size(); s++) {
		double year_base, sense_g_year, words_g_sense;
		double sense_univ;
		double joint_prob;
		words_g_sense = calc_words_g_sense(d, s);
		year_base = yearprobs[d.year];

		if (years_model) {
			sense_g_year = senseprobs[d.year][s];
			//k * P(C^d =i |year^d) * P(words^d|C^d = i)
			joint_prob = year_base * sense_g_year * words_g_sense;
			if (joint_prob == 0.0) {
				alts_at_0++;
			}
			joint_probs.push_back(joint_prob);
#if CAT_DEBUG
			cout << "for sense " << s << "year_base " << year_base;
			cout << " sense_g_year " << sense_g_year;
			cout << " words_g_sense " << words_g_sense;
			cout << endl << "joint " << joint_prob << endl;
#endif
		} else {
			sense_univ = senseprobs_univ[s];
			//k * P(C^d =i) * P(words^d|C^d = i)
			joint_prob = year_base * sense_univ * words_g_sense;
#if CAT_DEBUG
			cout << "for sense " << s << "year_base " << year_base;
			cout << " sense_univ " << sense_univ;
			cout << " words_g_sense " << words_g_sense;
			cout << endl << "joint " << joint_prob << endl;
#endif
		}
		data_prob += joint_prob;
		if (joint_prob > max_prob) {
			S = s;
			max_prob = joint_prob;
		}
	}
	sort(joint_probs.rbegin(), joint_probs.rend());
#if CAT_DEBUG
	cout << "chosen " << chosen << "prob " << max_prob << endl;
	cout << "num of alts at 0: " << alts_at_0 << endl;
#endif

}

