/**
 * Compare reconstructed observables between tau jets and quark jets
 *
 * Written by nils.feege@stonybrook.edu
 */

int compare_observables()
{
  gStyle->SetOptStat(1);

  /* open inout files and merge trees */
  TChain chain("ntp_jet2");
  chain.Add("data/p250_e20_1000events_file1_LeptoAna_r10.root");
  chain.Add("data/p250_e20_1000events_file2_LeptoAna_r10.root");
  chain.Add("data/p250_e20_1000events_file3_LeptoAna_r10.root");
  chain.Add("data/p250_e20_1000events_file4_LeptoAna_r10.root");
  chain.Add("data/p250_e20_1000events_file5_LeptoAna_r10.root");
  chain.Add("data/p250_e20_1000events_file6_LeptoAna_r10.root");
  chain.Add("data/p250_e20_1000events_file7_LeptoAna_r10.root");
  chain.Add("data/p250_e20_1000events_file8_LeptoAna_r10.root");
  chain.Add("data/p250_e20_1000events_file9_LeptoAna_r10.root");
  chain.Add("data/p250_e20_1000events_file10_LeptoAna_r10.root");

  /* particle selection */
  TCut select_true_uds("is_uds==1 && abs(uds_eta)<0.8");
  TCut select_true_tau("is_tau==1 && tau_decay_prong==3 && abs(tau_eta)<0.8");

  /* Create temporary canvas */
  TCanvas* ctemp = new TCanvas();

  vector< TString > observables;
  vector< TString > observables_name;

  vector< float > plots_ymax;
  vector< float > plots_xmin;
  vector< float > plots_xmax;

  /* R_max */
  observables.push_back( "tracks_rmax" );
  observables_name.push_back( "R_{track}^{max}" );
  plots_ymax.push_back(0.2);
  plots_xmin.push_back(0);
  plots_xmax.push_back(1);

  /* Number of tracks */
  observables.push_back( "tracks_count" );
  observables_name.push_back( "N_{tracks}" );
  plots_ymax.push_back(0.8);
  plots_xmin.push_back(0);
  plots_xmax.push_back(10);

  /* Charge sum from tracks */
  observables.push_back( "tracks_chargesum" );
  observables_name.push_back( "#Sigma q_{tracks}" );
  plots_ymax.push_back(0.8);
  plots_xmin.push_back(-5);
  plots_xmax.push_back(5);

  /* Jet radius */
  observables.push_back( "jetshape_radius" );
  observables_name.push_back( "R_{jet}" );
  plots_ymax.push_back(0.2);
  plots_xmin.push_back(0);
  plots_xmax.push_back(1);

  /* Jetshape */
  observables.push_back( "jetshape_econe_r1 / (jetshape_econe_r1 + jetshape_econe_r2 + jetshape_econe_r3)" );
  observables_name.push_back( "E_{cone}^{r1} / #Sigma E_{cone}^{ri}" );
  plots_ymax.push_back(0.1);
  plots_xmin.push_back(0);
  plots_xmax.push_back(1);

  /* Jetshape */
  observables.push_back( "jetshape_econe_r2 / (jetshape_econe_r1 + jetshape_econe_r2 + jetshape_econe_r3)" );
  observables_name.push_back( "E_{cone}^{r2} / #Sigma E_{cone}^{ri}" );
  plots_ymax.push_back(0.1);
  plots_xmin.push_back(0);
  plots_xmax.push_back(1);

  /* Jet mass */
  //observables.push_back( "jet_mass" );
  //observables_name.push_back( "Mass_{jet}" );
  //plots_ymax.push_back(0.06);
  //plots_xmin.push_back(0);
  //plots_xmax.push_back(10);

  /* Plot distributions */
  TString name_uds_base("h_uds_");
  TString name_tau_base("h_tau_");

  for ( unsigned i = 0; i < observables.size(); i++ )
    {
      cout << "Plotting " << observables.at(i) << endl;

      ctemp->cd();

      TString name_uds_i = name_uds_base;
      name_uds_i.Append(i);

      TString name_tau_i = name_tau_base;
      name_tau_i.Append(i);

      TH1F* h_uds = new TH1F( name_uds_i, "", 100, plots_xmin.at(i), plots_xmax.at(i));
      TH1F* h_tau = new TH1F( name_tau_i, "", 100, plots_xmin.at(i), plots_xmax.at(i));

      chain.Draw( observables.at(i) + " >> " + name_uds_i, select_true_uds, "");
      h_uds->Scale(1./h_uds->Integral());
      h_uds->GetXaxis()->SetTitle( observables_name.at(i) );
      h_uds->GetYaxis()->SetRangeUser(0, plots_ymax.at(i) );

      chain.Draw( observables.at(i) + " >> " + name_tau_i, select_true_tau, "");
      h_tau->Scale(1./h_tau->Integral());
      h_tau->SetLineColor(kRed);
      h_tau->SetFillColor(kRed);
      h_tau->SetFillStyle(1001);

      /* create Canvas and draw histograms */
      TCanvas *c1 = new TCanvas();

      h_uds->DrawClone("");
      h_tau->DrawClone("sames");
      h_uds->DrawClone("sames");

      gPad->RedrawAxis();
    }

  /* test manual cuts */
  TCut select_tau("tracks_chargesum == -1 && tracks_count == 3 && tracks_rmax < 0.25 && jetshape_radius < 0.2");

  float n_true_uds = chain.GetEntries(select_true_uds);
  float n_true_tau = chain.GetEntries(select_true_tau);

  float n_uds_as_tau = chain.GetEntries(select_true_uds && select_tau);
  float n_tau_as_tau = chain.GetEntries(select_true_tau && select_tau);

  cout << "CUT tests: " << endl;
  cout << "Selection of TRUE TAU:   " << select_true_tau.GetTitle() << endl;
  cout << "Selection of TRUE QUARK: " << select_true_uds.GetTitle() << endl;
  cout << endl;
  cout << "Total TAU:   " << n_true_tau << endl;
  cout << "Total QUARK: " << n_true_uds << endl;
  cout << endl;
  cout << "Selection of RECO TAU:   " << select_tau.GetTitle() << endl;
  cout << endl;
  cout << "True  positive rate: " << n_tau_as_tau / n_true_tau << endl;
  cout << "False positive rate: " << n_uds_as_tau / n_true_uds << endl;

  return 0;
}
