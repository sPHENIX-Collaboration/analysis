#ifndef __PIDPROBABILITIES_H_
#define __PIDPROBABILITIES_H_

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <cmath>

class TDatabasePDG;
class Poisson;

using namespace std;


// PIDProbabilities class //

class PIDProbabilities {

 private:
 
 public:

  PIDProbabilities();

  bool particle_probs( vector<float> angles, double momentum, double index, long double probs[4] );

  /* PDG database access object */
  TDatabasePDG *_pdg;
  Poisson *_poisson;

};

#endif
