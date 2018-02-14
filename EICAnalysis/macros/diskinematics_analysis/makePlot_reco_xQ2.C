/* em_cluster_caloid values:
   1 = CEMC
   4 = EEMC
   5 = FEMC
*/

int
makePlot_reco_xQ2()
{
  gStyle->SetOptStat(0);

  //TFile *fin = new TFile("../../data/eicana_disreco_nc_20x250_1k.root", "OPEN");
  TFile *fin = new TFile("../../data/eicana_Pythia6_DIS_10x250_1M.root", "OPEN");

  //TTree *t_reco = (TTree*)fin->Get("event_cluster");
  TTree *t_reco = (TTree*)fin->Get("event_truth");

  /*------------------------------------------*/
  /*------------Create Histogram--------------*/
  /*------------------------------------------*/
  /**
   *\ The creation of this histogram includes a method for setting the bins to a logarithmic scale along with the axes
   **/
  /*------------------------------------------*/

  TH2F* hQ2_reco_truth = new TH2F("hQ2_reco_truth", ";Q^{2}_{truth} (GeV^{2});Q^{2}_{reco} (GeV^{2})", 60, 0, 3, 60, 0, 3);
  TAxis *xaxis = hQ2_reco_truth->GetXaxis();
  TAxis *yaxis = hQ2_reco_truth->GetYaxis();
  int xbins = xaxis->GetNbins();
  int ybins = yaxis->GetNbins();

  Axis_t ymin = yaxis->GetXmin();
  Axis_t ymax = yaxis->GetXmax();
  Axis_t ywidth = (ymax - ymin) / ybins;
  Axis_t *new_xbins = new Axis_t[xbins + 1];
  Axis_t *new_ybins = new Axis_t[ybins + 1];

  for( int i =0; i <= ybins; i++)
    {
      new_xbins[i] = TMath::Power( 10, ymin + i*ywidth);
      new_ybins[i] = TMath::Power( 10, ymin + i*ywidth);
    }
  xaxis->Set(xbins, new_xbins);
  yaxis->Set(ybins, new_ybins);


  /* Fill histogram */
  TCanvas *c1 = new TCanvas();
  c1->SetLogx(1);
  c1->SetLogy(1);
  c1->SetLogz(1);
  t_reco->Draw("em_reco_q2_e:evtgen_Q2 >> hQ2_reco_truth","em_evtgen_pid==11", "colz");

  return 1;
}
