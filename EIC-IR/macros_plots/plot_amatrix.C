#include "TraceBox.h"
#include <cmath>
int
plot_amatrix()
{
  /**** Chose input files ****/

  /* Open iput file with trajectories from GEANT4 */
  TFile *fin_0 =   new TFile( "data_amatrix/G4EICIR_DSTReader_p250_vx0_vy0_theta22mrad.root", "OPEN" );
  TFile *fin_a11 = new TFile( "data_amatrix/G4EICIR_DSTReader_p250_vx0_vy1_theta0mrad.root", "OPEN" );
  TFile *fin_a21 = new TFile( "data_amatrix/G4EICIR_DSTReader_p250_vx0_vy0_theta10mrad.root", "OPEN" );
  TFile *fin_a12 = new TFile( "data_amatrix/G4EICIR_DSTReader_p250_vx0_vy1_theta0mrad.root", "OPEN" );
  TFile *fin_a22 = new TFile( "data_amatrix/G4EICIR_DSTReader_p250_vx0_vy0_theta10mrad.root", "OPEN" );

  /* Get tree from file */
  TTree *tin_0 = (TTree*)fin_0->Get("T");
  TTree *tin_a11 = (TTree*)fin_a11->Get("T");
  TTree *tin_a21 = (TTree*)fin_a21->Get("T");
  TTree *tin_a12 = (TTree*)fin_a12->Get("T");
  TTree *tin_a22 = (TTree*)fin_a22->Get("T");

  /* define initial values for read-in trajectories */
  float tin_a11_vy = 1.0; // this is y_0
  float tin_a21_theta = 10.0; // this is theta_y*
  float tin_a12_vy = 1.0; // this is y_0
  float tin_a22_theta = 10.0; // theta_y*

  /* get number of hits */
  int nhits = 0;
  tin_a11->SetBranchAddress("n_G4HIT_FWDDISC",&nhits);
  tin_a11->GetEntry(0);
  cout << "hits: " << nhits << endl;

  /* create graph of nominal beam position (z) */
  tin_0->Draw( "0.5*(G4HIT_FWDDISC.y[][0]+G4HIT_FWDDISC.y[][1]) : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])","Entry$==0","");
  TGraph* g_0 = new TGraph(nhits, &(tin_0->GetV2()[0]), &(tin_0->GetV1()[0]));

  /* create graph of a11(z) */
  //   tin_a11->Draw(TString::Format( "0.5*(G4HIT_FWDDISC.y[][0]+G4HIT_FWDDISC.y[][1]) / %f : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])", tin_a11_vy ),"Entry$==0","");
  
  /* a11 = y/y_0 */
  tin_a11->Draw("0.5*(G4HIT_FWDDISC.y[][0]+G4HIT_FWDDISC.y[][1]) / 1 : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])","Entry$==0","" );
  TGraph* g_a11 = new TGraph(nhits, &(tin_a11->GetV2()[0]), &(tin_a11->GetV1()[0]));
  g_a11->SetMarkerStyle(7);
  g_a11->SetMarkerSize(1);
  g_a11->SetMarkerColor(kRed);

 /* create graph of a21(z) */
  //tin_a21->Draw(TString::Format( "0.5*(G4HIT_FWDDISC.y[][0]+G4HIT_FWDDISC.y[][1]) / %f : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])", tin_a21_theta ),"Entry$==0","");
  
/* a21 = y/theta* */
  tin_a21->Draw( "0.5*(G4HIT_FWDDISC.y[][0]+G4HIT_FWDDISC.y[][1]) / 10 : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])","Entry$==0","");
  TGraph* g_a21 = new TGraph(nhits, &(tin_a21->GetV2()[0]), &(tin_a21->GetV1()[0]));
  g_a21->SetMarkerStyle(7);
  g_a21->SetMarkerSize(1);
  g_a21->SetMarkerColor(kBlue);

  /* create graph of a12(z) */
  //  tin_a12->Draw(TString::Format( "0.5*(G4HIT_FWDDISC.y[][0]+G4HIT_FWDDISC.y[][1]) / %f : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])", tin_a12_vy ),"Entry$==0","");
  
  /* a12 = theta_y/y_0, theta*=atan( |exit-entry| / thickness ), here thickness = 1  */
  tin_a12->Draw("atan( abs(G4HIT_FWDDISC.y[][0]-G4HIT_FWDDISC.y[][1]) ) / 1 : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])","Entry$==0","");
  TGraph* g_a12 = new TGraph(nhits, &(tin_a12->GetV2()[0]), &(tin_a12->GetV1()[0]));
  g_a12->SetMarkerStyle(7);
  g_a12->SetMarkerSize(1);
  g_a12->SetMarkerColor(kGreen);

 /* create graph of a22(z) */
  //  tin_a22->Draw(TString::Format( "0.5*(G4HIT_FWDDISC.y[][0]+G4HIT_FWDDISC.y[][1]) / %f : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])", tin_a22_theta ),"Entry$==0","");
  
  /* a22 = thetay/thetay*, theta*=atan( |exit-entry| / thickness ), here thickness = 1 */
  tin_a22->Draw("atan( abs(G4HIT_FWDDISC.y[][0]-G4HIT_FWDDISC.y[][1]) ) / 10 : 0.5*(G4HIT_FWDDISC.z[][0]+G4HIT_FWDDISC.z[][1])","Entry$==0","");
  TGraph* g_a22 = new TGraph(nhits, &(tin_a22->GetV2()[0]), &(tin_a22->GetV1()[0]));
  g_a22->SetMarkerStyle(7);
  g_a22->SetMarkerSize(1);
  g_a22->SetMarkerColor(kBlue);
  g_a22->SetMarkerColor(kOrange);

  /* Create Legend */
  TLegend* leg = new TLegend(0.3,0.7,0.6,0.9);
  leg->AddEntry(g_0,"Nominal beam","l");
  leg->AddEntry(g_a11,"a11","p");
  leg->AddEntry(g_a21,"a21","p");
  leg->AddEntry(g_a12,"a12","p");
  leg->AddEntry(g_a22,"a22","p");

  /* Create frame histogram for plot */
  TH1F *h1 = new TH1F("h1","",10,0,15000);
  h1->GetXaxis()->SetRangeUser(0,12000);
  h1->GetYaxis()->SetRangeUser(-50,200);
  h1->GetXaxis()->SetTitle("Z(cm)");
  h1->GetYaxis()->SetTitle("a_{xx}");

  /* Plot frame histogram */
  TCanvas *c1 = new TCanvas();
  h1->Draw("AXIS");

  leg->Draw();

   g_0->Draw("Lsame");
   g_a11->Draw("Psame");
   g_a21->Draw("Psame");
   g_a12->Draw("Psame");
   g_a22->Draw("Psame");

  c1->Print("amatrix_new.eps");

  return 0;
}
