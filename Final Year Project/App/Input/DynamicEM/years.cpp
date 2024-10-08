#include "years.h"
#include "StringUtil.h"
#include <algorithm>

vector<string> years;

int grouping_size = 1;

void add_to_years(string year) {
  vector<string>::iterator yr_itr;
  yr_itr = find(years.begin(), years.end(), year);
  if (yr_itr == years.end()) {
    years.push_back(year);
  }
}

// --> convert year to int version y

  // --> do int division and multiplication

  //     (y / N) * N

  //     eg. for 0 - 4, get 0
  //         for 5 - 9, get 5

  //     etc

string year_to_grouped_year(string year) {
  int year_int;
  year_int = stl_to_int(year);
  int grouped_year_int;
  grouped_year_int = (year_int / grouping_size) * grouping_size;
  return int_to_stl(grouped_year_int);
}

