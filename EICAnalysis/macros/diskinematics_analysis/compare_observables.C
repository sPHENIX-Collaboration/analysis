#include "emid_commons.h"

/**
 * Compare reconstructed observables between tau jets and quark jets
 *
 * Written by nils.feege@stonybrook.edu
 */
int compare_observables()
{
  gStyle->SetOptStat(0);

  unsigned col_ele = kBlue+2;
  unsigned col_cpi = kOrange+7;

  /* open inout files and merge trees */
  TChain chain("event");
  chain.Add("eicana_disreco_nc_1k.root");

  /* Create temporary canvas */
  TCanvas* ctemp = new TCanvas();

  vector< TString > observables;
  vector< TString > observables_name;

  vector< float > plots_ymax;
  vector< float > plots_xmin;
  vector< float > plots_xmax;

  /* eta_true */
  observables.push_back( "em_evtgen_eta" );
  observables_name.push_back( "#eta_{true}" );
  plots_ymax.push_back(0.3);
  plots_xmin.push_back(-5.);
  plots_xmax.push_back(5.);

  /* em_cluster_eta */
  observables.push_back( "-1*log(tan(em_cluster_theta/2.0))" );
  observables_name.push_back( "#eta_{cluster}" );
  plots_ymax.push_back(0.3);
  plots_xmin.push_back(-5.);
  plots_xmax.push_back(5.);

  /* em_cluster_prob */
  observables.push_back( "em_cluster_prob" );
  observables_name.push_back( "em_cluster probablility" );
  plots_ymax.push_back(0.3);
  plots_xmin.push_back(0.0);
  plots_xmax.push_back(1.0);

  /*E/p*/
  observables.push_back( "em_cluster_e / em_track_ptotal" );
  observables_name.push_back( "E/p" );
  plots_ymax.push_back(0.3);
  plots_xmin.push_back(0.0);
  plots_xmax.push_back(100.0);

  /* Plot distributions */
  TString name_ele_base("h_ele_");
  TString name_cpi_base("h_cpi_");

  for ( unsigned i = 0; i < observables.size(); i++ )
    {
      cout << "Plotting " << observables.at(i) << endl;

      ctemp->cd();

      TString name_ele_i = name_ele_base;
      name_ele_i.Append(i);

      TString name_cpi_i = name_cpi_base;
      name_cpi_i.Append(i);

      TH1F* h_ele = new TH1F( name_ele_i, "", 100, plots_xmin.at(i), plots_xmax.at(i));
      TH1F* h_cpi = new TH1F( name_cpi_i, "", 100, plots_xmin.at(i), plots_xmax.at(i));

      chain.Draw( observables.at(i) + " >> " + name_ele_i, emid_commons::select_true_ele && emid_commons::select_match, "");
      h_ele->Scale(1./h_ele->Integral());
      h_ele->GetXaxis()->SetTitle( observables_name.at(i) );
      h_ele->GetYaxis()->SetRangeUser(0, plots_ymax.at(i) );
      h_ele->SetLineColor(col_ele);
      h_ele->SetFillColor(col_ele);
      h_ele->SetFillStyle(0);

      chain.Draw( observables.at(i) + " >> " + name_cpi_i, emid_commons::select_true_cpi && emid_commons::select_match, "");
      h_cpi->Scale(1./h_cpi->Integral());
      h_cpi->SetLineColor(col_cpi);
      h_cpi->SetFillColor(col_cpi);
      h_cpi->SetFillStyle(1001);

      /* create Canvas and draw histograms */
      TCanvas *c1 = new TCanvas();

      h_ele->DrawClone("");
      h_cpi->DrawClone("sames");
      h_ele->DrawClone("sames");

      gPad->RedrawAxis();

      c1->Print( Form( "plots/compare_observables_%d.eps", i ) );
      c1->Print( Form( "plots/compare_observables_%d.png", i ) );
    }

  return 0;
}
