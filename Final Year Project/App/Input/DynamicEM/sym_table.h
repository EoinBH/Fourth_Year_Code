#include <vector>
#include <map>
#include <string>
#include <stdlib.h>

using namespace std;

extern bool whether_closed;
extern string symbol_table_file_source; /*! \brief file from which table may have initially been read */
extern unsigned int symbol_total; /*!< \brief the number of symbols ie. the size of the vocab */
extern map<string,int> the_symbols; /*!< \brief the symbol table itself */

unsigned int get_code(string cat); /*!< \brief returns code for a word \n
				     if not in table, adds it, returning the assigned code */
unsigned int check_code(string cat); /*!< \brief returns symbol code if there, else 0 */
string decode_to_symbol(int code); /*!< \brief returns the string represented by the code, else "none" */

void initialize_symbol_table(void);  

void read_symbol_table(string symbol_table_file); 
void write_symbol_table(string symbol_table_file);

void decode_print(int code); /*!< prints whatever string is returned by decode_to_symbol */

