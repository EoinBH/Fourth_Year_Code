#include <iostream>
#include "Occ.h"
#include "senses.h"      

using namespace std;



/*! \brief prob of the observed words in occurrence occ, given sense s 

    just multiplies individual word probs for particular sense
*/
double calc_words_g_sense(Occ occ, unsigned int s);

/*! \brief gets prob of the observed words in occurrence occ, given sense s \n
    used in code to retrieve likely examples of a sense */
double calc_words_g_sense_with_eps(Occ occ, unsigned int s, double epsilon);

/*! \brief use in code to retrieve likely examples of a sense */
void categorise_by_data_with_eps(Occ &d, double &data_prob, double &max_prob,
				 vector<double> &joint_probs, SENSE &S, double epsilon);


/*! \brief seldom used function assigning likeliest senses given parameters */
SENSE categorise(Occ d, ostream& cat_dtls);

/*! \brief seldom used function assigning likeliest senses given parameters */
void categorise_by_data(Occ &d, double &data_prob, double &max_prob,
		vector<double> &joint_probs, SENSE &S);

