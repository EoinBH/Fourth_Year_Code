#include <vector>
#include <string>

using namespace std;

/*! vector giving the year identifiers */
extern vector<string> years;
extern int grouping_size;

string year_to_grouped_year(string);

/*! adds a year string to the years vector if not already there */
extern void add_to_years(string year);
