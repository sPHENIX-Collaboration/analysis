#include "../CommonTools.h"
#include <sPhenixStyle.C>


void VertexingQA(std::string reffile, std::string newfile, std::string outfile)
{
  SetsPhenixStyle();
  TFile *qa_file_new = TFile::Open(newfile.c_str());
  TFile *qa_file_ref = TFile::Open(reffile.c_str());

  TFile *outfilef = new TFile(outfile.c_str(), "recreate");


  {
    const char *hist_name_prefix = "QAG4SimulationTracking";
    TString prefix = TString("h_") + hist_name_prefix + TString("_");

    
  // obtain normalization
  double Nevent_new = 1;
  double Nevent_ref = 1;
    
  TH2 *h_new = (TH2 *) qa_file_new->GetObjectChecked(
      prefix + TString("DCArPhi_pT"), "TH2");
  assert(h_new);

  //  h_new->Rebin(1, 2);
  //h_new->Sumw2();
  //  h_new->Scale(1. / Nevent_new);

  TH2 *h_ref = NULL;
  if (qa_file_ref)
  {
    h_ref = (TH2 *) qa_file_ref->GetObjectChecked(
        prefix + TString("DCArPhi_pT"), "TH2");
    assert(h_ref);

    //    h_ref->Rebin(1, 2);
    //h_ref->Sumw2();
    h_ref->Scale(Nevent_new / Nevent_ref);
  }

  TCanvas *c1 = new TCanvas(TString("QA_Draw_Tracking_DCArPhi") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Tracking_DCArPhi") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c1->Divide(4, 2);
  int idx = 1;
  TPad *p;

  vector<pair<double, double>> gpt_ranges{
      {0, 0.5},
      {0.5, 1},
      {1, 1.5},
      {1.5, 2},
      {2, 4},
      {4, 16},
      {16, 40}};

  TF1 *f1 = nullptr;
  TF1 *fit = nullptr;
  Double_t sigma = 0;
  Double_t sigma_unc = 0;
  char resstr[500];
  TLatex *res = nullptr;
  for (auto pt_range : gpt_ranges)
  {
    //cout << __PRETTY_FUNCTION__ << " process " << pt_range.first << " - " << pt_range.second << " GeV/c";

    p = (TPad *) c1->cd(idx++);
    c1->Update();
    p->SetLogy();

    const double epsilon = 1e-6;
    const int bin_start = h_new->GetXaxis()->FindBin(pt_range.first + epsilon);
    const int bin_end = h_new->GetXaxis()->FindBin(pt_range.second - epsilon);

    TH1 *h_proj_new = h_new->ProjectionY(
        TString::Format(
            "%s_New_ProjX_%d_%d",
            h_new->GetName(), bin_start, bin_end),
        bin_start, bin_end);
    if (pt_range.first < 2.0)
    {
      h_proj_new->GetXaxis()->SetRangeUser(-.05,.05);
      h_proj_new->Rebin(5);
    }
    else
    {
      h_proj_new->GetXaxis()->SetRangeUser(-.01,.01);
    }
    h_proj_new->SetTitle(TString(hist_name_prefix) + TString::Format(
                                                         ": %.1f - %.1f GeV/c", pt_range.first, pt_range.second));
    h_proj_new->GetXaxis()->SetTitle(TString::Format(
        "DCA (r #phi) [cm]"));
    h_proj_new->GetXaxis()->SetNdivisions(5,5);
    f1 = new TF1("f1","gaus",-.01,.01);
    h_proj_new->Fit(f1,"qm");
    sigma = f1->GetParameter(2);
    sigma_unc = f1->GetParError(2);

    TH1 *h_proj_ref = nullptr;
    if (h_ref)
    {
      h_proj_ref =
          h_ref->ProjectionY(
              TString::Format(
                  "%s_Ref_ProjX_%d_%d",
                  h_new->GetName(), bin_start, bin_end),
              bin_start, bin_end);
      if (pt_range.first < 2.0)
      {
	//h_proj_ref->GetXaxis()->SetRangeUser(-.05,.05);
	h_proj_ref->Rebin(5);
      }
    }
    
    DrawReference(h_proj_new, h_proj_ref);

    sprintf(resstr,"#sigma = %.5f #pm %.5f cm", sigma, sigma_unc);
    res = new TLatex(0.325,0.825,resstr);
    res->SetNDC();
    res->SetTextSize(0.05);
    res->SetTextAlign(13);
    res->Draw();
  }
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  TPaveText *pt = new TPaveText(.05,.1,.95,.8);
  pt->AddText("No cuts");
  pt->Draw();

  //SaveCanvas(c1, TString(qa_file_name_new) + TString("_") + TString(c1->GetName()), true);



    c1->Draw();
    outfilef->cd();
    c1->Write();
}
{
    const char *hist_name_prefix = "QAG4SimulationTracking";
    TString prefix = TString("h_") + hist_name_prefix + TString("_");

    
  // obtain normalization
  double Nevent_new = 1;
  double Nevent_ref = 1;
    
  TH2 *h_new2 = (TH2 *) qa_file_new->GetObjectChecked(
      prefix + TString("DCArPhi_pT_cuts"), "TH2");
  assert(h_new2);

  //  h_new->Rebin(1, 2);
  //h_new2->Sumw2();
  //  h_new->Scale(1. / Nevent_new);

  TH2 *h_ref2 = NULL;
  if (qa_file_ref)
  {
    h_ref2 = (TH2 *) qa_file_ref->GetObjectChecked(
        prefix + TString("DCArPhi_pT_cuts"), "TH2");
    assert(h_ref2);

    //    h_ref->Rebin(1, 2);
    //h_ref2->Sumw2();
    h_ref2->Scale(Nevent_new / Nevent_ref);
  }

  TCanvas *c2 = new TCanvas(TString("QA_Draw_Tracking_DCArPhi2") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Tracking_DCArPhi2") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c2->Divide(4, 2);
  int idx2 = 1;
  TPad *p2;

  vector<pair<double, double>> gpt_ranges2{
      {0, 0.5},
      {0.5, 1},
      {1, 1.5},
      {1.5, 2},
      {2, 4},
      {4, 16},
      {16, 40}};
  TF1 *f2 = nullptr;
  TF1 *fit2 = nullptr;
  Double_t sigma2 = 0;
  Double_t sigma_unc2 = 0;
  char resstr2[500];
  TLatex *res2 = nullptr;
  for (auto pt_range : gpt_ranges2)
  {
    //cout << __PRETTY_FUNCTION__ << " process " << pt_range.first << " - " << pt_range.second << " GeV/c";

    p2 = (TPad *) c2->cd(idx2++);
    c2->Update();
    p2->SetLogy();

    const double epsilon = 1e-6;
    const int bin_start = h_new2->GetXaxis()->FindBin(pt_range.first + epsilon);
    const int bin_end = h_new2->GetXaxis()->FindBin(pt_range.second - epsilon);

    TH1 *h_proj_new2 = h_new2->ProjectionY(
        TString::Format(
            "%s_New_ProjX_%d_%d",
            h_new2->GetName(), bin_start, bin_end),
        bin_start, bin_end);
    if (pt_range.first < 2.0)
    {
      h_proj_new2->GetXaxis()->SetRangeUser(-.05,.05);
      h_proj_new2->Rebin(5);
    }
    else
    {
      h_proj_new2->GetXaxis()->SetRangeUser(-.01,.01);
    }
    h_proj_new2->SetTitle(TString(hist_name_prefix) + TString::Format(
                                                         ": %.1f - %.1f GeV/c", pt_range.first, pt_range.second));
    h_proj_new2->GetXaxis()->SetTitle(TString::Format(
        "DCA (r #phi) [cm]"));
    h_proj_new2->GetXaxis()->SetNdivisions(5,5);
    f2 = new TF1("f2","gaus",-.01,.01);
    h_proj_new2->Fit(f2 , "mq");
    fit2 = h_proj_new2->GetFunction("f2");
    sigma2 = fit2->GetParameter(2);
    sigma_unc2 = fit2->GetParError(2);

    TH1 *h_proj_ref2 = nullptr;
    if (h_ref2)
    {
      h_proj_ref2 =
          h_ref2->ProjectionY(
              TString::Format(
                  "%s_Ref_ProjX_%d_%d",
                  h_new2->GetName(), bin_start, bin_end),
              bin_start, bin_end);
      if (pt_range.first < 2.0)
      {
	//h_proj_ref->GetXaxis()->SetRangeUser(-.05,.05);
	h_proj_ref2->Rebin(5);
      }
    }
    DrawReference(h_proj_new2, h_proj_ref2);

    sprintf(resstr2,"#sigma = %.5f #pm %.5f cm", sigma2, sigma_unc2);
    res2 = new TLatex(0.325,0.825,resstr2);
    res2->SetNDC();
    res2->SetTextSize(0.05);
    res2->SetTextAlign(13);
    res2->Draw();
  }
    
    
  p2 = (TPad *) c2->cd(idx2++);
  c2->Update();
  TPaveText *pt2 = new TPaveText(.05,.1,.95,.8);
  pt2->AddText("Cuts: MVTX hits>=2, INTT hits>=1,");
  pt2->AddText("TPC hits>=20");
  pt2->Draw();

  //SaveCanvas(c2, TString(qa_file_name_new) + TString("_") + TString(c2->GetName()), true);

    c2->Draw();
    outfilef->cd();
    c2->Write();
 }

{
    const char *hist_name_prefix = "QAG4SimulationTracking";
      TString prefix = TString("h_") + hist_name_prefix + TString("_");

    
  // obtain normalization
  double Nevent_new = 1;
  double Nevent_ref = 1;

  if (qa_file_new)
  {
    TH1 *h_norm = (TH1 *) qa_file_new->GetObjectChecked(
        prefix + TString("Normalization"), "TH1");
    assert(h_norm);

    Nevent_new = h_norm->GetBinContent(h_norm->GetXaxis()->FindBin("Truth Track"));
  }
  if (qa_file_ref)
  {
    TH1 *h_norm = (TH1 *) qa_file_ref->GetObjectChecked(
        prefix + TString("Normalization"), "TH1");
    assert(h_norm);

    Nevent_ref = h_norm->GetBinContent(h_norm->GetXaxis()->FindBin("Truth Track"));
  }

  TH2 *h_new = (TH2 *) qa_file_new->GetObjectChecked(
      prefix + TString("DCAZ_pT"), "TH2");
  assert(h_new);

  //  h_new->Rebin(1, 2);
  //h_new->Sumw2();
  //  h_new->Scale(1. / Nevent_new);

  TH2 *h_ref = NULL;
  if (qa_file_ref)
  {
    h_ref = (TH2 *) qa_file_ref->GetObjectChecked(
        prefix + TString("DCAZ_pT"), "TH2");
    assert(h_ref);

    //    h_ref->Rebin(1, 2);
   // h_ref->Sumw2();
    h_ref->Scale(Nevent_new / Nevent_ref);
  }

  TCanvas *c1 = new TCanvas(TString("QA_Draw_Tracking_DCAZ") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Tracking_DCAZ") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c1->Divide(4, 2);
  int idx = 1;
  TPad *p;

  vector<pair<double, double>> gpt_ranges{
      {0, 0.5},
      {0.5, 1},
      {1, 1.5},
      {1.5, 2},
      {2, 4},
      {4, 16},
      {16, 40}};
  TF1 *f1 = nullptr;
  TF1 *fit = nullptr;
  Double_t sigma = 0;
  Double_t sigma_unc = 0;
  char resstr[500];
  TLatex *res = nullptr;
  for (auto pt_range : gpt_ranges)
  {
    //cout << __PRETTY_FUNCTION__ << " process " << pt_range.first << " - " << pt_range.second << " GeV/c";

    p = (TPad *) c1->cd(idx++);
    c1->Update();
    p->SetLogy();

    const double epsilon = 1e-6;
    const int bin_start = h_new->GetXaxis()->FindBin(pt_range.first + epsilon);
    const int bin_end = h_new->GetXaxis()->FindBin(pt_range.second - epsilon);

    TH1 *h_proj_new = h_new->ProjectionY(
        TString::Format(
            "%s_New_ProjX_%d_%d",
            h_new->GetName(), bin_start, bin_end),
        bin_start, bin_end);
    if (pt_range.first < 2.0)
    {
      h_proj_new->GetXaxis()->SetRangeUser(-.05, .05);
      h_proj_new->Rebin(5);
    }
    else
    {
      h_proj_new->GetXaxis()->SetRangeUser(-.01, .01);
    }
    h_proj_new->SetTitle(TString(hist_name_prefix) + TString::Format(
                                                         ": %.1f - %.1f GeV/c", pt_range.first, pt_range.second));
    h_proj_new->GetXaxis()->SetTitle(TString::Format(
        "DCA (Z) [cm]"));
    h_proj_new->GetXaxis()->SetNdivisions(5, 5);

    f1 = new TF1("f1", "gaus", -.01, .01);
    h_proj_new->Fit(f1,"mq");
    sigma = f1->GetParameter(2);
    sigma_unc = f1->GetParError(2);

    TH1 *h_proj_ref = nullptr;
    if (h_ref)
    {
      h_proj_ref =
          h_ref->ProjectionY(
              TString::Format(
                  "%s_Ref_ProjX_%d_%d",
                  h_new->GetName(), bin_start, bin_end),
              bin_start, bin_end);
      if (pt_range.first < 2.0)
      {
        //h_proj_ref->GetXaxis()->SetRangeUser(-.05,.05);
        h_proj_ref->Rebin(5);
      }
    }
    DrawReference(h_proj_new, h_proj_ref);
    sprintf(resstr, "#sigma = %.5f #pm %.5f cm", sigma, sigma_unc);
    res = new TLatex(0.325, 0.825, resstr);
    res->SetNDC();
    res->SetTextSize(0.05);
    res->SetTextAlign(13);
    res->Draw();
  }
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  TPaveText *pt = new TPaveText(.05, .1, .95, .8);
  pt->AddText("No cuts");
  pt->Draw();

//  SaveCanvas(c1, TString(qa_file_name_new) + TString("_") + TString(c1->GetName()), true);
    
    c1->Draw();
    outfilef->cd();
    c1->Write();
 }

{
    
    const char *hist_name_prefix = "QAG4SimulationTracking";
      TString prefix = TString("h_") + hist_name_prefix + TString("_");

    
  // obtain normalization
  double Nevent_new = 1;
  double Nevent_ref = 1;
    
  // cuts plots
  TH2 *h_new2 = (TH2 *) qa_file_new->GetObjectChecked(
      prefix + TString("DCAZ_pT_cuts"), "TH2");
  assert(h_new2);

  //  h_new->Rebin(1, 2);
  //h_new2->Sumw2();
  //  h_new->Scale(1. / Nevent_new);

  TH2 *h_ref2 = NULL;
  if (qa_file_ref)
  {
    h_ref2 = (TH2 *) qa_file_ref->GetObjectChecked(
        prefix + TString("DCAZ_pT_cuts"), "TH2");
    assert(h_ref2);

    //    h_ref->Rebin(1, 2);
    //h_ref2->Sumw2();
    h_ref2->Scale(Nevent_new / Nevent_ref);
  }

  TCanvas *c2 = new TCanvas(TString("QA_Draw_Tracking_DCAZ2") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Tracking_DCAZ2") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c2->Divide(4, 2);
  int idx2 = 1;
  TPad *p2;

  vector<pair<double, double>> gpt_ranges2{
      {0, 0.5},
      {0.5, 1},
      {1, 1.5},
      {1.5, 2},
      {2, 4},
      {4, 16},
      {16, 40}};
  TF1 *f2 = nullptr;
  TF1 *fit2 = nullptr;
  Double_t sigma2 = 0;
  Double_t sigma_unc2 = 0;
  char resstr2[500];
  TLatex *res2 = nullptr;
  for (auto pt_range : gpt_ranges2)
  {
    //cout << __PRETTY_FUNCTION__ << " process " << pt_range.first << " - " << pt_range.second << " GeV/c";

    p2 = (TPad *) c2->cd(idx2++);
    c2->Update();
    p2->SetLogy();

    const double epsilon = 1e-6;
    const int bin_start = h_new2->GetXaxis()->FindBin(pt_range.first + epsilon);
    const int bin_end = h_new2->GetXaxis()->FindBin(pt_range.second - epsilon);

    TH1 *h_proj_new2 = h_new2->ProjectionY(
        TString::Format(
            "%s_New_ProjX_%d_%d",
            h_new2->GetName(), bin_start, bin_end),
        bin_start, bin_end);
    if (pt_range.first < 2.0)
    {
      h_proj_new2->GetXaxis()->SetRangeUser(-.05, .05);
      h_proj_new2->Rebin(5);
    }
    else
    {
      h_proj_new2->GetXaxis()->SetRangeUser(-.01, .01);
    }
    h_proj_new2->SetTitle(TString(hist_name_prefix) + TString::Format(
                                                          ": %.1f - %.1f GeV/c", pt_range.first, pt_range.second));
    h_proj_new2->GetXaxis()->SetTitle(TString::Format(
        "DCA (Z) [cm]"));
    h_proj_new2->GetXaxis()->SetNdivisions(5, 5);

    f2 = new TF1("f2", "gaus", -.01, .01);
    h_proj_new2->Fit(f2,"mq");
    fit2 = h_proj_new2->GetFunction("f2");
    sigma2 = fit2->GetParameter(2);
    sigma_unc2 = fit2->GetParError(2);

    TH1 *h_proj_ref2 = nullptr;
    if (h_ref2)
    {
      h_proj_ref2 =
          h_ref2->ProjectionY(
              TString::Format(
                  "%s_Ref_ProjX_%d_%d",
                  h_new2->GetName(), bin_start, bin_end),
              bin_start, bin_end);
      if (pt_range.first < 2.0)
      {
        //h_proj_ref->GetXaxis()->SetRangeUser(-.05,.05);
        h_proj_ref2->Rebin(5);
      }
    }
    DrawReference(h_proj_new2, h_proj_ref2);
    sprintf(resstr2, "#sigma = %.5f #pm %.5f cm", sigma2, sigma_unc2);
    res2 = new TLatex(0.325, 0.825, resstr2);
    res2->SetNDC();
    res2->SetTextSize(0.05);
    res2->SetTextAlign(13);
    res2->Draw();
  }
  p2 = (TPad *) c2->cd(idx2++);
  c2->Update();
  TPaveText *pt2 = new TPaveText(.05, .1, .95, .8);
  pt2->AddText("Cuts: MVTX hits>=2, INTT hits>=1,");
  pt2->AddText("TPC hits>=20");
  pt2->Draw();

//  SaveCanvas(c2, TString(qa_file_name_new) + TString("_") + TString(c2->GetName()), true);
    c2->Draw();
    outfilef->cd();
    c2->Write();
 }

{
    const char *hist_name_prefix = "QAG4SimulationTracking";
    TString prefix = TString("h_") + hist_name_prefix + TString("_");
    
    
  // obtain normalization
  double Nevent_new = 1;
  double Nevent_ref = 1;

  if (qa_file_new)
  {

    TH1 *h_norm = (TH1 *) qa_file_new->GetObjectChecked(
        prefix + TString("Normalization"), "TH1");
    assert(h_norm);

    Nevent_new = h_norm->GetBinContent(h_norm->GetXaxis()->FindBin("Event"));
  }
  if (qa_file_ref)
  {
    TH1 *h_norm = (TH1 *) qa_file_ref->GetObjectChecked(
        prefix + TString("Normalization"), "TH1");
    assert(h_norm);

    Nevent_ref = h_norm->GetBinContent(h_norm->GetXaxis()->FindBin("Event"));
  }

  TCanvas *c1 = new TCanvas(TString("QA_Draw_Tracking_DCA_Resolution") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Tracking_DCA_Resolution") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  {
    p = (TPad *) c1->cd(idx++);
    c1->Update();
    p->SetLogx();
    TH1 *frame = p->DrawFrame(0.1, -0.01, 50, 0.01,
                              ";Truth p_{T} [GeV/c];<DCA (r #phi)> #pm #sigma(DCA (r #phi)) [cm]");
    gPad->SetLeftMargin(.2);
    frame->GetYaxis()->SetTitleOffset(2);
    TLine *l = new TLine(0.1, 0, 50, 0);
    l->SetLineColor(kGray);
    l->Draw();

    TH2 *h_QAG4SimulationTracking_DCArPhi = (TH2 *) qa_file_new->GetObjectChecked(
        prefix + "DCArPhi_pT_cuts", "TH2");
    assert(h_QAG4SimulationTracking_DCArPhi);

    h_QAG4SimulationTracking_DCArPhi->Rebin2D(20, 1);

    // h_QAG4SimulationTracking_DCArPhi->Draw("colz");
    TGraphErrors *ge_QAG4SimulationTracking_DCArPhi = FitProfile(h_QAG4SimulationTracking_DCArPhi);
    ge_QAG4SimulationTracking_DCArPhi->Draw("pe");

    TGraphErrors *h_ratio_ref = NULL;
    if (qa_file_ref)
    {
      TH2 *h_QAG4SimulationTracking_DCArPhi = (TH2 *) qa_file_ref->GetObjectChecked(
          prefix + "DCArPhi_pT_cuts", "TH2");
      assert(h_QAG4SimulationTracking_DCArPhi);

      h_QAG4SimulationTracking_DCArPhi->Rebin2D(20, 1);

      h_ratio_ref = FitProfile(h_QAG4SimulationTracking_DCArPhi);
      ge_QAG4SimulationTracking_DCArPhi->Draw("pe");
    }

    ge_QAG4SimulationTracking_DCArPhi->SetTitle("DCA (r #phi, #geq 2MVTX, #geq 1INTT, #geq 20TPC) [cm]");
    DrawReference(ge_QAG4SimulationTracking_DCArPhi, h_ratio_ref, true);
  }

  {
    p = (TPad *) c1->cd(idx++);
    c1->Update();
    p->SetLogx();
    TH1 *frame = p->DrawFrame(0.1, -0.01, 50, 0.01,
                              "DCA (Z) [cm];Truth p_{T} [GeV/c];<DCA (Z)> #pm #sigma(DCA (Z)) [cm]");
    // gPad->SetLeftMargin(.2);
    gPad->SetTopMargin(-1);
    frame->GetYaxis()->SetTitleOffset(1.7);
    //TLine *l = new TLine(0.1, 0, 50, 0);
    //l->SetLineColor(kGray);
    //l->Draw();
    HorizontalLine(gPad, 1)->Draw();

    TH2 *h_QAG4SimulationTracking_DCAZ = (TH2 *) qa_file_new->GetObjectChecked(
        prefix + "DCAZ_pT_cuts", "TH2");
    assert(h_QAG4SimulationTracking_DCAZ);

    h_QAG4SimulationTracking_DCAZ->Rebin2D(40, 1);

    TGraphErrors *ge_QAG4SimulationTracking_DCAZ = FitProfile(h_QAG4SimulationTracking_DCAZ);
    ge_QAG4SimulationTracking_DCAZ->Draw("pe");
    ge_QAG4SimulationTracking_DCAZ->SetTitle("DCA (Z) [cm]");

    TGraphErrors *h_ratio_ref = NULL;
    if (qa_file_ref)
    {
      TH2 *h_QAG4SimulationTracking_DCAZ = (TH2 *) qa_file_ref->GetObjectChecked(
          prefix + "DCAZ_pT_cuts", "TH2");
      assert(h_QAG4SimulationTracking_DCAZ);

      h_QAG4SimulationTracking_DCAZ->Rebin2D(40, 1);

      h_ratio_ref = FitProfile(h_QAG4SimulationTracking_DCAZ);
      ge_QAG4SimulationTracking_DCAZ->Draw("pe");
    }

    DrawReference(ge_QAG4SimulationTracking_DCAZ, h_ratio_ref, true);
  }

  //SaveCanvas(c1, TString(qa_file_name_new) + TString("_") + TString(c1->GetName()), true);
    c1->Draw();
    outfilef->cd();
    c1->Write();
 }

 {
    const char *hist_name_prefix = "QAG4SimulationTracking";
  TString prefix = TString("h_") + hist_name_prefix + TString("_");

  // obtain normalization
  double Nevent_new = 1;
  double Nevent_ref = 1;

  if (qa_file_new)
  {
    TH1 *h_norm = (TH1 *) qa_file_new->GetObjectChecked(
        prefix + TString("Normalization"), "TH1");
    assert(h_norm);

    Nevent_new = h_norm->GetBinContent(h_norm->GetXaxis()->FindBin("Truth Track"));
  }
  if (qa_file_ref)
  {
    TH1 *h_norm = (TH1 *) qa_file_ref->GetObjectChecked(
        prefix + TString("Normalization"), "TH1");
    assert(h_norm);

    Nevent_ref = h_norm->GetBinContent(h_norm->GetXaxis()->FindBin("Truth Track"));
  }
    
    
    
  TH2 *h_new = (TH2 *) qa_file_new->GetObjectChecked(
      prefix + TString("SigmalizedDCArPhi_pT"), "TH2");
  assert(h_new);

  //  h_new->Rebin(1, 2);
  //h_new->Sumw2();
  //  h_new->Scale(1. / Nevent_new);

  TH2 *h_ref = NULL;
  if (qa_file_ref)
  {
    h_ref = (TH2 *) qa_file_ref->GetObjectChecked(
        prefix + TString("SigmalizedDCArPhi_pT"), "TH2");
    assert(h_ref);

    //    h_ref->Rebin(1, 2);
    //h_ref->Sumw2();
    h_ref->Scale(Nevent_new / Nevent_ref);
  }

  TCanvas *c1 = new TCanvas(TString("QA_Draw_Tracking_SigmalizedDCArPhi") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Tracking_SigmalizedDCArPhi") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c1->Divide(4, 2);
  int idx = 1;
  TPad *p;

  vector<pair<double, double>> gpt_ranges{
      {0, 0.5},
      {0.5, 1},
      {1, 1.5},
      {1.5, 2},
      {2, 4},
      {4, 16},
      {16, 40}};
  TF1 *f1 = nullptr;
  TF1 *fit = nullptr;
  Double_t sigma = 0;
  Double_t sigma_unc = 0;
  char resstr[500];
  TLatex *res = nullptr;
  for (auto pt_range : gpt_ranges)
  {
    // cout << __PRETTY_FUNCTION__ << " process " << pt_range.first << " - " << pt_range.second << " GeV/c";

    p = (TPad *) c1->cd(idx++);
    c1->Update();
    p->SetLogy();

    const double epsilon = 1e-6;
    const int bin_start = h_new->GetXaxis()->FindBin(pt_range.first + epsilon);
    const int bin_end = h_new->GetXaxis()->FindBin(pt_range.second - epsilon);

    TH1 *h_proj_new = h_new->ProjectionY(
        TString::Format(
            "%s_New_ProjX_%d_%d",
            h_new->GetName(), bin_start, bin_end),
        bin_start, bin_end);
    h_proj_new->GetXaxis()->SetRangeUser(-5.,5.);
    h_proj_new->Rebin(5);
    h_proj_new->SetTitle(TString(hist_name_prefix) + TString::Format(
                                                         ": %.1f - %.1f GeV/c", pt_range.first, pt_range.second));
    h_proj_new->GetXaxis()->SetTitle(TString::Format(
        "Sigmalized DCA (r #phi)"));
    h_proj_new->GetXaxis()->SetNdivisions(5,5);

    f1 = new TF1("f1","gaus",-4.,4.);
    h_proj_new->Fit(f1, "mq");
    sigma = f1->GetParameter(2);
    sigma_unc = f1->GetParError(2);
    
    TH1 *h_proj_ref = nullptr;
    if (h_ref)
    {
      h_proj_ref =
          h_ref->ProjectionY(
              TString::Format(
                  "%s_Ref_ProjX_%d_%d",
                  h_new->GetName(), bin_start, bin_end),
              bin_start, bin_end);
      //h_proj_ref->GetXaxis()->SetRangeUser(-.05,.05);
      h_proj_ref->Rebin(5);
    }
    
    DrawReference(h_proj_new, h_proj_ref);
    sprintf(resstr,"#sigma = %.5f #pm %.5f", sigma, sigma_unc);
    res = new TLatex(0.325,0.825,resstr);
    res->SetNDC();
    res->SetTextSize(0.05);
    res->SetTextAlign(13);
    res->Draw();
  }
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  TPaveText *pt = new TPaveText(.05,.1,.95,.8);
  pt->AddText("Cuts: MVTX hits>=2, INTT hits>=1,");
  pt->AddText("TPC hits>=20");
  pt->Draw();

  // SaveCanvas(c1, TString(qa_file_name_new) + TString("_") + TString(c1->GetName()), true);
  c1->Draw();
  outfilef->cd();
  c1->Write();
}

 {
    
    const char *hist_name_prefix = "QAG4SimulationTracking";
  TString prefix = TString("h_") + hist_name_prefix + TString("_");

  // obtain normalization
  double Nevent_new = 1;
  double Nevent_ref = 1;

  if (qa_file_new)
  {
    TH1 *h_norm = (TH1 *) qa_file_new->GetObjectChecked(
        prefix + TString("Normalization"), "TH1");
    assert(h_norm);

    Nevent_new = h_norm->GetBinContent(h_norm->GetXaxis()->FindBin("Truth Track"));
  }
  if (qa_file_ref)
  {
    TH1 *h_norm = (TH1 *) qa_file_ref->GetObjectChecked(
        prefix + TString("Normalization"), "TH1");
    assert(h_norm);

    Nevent_ref = h_norm->GetBinContent(h_norm->GetXaxis()->FindBin("Truth Track"));
  }
    
  TH2 *h_new2 = (TH2 *) qa_file_new->GetObjectChecked(
      prefix + TString("SigmalizedDCAZ_pT"), "TH2");
  assert(h_new2);

  //  h_new->Rebin(1, 2);
  //h_new2->Sumw2();
  //  h_new->Scale(1. / Nevent_new);

  TH2 *h_ref2 = NULL;
  if (qa_file_ref)
  {
    h_ref2 = (TH2 *) qa_file_ref->GetObjectChecked(
        prefix + TString("SigmalizedDCAZ_pT"), "TH2");
    assert(h_ref2);

    //    h_ref->Rebin(1, 2);
    //h_ref2->Sumw2();
    h_ref2->Scale(Nevent_new / Nevent_ref);
  }

  TCanvas *c2 = new TCanvas(TString("QA_Draw_Tracking_SigmalizedDCAZ") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Tracking_SigmalizedDCAZ") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c2->Divide(4, 2);
  int idx2 = 1;
  TPad *p2;

  vector<pair<double, double>> gpt_ranges2{
      {0, 0.5},
      {0.5, 1},
      {1, 1.5},
      {1.5, 2},
      {2, 4},
      {4, 16},
      {16, 40}};
  TF1 *f2 = nullptr;
  TF1 *fit2 = nullptr;
  Double_t sigma2 = 0;
  Double_t sigma_unc2 = 0;
  char resstr2[500];
  TLatex *res2 = nullptr;
  for (auto pt_range : gpt_ranges2)
  {
   // cout << __PRETTY_FUNCTION__ << " process " << pt_range.first << " - " << pt_range.second << " GeV/c";

    p2 = (TPad *) c2->cd(idx2++);
    c2->Update();
    p2->SetLogy();

    const double epsilon = 1e-6;
    const int bin_start = h_new2->GetXaxis()->FindBin(pt_range.first + epsilon);
    const int bin_end = h_new2->GetXaxis()->FindBin(pt_range.second - epsilon);

    TH1 *h_proj_new2 = h_new2->ProjectionY(
        TString::Format(
            "%s_New_ProjX_%d_%d",
            h_new2->GetName(), bin_start, bin_end),
        bin_start, bin_end);
    h_proj_new2->GetXaxis()->SetRangeUser(-5.,5.);
    h_proj_new2->Rebin(5);
    h_proj_new2->SetTitle(TString(hist_name_prefix) + TString::Format(
                                                         ": %.1f - %.1f GeV/c", pt_range.first, pt_range.second));
    h_proj_new2->GetXaxis()->SetTitle(TString::Format(
        "Sigmalized DCA (Z)"));
    h_proj_new2->GetXaxis()->SetNdivisions(5,5);
    
    f2 = new TF1("f2","gaus",-4.,4.);
    h_proj_new2->Fit(f2, "mq");
    fit2 = f2; //h_proj_new2->GetFunction("f2");
    sigma2 = fit2->GetParameter(2);
    sigma_unc2 = fit2->GetParError(2);

    TH1 *h_proj_ref2 = nullptr;
    if (h_ref2)
    {
      h_proj_ref2 =
          h_ref2->ProjectionY(
              TString::Format(
                  "%s_Ref_ProjX_%d_%d",
                  h_new2->GetName(), bin_start, bin_end),
              bin_start, bin_end);
      //h_proj_ref->GetXaxis()->SetRangeUser(-.05,.05);
      h_proj_ref2->Rebin(5);
    }
    DrawReference(h_proj_new2, h_proj_ref2);
    sprintf(resstr2,"#sigma = %.5f #pm %.5f", sigma2, sigma_unc2);
    res2 = new TLatex(0.325,0.825,resstr2);
    res2->SetNDC();
    res2->SetTextSize(0.05);
    res2->SetTextAlign(13);
    res2->Draw();
  }
  p2 = (TPad *) c2->cd(idx2++);
  c2->Update();
  TPaveText *pt2 = new TPaveText(.05,.1,.95,.8);
  pt2->AddText("Cuts: MVTX hits>=2, INTT hits>=1,");
  pt2->AddText("TPC hits>=20");
  pt2->Draw();

  //SaveCanvas(c2, TString(qa_file_name_new) + TString("_") + TString(c2->GetName()), true);
    c2->Draw();
    outfilef->cd();
    c2->Write();
}

{
    const char *hist_name_prefix = "QAG4SimulationTracking";
    TString prefix = TString("h_") + hist_name_prefix + TString("_");
    
    
  // obtain normalization
  double Nevent_new = 1;
  double Nevent_ref = 1;

  if (qa_file_new)
  {

    TH1 *h_norm = (TH1 *) qa_file_new->GetObjectChecked(
        prefix + TString("Normalization"), "TH1");
    assert(h_norm);

    Nevent_new = h_norm->GetBinContent(h_norm->GetXaxis()->FindBin("Event"));
  }
  if (qa_file_ref)
  {
    TH1 *h_norm = (TH1 *) qa_file_ref->GetObjectChecked(
        prefix + TString("Normalization"), "TH1");
    assert(h_norm);

    Nevent_ref = h_norm->GetBinContent(h_norm->GetXaxis()->FindBin("Event"));
  }

  TCanvas *c1 = new TCanvas(TString("QA_Draw_Tracking_SigmalizedDCA_Resolution") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Tracking_SigmalizedDCA_Resolution") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  {
    p = (TPad *) c1->cd(idx++);
    c1->Update();
    p->SetLogx();
    TH1 *frame = p->DrawFrame(0.1, -2, 50, 2,
                              ";Truth p_{T} [GeV/c];<Sigmalized DCA (r #phi)> #pm #sigma(Sigmalized DCA (r #phi))");
    gPad->SetLeftMargin(.2);
    frame->GetYaxis()->SetTitleOffset(2);
    TLine *l = new TLine(0.1, 0, 50, 0);
    l->SetLineColor(kGray);
    l->Draw();
    HorizontalLine(gPad, 1)->Draw();
    HorizontalLine(gPad, -1)->Draw();

    TH2 *h_QAG4SimulationTracking_DCArPhi = (TH2 *) qa_file_new->GetObjectChecked(
        prefix + "SigmalizedDCArPhi_pT", "TH2");
    assert(h_QAG4SimulationTracking_DCArPhi);

    h_QAG4SimulationTracking_DCArPhi->Rebin2D(20, 1);

    // h_QAG4SimulationTracking_DCArPhi->Draw("colz");
    TGraphErrors *ge_QAG4SimulationTracking_DCArPhi = FitProfile(h_QAG4SimulationTracking_DCArPhi);
    ge_QAG4SimulationTracking_DCArPhi->Draw("pe");

    TGraphErrors *h_ratio_ref = NULL;
    if (qa_file_ref)
    {
      TH2 *h_QAG4SimulationTracking_DCArPhi = (TH2 *) qa_file_ref->GetObjectChecked(
          prefix + "SigmalizedDCArPhi_pT", "TH2");
      assert(h_QAG4SimulationTracking_DCArPhi);

      h_QAG4SimulationTracking_DCArPhi->Rebin2D(20, 1);

      h_ratio_ref = FitProfile(h_QAG4SimulationTracking_DCArPhi);
      ge_QAG4SimulationTracking_DCArPhi->Draw("pe");
    }

    ge_QAG4SimulationTracking_DCArPhi->SetTitle("DCA_{r#phi}/#sigma[DCA_{r#phi}]");
    DrawReference(ge_QAG4SimulationTracking_DCArPhi, h_ratio_ref, true);
  }

  {
    p = (TPad *) c1->cd(idx++);
    c1->Update();
    p->SetLogx();
    TH1 *frame = p->DrawFrame(0.1, -2, 50, 2,
                              "DCA_z/#sigma[DCA_z];Truth p_{T} [GeV/c];<Sigmalized DCA (Z)> #pm #sigma(Sigmalized DCA (Z))");
    // gPad->SetLeftMargin(.2);
    gPad->SetTopMargin(-1);
    frame->GetYaxis()->SetTitleOffset(1.7);
    TLine *l = new TLine(0.1, 0, 50, 0);
    l->SetLineColor(kGray);
    l->Draw();
    HorizontalLine(gPad, 1)->Draw();
    HorizontalLine(gPad, -1)->Draw();

    TH2 *h_QAG4SimulationTracking_DCAZ = (TH2 *) qa_file_new->GetObjectChecked(
        prefix + "SigmalizedDCAZ_pT", "TH2");
    assert(h_QAG4SimulationTracking_DCAZ);

    h_QAG4SimulationTracking_DCAZ->Rebin2D(40, 1);

    TGraphErrors *ge_QAG4SimulationTracking_DCAZ = FitProfile(h_QAG4SimulationTracking_DCAZ);
    ge_QAG4SimulationTracking_DCAZ->Draw("pe");
    ge_QAG4SimulationTracking_DCAZ->SetTitle("DCA_z/#sigma[DCA_z]");

    TGraphErrors *h_ratio_ref = NULL;
    if (qa_file_ref)
    {
      TH2 *h_QAG4SimulationTracking_DCAZ = (TH2 *) qa_file_ref->GetObjectChecked(
          prefix + "SigmalizedDCAZ_pT", "TH2");
      assert(h_QAG4SimulationTracking_DCAZ);

      h_QAG4SimulationTracking_DCAZ->Rebin2D(40, 1);

      h_ratio_ref = FitProfile(h_QAG4SimulationTracking_DCAZ);
      ge_QAG4SimulationTracking_DCAZ->Draw("pe");
    }

    DrawReference(ge_QAG4SimulationTracking_DCAZ, h_ratio_ref, true);
  }

  //SaveCanvas(c1, TString(qa_file_name_new) + TString("_") + TString(c1->GetName()), true);
    c1->Draw();
    
    outfilef->cd();
    c1->Write();
}


{
    const char *hist_name_prefix = "QAG4SimulationVertex_SvtxVertexMap";
    TString prefix = TString("h_") + hist_name_prefix + TString("_");
    
    
  TCanvas *c1 = new TCanvas(TString("QA_Draw_Vertex_nVertex") +
                                TString("_") + hist_name_prefix,
                            TString("QA_Draw_Vertex_nVertex") +
                                TString("_") + hist_name_prefix,
                            1800, 1000);
  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  {
    static const int nrebin = 1;

    p = (TPad *)c1->cd(idx++);
    c1->Update();
    // p->SetLogx();
    p->SetGridy();

    TH1 *h_pass =
        (TH1 *)qa_file_new->GetObjectChecked(prefix + "gntracks", "TH1");
    assert(h_pass);

    h_pass->Rebin(nrebin);

    //    h_ratio->GetXaxis()->SetRangeUser(min_Et, max_Et);
    h_pass->GetYaxis()->SetTitle("Counts");
    // h_pass->GetYaxis()->SetRangeUser(-0, 1.);

    TH1 *h_ref = NULL;
    if (qa_file_ref) {
      h_ref =
          (TH1 *)qa_file_ref->GetObjectChecked(prefix + "gntracks", "TH1");
      assert(h_ref);

      h_ref->Rebin(nrebin);
    }

    h_pass->SetTitle(TString(hist_name_prefix) + ": gntracks");

    DrawReference(h_pass, h_ref, false);
  }

  {
    static const int nrebin = 1;
    
    p = (TPad *)c1->cd(idx++);
    c1->Update();
    // p->SetLogx();
    p->SetGridy();

    TH1 *h_pass =
        (TH1 *)qa_file_new->GetObjectChecked(prefix + "gntracksmaps", "TH1");
    assert(h_pass);

    h_pass->Rebin(nrebin);

    //    h_ratio->GetXaxis()->SetRangeUser(min_Et, max_Et);
    h_pass->GetYaxis()->SetTitle("Counts");
    // h_pass->GetYaxis()->SetRangeUser(-0, 1.);

    TH1 *h_ref = NULL;
    if (qa_file_ref) {
      h_ref =
          (TH1 *)qa_file_ref->GetObjectChecked(prefix + "gntracksmaps", "TH1");
      assert(h_pass);

      h_ref->Rebin(nrebin);
    }

    h_pass->SetTitle(TString(hist_name_prefix) + ": gntracksmaps");

    DrawReference(h_pass, h_ref, false);
  }

  {
    static const int nrebin = 1;
    
    p = (TPad *)c1->cd(idx++);
    c1->Update();
    // p->SetLogx();
    p->SetGridy();

    TH1 *h_pass =
        (TH1 *)qa_file_new->GetObjectChecked(prefix + "ntracks", "TH1");
    assert(h_pass);

    h_pass->Rebin(nrebin);

    //    h_ratio->GetXaxis()->SetRangeUser(min_Et, max_Et);
    h_pass->GetYaxis()->SetTitle("Counts");
    // h_pass->GetYaxis()->SetRangeUser(-0, 1.);

    TH1 *h_ref = NULL;
    if (qa_file_ref) {
      h_ref =
          (TH1 *)qa_file_ref->GetObjectChecked(prefix + "ntracks", "TH1");
      assert(h_pass);

      h_ref->Rebin(nrebin);
    }

    h_pass->SetTitle(TString(hist_name_prefix) + ": ntracks");

    DrawReference(h_pass, h_ref, false);
  }

  {
    static const int nrebin = 1;
    
    p = (TPad *)c1->cd(idx++);
    c1->Update();
    // p->SetLogx();
    p->SetGridy();

    TH1 *h_pass =
        (TH1 *)qa_file_new->GetObjectChecked(prefix + "ntracks_cuts", "TH1");
    assert(h_pass);

    h_pass->Rebin(nrebin);

    //    h_ratio->GetXaxis()->SetRangeUser(min_Et, max_Et);
    h_pass->GetYaxis()->SetTitle("Counts");
    // h_pass->GetYaxis()->SetRangeUser(-0, 1.);

    TH1 *h_ref = NULL;
    if (qa_file_ref) {
      h_ref =
          (TH1 *)qa_file_ref->GetObjectChecked(prefix + "ntracks_cuts", "TH1");
      assert(h_pass);

      h_ref->Rebin(nrebin);
    }

    h_pass->SetTitle(TString(hist_name_prefix) + ": ntracks (#geq 2 MVTX)");

    DrawReference(h_pass, h_ref, false);
  }

 // SaveCanvas(c1,
 //            TString(qa_file_name_new) + TString("_") + TString(c1->GetName()),
 //            true);
    
    c1->Draw();
    outfilef->cd();
    c1->Write();
}

{
    const char *hist_name_prefix = "QAG4SimulationVertex_SvtxVertexMap";
    TString prefix = TString("h_") + hist_name_prefix + TString("_");
    
    
  // X-direction

  TH2 *h_new = (TH2 *) qa_file_new->GetObjectChecked(
      prefix + TString("vxRes_gvz"), "TH2");
  assert(h_new);

  // h_new->Rebin2D(1, 5);
  //h_new->Sumw2();
  // h_new->GetXaxis()->SetRangeUser(-15,15);
  //  h_new->Scale(1. / Nevent_new);

  TH2 *h_ref = NULL;
  if (qa_file_ref)
  {
    h_ref = (TH2 *) qa_file_ref->GetObjectChecked(
        prefix + TString("vxRes_gvz"), "TH2");
    assert(h_ref);

    // h_ref->Rebin2D(1, 5);
    //h_ref->Sumw2();
    // h_ref->Scale(Nevent_new / Nevent_ref);
  }

  TCanvas *c1 = new TCanvas(TString("QA_Draw_Vertex_Resolution_x") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Vertex_Resolution_x") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c1->Divide(2,1);
  int idx = 1;
  TPad *p;

  vector<pair<double, double>> gvz_ranges{
      {-10.0, 10.0}};
  TF1 *f1 = nullptr;
  TF1 *fit = nullptr;
  Double_t sigma = 0;
  Double_t sigma_unc = 0;
  char resstr[500];
  TLatex *res = nullptr;
  for (auto gvz_range : gvz_ranges)
  {
   // cout << __PRETTY_FUNCTION__ << " process " << gvz_range.first << " - " << gvz_range.second << " cm";

    p = (TPad *) c1->cd(idx++);
    c1->Update();
    // p->SetLogy();

    const double epsilon = 1e-6;
    const int bin_start = h_new->GetXaxis()->FindBin(gvz_range.first + epsilon);
    const int bin_end = h_new->GetXaxis()->FindBin(gvz_range.second - epsilon);

    TH1 *h_proj_new = h_new->ProjectionY(
        TString::Format(
            "%s_New_ProjX_%d_%d",
            h_new->GetName(), bin_start, bin_end));
	// bin_start, bin_end);

    h_proj_new->SetTitle(TString(hist_name_prefix) + TString::Format(
                                                         ": %.1f - %.1f cm - gvz", gvz_range.first, gvz_range.second));
    h_proj_new->GetXaxis()->SetTitle(TString::Format(
        "Vertex Resolution (x) [cm]"));
    h_proj_new->GetXaxis()->SetNdivisions(5,5);
    h_proj_new->GetXaxis()->SetRangeUser(-0.002,0.002);

    f1 = new TF1("f1","gaus",-.002,.002);
    h_proj_new->Fit(f1, "qm");
    sigma = f1->GetParameter(2);
    sigma_unc = f1->GetParError(2);


    TH1 *h_proj_ref = nullptr;
    if (h_ref)
    {
      h_proj_ref =
          h_ref->ProjectionY(
              TString::Format(
                  "%s_Ref_ProjX_%d_%d",
                  h_new->GetName(), bin_start, bin_end));
              // bin_start, bin_end);
      h_proj_ref->GetXaxis()->SetRangeUser(-10,10);
    }
    
    DrawReference(h_proj_new, h_proj_ref);
    sprintf(resstr,"#sigma = %.5f #pm %.5f cm", sigma, sigma_unc);
    res = new TLatex(0.325,0.825,resstr);
    res->SetNDC();
    res->SetTextSize(0.05);
    res->SetTextAlign(13);
    res->Draw();
  }
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  gPad->SetLeftMargin(.2);
  //h_new->GetYaxis()->SetTitleOffset(2);
  h_new->Draw("colz");
  

//  SaveCanvas(c1, TString(qa_file_name_new) + TString("_") + TString(c1->GetName()), true);
    c1->Draw();

  // Y-direction

  TH2 *h_new2 = (TH2 *) qa_file_new->GetObjectChecked(
      prefix + TString("vyRes_gvz"), "TH2");
  assert(h_new2);

  //  h_new->Rebin(1, 2);
  //h_new2->Sumw2();
  //  h_new->Scale(1. / Nevent_new);

  TH2 *h_ref2 = NULL;
  if (qa_file_ref)
  {
    h_ref2 = (TH2 *) qa_file_ref->GetObjectChecked(
        prefix + TString("vyRes_gvz"), "TH2");
    assert(h_ref2);

    // h_ref->Rebin(1, 2);
    //h_ref2->Sumw2();
    // h_ref->Scale(Nevent_new / Nevent_ref);
  }

  TCanvas *c2 = new TCanvas(TString("QA_Draw_Vertex_Resolution_y") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Vertex_Resolution_y") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c2->Divide(2,1);
  int idx2 = 1;
  TPad *p2;

  vector<pair<double, double>> gvz_ranges2{
      {-10.0, 10.0}};
  TF1 *f2 = nullptr;
  TF1 *fit2 = nullptr;
  Double_t sigma2 = 0;
  Double_t sigma_unc2 = 0;
  char resstr2[500];
  TLatex *res2 = nullptr;
  for (auto gvz_range : gvz_ranges2)
  {
    //cout << __PRETTY_FUNCTION__ << " process " << gvz_range.first << " - " << gvz_range.second << " cm";

    p2 = (TPad *) c2->cd(idx2++);
    c2->Update();
    // p->SetLogy();

    const double epsilon = 1e-6;
    const int bin_start = h_new2->GetXaxis()->FindBin(gvz_range.first + epsilon);
    const int bin_end = h_new2->GetXaxis()->FindBin(gvz_range.second - epsilon);

    TH1 *h_proj_new2 = h_new2->ProjectionY(
        TString::Format(
            "%s_New_ProjX_%d_%d",
            h_new2->GetName(), bin_start, bin_end),
        bin_start, bin_end);

    h_proj_new2->SetTitle(TString(hist_name_prefix) + TString::Format(
                                                         ": %.1f - %.1f cm - gvz", gvz_range.first, gvz_range.second));
    h_proj_new2->GetXaxis()->SetTitle(TString::Format(
        "Vertex Resolution (y) [cm]"));
    h_proj_new2->GetXaxis()->SetNdivisions(5,5);
    h_proj_new2->GetXaxis()->SetRangeUser(-0.002,0.002);
    
    f2 = new TF1("f2","gaus",-.002,.002);
    h_proj_new2->Fit(f2,  "qm");
    fit2 = h_proj_new2->GetFunction("f2");
    sigma2 = fit2->GetParameter(2);
    sigma_unc2 = fit2->GetParError(2);

    TH1 *h_proj_ref2 = nullptr;
    if (h_ref2)
    {
      h_proj_ref2 =
          h_ref2->ProjectionY(
              TString::Format(
                  "%s_Ref_ProjX_%d_%d",
                  h_new2->GetName(), bin_start, bin_end),
              bin_start, bin_end);
    }
    
    DrawReference(h_proj_new2, h_proj_ref2);
    sprintf(resstr2,"#sigma = %.5f #pm %.5f cm", sigma2, sigma_unc2);
    res2 = new TLatex(0.325,0.825,resstr2);
    res2->SetNDC();
    res2->SetTextSize(0.05);
    res2->SetTextAlign(13);
    res2->Draw();
  }
  p2 = (TPad *) c2->cd(idx2++);
  c2->Update();
  gPad->SetLeftMargin(.2);
  //h_new2->GetYaxis()->SetTitleOffset(2);
  h_new2->Draw("colz");

  //SaveCanvas(c2, TString(qa_file_name_new) + TString("_") + TString(c2->GetName()), true);
    c2->Draw();
    
  // Z-direction

  TH2 *h_new3 = (TH2 *) qa_file_new->GetObjectChecked(
      prefix + TString("vzRes_gvz"), "TH2");
  assert(h_new3);

  //  h_new->Rebin(1, 2);
  //h_new3->Sumw2();
  //  h_new->Scale(1. / Nevent_new);

  TH2 *h_ref3 = NULL;
  if (qa_file_ref)
  {
    h_ref3 = (TH2 *) qa_file_ref->GetObjectChecked(
        prefix + TString("vzRes_gvz"), "TH2");
    assert(h_ref3);

    // h_ref->Rebin(1, 2);
    //h_ref3->Sumw2();
    // h_ref->Scale(Nevent_new / Nevent_ref);
  }

  TCanvas *c3 = new TCanvas(TString("QA_Draw_Vertex_Resolution_z") + TString("_") + hist_name_prefix,
                            TString("QA_Draw_Vertex_Resolution_z") + TString("_") + hist_name_prefix,
                            1800, 1000);
  c3->Divide(2,1);
  int idx3 = 1;
  TPad *p3;

  vector<pair<double, double>> gvz_ranges3{
      {-10.0, 10.0}};
  TF1 *f3 = nullptr;
  TF1 *fit3 = nullptr;
  Double_t sigma3 = 0;
  Double_t sigma_unc3 = 0;
  char resstr3[500];
  TLatex *res3 = nullptr;
  for (auto gvz_range : gvz_ranges3)
  {
   // cout << __PRETTY_FUNCTION__ << " process " << gvz_range.first << " - " << gvz_range.second << " cm";

    p3 = (TPad *) c3->cd(idx3++);
    c3->Update();
    // p->SetLogy();

    const double epsilon = 1e-6;
    const int bin_start = h_new3->GetXaxis()->FindBin(gvz_range.first + epsilon);
    const int bin_end = h_new3->GetXaxis()->FindBin(gvz_range.second - epsilon);

    TH1 *h_proj_new3 = h_new3->ProjectionY(
        TString::Format(
            "%s_New_ProjX_%d_%d",
            h_new3->GetName(), bin_start, bin_end),
        bin_start, bin_end);

    h_proj_new3->SetTitle(TString(hist_name_prefix) + TString::Format(
                                                         ": %.1f - %.1f cm -gvz", gvz_range.first, gvz_range.second));
    h_proj_new3->GetXaxis()->SetTitle(TString::Format(
        "Vertex Resolution (z) [cm]"));
    h_proj_new3->GetXaxis()->SetNdivisions(5,5);
    h_proj_new3->GetXaxis()->SetRangeUser(-0.002,0.002);
    
    f3 = new TF1("f3","gaus",-.002,.002);
    h_proj_new3->Fit(f3,  "qm");
    fit3 = h_proj_new3->GetFunction("f3");
    sigma3 = fit3->GetParameter(2);
    sigma_unc3 = fit3->GetParError(2);

    TH1 *h_proj_ref3 = nullptr;
    if (h_ref3)
    {
      h_proj_ref3 =
          h_ref3->ProjectionY(
              TString::Format(
                  "%s_Ref_ProjX_%d_%d",
                  h_new3->GetName(), bin_start, bin_end),
              bin_start, bin_end);
    }
    
    DrawReference(h_proj_new3, h_proj_ref3);
    sprintf(resstr3,"#sigma = %.5f #pm %.5f cm", sigma3, sigma_unc3);
    res3 = new TLatex(0.325,0.825,resstr3);
    res3->SetNDC();
    res3->SetTextSize(0.05);
    res3->SetTextAlign(13);
    res3->Draw();
  }
  p3 = (TPad *) c3->cd(idx3++);
  c3->Update();
  gPad->SetLeftMargin(.2);
  //h_new3->GetYaxis()->SetTitleOffset(2);
  h_new3->Draw("colz");

//  SaveCanvas(c3, TString(qa_file_name_new) + TString("_") + TString(c3->GetName()), true);
    c3->Draw();
    outfilef->cd();
    c3->Write();
}

}
