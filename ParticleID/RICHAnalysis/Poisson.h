#ifndef __POISSON_H_
#define __POISSON_H_

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <cmath>

using namespace std;


// Poisson class //                                                                                                                                           

class Poisson {

 private:

 public:

  double poisson_prob( double mean, int value );
  double exp1( double mean, int value );
  double exp2( double mean );
  int fact( int value );
  
};

#endif
