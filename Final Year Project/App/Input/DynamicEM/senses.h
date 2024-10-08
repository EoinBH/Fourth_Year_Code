#include <string>
#include <vector>

using namespace std;

#if !defined SENSE_H
#define SENSE_H
typedef int SENSE;
//enum SENSE {WALLS, PHONE, OTHER, UNK};
#endif

extern vector<string> senses;
extern vector<string> true_senses;

/*! \brief
 the number of senses \n
 if its value is N \n 
  senseprobs[y][s] is valid for s up to N-1 \n
  wordprobs[s][wn] is value for s up to N-1
 */
extern unsigned int sense_size;

extern string sense_to_string(SENSE S);
extern string unsup_sense_to_string(SENSE S);
extern void initialize_true_senses(string sense_file);

/*! \brief sets sense_size \n
 * \param num_senses is the number of senses that si goind to be used
 *
 * global var sense_size is set to this number
 *
 * global var senses is set to vector of dummy (string) names for these senses
 *
 * this does not initialize any probabilities relating to senses but must be called
 * before functions to initialize probabilities
 */
extern void initialize_unsup_senses(unsigned int num_senses);
