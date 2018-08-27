// Some documentation //

#include "Poisson.h"

using namespace std;


long double
Poisson::poisson_prob( double mean, int value ){
  
  double prob = exp1( mean, value ) * exp2(mean) / fact(value);
  return prob;

}

long double
Poisson::exp1( double mean, int value ){

  long double output = pow( mean, value );
  return output;

}

long double
Poisson::exp2( double mean ){
  
  long double output = pow( 2.718, -1*mean );
  return output;

}

long double
Poisson::fact( int value ){

  long double output = 1;

  for( int i = 1; i <= value; ++i)
    output *= i;
  
  return output;
  
}
