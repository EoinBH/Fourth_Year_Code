#include <vector>
#include <string>
#include <fstream>

using namespace std;

#if !defined CMD_H
#define CMD_H

/*! \brief class to represent a '-param value' part of a command line  \n
    has arg_id for the 'param' part \n
    has arg_val for the 'value' part
*/
class ArgVal {
public:
  ArgVal();
  ArgVal(string id, char *v);
  void show();
  string arg_id;
  char *arg_val;
};


typedef vector<ArgVal> ParList;

/*! \brief turns the command line parameters into a vector of pairs (codes as ArgVal objects) \n
    each pair contains a parameter name as a string \n
    and the char * part of the command line that is supposed to represent its value
 */
ParList parse_command_line(int argc, char **argv);
ParList parse_fresh_params(string new_params_line, bool &ok);

/*! \brief effectively checks whether -help yes was in the parameters */
bool check_for_help(ParList l);

void show_params(ParList l,ostream& f); /*!< \brief this shows/saves verbatim the command-line params */

/*! \brief called when there are uninterpreted left-overs \n
  there are functions like compPar::process_params  dynEMPar::process_params which take the parameter list \n
  and delete from it the meaningful parts so this function is only called when these other functions have \n
  not mopped up a pairing
 */
void exit_with_help(ParList l); 

/*! \brief turns yes/no to the bool values */
bool convert_to_bool(string option);
/*! \brief turns the bool values to yes/no */
string bool_to_yes_no(bool b);

#endif

