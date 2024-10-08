#include<string>
using namespace std;

#if !defined VERSID_H
#define VERSID_H
class Version {
 public:
  static string git_commit_id;
  static string git_commit_date;
  static string git_commit_person;
};
#endif



