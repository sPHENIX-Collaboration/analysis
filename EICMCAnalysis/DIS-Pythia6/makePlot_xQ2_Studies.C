/**

 *\Brief Macro to compare EIC theoretical kinematics to Pythia simulation data
 *\Author: Thomas Krahulik <thomas.krahulik@stonybrook.edu>

 **/


int makePlot_xQ2_Studies(
                         const TString infile = "data/pythia.ep.100.test.root"
                         )

{
  gStyle->SetOptStat(kFALSE);

  /*--------------Get Input File--------------*/

  TFile *f_pyth = new TFile( infile, "OPEN");
  TTree* T_pyth = (TTree*)f_pyth->Get("tree");
  T_pyth->Print();

  /*------------------------------------------*/
  /*---------------Define Cuts----------------*/

  TCut electron_cut = "p.fKF == 11";

  /*------------------------------------------*/
  /*---------x-Q2 Space for Electrons---------*/
  /*------------------------------------------*/

  /*------------------------------------------*/
  /*------------Create Histogram--------------*/
  /*------------------------------------------*/
  /**
   *\ The creation of this histogram includes a method for setting the bins to a logarithmic scale along with the axes
   **/
  /*------------------------------------------*/

  TH2F* hQ2x_e = new TH2F("hQ2x_e", "", 40, -4, 0, 60, 0, 3);
  TAxis *xaxis = hQ2x_e->GetXaxis();
  TAxis *yaxis = hQ2x_e->GetYaxis();
  int xbins = xaxis->GetNbins();
  int ybins = yaxis->GetNbins();

  Axis_t xmin = xaxis->GetXmin();
  Axis_t xmax = xaxis->GetXmax();
  Axis_t xwidth = (xmax - xmin ) / xbins;
  Axis_t *new_xbins = new Axis_t[xbins + 1];

  for( int i =0; i <= xbins; i++)
    {
      new_xbins[i] = TMath::Power( 10, xmin + i*xwidth);
    }
  xaxis->Set(xbins, new_xbins);

  Axis_t ymin = yaxis->GetXmin();
  Axis_t ymax = yaxis->GetXmax();
  Axis_t ywidth = (ymax - ymin) / ybins;
  Axis_t *new_ybins = new Axis_t[ybins + 1];

  for( int i2 =0; i2 <= ybins; i2++)
    {
      new_ybins[i2] = TMath::Power( 10, ymin + i2*ywidth);
    }

  yaxis->Set(ybins, new_ybins);
  /*------------------------------------------*/
  /*-------------Plot x-Q2 Space--------------*/
  /*------------------------------------------*/

  /*------------------------------------------*/
  /*----------Scattered Lepton Angle----------*/
  /*------------------------------------------*/

  TCanvas *cQ2x_e1 = new TCanvas("cQ2x_e1");
  cQ2x_e1->SetLogx();
  cQ2x_e1->SetLogy();
  T_pyth->Draw("Q2:x>>hQ2x_e", electron_cut , "colz");

  hQ2x_e->GetXaxis()->SetTitle("x");
  hQ2x_e->GetYaxis()->SetTitle("Q^{2} [GeV^{2}]");

  TF1 *f_LepA_0 = new TF1("f_LepA_0", "2*[0]*[1]*x*( 1 + cos([2])) / (1 + ((x*[1])/(2*[0]))*(1-cos([2])) - 0.5 * (1 - cos([2])))", 10e-5, 1);
  f_LepA_0->SetParameter( 0, 10);
  f_LepA_0->SetParameter( 1, 250);
  f_LepA_0->SetParameter( 2, (0 * TMath::Pi())/180 );
  TF1 *f_LepA_90 = (TF1*)f_LepA_0->Clone("f_LepA_90");
  f_LepA_90->SetParameter( 2, (90 * TMath::Pi())/180 );
  TF1 *f_LepA_150 = (TF1*)f_LepA_0->Clone("f_LepA_150");
  f_LepA_150->SetParameter( 2, (150 * TMath::Pi())/180 );
  TF1 *f_LepA_170 = (TF1*)f_LepA_0->Clone("f_LepA_170");
  f_LepA_170->SetParameter( 2, (170 * TMath::Pi())/180 );

  f_LepA_0->Draw("SAME");
  f_LepA_0->SetLineColor(1);
  f_LepA_90->Draw("SAME");
  f_LepA_90->SetLineColor(1);
  f_LepA_150->Draw("SAME");
  f_LepA_150->SetLineColor(1);
  f_LepA_170->Draw("SAME");
  f_LepA_170->SetLineColor(1);

  /*---------------Label Lines----------------*/

  TPaveText *pt_LepA_0 = new TPaveText(0.4, 1.2, 0.6, 1.4);
  pt_LepA_0->AddText("0^{/circ}");
  pt_LepA_0->Draw();

  /*------------------------------------------*/
  /*---------Scattered Lepton Energy----------*/
  /*------------------------------------------*/

  TCanvas *cQ2x_e2 = new TCanvas("cQ2x_e2");
  cQ2x_e2->SetLogx();
  cQ2x_e2->SetLogy();
  T_pyth->Draw("Q2:x>>hQ2x_e", electron_cut , "colz");

  hQ2x_e->GetXaxis()->SetTitle("x");
  hQ2x_e->GetYaxis()->SetTitle("Q^{2} [GeV^{2}]");

  TF1 *f_LepE_5 = new TF1("f_LepE_5", "2*[0]*[2]*( 1 + ((((x*[1]*([2]-[0]))/(x*[1]-[0])) - ([0] - (([0]*([2]-[0]))/(x*[1]-[0])))) / (((x*[1]*([2]-[0]))/(x*[1]-[0])) + ([0] - (([0]*([2]-[0]))/(x*[1]-[0]))))))", 10e-5, 1);
  f_LepE_5->SetParameter( 0, 10);
  f_LepE_5->SetParameter( 1, 250);
  f_LepE_5->SetParameter( 2, 5);
  TF1 *f_LepE_9 = (TF1*)f_LepE_5->Clone("f_LepE_9");
  f_LepE_9->SetParameter( 2, 9);
  TF1 *f_LepE_11 = (TF1*)f_LepE_5->Clone("f_LepE_11");
  f_LepE_11->SetParameter( 2, 11);
  TF1 *f_LepE_15 = (TF1*)f_LepE_5->Clone("f_LepE_15");
  f_LepE_15->SetParameter( 2, 15);

  f_LepE_5->Draw("SAME");
  f_LepE_5->SetLineColor(1);
  f_LepE_9->Draw("SAME");
  f_LepE_9->SetLineColor(1);
  f_LepE_11->Draw("SAME");
  f_LepE_11->SetLineColor(1);
  f_LepE_15->Draw("SAME");
  f_LepE_15->SetLineColor(1);

  /*------------------------------------------*/
  /*------------Current Jet Angle-------------*/
  /*------------------------------------------*/

  TCanvas *cQ2x_e3 = new TCanvas("cQ2x_e3");
  cQ2x_e3->SetLogx();
  cQ2x_e3->SetLogy();
  T_pyth->Draw("Q2:x>>hQ2x_e", electron_cut , "colz");

  hQ2x_e->GetXaxis()->SetTitle("x");
  hQ2x_e->GetYaxis()->SetTitle("Q^{2} [GeV^{2}]");

  TF1 *f_JetA_180 = new TF1("f_JetA_180" ,"(4 * (x**2)*([1]**2)*[0]*(1-cos([2]))) / (cos([2]) * ([0] -x*[1]) + [0] + x*[1])" , 10e-6, 1);
  f_JetA_180->SetParameter( 0 , 10);
  f_JetA_180->SetParameter( 1 , 250.);
  f_JetA_180->SetParameter( 2 , (180 * TMath::Pi())/180 );
  TF1 *f_JetA_150 = (TF1*)f_JetA_180->Clone("f_JetA_150");
  f_JetA_150->SetParameter( 2 , (150 * TMath::Pi())/180 );
  TF1 *f_JetA_30 = (TF1*)f_JetA_180->Clone("f_JetA_30");
  f_JetA_30->SetParameter( 2 , (30 * TMath::Pi())/180 );
  TF1 *f_JetA_10 = (TF1*)f_JetA_180->Clone("f_JetA_10");
  f_JetA_10->SetParameter( 2 , (10 * TMath::Pi())/180 );

  f_JetA_180->Draw("SAME");
  f_JetA_180->SetLineColor(1);
  f_JetA_150->Draw("SAME");
  f_JetA_150->SetLineColor(1);
  f_JetA_30->Draw("SAME");
  f_JetA_30->SetLineColor(1);
  f_JetA_10->Draw("SAME");
  f_JetA_10->SetLineColor(1);

  /*------------------------------------------*/
  /*------------Current Jet Energy------------*/
  /*------------------------------------------*/

  TCanvas *cQ2x_e4 = new TCanvas("cQ2x_e4");
  cQ2x_e4->SetLogx();
  cQ2x_e4->SetLogy();
  T_pyth->Draw("Q2:x>>hQ2x_e", electron_cut , "colz");

  hQ2x_e->GetXaxis()->SetTitle("x");
  hQ2x_e->GetYaxis()->SetTitle("Q^{2} [GeV^{2}]");

  TF1 *f_JetE_5 = new TF1("f_JetE_5" , "(4*x*[0]*[1]*([2]-x*[1])) / ([0] - x*[1])", 10e-6, 1);
  f_JetE_5->SetParameter( 0 , 10);
  f_JetE_5->SetParameter( 1 , 250.);
  f_JetE_5->SetParameter( 2 , 5. );
  TF1 *f_JetE_7 = (TF1*)f_JetE_5->Clone("f_JetE_7");
  f_JetE_7->SetParameter( 2 , 7);
  TF1 *f_JetE_10 = (TF1*)f_JetE_5->Clone("f_JetE_10");
  f_JetE_10->SetParameter( 2 , 10);
  TF1 *f_JetE_20 = (TF1*)f_JetE_5->Clone("f_JetE_20");
  f_JetE_20->SetParameter( 2 , 20);

  f_JetE_5->Draw("SAME");
  f_JetE_5->SetLineColor(1);
  f_JetE_7->Draw("SAME");
  f_JetE_7->SetLineColor(1);
  f_JetE_10->Draw("SAME");
  f_JetE_10->SetLineColor(1);
  f_JetE_20->Draw("SAME");
  f_JetE_20->SetLineColor(1);

  /*------------------------------------------*/
  /*---------------Inelasticity---------------*/
  /*------------------------------------------*/

  TCanvas *cQ2x_e5 = new TCanvas("cQ2x_e5");
  cQ2x_e5->SetLogx();
  cQ2x_e5->SetLogy();
  T_pyth->Draw("Q2:x>>hQ2x_e", electron_cut , "colz");

  hQ2x_e->GetXaxis()->SetTitle("x");
  hQ2x_e->GetYaxis()->SetTitle("Q^{2} [GeV^{2}]");

  TF1 *f_y1 = new TF1("f_y1", "4*x*[0]*[1]*[2]", 10e-5, 1);
  f_y1->SetParameter( 0, 10);
  f_y1->SetParameter( 1, 250);
  f_y1->SetParameter( 2, 1);
  TF1 *f_y01 = (TF1*)f_y1->Clone("f_y01");
  f_y01->SetParameter(2 , 0.1);
  TF1 *f_y001 = (TF1*)f_y1->Clone("f_y001");
  f_y001->SetParameter(2 , 0.01);
  TF1 *f_y0001 = (TF1*)f_y1->Clone("f_y0001");
  f_y0001->SetParameter(2 , 0.001);

  f_y1->Draw("SAME");
  f_y1->SetLineColor(1);
  f_y01->Draw("SAME");
  f_y01->SetLineColor(1);
  f_y001->Draw("SAME");
  f_y001->SetLineColor(1);
  f_y0001->Draw("SAME");
  f_y0001->SetLineColor(1);

  /*------------------------------------------*/
  /*-----------Final Hadronic Mass------------*/
  /*------------------------------------------*/

  TCanvas *cQ2x_e6 = new TCanvas("cQ2x_e6");
  cQ2x_e6->SetLogx();
  cQ2x_e6->SetLogy();
  T_pyth->Draw("Q2:x>>hQ2x_e", electron_cut , "colz");

  hQ2x_e->GetXaxis()->SetTitle("x");
  hQ2x_e->GetYaxis()->SetTitle("Q^{2} [GeV^{2}]");

  TF1 *f_W10 = new TF1("f_W10" , "([1] - [0]**2) / ((1/x) - 1)", 10e-7, 1);
  f_W10->SetParameter( 0 , 0.938);
  f_W10->SetParameter( 1 , (10)**2 );
  TF1 *f_W20 = (TF1*)f_W10->Clone("f_W20");
  f_W20->SetParameter( 1 , (20)**2 );
  TF1 *f_W50 = (TF1*)f_W10->Clone("f_W50");
  f_W50->SetParameter( 1 , (50)**2 );
  TF1 *f_W100 = (TF1*)f_W10->Clone("f_W100");
  f_W100->SetParameter( 1 , (100)**2 );
  TF1 *f_W3 = (TF1*)f_W10->Clone("f_W3");
  f_W3->SetParameter( 1 , (3)**2 );

  f_W10->Draw("SAME");
  f_W10->SetLineColor(1);
  f_W20->Draw("SAME");
  f_W20->SetLineColor(1);
  f_W50->Draw("SAME");
  f_W50->SetLineColor(1);
  f_W100->Draw("SAME");
  f_W100->SetLineColor(1);
  f_W3->Draw("SAME");
  f_W3->SetLineColor(1);

  //   cQ2x_e->Print("Plots/Pythia_Q2x_e_10M_250x010.eps");
  // cQ2x_e1->Print("Plots/Pythia_Q2x_LepA_e_10k_250x010.png");
  // cQ2x_e2->Print("Plots/Pythia_Q2x_LepE_e_10k_250x010.png");
  // cQ2x_e3->Print("Plots/Pythia_Q2x_JetA_e_10k_250x010.png");
  // cQ2x_e4->Print("Plots/Pythia_Q2x_JetE_e_10k_250x010.png");
  // cQ2x_e5->Print("Plots/Pythia_Q2x_y_e_10k_250x010.png");
  // cQ2x_e6->Print("Plots/Pythia_Q2x_W_e_10k_250x010.png");


  return 0;

}
