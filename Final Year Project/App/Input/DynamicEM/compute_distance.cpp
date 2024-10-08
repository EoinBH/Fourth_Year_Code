/*
 * code for Kullback-Leiber , Cosine (and JensenShannon, Hellinger)
 *
 * orig versions 8 Jun 2015 by Arun Jayapal
 * modifications commenced 2018 Martin Emms
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <utility>
#include <algorithm>

#include "prob_tables.h"
#include "senses.h"
#include "compute_distance.h"

using namespace std;

// note: for kl, if p1 is 0, result should be 0
// but will get nan from p1 * log(p1/p2)
double computeKL_contrib(double p1, double p2) {
  double kl_contrib;
  if(p1 == 0.0) { kl_contrib = 0; }
  else { kl_contrib = p1 * (log2(p1) - log2(p2)); }
  return kl_contrib;
}

