#include <vector>
#include <string>
#include "senses.h"

using namespace std;

#if !defined GOOGLE_OCC_H
#define GOOGLE_OCC_H
/* occurrence data structure */
class GoogleOcc {
public:
  GoogleOcc();
  //??
  unsigned int num;
  string year;
  SENSE true_sense;
  string url_string;
  string hit_string;
  string url;
  void show(void);
};

class GoogleFile {
 public:
  vector<GoogleOcc> google_data;
  void read(string csv_file_in);
  void write(string csv_file_out);

};

#endif
