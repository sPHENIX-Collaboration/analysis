#include "tau_commons.h"

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


  string type = "DIScharged";
  string seed[7] = {"1","2","3","6","7","8","9","10"};
  string R_max = "5";
  /* open inout files and merge trees */
  TChain chain("event");

  for (int g = 0; g<seed->size();g++){
    if(g==4 || g==5) continue;
    string file = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_100events_"+seed[g]+"seed_"+type+"_tau_r0"+R_max+".root";
    chain.Add(file.c_str());
  }


  /* particle selection */
  //TCut select_prong("tracks_count_r02 == 3 && tracks_chargesum_r02 == -1");
  //TCut select_prong("tracks_count_r04 == 3");
  TCut select_prong("1");

  /* Create temporary canvas */
  TCanvas* ctemp = new TCanvas();

  vector< TString > observables;
  vector< TString > observables_name;

  vector< float > plots_ymax;
  vector< float > plots_xmin;
  vector< float > plots_xmax;

  /* R_max */
  observables.push_back( "tracks_rmax_r04" );
  observables_name.push_back( "R_{track}^{max}" );
  plots_ymax.push_back(0.06);
  plots_xmin.push_back(0);
  plots_xmax.push_back(0.5);

  /* Number of tracks */
  observables.push_back( "tracks_count_r04" );
  observables_name.push_back( "N_{tracks}" );
  plots_ymax.push_back(0.9);
  plots_xmin.push_back(0);
  plots_xmax.push_back(10);

  /* Charge sum from tracks */
  observables.push_back( "tracks_chargesum_r04" );
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
  observables.push_back( "jetshape_econe_r01 / jetshape_econe_r05" );
  observables_name.push_back( "E_{cone}^{R<0.1} / E_{cone}^{R<0.5}" );
  plots_ymax.push_back(0.08);
  plots_xmin.push_back(0);
  plots_xmax.push_back(1);

  /* Jetshape */
  observables.push_back( "jetshape_econe_r02 / jetshape_econe_r05" );
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
  observables.push_back( "jet_minv" );
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
  /*
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

      chain.Draw( observables.at(i) + " >> " + name_uds_i, tau_commons::select_true_uds && tau_commons::select_accept_jet, "");
      h_uds->Scale(1./h_uds->Integral());
      h_uds->GetXaxis()->SetTitle( observables_name.at(i) );
      h_uds->GetYaxis()->SetRangeUser(0, plots_ymax.at(i) );
      h_uds->SetLineColor(col1);
      h_uds->SetFillColor(col1);
      h_uds->SetFillStyle(1001);

      chain.Draw( observables.at(i) + " >> " + name_tau_i, tau_commons::select_true_tau && tau_commons::select_accept_jet, "");
      h_tau->Scale(1./h_tau->Integral());
      h_tau->SetLineColor(col2);
      h_tau->SetFillColor(col2);
      h_tau->SetFillStyle(0);

      // create Canvas and draw histograms 
      TCanvas *c1 = new TCanvas();

      h_uds->DrawClone("");
      h_tau->DrawClone("sames");

      TLegend *legend = new TLegend(0.3,0.7,0.48,0.9);
      legend->AddEntry(h_uds,"quark jet","l");
      legend->AddEntry(h_tau,"#tau jet","l");
      legend->Draw();

      gPad->RedrawAxis();

      //c1->Print( Form( "plots/compare_observables_%d.eps", i ) );
      //c1->Print( Form( "plots/compare_observables_%d.png", i ) );
    }
*/
  return 0;
}
