/**
 * Save ROOT::TTree as CSV file
 *
 * Written by nils.feege@stonybrook.edu
 */

int tree2csv()
{
  gStyle->SetOptStat(0);

  /* open inout files and merge trees */
  TChain chain("ntp_jet2");
  chain.Add("data_3pions/p250_e20_0events_file1093_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1096_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1101_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1115_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1122_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1127_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1131_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1164_LeptoAna_r05.root");

  /* select output file */
  TString csv_out("LeptoAna_r05_p250_e20.csv");

  /* particle selection */
  TCut select_jetcandidates("abs(jet_eta)<1.0 && jet_ptrans>5");

  /* select columns to keep */
  TString cols_keep("is_tau:is_uds:tracks_count_r1:tracks_count_r2:tracks_rmax_r2:jetshape_radius:jet_mass");

  ((TTreePlayer*)(chain.GetPlayer()))->SetScanRedirect(true);
  ((TTreePlayer*)(chain.GetPlayer()))->SetScanFileName( csv_out );
  chain.SetScanField(0);
  chain.Scan( cols_keep , select_jetcandidates );

  return 0;
}
