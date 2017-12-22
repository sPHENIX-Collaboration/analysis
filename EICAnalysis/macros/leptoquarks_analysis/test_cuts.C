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
  TChain chain("candidates");
  chain.Add("data_3pions/p250_e20_0events_file1093_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1096_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1101_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1115_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1122_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1127_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1131_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1164_LeptoAna_r05.root");

  /* particle selection */
  TCut select_true_uds("evtgen_is_tau==0");
  TCut select_true_tau("evtgen_is_tau==1 && sqrt( (evtgen_tau_eta-jet_eta)*(evtgen_tau_eta-jet_eta) + (evtgen_tau_phi-jet_phi)*(evtgen_tau_phi-jet_phi) ) < 0.1");

  TCut select_accept_jet("abs(jet_eta)<1.0 && jet_ptrans > 5");

  TCut select_tau("tracks_count_r04 == 3 && tracks_chargesum_r04 == -1 && tracks_rmax_r04 < 0.20 && jetshape_radius < 0.15");

  float n_trueaccept_uds = chain.GetEntries(select_true_uds && select_accept_jet);
  float n_trueaccept_tau = chain.GetEntries(select_true_tau && select_accept_jet);

  /* True positive = tau-identified-as-tau */
  float n_tau_as_tau = chain.GetEntries( select_true_tau && select_accept_jet &&  select_tau);
  float n_tau_as_uds = chain.GetEntries( select_true_tau && select_accept_jet && !select_tau);

  float n_uds_as_uds = chain.GetEntries( select_true_uds && select_accept_jet && !select_tau );
  float n_uds_as_tau = chain.GetEntries( select_true_uds && select_accept_jet &&  select_tau );

  cout << "Selection of TRUE TAU:   " << select_true_tau.GetTitle() << endl;
  cout << "Selection of TRUE QUARK: " << select_true_uds.GetTitle() << endl;
  cout << "Selection of JET acceptance:   " << select_accept_jet.GetTitle() << endl;
  cout << endl;
  cout << "Total sample TAU:   " << n_trueaccept_tau << endl;
  cout << "Total sample QUARK: " << n_trueaccept_uds << endl;
  cout << endl;
  cout << "Selection of RECO TAU:   " << select_tau.GetTitle() << endl;
  cout << endl;
  cout << "Tau-as-tau (True  Positive): " << n_tau_as_tau << endl;
  cout << "Tau-as-uds (False Negative): " << n_tau_as_uds << endl;
  cout << "Uds-as-uds (True  Negative): " << n_uds_as_uds << endl;
  cout << "Uds-as-tau (False Positive): " << n_uds_as_tau << endl;

  cout << endl;
  cout << "True  positive rate: " << n_tau_as_tau / ( n_tau_as_tau + n_tau_as_uds ) << endl;
  cout << "False positive rate: " << n_uds_as_tau / ( n_uds_as_tau + n_uds_as_uds ) << endl;
  cout << endl;
  cout << "Tau recovery efficiency: " << n_tau_as_tau / n_trueaccept_tau << endl;
  cout << endl;


  return 0;
}
