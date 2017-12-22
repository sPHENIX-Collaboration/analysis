int event_counts()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  /* open inout files and merge trees */
  TChain chain_event("ntp_event");
  chain_event.Add("data_3pions/p250_e20_0events_file1093_LeptoAna_TruthEvent.root");
  chain_event.Add("data_3pions/p250_e20_0events_file1096_LeptoAna_TruthEvent.root");
  chain_event.Add("data_3pions/p250_e20_0events_file1101_LeptoAna_TruthEvent.root");
  chain_event.Add("data_3pions/p250_e20_0events_file1115_LeptoAna_TruthEvent.root");
  chain_event.Add("data_3pions/p250_e20_0events_file1122_LeptoAna_TruthEvent.root");
  chain_event.Add("data_3pions/p250_e20_0events_file1127_LeptoAna_TruthEvent.root");
  chain_event.Add("data_3pions/p250_e20_0events_file1131_LeptoAna_TruthEvent.root");
  chain_event.Add("data_3pions/p250_e20_0events_file1164_LeptoAna_TruthEvent.root");

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
  cout << "TOTAL events: " << chain_event.GetEntries() << endl;
  cout << "  && tau within abs(eta)<1: " << chain_event.GetEntries("abs(tau_eta) < 1.0") << endl;

  cout << "TOTAL tau found: " << chain.GetEntries("evtgen_is_tau == 1") << endl;
  cout << "  && tau within abs(jet_eta)<1 && jet_pT > 5 GeV: " << chain.GetEntries("evtgen_is_tau == 1 && abs(jet_eta) < 1.0 && jet_ptrans > 5") << endl;
  cout << "  && Delte_R < 0.1: " << chain.GetEntries("evtgen_is_tau == 1 && abs(jet_eta) < 1.0 && jet_ptrans>5 && sqrt( (evtgen_tau_eta-jet_eta)*(evtgen_tau_eta-jet_eta) + (evtgen_tau_phi-jet_phi)*(evtgen_tau_phi-jet_phi) ) < 0.1") << endl;

  cout << endl;
  cout << endl;
  cout << endl;

  cout << "OTHER JETS found within acceptance: " << chain.GetEntries("evtgen_is_tau == 0 && abs(jet_eta) < 1.0 && jet_ptrans>5") << endl;



  /* Plot tau angle reconstruction quality */
  TCanvas *c1 = new TCanvas();
  chain.Draw("sqrt( (evtgen_tau_eta-jet_eta)*(evtgen_tau_eta-jet_eta) + (evtgen_tau_phi-jet_phi)*(evtgen_tau_phi-jet_phi) ) >> h1(50,0,0.5)", "evtgen_is_tau == 1 && abs(jet_eta) < 1.0 && jet_ptrans > 5" );
  h1->GetXaxis()->SetTitle("#DeltaR (#tau_{jet} - #tau_{true})");
  c1->Print("plots/event_counts_deltaR.eps");
  c1->Print("plots/event_counts_deltaR.png");


  return 0;
}
