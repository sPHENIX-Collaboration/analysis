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
PIDProbabilities::particle_probs( vector<float> angles, double momentum, double index, long double probs[4] ){

  bool use_reconstructed_momentum = false;
  bool use_truth_momentum = false;
  bool use_emission_momentum = true;
  

  /* Initialize particle masses, probably want to use pdg database */
  int pid[4] = { 11, 211, 321, 2212 };
  double m[4] = {
    _pdg->GetParticle(pid[0])->Mass(),
    _pdg->GetParticle(pid[1])->Mass(),
    _pdg->GetParticle(pid[2])->Mass(),
    _pdg->GetParticle(pid[3])->Mass()
  };

  /* Set angle window by taking smallest difference, highest momentum in question (70 GeV) */
  double test_p = 70;
  double windowx2 = acos( sqrt( m[1]*m[1] + test_p*test_p ) / index / test_p ) - acos( sqrt( m[2]*m[2] + test_p*test_p ) / index / test_p );
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
  double counts_cal[4] = {0,0,0,0};
  if ( use_reconstructed_momentum )
    {
      cout << "Reconstructed momentum function undefined now!" << endl;
      counts_cal[0] = 0;
      counts_cal[1] = 0;
      counts_cal[2] = 0;
      counts_cal[3] = 0;
    }
  if ( use_truth_momentum )
    {
      cout << "Truth momentum function undefined now!" << endl;
      counts_cal[0] = 0;
      counts_cal[1] = 0;
      counts_cal[2] = 0;
      counts_cal[3] = 0;
    }
  if ( use_emission_momentum )
    {
      // Proton function still needs to be implemented after sims are finished //
      counts_cal[0] = 29.5;
      counts_cal[1] = 25.8 * acos( sqrt(178.4 + momentum*momentum) / (2.62*momentum) );
      counts_cal[2] = 22.08 * acos( sqrt(982600 + momentum*momentum) / (52.93*momentum) );
      counts_cal[3] = 0;
    }      


  /* Count hits within window */
  int counts[4] = {0,0,0,0};
  for (int i=0; i<300; i++){
    for (int j=0; j<4; j++){
      if ( angles[i] > (theta_expect[j] - window) && angles[i] < (theta_expect[j] + window) )
	counts[j]++;
    }
  }

  /* Determine particle probabilities, Poisson probability mass function */
  long double probs_norm = 0; 
  for (int k=1; k<3; k++)
    {
      probs[k] = _poisson->poisson_prob( counts_cal[k], counts[k] );
      probs_norm += probs[k];
    }
  for (int k=1; k<3; k++)
    probs[k] /= probs_norm;


  /* Alert that this may have been a glitched event if very few photons */
  double allcounts = counts[0] + counts[1] + counts[2] + counts[3];
  double allcal = counts_cal[0] + counts_cal[1] + counts_cal[2] + counts_cal[3];
  if ( allcounts / allcal < 0.2 )
    cout << "WARNING: Very few photons in event. May be an unintended track!" << endl;


  return true;
}
