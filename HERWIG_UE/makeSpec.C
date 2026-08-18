// makeSpec.C
//
// Draws the HERWIG UE cross-checks against STAR and against the PPG10
// sPHENIX result. PYTHIA has been dropped; instead the two particle sources
// written by HerwigSTARUECheck are overlaid:
//   "_hepmc" -- generator-level HepMC record
//   "_g4"    -- PHG4TruthInfoContainer sPHENIX primaries
// Both live in the same per-sample output file.
//
// The sumET panel additionally shows the PPG10 unfolded data (points + band)
// and the PPG10 HERWIG truth curve.

const int NSAMP = 7;
const std::string samp[NSAMP] = {"MB", "Jet5", "Jet12", "Jet20", "Jet30", "Jet40", "Jet50"};
const double HerwigCS[NSAMP] = {3.1909e+10, 1.8437e+08, 1.132355e+06, 5.2613e+04, 2.0694e+03, 1.0510e+02, 5.2089};

bool csWeightsAlreadyInFiles = true;

// ---- the two particle sources written by HerwigSTARUECheck -------------
const int NVERS = 2;
const std::string versTag[NVERS] = {"hepmc", "g4"};
const std::string versLabel[NVERS] = {"HERWIG (HepMC)", "HERWIG (sPHENIX primary)"};
const int versColor[NVERS] = {kBlue, kOrange + 7};
const int versMarker[NVERS] = {33, 34};
// h_nevents bins in the new module: 1 = processed, 2 = HepMC node found,
// 3 = G4TruthInfo node found. Normalize each source by its own bin.
const int versNevtBin[NVERS] = {2, 3};

const std::string inDir = "/sphenix/tg/tg01/jets/bkimelman/Herwig_UE";
const std::string prodTag = "Aug5";  // production tag in the input file names

namespace
{
  void styleObj(TH1 *h, int color, int marker)
  {
    if (!h) return;
    h->SetMarkerStyle(marker);
    h->SetMarkerColor(color);
    h->SetLineColor(color);
    h->SetMarkerSize(1);
  }
}  // namespace

void makeSpec()
{
  gStyle->SetOptStat(0);

  // ------------------------------------------------------------------
  // PPG10 reference: unfolded sPHENIX data + HERWIG truth
  // ------------------------------------------------------------------
  TFile *fPPG = new TFile("/sphenix/u/bkimelman/PPG10_Prelim/plot_result_hist_output_efrac_bkg_cut_run28_iter_3_1000toys.root", "READ");

  TH1D *hPPG = (TH1D *) fPPG->Get("unfold_hist_calib_dijet_reweight_trim_10_2_etEffCorrected_2");
  hPPG->SetMarkerStyle(20);
  hPPG->SetMarkerColor(kBlack);
  hPPG->SetLineColor(kBlack);
  hPPG->SetLineWidth(2);
  hPPG->SetMarkerSize(1);

  TGraphAsymmErrors *grPPG = (TGraphAsymmErrors *) fPPG->Get("Graph");
  grPPG->SetFillColorAlpha(kBlack, 0.60);
  grPPG->SetFillStyle(1001);
  grPPG->SetLineWidth(2);
  grPPG->SetMarkerStyle(20);
  grPPG->SetMarkerSize(1);
  grPPG->SetMarkerColor(kBlack);
  grPPG->SetLineColor(kBlack);
  grPPG->RemovePoint(0);

  TH1D *hPPGH = (TH1D *) fPPG->Get("truth_hist_herwig");
  styleObj(hPPGH, kCyan + 1, 41);
  hPPGH->SetLineWidth(2);

  // ------------------------------------------------------------------
  // STAR references
  // ------------------------------------------------------------------
  TFile *fS = new TFile((inDir + "/HEPData-ins709170-v1-Table_2.root").c_str(), "READ");
  TGraphAsymmErrors *grS = (TGraphAsymmErrors *) fS->Get("Table 2/Graph1D_y1");
  grS->SetMarkerStyle(20);
  grS->SetMarkerColor(kBlack);
  grS->SetLineColor(kBlack);

  TFile *fN = new TFile((inDir + "/HEPData-ins1771348-v1-Figure_3.root").c_str(), "READ");
  TGraphAsymmErrors *grN = (TGraphAsymmErrors *) fN->Get("Figure 3/Graph1D_y1");
  grN->SetMarkerStyle(20);
  grN->SetMarkerColor(kBlack);
  grN->SetLineColor(kBlack);

  // ------------------------------------------------------------------
  // accumulate the HERWIG samples, once per particle source
  // ------------------------------------------------------------------
  TH1D *spec[NVERS] = {nullptr, nullptr};  // pi+ spectrum (MB only)
  TH2D *N[NVERS] = {nullptr, nullptr};     // transverse dNch/detadphi
  TH2D *E[NVERS] = {nullptr, nullptr};     // transverse sumET, R = 0.4

  for (int i = 0; i < NSAMP; i++)
  {
    std::cout << "working on sample " << i << ": " << samp[i] << std::endl;

    TFile *fSamp = new TFile(std::format("{}/Herwig_UE_{}_{}.root", inDir, prodTag, samp[i]).c_str(), "READ");
    if (!fSamp || fSamp->IsZombie())
    {
      std::cerr << "  could not open input for " << samp[i] << ", skipping" << std::endl;
      continue;
    }

    TH1D *ev = (TH1D *) fSamp->Get("h_nevents");

    for (int v = 0; v < NVERS; v++)
    {
      const std::string t = "_" + versTag[v];

      const double nev = ev->GetBinContent(versNevtBin[v]);
      if (nev <= 0)
      {
        std::cerr << "  " << samp[i] << ": no events for source " << versTag[v]
                  << ", skipping" << std::endl;
        continue;
      }

      // per-sample stitching weight still to be applied here
      const double sampWeight = csWeightsAlreadyInFiles ? 1.0 : HerwigCS[i];
      // extra factor needed to undo a weight already baked into the file
      const double undoWeight = csWeightsAlreadyInFiles ? HerwigCS[i] : 1.0;

      if (i == 0)
      {
        spec[v] = (TH1D *) fSamp->Get(("pip_spec" + t).c_str());
        if (spec[v])
        {
          spec[v] = (TH1D *) spec[v]->Clone(("spec_" + versTag[v]).c_str());
          spec[v]->SetDirectory(nullptr);
          spec[v]->Scale(1.0 / (nev * undoWeight));
        }
      }

      TH2D *NTmp = (TH2D *) fSamp->Get(("p_dens_trans_pt02" + t).c_str());
      if (NTmp)
      {
        NTmp->Scale(sampWeight / nev);
        if (!N[v])
        {
          N[v] = (TH2D *) NTmp->Clone(("N_" + versTag[v]).c_str());
          N[v]->SetDirectory(nullptr);
        }
        else
        {
          N[v]->Add(NTmp);
        }
      }

      TH2D *ETmp = (TH2D *) fSamp->Get(("p_sumET_trans_R04" + t).c_str());
      if (ETmp)
      {
        ETmp->Scale(sampWeight / nev);
        if (!E[v])
        {
          E[v] = (TH2D *) ETmp->Clone(("E_" + versTag[v]).c_str());
          E[v]->SetDirectory(nullptr);
        }
        else
        {
          E[v]->Add(ETmp);
        }
      }
    }

    fSamp->Close();
  }

  // ------------------------------------------------------------------
  // profiles and styling
  // ------------------------------------------------------------------
  TProfile *pN[NVERS] = {nullptr, nullptr};
  TProfile *pE[NVERS] = {nullptr, nullptr};

  for (int v = 0; v < NVERS; v++)
  {
    if (spec[v])
    {
      spec[v]->Scale(1.0, "width");
      styleObj(spec[v], versColor[v], versMarker[v]);
    }

    if (N[v])
    {
      pN[v] = N[v]->ProfileX(("pN_" + versTag[v]).c_str());
      pN[v]->Scale(1.0 / (2.0 * 2.0 * TMath::Pi() / 3.0));
      styleObj(pN[v], versColor[v], versMarker[v]);
    }

    if (E[v])
    {
      pE[v] = E[v]->ProfileX(("pE_" + versTag[v]).c_str());
      styleObj(pE[v], versColor[v], versMarker[v]);
    }
  }

  TCanvas *c1 = new TCanvas();

  // ------------------------------------------------------------------
  // transverse charged-particle density vs STAR PRD 101
  // ------------------------------------------------------------------
  c1->Clear();
  c1->SetLogy(0);

  TProfile *pNFrame = pN[0] ? pN[0] : pN[1];
  pNFrame->GetYaxis()->SetRangeUser(0.0, 1.5);
  pNFrame->Draw("P");
  grN->Draw("PSAME");
  for (int v = 0; v < NVERS; v++)
  {
    if (pN[v]) pN[v]->Draw("PSAME");
  }

  TLegend *leg = new TLegend(0.5, 0.5, 0.85, 0.85);
  leg->AddEntry(grN, "STAR PRD 101", "P");
  for (int v = 0; v < NVERS; v++)
  {
    if (pN[v]) leg->AddEntry(pN[v], versLabel[v].c_str(), "P");
  }
  leg->Draw("same");

  c1->SaveAs((inDir + "/multDens.pdf").c_str());

  // ------------------------------------------------------------------
  // transverse sumET vs PPG10 data and PPG10 HERWIG
  // ------------------------------------------------------------------
  c1->Clear();
  c1->SetLogy(0);

  TProfile *pEFrame = pE[0] ? pE[0] : pE[1];
  pEFrame->GetYaxis()->SetRangeUser(0.0, 0.85);
  pEFrame->GetYaxis()->SetTitle("#LT#Sigma E_{T}/#delta#eta#delta#phi#GT [GeV]");
  pEFrame->Draw("P");
  grPPG->Draw("E2 SAME");
  hPPG->Draw("E1 SAME");
  hPPGH->Draw("PSAME");
  for (int v = 0; v < NVERS; v++)
  {
    if (pE[v]) pE[v]->Draw("PSAME");
  }

  leg = new TLegend(0.15, 0.15, 0.45, 0.4);
  leg->AddEntry(grPPG, "sPHENIX PPG10 data", "PE");
  leg->AddEntry(hPPGH, "HERWIG PPG10", "P");
  for (int v = 0; v < NVERS; v++)
  {
    if (pE[v]) leg->AddEntry(pE[v], versLabel[v].c_str(), "P");
  }
  leg->Draw("same");

  c1->SaveAs((inDir + "/sumET.pdf").c_str());

  // ------------------------------------------------------------------
  // pi+ spectrum vs STAR PLB 637, with MC/data ratio panel
  // ------------------------------------------------------------------
  c1->Clear();

  TPad *topPad = new TPad("topPad", "", 0.0, 0.4, 1.0, 1.0);
  topPad->SetTopMargin(0.05);
  topPad->SetRightMargin(0.05);
  topPad->SetBottomMargin(0.0);
  topPad->SetLogy();
  topPad->Draw();

  TPad *bottomPad = new TPad("bottomPad", "", 0.0, 0.0, 1.0, 0.4);
  bottomPad->SetTopMargin(0.0);
  bottomPad->SetRightMargin(0.05);
  bottomPad->SetBottomMargin(0.16);
  bottomPad->Draw();

  topPad->cd();

  // set the y range from data AND MC together, so a mis-normalized MC shows
  // up as an off-scale curve instead of silently pushing the data off the pad
  double ylo = 1e300, yhi = -1e300;
  for (int i = 0; i < grS->GetN(); i++)
  {
    const double y = grS->GetY()[i];
    if (y > 0)
    {
      ylo = std::min(ylo, y);
      yhi = std::max(yhi, y);
    }
  }
  for (int v = 0; v < NVERS; v++)
  {
    if (!spec[v]) continue;
    for (int i = 1; i <= spec[v]->GetNbinsX(); i++)
    {
      const double y = spec[v]->GetBinContent(i);
      if (y > 0)
      {
        ylo = std::min(ylo, y);
        yhi = std::max(yhi, y);
      }
    }
  }

  TH1D *specFrame = spec[0] ? spec[0] : spec[1];
  specFrame->GetYaxis()->SetRangeUser(0.2 * ylo, 5.0 * yhi);
  specFrame->Draw("P");
  grS->Draw("P SAME");
  for (int v = 0; v < NVERS; v++)
  {
    if (spec[v]) spec[v]->Draw("P SAME");
  }

  leg = new TLegend(0.5, 0.5, 0.85, 0.85);
  leg->AddEntry(grS, "STAR PLB 637", "P");
  for (int v = 0; v < NVERS; v++)
  {
    if (spec[v]) leg->AddEntry(spec[v], versLabel[v].c_str(), "P");
  }
  leg->Draw("same");

  bottomPad->cd();

  TGraphAsymmErrors *grSRat = new TGraphAsymmErrors();
  grSRat->SetFillColorAlpha(kYellow, 1.0);
  grSRat->SetFillStyle(1001);
  grSRat->SetLineWidth(2);
  grSRat->SetMarkerStyle(20);
  grSRat->SetMarkerSize(0);
  grSRat->SetMarkerColor(kYellow);
  grSRat->SetLineColor(kYellow);
  for (int i = 0; i < grS->GetN(); i++)
  {
    double x = grS->GetX()[i];
    double y = grS->GetY()[i];

    grSRat->AddPoint(x, 1.0);
    grSRat->SetPointError(i, grS->GetErrorXlow(i), grS->GetErrorXhigh(i),
                          grS->GetErrorYlow(i) / y, grS->GetErrorYhigh(i) / y);
  }
  grSRat->Print();

  TH1D *specRat[NVERS] = {nullptr, nullptr};
  for (int v = 0; v < NVERS; v++)
  {
    if (!spec[v]) continue;
    specRat[v] = (TH1D *) spec[v]->Clone(("specRat_" + versTag[v]).c_str());
    for (int i = 1; i <= spec[v]->GetNbinsX(); i++)
    {
      const double num = spec[v]->GetBinContent(i);
      const double den = grS->GetY()[i - 1];
      if (den == 0.0 || num == 0.0)
      {
        specRat[v]->SetBinContent(i, 0.0);
        specRat[v]->SetBinError(i, 0.0);
        continue;
      }
      specRat[v]->SetBinContent(i, num / den);
      specRat[v]->SetBinError(i, (num / den) * spec[v]->GetBinError(i) / num);
    }
  }

  TLine *l = new TLine(0.3, 1.0, 10.0, 1.0);
  l->SetLineColor(kBlack);

  TH1D *ratFrame = specRat[0] ? specRat[0] : specRat[1];
  ratFrame->GetYaxis()->SetRangeUser(0.0, 2.0);
  ratFrame->GetYaxis()->SetTitle("MC / Data");
  ratFrame->SetTitle("");
  ratFrame->Draw("P");
  grSRat->Draw("E2 SAME");
  l->Draw("same");
  for (int v = 0; v < NVERS; v++)
  {
    if (specRat[v]) specRat[v]->Draw("P SAME");
  }

  c1->SaveAs((inDir + "/pip_spec.pdf").c_str());
}