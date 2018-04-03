// Some documentation //

#include "PIDProbabilities.h"

#include <TH1.h>
#include <TDatabasePDG.h>

using namespace std;


PIDProbabilities::PIDProbabilities()
{
  _pdg = new TDatabasePDG();
}

bool 
PIDProbabilities::particle_probs( vector<float> angles, double momentum, double index, double probs[4] ){

  /* Initialize particle masses, probably want to use pdg database */
  int pid[4] = { 11, 211, 321, 2212 };
  double mass[4] = {
    _pdg->GetParticle(pid[0])->Mass(),
    _pdg->GetParticle(pid[1])->Mass(),
    _pdg->GetParticle(pid[2])->Mass(),
    _pdg->GetParticle(pid[3])->Mass()
  };
  double mass_diff[4];


  /* Fill histogram for track, cut out (most) non-physical angles */
  TH1F* hist = new TH1F("hist","hist",1000,0.0,0.04);

  for (int i=0; i<300; i++)
    hist->Fill(angles[i]);

  /* Get mean from histogram, can refine with peak finding later; may work with just theta <= 2 deg (0.035 rad) */
  double theta_mean = hist->GetMean();

  /* Calculate beta from reco angle */
  double beta = 1/( index * cos(theta_mean) );

  /* Calculate mass from beta */
  double mass_reco;
  if (beta<1 && beta>0)
    mass_reco = momentum * sqrt( 1/beta - 1  );
  else
    mass_reco = 0;


  /* Fill mass differences */
  /* Will eventually need to determine how to characterize the probabilities */
  for (int i=0; i<4; i++)
    {
      mass_diff[i] = abs(mass_reco - mass[i]);
    }

  double total_diff = mass_diff[0] + mass_diff[1] + mass_diff[2] + mass_diff[3];

  for (int j=0; j<4; j++)
    {
      probs[j] = 1.0/3.0*( 1 - (mass_diff[j]/total_diff) );
    }


  /* Delete histogram */
  delete hist;


  return true;
}
