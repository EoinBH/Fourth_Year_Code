#include <vector>
#include <string>
#include "senses.h"

using namespace std;

#if !defined OCC_H
#define OCC_H

extern unsigned int desired_left;
extern unsigned int desired_right;
extern bool whether_pad;
extern bool whole_coha_article;
/*! \brief occurrence data structure for an occurrence of a word \n
    and words to left and right
 */
class Occ {
public:
  Occ();
  vector<int> words; /*!< \brief vector of codes of words to left and right \n
                      note this vector does not distinguish left and right \n
                      these can be found using size_left and size_right */
  SENSE true_sense;
  SENSE inferred_sense;
  string year; /*!< \brief year of the publication */
  void show(void);
  void show_inf(void);
  string make_string();
  double data_prob, max_prob, diff_to_max;
  int rpt_cntr; /*!< \brief how many times this repeats in the publication year */
  int size_left; /*!< \brief how many in words are representing things left of target \n
                      note some might have been added as 'padding' at left */
  int size_right; /*!< \brief how many in words are representing things right of target \n
                      note some might have been added as 'padding' at right */
  int num_L; /*!< \brief how many of the words are padding L added to left */
  int num_R; /*!< \brief how many of the words are padding R added to right */
};
#endif
