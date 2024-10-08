#include "GoogleOcc.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>


GoogleOcc::GoogleOcc() {}

void GoogleOcc::show() {

  cout << "year: " << year << endl;
  cout << "num: " << num << endl;
  cout << "sense: " << sense_to_string(true_sense) << endl;
  cout << "url_string: " << url_string << endl;
  cout << "hit_string: " << hit_string << endl;

}

void GoogleFile::read(string csv_file_in) {

}

void GoogleFile::write(string csv_file_out) {

  ofstream f;
  f.open(csv_file_out.c_str());
  if(!f) { cout << "prob opening " << csv_file_out; exit(1); }

  for(unsigned int i = 0; i < google_data.size(); i++) {
    f << google_data[i].num << '\t';
    if(google_data[i].true_sense == -1) {
      f << '\t';
    }
    else {
      f << google_data[i].true_sense << '\t';
    }

    f << google_data[i].url_string << '\t';
    f << google_data[i].hit_string << endl;
  }

  f.close();

  
}
