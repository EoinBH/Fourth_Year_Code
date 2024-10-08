#include <string> 
using namespace std;

#if !defined UNIFTYPE_H
#define UNIFTYPE_H

enum UNIFTYPE {FLAT, CORPUS};
#endif

UNIFTYPE convert_to_uniftype(string s);
