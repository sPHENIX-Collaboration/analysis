#include "tau_commons.h"

/**
 * Test manual cuts
 *
 * Written by nils.feege@stonybrook.edu
 */
int test_cuts()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  /* open inout files and merge trees */
  TChain chain("event");
  chain.Add("/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_1000events_1seed_3pion_r05.root");

  /* particle selection */
  float n_trueaccept_uds = chain.GetEntries(tau_commons::select_true_uds );
  float n_trueaccept_tau = chain.GetEntries(tau_commons::select_true_tau );

  
  //&& tau_commons::select_accept_jet

  /* True positive = tau-identified-as-tau */
  float n_tau_as_tau = chain.GetEntries( tau_commons::select_true_tau  &&  tau_commons::select_tau);
  float n_tau_as_uds = chain.GetEntries( tau_commons::select_true_tau  && !tau_commons::select_tau);

  float n_uds_as_uds = chain.GetEntries( tau_commons::select_true_uds && !tau_commons::select_tau );
  float n_uds_as_tau = chain.GetEntries( tau_commons::select_true_uds &&  tau_commons::select_tau );

  cout << "Selection of TRUE TAU:   " << tau_commons::select_true_tau.GetTitle() << endl;
  cout << "Selection of TRUE QUARK: " << tau_commons::select_true_uds.GetTitle() << endl;
  cout << "Selection of JET acceptance:   " << tau_commons::select_accept_jet.GetTitle() << endl;
  cout << endl;
  cout << "Total sample TAU:   " << n_trueaccept_tau << endl;
  cout << "Total sample QUARK: " << n_trueaccept_uds << endl;
  cout << endl;
  cout << "Selection of RECO TAU:   " << tau_commons::select_tau.GetTitle() << endl;
  cout << endl;
  cout << "Tau-as-tau (True  Positive): " << n_tau_as_tau << endl;
  cout << "Tau-as-uds (False Negative): " << n_tau_as_uds << endl;
  cout << "Uds-as-uds (True  Negative): " << n_uds_as_uds << endl;
  cout << "Uds-as-tau (False Positive): " << n_uds_as_tau << endl;

  cout << endl;
  cout << "True  positive rate: " << n_tau_as_tau / ( n_tau_as_tau + n_tau_as_uds ) << endl;
  cout << "False positive rate: " << n_uds_as_tau / ( n_uds_as_tau + n_tau_as_tau ) << endl;
  cout << endl;
  cout << "Tau recovery efficiency: " << n_tau_as_tau / n_trueaccept_tau << endl;
  cout << endl;


  return 0;
}
