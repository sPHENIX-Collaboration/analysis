/**
 * Compare reconstructed observables between tau jets and quark jets
 *
 * Written by nils.feege@stonybrook.edu
 */

int compare_observables()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

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

  /* particle selection */
  TCut select_true_uds("is_tau==0");
  TCut select_true_tau("is_tau==1 && sqrt( (tau_eta-jet_eta)*(tau_eta-jet_eta) + (tau_phi-jet_phi)*(tau_phi-jet_phi) ) < 0.1");

  TCut select_accept_jet("abs(jet_eta)<1.0 && jet_ptrans > 5");

  //TCut select_prong("tracks_count_r1 == 3 && tracks_chargesum_r1 == -1");
  //TCut select_prong("tracks_count_r2 == 3");
  TCut select_prong("1");

  /* Create temporary canvas */
  TCanvas* ctemp = new TCanvas();

  vector< TString > observables;
  vector< TString > observables_name;

  vector< float > plots_ymax;
  vector< float > plots_xmin;
  vector< float > plots_xmax;

  /* R_max */
  observables.push_back( "tracks_rmax_r2" );
  observables_name.push_back( "R_{track}^{max}" );
  plots_ymax.push_back(0.06);
  plots_xmin.push_back(0);
  plots_xmax.push_back(0.5);

  /* Number of tracks */
  observables.push_back( "tracks_count_r2" );
  observables_name.push_back( "N_{tracks}" );
  plots_ymax.push_back(0.9);
  plots_xmin.push_back(0);
  plots_xmax.push_back(10);

  /* Charge sum from tracks */
  observables.push_back( "tracks_chargesum_r2" );
  observables_name.push_back( "#Sigma q_{tracks}" );
  plots_ymax.push_back(0.9);
  plots_xmin.push_back(-5);
  plots_xmax.push_back(5);

  /* Jet radius */
  observables.push_back( "jetshape_radius" );
  observables_name.push_back( "R_{jet}" );
  plots_ymax.push_back(0.08);
  plots_xmin.push_back(0);
  plots_xmax.push_back(0.5);

  /* Jetshape */
  observables.push_back( "jetshape_econe_r1 / (jetshape_econe_r1 + jetshape_econe_r2 + jetshape_econe_r3 + jetshape_econe_r4 + jetshape_econe_r5 + jetshape_econe_r6 + jetshape_econe_r7 + jetshape_econe_r8 + jetshape_econe_r9 + jetshape_econe_r10)" );
  observables_name.push_back( "E_{cone}^{R<0.1} / E_{cone}^{R<1.0}" );
  plots_ymax.push_back(0.08);
  plots_xmin.push_back(0);
  plots_xmax.push_back(1);

  /* Jetshape */
  observables.push_back( "(jetshape_econe_r1 + jetshape_econe_r2) / (jetshape_econe_r1 + jetshape_econe_r2 + jetshape_econe_r3 + jetshape_econe_r4 + jetshape_econe_r5)" );
  observables_name.push_back( "E_{cone}^{R<0.2} / E_{cone}^{R<0.5}" );
  plots_ymax.push_back(0.08);
  plots_xmin.push_back(0);
  plots_xmax.push_back(1);

  /* Jet eta */
  observables.push_back( "jet_eta" );
  observables_name.push_back( "#eta_{jet}" );
  plots_ymax.push_back(0.1);
  plots_xmin.push_back(-2);
  plots_xmax.push_back(2);

  /* Jet mass */
  observables.push_back( "jet_mass" );
  observables_name.push_back( "Mass_{jet}" );
  plots_ymax.push_back(0.1);
  plots_xmin.push_back(0);
  plots_xmax.push_back(10);

  /* Jet energy */
  observables.push_back( "jet_etotal" );
  observables_name.push_back( "E_{jet}" );
  plots_ymax.push_back(0.08);
  plots_xmin.push_back(0);
  plots_xmax.push_back(70);

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

      chain.Draw( observables.at(i) + " >> " + name_uds_i, select_true_uds && select_accept_jet, "");
      h_uds->Scale(1./h_uds->Integral());
      h_uds->GetXaxis()->SetTitle( observables_name.at(i) );
      h_uds->GetYaxis()->SetRangeUser(0, plots_ymax.at(i) );
      h_uds->SetLineColor(col1);
      h_uds->SetFillColor(col1);
      h_uds->SetFillStyle(1001);

      chain.Draw( observables.at(i) + " >> " + name_tau_i, select_true_tau && select_accept_jet, "");
      h_tau->Scale(1./h_tau->Integral());
      h_tau->SetLineColor(col2);
      h_tau->SetFillColor(col2);
      h_tau->SetFillStyle(0);

      /* create Canvas and draw histograms */
      TCanvas *c1 = new TCanvas();

      h_uds->DrawClone("");
      h_tau->DrawClone("sames");

      gPad->RedrawAxis();

      c1->Print( Form( "plots/compare_observables_%d.eps", i ) );
      c1->Print( Form( "plots/compare_observables_%d.png", i ) );
    }

  return 0;
}
