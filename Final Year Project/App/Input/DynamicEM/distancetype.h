#include <string> 
using namespace std;

#if !defined DISTTYPE_H
#define DISTTYPE_H

enum DISTTYPE {RATIO,KL_LR,KL_RL,KL_SYM};
#endif

DISTTYPE convert_to_disttype(string s);
