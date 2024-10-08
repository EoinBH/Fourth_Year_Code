
#include <string>
using namespace std;

int stl_to_int(string s);
string int_to_stl(int);
void tolower(string& s);
string time_stamp(void);
string add_suffix(string name_in);
string add_input_suffix(string name_in);
void dir_and_file(string path, string& dir, string& file);

extern string output_suffix;
extern string input_suffix;
