#include <string>
#include <vector>

using namespace std;

/*! \brief
  which field of a wlp line to take
  w for first (word) \n
  l for second (lemma) \n
  p for third (pos)
 */
extern char coha_wlp_field;
/*! \brief
  which field of a wlp line to consider in cheching for a target match
  0 for follow whatever wlp says \n
  w for first (word) \n
  l for second (lemma) \n
  p for third (pos) \n
  allows say 'w' for target match, but 'l' for context words around target
 */
extern char coha_targ_wlp_field;
extern bool excl_targ_np1; // used to eliminate lemma target matches was originally upper-case name
extern bool filter_coha_at_token; // used to skip @ tokens

#if !defined COHATOKEN_H
#define COHATOKEN_H

class CohaToken {
public:
  CohaToken();
  CohaToken(string wlp_line);

  string word;
  string lemma;
  string pos;
  string fetch_field(char wlp);
  void show(void);
  bool is_at_token;
 private:
  void fix_a_coha_token(string& s);
};


#endif
