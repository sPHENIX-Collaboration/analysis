// Some documentation //

#include "PIDProbabilities.h"
#include "Poisson.h"

#include <TDatabasePDG.h>

using namespace std;


PIDProbabilities::PIDProbabilities()
{
  _pdg = new TDatabasePDG();
  _poisson = new Poisson();
}

bool 
PIDProbabilities::particle_probs( vector<float> angles, double momentum, double index, double probs[4] ){

  /* Initialize particle masses, probably want to use pdg database */
  int pid[4] = { 11, 211, 321, 2212 };
  double m[4] = {
    _pdg->GetParticle(pid[0])->Mass(),
    _pdg->GetParticle(pid[1])->Mass(),
    _pdg->GetParticle(pid[2])->Mass(),
    _pdg->GetParticle(pid[3])->Mass()
  };

  /* Set angle window by taking smallest difference at high momentum in question (70 GeV) */
  double test_p = 70;
  double windowx2 = acos( sqrt( m[1]*m[1] + test_p*test_p ) / index / test_p ) - acos( sqrt( m[1]*m[1] + test_p*test_p ) / index / test_p );
  double window = windowx2/2;

  /* Determine expectation value for each particle */
  double beta[4] = {
    momentum/sqrt( m[0]*m[0] + momentum*momentum ),
    momentum/sqrt( m[1]*m[1] + momentum*momentum ),
    momentum/sqrt( m[2]*m[2] + momentum*momentum ),
    momentum/sqrt( m[3]*m[3] + momentum*momentum )
  };

  double theta_expect[4] = {
    acos( 1/( index * beta[0] ) ),
    acos( 1/( index * beta[1] ) ),
    acos( 1/( index * beta[2] ) ),
    acos( 1/( index * beta[3] ) )
  };


  /* Calculate average counts based on sims */
  // Needs to be implemented after sims are finished //
  double counts_cal[4] = {
    20,
    20,
    20,
    20
  };

  /* Count hits within window */
  int counts[4] = {0,0,0,0};
  for (int i=0; i<300; i++){
    for (int j=0; j<4; j++){
      if ( angles[i] > theta_expect[j] - window && angles[i] < theta_expect[j] + window )
	counts[j]++;
    }
  }


  /* Determine particle probabilities, Poisson probability mass function */
  for (int k=0; k<4; k++)
    probs[k] = _poisson->poisson_prob( counts_cal[k], counts[k] );


  return true;
}
