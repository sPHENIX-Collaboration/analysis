int
plot_dis_acceptance()
{
  // TFile *fin = new TFile("output/eic_sphenix_dis_histo_1M.root","OPEN");
 TFile *fin = new TFile("output/x_q2_pythia_ep_18x275_10M.root","OPEN");

  THnSparse *hfull = (THnSparse*)fin->Get("hn_dis_electron");
  THnSparse *hfull_accept = (THnSparse*)fin->Get("hn_dis_electron_accept");

  TH2F* hxQ2 = (TH2F*)hfull->Projection(1,0);
  hxQ2->SetName("hxQ2");

  TH2F* hxQ2_accept = (TH2F*)hfull_accept->Projection(1,0);
  hxQ2_accept->SetName("hxQ2_accept");

  TH2F *hxQ2 = (TH2F*)fin->Get("hxQ2");
  TH2F *hxQ2_fullaccept = (TH2F*)fin->Get("hxQ2");

  /* beam energies */
  float ebeam_e = 20;
  float ebeam_p = 250;

  /* Prepare TPaveText for plots */
  TString str_ebeam = TString::Format("%.0f GeV x %.0f GeV", ebeam_e, ebeam_p );

  TPaveText *pt_ebeam_lumi_ul = new TPaveText(1e-5,1e3,1e-3,1e4,"none");
  pt_ebeam_lumi_ul->SetFillStyle(0);
  pt_ebeam_lumi_ul->SetLineColor(0);
  pt_ebeam_lumi_ul->AddText(str_ebeam);

  /* Prepare inelasticity (y) cut lines for plots */
  TF1 *f_y095 = new TF1("f_y095", "4*x*[0]*[1]*[2]", 1e-5, 1);
  f_y095->SetParameter( 0, ebeam_e);
  f_y095->SetParameter( 1, ebeam_p);
  f_y095->SetParameter( 2, 0.95);
  TF1 *f_y001 = (TF1*)f_y095->Clone("f_y01");
  f_y001->SetParameter(2 , 0.01);


  /* make x-Q2 acceptance fraction pot */
  TCanvas *c3 = new TCanvas();
  c3->SetRightMargin(0.12);
  TH2F* hxQ2_acceptance_ratio = hxQ2_accept->Clone("x_Q2_acceptance_ratio");
  hxQ2_acceptance_ratio->GetZaxis()->SetNdivisions(505);
  hxQ2_acceptance_ratio->Divide(hxQ2);
  hxQ2_acceptance_ratio->Draw("colz");
  c3->SetLogx(1);
  c3->SetLogy(1);

  f_y095->Draw("same");
  f_y001->Draw("same");

  pt_ebeam_lumi_ul->Draw();
  gPad->RedrawAxis();

  return 0;
}
