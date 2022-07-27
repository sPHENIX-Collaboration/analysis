#include "TraceBox.h"
#include <cmath>

TGraph*
graph_delta( TGraph* g1, TGraph* g2 )
{
  /* make sure both graph have smae number of entries- return NULL pointer if they don't */
  if ( g1->GetN() != g2->GetN() )
    {
      cerr << "Mismatch in number of points for graphs in calculating delta!" << endl;
      return NULL;
    }

  TGraph* gdelta = new TGraph( g1->GetN() );// (TGraph*)g1->Clone("gdelta");

  for ( unsigned i = 0; i < g1->GetN(); i++ )
    {
      Double_t x1 = 0;
      Double_t y1 = 0;
      Double_t x2 = 0;
      Double_t y2 = 0;

      g1->GetPoint( i , x1 , y1 );
      g2->GetPoint( i , x2 , y2 );

      Double_t xdelta = x1;
      Double_t ydelta = y1-y2;

      gdelta->SetPoint(i, xdelta, ydelta);
    }

  return gdelta;
}

int
plot_track_deviations()
{
  /**** Chose input files ****/

  /* Open iput file with trajectories from GEANT4 */
  TFile *file_nominal = new TFile( "data_amatrix/G4EICIR_DSTReader_p250_vx0_vy0_theta22mrad.root", "OPEN" );
  TFile *file_nominal_upper = new TFile( "data_amatrix/G4EICIR_DSTReader_p250_vx0_vy1_theta0mrad.root", "OPEN" ); // +200 MeV pT
  TFile *file_nominal_lower = new TFile( "data_amatrix/G4EICIR_DSTReader_p250_vx0_vy0_theta10mrad.root", "OPEN" ); // -200 MeV pT

  TFile *file_trial = new TFile( "data_amatrix/G4EICIR_DSTReader_p250_vx0_vy0_theta10mrad.root", "OPEN" );

  /* Get tree from file */
  TTree *tree_nominal = (TTree*)file_nominal->Get("T");
  TTree *tree_nominal_upper = (TTree*)file_nominal_upper->Get("T");
  TTree *tree_nominal_lower = (TTree*)file_nominal_lower->Get("T");

  TTree *tree_trial = (TTree*)file_trial->Get("T");

  /* get number of hits */
  int nhits = 0;
  tree_nominal->SetBranchAddress("n_G4HIT_FWDDISC",&nhits);
  tree_nominal->GetEntry(0);
  cout << "hits: " << nhits << endl;

  /* create graph of nominal beam position (z) */
  tree_nominal->Draw( "0.5*(G4HIT_FWDDISC.y[][0]+G4HIT_FWDDISC.y[][1]) : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])","Entry$==0","");
  TGraph* g_nominal = new TGraph(nhits, &(tree_nominal->GetV2()[0]), &(tree_nominal->GetV1()[0]));
  g_nominal->SetName("g_nominal");

  /* create graph of beam positions relative to nominal */
  TGraph* g_nominal_delta = graph_delta(g_nominal, g_nominal);
  if ( ! g_nominal_delta )
    return 1;
  g_nominal_delta->SetName("g_nominal_delta");
  g_nominal_delta->SetLineWidth(2);

  /* create graph of nominal_upper beam position (z) */
  tree_nominal_upper->Draw( "0.5*(G4HIT_FWDDISC.y[][0]+G4HIT_FWDDISC.y[][1]) : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])","Entry$==0","");
  TGraph* g_nominal_upper = new TGraph(nhits, &(tree_nominal_upper->GetV2()[0]), &(tree_nominal_upper->GetV1()[0]));
  g_nominal_upper->SetName("g_nominal_upper");

  /* create graph of beam positions relative to nominal_upper */
  TGraph* g_nominal_upper_delta = graph_delta(g_nominal_upper, g_nominal);
  if ( ! g_nominal_upper_delta )
    return 1;
  g_nominal_upper_delta->SetName("g_nominal_upper_delta");
  g_nominal_upper_delta->SetLineWidth(2);
  g_nominal_upper_delta->SetLineColor(kBlue);

  /* create graph of nominal_lower beam position (z) */
  tree_nominal_lower->Draw( "0.5*(G4HIT_FWDDISC.y[][0]+G4HIT_FWDDISC.y[][1]) : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])","Entry$==0","");
  TGraph* g_nominal_lower = new TGraph(nhits, &(tree_nominal_lower->GetV2()[0]), &(tree_nominal_lower->GetV1()[0]));
  g_nominal_lower->SetName("g_nominal_lower");

  /* create graph of beam positions relative to nominal_lower */
  TGraph* g_nominal_lower_delta = graph_delta(g_nominal_lower, g_nominal);
  if ( ! g_nominal_lower_delta )
    return 1;
  g_nominal_lower_delta->SetName("g_nominal_lower_delta");
  g_nominal_lower_delta->SetLineWidth(2);
  g_nominal_lower_delta->SetLineColor(kRed);

  /* Create Legend */
//  TLegend* leg = new TLegend(0.3,0.7,0.6,0.9);
//  leg->AddEntry(g_0,"Nominal beam","l");
//  leg->AddEntry(g_a11,"a11","p");
//  leg->AddEntry(g_a21,"a21","p");
//  leg->AddEntry(g_a12,"a12","p");
//  leg->AddEntry(g_a22,"a22","p");

  /* Create frame histogram for plot */
  TH1F *h1 = new TH1F("h1","",10,0,15000);
  h1->GetXaxis()->SetRangeUser(0,12000);
  h1->GetYaxis()->SetRangeUser(-10,10);
  h1->GetXaxis()->SetTitle("z (cm)");
  h1->GetYaxis()->SetTitle("#Delta_{x} (mm)");

  /* Plot frame histogram */
  TCanvas *c1 = new TCanvas();
  h1->Draw("AXIS");

  //leg->Draw();

  g_nominal_delta->Draw("Lsame");
  g_nominal_upper_delta->Draw("Lsame");
  g_nominal_lower_delta->Draw("Lsame");

  c1->Print("track_deviations_new.eps");

  return 0;
}
