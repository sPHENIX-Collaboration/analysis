// Some documentation //

#include "Poisson.h"

using namespace std;


double
Poisson::poisson_prob( double mean, int value ){
  
  double prob = exp1( mean, value ) * exp2(mean) / fact(value);
  return prob;

}

double
Poisson::exp1( double mean, int value ){

  double output = pow( mean, value );
  return output;

}

double
Poisson::exp2( double mean ){
  
  double output = pow( 2.718, mean );
  return output;
}

int
Poisson::fact( int value ){

  int output = 0;
  for( int i = 1; i <= value; ++i)
    output *= i;
  
  return output;
  
}
