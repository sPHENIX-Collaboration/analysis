

#include "SaveCanvas.C"
#include "sPhenixStyle.C"

#include <TCanvas.h>
#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TLegend.h>
#include <TLine.h>
#include <TLorentzVector.h>
#include <TRandom3.h>
#include <TString.h>
#include <TTree.h>
#include <TVirtualFitter.h>

#include <assert.h>

#include <cmath>
#include <cstddef>
#include <iostream>

using namespace std;

TFile *_file0 = NULL;
TFile *_file_trigger = NULL;
TString description;

typedef vector<vector<TH1 *> > chipMultiplicitySet_vec;

chipMultiplicitySet_vec MakeChipMultiplicitySet(TFile *file)
{
  assert(file);

  TH3 *LayerChipMultiplicity = (TH3 *) file->GetObjectChecked("LayerChipMultiplicity", "TH3");

  TCanvas *c1 = new TCanvas("MakeChipMultiplicitySet", "MakeChipMultiplicitySet", 1800, 960);
  c1->Divide(9, 3, 0, 0);
  int idx = 1;
  TPad *p;

  chipMultiplicitySet_vec chipMultiplicitySet;

  for (int ilayer = 1; ilayer <= LayerChipMultiplicity->GetNbinsX(); ++ilayer)
  {
    vector<TH1 *> chipMultiplicityLayer;

    for (int ichip = 1; ichip <= LayerChipMultiplicity->GetNbinsY(); ++ichip)
    {
      p = (TPad *) c1->cd(idx++);
      c1->Update();
      p->SetLogy();

      LayerChipMultiplicity->GetXaxis()->SetRange(ilayer, ilayer);
      LayerChipMultiplicity->GetYaxis()->SetRange(ichip, ichip);

      TString histname(Form("ChipMultiplicity_Layer%dChip%d", ilayer - 1, ichip - 1));
      TH1 *h = LayerChipMultiplicity->Project3D("z");
      h->SetTitle(";Chip Multiplicity [pixel];Count");

      h->SetName(histname);

      h->Draw();

      h->SetDirectory(NULL);
      h->ComputeIntegral(true);

      chipMultiplicityLayer.push_back(h);
    }

    chipMultiplicitySet.push_back(chipMultiplicityLayer);
  }

  SaveCanvas(c1, TString(file->GetName()) + TString(c1->GetName()), false);

  c1 = new TCanvas("MakeChipMultiplicitySetChip4", "MakeChipMultiplicitySetChip4", 1200, 960);
  gPad->SetLogy();
  TH1 *LayerMultiplicityLayer0 = chipMultiplicitySet[0][4];
  TH1 *LayerMultiplicityLayer1 = chipMultiplicitySet[1][4];
  TH1 *LayerMultiplicityLayer2 = chipMultiplicitySet[2][4];

  LayerMultiplicityLayer0->SetLineColor(kRed + 2);
  LayerMultiplicityLayer1->SetLineColor(kBlue + 2);
  LayerMultiplicityLayer2->SetLineColor(kGreen + 2);

  LayerMultiplicityLayer0->Draw();
  LayerMultiplicityLayer1->Draw("same");
  LayerMultiplicityLayer2->Draw("same");

  LayerMultiplicityLayer0->SetTitle(";Chip multiplicity [Pixel];Count");

  TLegend *leg = new TLegend(.5, .5, .93, .93);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  leg->AddEntry("", description, "");
  leg->AddEntry(LayerMultiplicityLayer0, Form("MVTX Layer0 Chip4, <hit> = %.1f", LayerMultiplicityLayer0->GetMean()), "l");
  leg->AddEntry(LayerMultiplicityLayer1, Form("MVTX Layer1 Chip4, <hit> = %.1f", LayerMultiplicityLayer1->GetMean()), "l");
  leg->AddEntry(LayerMultiplicityLayer2, Form("MVTX Layer2 Chip4, <hit> = %.1f", LayerMultiplicityLayer2->GetMean()), "l");
  leg->Draw();

  SaveCanvas(c1, TString(file->GetName()) + TString(c1->GetName()), false);

  return chipMultiplicitySet;
}

void Check(TFile *file)
{
  assert(file);

  TCanvas *c1 = new TCanvas("Check", "Check", 1800, 960);
  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogz();

  TH1 *hNormalization = (TH1 *) file->GetObjectChecked("hNormalization", "TH1");
  hNormalization->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  TH1 *hNChEta = (TH1 *) file->GetObjectChecked("hNChEta", "TH1");
  assert(hNChEta);
  double norm = hNChEta->GetBinWidth(1) * hNormalization->GetBinContent(2);
  hNChEta->Scale(1. / norm);
  hNChEta->Draw();

  hNChEta->GetYaxis()->SetTitle("dN_{Ch}/d#eta");
  //  hNormalization->Draw()

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  TH1 *hVertexZ = (TH1 *) file->GetObjectChecked("hVertexZ", "TH1");
  hVertexZ->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH2 *LayerMultiplicity = (TH2 *) file->GetObjectChecked("LayerMultiplicity", "TH2");
  TH1 *LayerMultiplicityLayer0 =
      LayerMultiplicity->ProjectionY("LayerMultiplicityLayer0", 1, 1);
  TH1 *LayerMultiplicityLayer1 =
      LayerMultiplicity->ProjectionY("LayerMultiplicityLayer1", 2, 2);
  TH1 *LayerMultiplicityLayer2 =
      LayerMultiplicity->ProjectionY("LayerMultiplicityLayer2", 3, 3);

  LayerMultiplicityLayer0->SetLineColor(kRed + 2);
  LayerMultiplicityLayer1->SetLineColor(kBlue + 2);
  LayerMultiplicityLayer2->SetLineColor(kGreen + 2);

  LayerMultiplicityLayer0->Draw();
  LayerMultiplicityLayer1->Draw("same");
  LayerMultiplicityLayer2->Draw("same");

  LayerMultiplicityLayer0->SetTitle(";Chip multiplicity [Pixel];Count");

  TLegend *leg = new TLegend(.5, .5, .93, .93);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  leg->AddEntry("", description, "");
  leg->AddEntry(LayerMultiplicityLayer0, Form("MVTX Layer0, <hit> = %.1f", LayerMultiplicityLayer0->GetMean()), "l");
  leg->AddEntry(LayerMultiplicityLayer1, Form("MVTX Layer1, <hit> = %.1f", LayerMultiplicityLayer1->GetMean()), "l");
  leg->AddEntry(LayerMultiplicityLayer2, Form("MVTX Layer2, <hit> = %.1f", LayerMultiplicityLayer2->GetMean()), "l");
  leg->Draw();

  SaveCanvas(c1, TString(file->GetName()) + TString(c1->GetName()), false);
}

TH1 *MakeCDF(TH1 *h)
{
  assert(h);

  TH1 *hCDF = (TH1 *) h->Clone(TString("CDF") + h->GetName());
  hCDF->SetDirectory(NULL);

  double integral = 0;

  for (int bin = h->GetNbinsX() + 1; bin >= 0; --bin)
  {
    integral += h->GetBinContent(bin);
    hCDF->SetBinContent(bin, integral);
  }

  for (int bin = h->GetNbinsX(); bin >= 1; --bin)
  {
    hCDF->SetBinContent(bin, hCDF->GetBinContent(bin) / integral);
  }

  return hCDF;
}

chipMultiplicitySet_vec TriggerMultiplicity(chipMultiplicitySet_vec cm_MB, chipMultiplicitySet_vec cm_Trigger, const double mu_MB, const int n_Trigger, const double mu_Noise)
{
  //init
  chipMultiplicitySet_vec cm;

  const static int MaxMultiplicity(2000);

  for (auto &layer : cm_MB)
  {
    vector<TH1 *> chipMultiplicityLayer;

    for (auto &chip : layer)
    {
      TH1 *h = new TH1D(
          TString("Trigger") + chip->GetName(), TString("Trigger") + chip->GetName(),
          MaxMultiplicity, -.5, MaxMultiplicity - .5);
      //          (TH1 *) chip->Clone(TString("Trigger") + chip->GetName());

      h->SetDirectory(NULL);
      h->GetXaxis()->SetTitle(chip->GetXaxis()->GetTitle());
      h->GetYaxis()->SetTitle(chip->GetYaxis()->GetTitle());

      assert(h);
      chipMultiplicityLayer.push_back(h);
    }
    cm.push_back(chipMultiplicityLayer);
  }

  //composition for trigger
  TRandom3 rnd(1234);
  assert(mu_MB >= 0);
  assert(n_Trigger >= 0 && n_Trigger <= 1);
  assert(mu_Noise >= 0);

  //  const int NSample = 1000000;
  const int NSample = 10000000;
  for (int i = 0; i < NSample; ++i)
  {
    if (i % (NSample / 10) == 0)
    {
      cout << "TriggerMultiplicity - " << i << endl;
    }

    int n_MB = rnd.Poisson(mu_MB);
    int n_Noise = rnd.Poisson(mu_Noise);

    int ilayer = 0;
    for (auto &layer : cm)
    {
      int ichip = 0;
      for (auto &chip : layer)
      {
        assert(chip);
        int n_hit = n_Noise;

        assert(ilayer < cm_Trigger.size());
        assert(ichip < cm_Trigger[ilayer].size());
        assert(cm_Trigger[ilayer][ichip]);
        if (n_Trigger)
        {
          double rndHit = cm_Trigger[ilayer][ichip]->GetRandom();
          if (rndHit < .5) rndHit = 0;

          //          cout << "n_hit += " << cm_Trigger[ilayer][ichip]->GetRandom() << endl;
          n_hit += rndHit;
        }

        assert(ilayer < cm_MB.size());
        assert(ichip < cm_MB[ilayer].size());
        assert(cm_MB[ilayer][ichip]);
        for (int iMB = 0; iMB < n_MB; ++iMB)
        {
          double rndHit = cm_MB[ilayer][ichip]->GetRandom();
          if (rndHit < .5) rndHit = 0;
          //          cout << "n_hit +=" << cm_MB[ilayer][ichip]->GetRandom() << endl;
          n_hit += rndHit;
        }

        //        cout << "chip->Fill(n_hit);" << n_hit << ", n_Noise = " << n_Noise << endl;
        chip->Fill(n_hit);

        ++ichip;
      }
      ++ilayer;
    }
  }

  TString UniqueName(Form("_muMB%.0f_nTrig%d_muNoise%.0f", mu_MB * 100, n_Trigger, mu_Noise));

  //save - plot
  TCanvas *c1 = new TCanvas("TriggerMultiplicity" + UniqueName, "TriggerMultiplicity" + UniqueName, 1800, 960);
  c1->Divide(9, 3, 0, 0);
  int idx = 1;
  TPad *p;

  for (auto &layer : cm)
  {
    vector<TH1 *> chipMultiplicityLayer;

    for (auto &chip : layer)
    {
      p = (TPad *) c1->cd(idx++);
      c1->Update();
      p->SetLogy();

      chip->SetTitle(";Chip Multiplicity [pixel];Count");
      chip->Draw();
    }
  }

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), false);

  c1 = new TCanvas("TriggerMultiplicityChip4" + UniqueName, "TriggerMultiplicityChip4" + UniqueName, 1900, 750);
  c1->Divide(2, 1);
  idx = 1;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 *LayerMultiplicityLayer0 = cm[0][4];
  TH1 *LayerMultiplicityLayer1 = cm[1][4];
  TH1 *LayerMultiplicityLayer2 = cm[2][4];

  LayerMultiplicityLayer0->SetLineColor(kRed + 2);
  LayerMultiplicityLayer1->SetLineColor(kBlue + 2);
  LayerMultiplicityLayer2->SetLineColor(kGreen + 2);

  LayerMultiplicityLayer0->Draw();
  LayerMultiplicityLayer1->Draw("same");
  LayerMultiplicityLayer2->Draw("same");

  LayerMultiplicityLayer0->SetTitle(";Chip multiplicity [Pixel];Count");

  TLegend *leg = new TLegend(.45, .55, .93, .93);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  leg->AddEntry("", description, "");
  leg->AddEntry("", Form("#mu_{MB} = %.1f, N_{Trig} = %d, #mu_{Noise} = %.1f", mu_MB, n_Trigger, mu_Noise), "");
  leg->AddEntry(LayerMultiplicityLayer0, Form("MVTX Layer0 Chip4, <hit> = %.1f", LayerMultiplicityLayer0->GetMean()), "l");
  leg->AddEntry(LayerMultiplicityLayer1, Form("MVTX Layer1 Chip4, <hit> = %.1f", LayerMultiplicityLayer1->GetMean()), "l");
  leg->AddEntry(LayerMultiplicityLayer2, Form("MVTX Layer2 Chip4, <hit> = %.1f", LayerMultiplicityLayer2->GetMean()), "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  p->DrawFrame(0, 1. / NSample, MaxMultiplicity, 1)->SetTitle(";Chip multiplicity [Pixel];CCDF");
  TLine *line = new TLine(0, 1e-3, MaxMultiplicity, 1e-3);
  line->SetLineColor(kGray);
  line->SetLineWidth(3);
  line->Draw("same");

  MakeCDF(LayerMultiplicityLayer0)->Draw("same");
  MakeCDF(LayerMultiplicityLayer1)->Draw("same");
  MakeCDF(LayerMultiplicityLayer2)->Draw("same");

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), false);

  return cm;
}

void OccupancySim(                                                                                                                                                     //
                                                                                                                                                                       //    const TString infile = "/sphenix/user/jinhuang/HF-jet/MVTX_Multiplicity/pp200MB_30cmVZ_Iter5/pp200MB_30cmVZ_Iter5_SUM.cfg_HFMLTriggerOccupancy.root",          //
                                                                                                                                                                       //    const TString infile_trigger = "/sphenix/user/jinhuang/HF-jet/MVTX_Multiplicity/pp200MB_30cmVZ_Iter5/pp200MB_30cmVZ_Iter5_SUM.cfg_HFMLTriggerOccupancy.root",  //
                                                                                                                                                                       //    const TString disc = "p+p MB, #sqrt{s} = 200 GeV"                                                                                                              //
    const TString infile = "/sphenix/user/jinhuang/HF-jet/MVTX_Multiplicity/AuAu200MB_30cmVZ_Iter5/AuAu200MB_30cmVZ_Iter5_SUM.xml_HFMLTriggerOccupancy.root",          //
    const TString infile_trigger = "/sphenix/user/jinhuang/HF-jet/MVTX_Multiplicity/AuAu200MB_10cmVZ_Iter5/AuAu200MB_10cmVZ_Iter5_SUM.xml_HFMLTriggerOccupancy.root",  //
    const TString disc = "Au+Au MB, #sqrt{s_{NN}} = 200 GeV"                                                                                                           //
)
{
  SetsPhenixStyle();
  TVirtualFitter::SetDefaultFitter("Minuit2");

  description = disc;
  _file0 = new TFile(infile);
  assert(_file0->IsOpen());
  _file_trigger = new TFile(infile_trigger);
  assert(_file_trigger->IsOpen());

  _file0->cd();
  Check(_file0);
  chipMultiplicitySet_vec chipMultiplicity = MakeChipMultiplicitySet(_file0);

  _file_trigger->cd();
  Check(_file_trigger);
  chipMultiplicitySet_vec chipMultiplicityTrigger = MakeChipMultiplicitySet(_file_trigger);

  // AuAu
  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
                      200e3 * 10e-6, 1, 1024 * 512 * 1e-6 + 2);

  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
                      200e3 * 15e-6, 0, 1024 * 512 * 1e-6 + 2);

  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
                      200e3 * 10e-6, 0, 1024 * 512 * 1e-6 + 2);

  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
                      200e3 * 5e-6, 0, 1024 * 512 * 1e-6 + 2);

  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
                      100e3 * 5e-6, 0, 1024 * 512 * 1e-6 + 2);

  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
                      50e3 * 5e-6, 0, 1024 * 512 * 1e-6 + 2);

  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
                      200e3 * 15e-6, 0, 1024 * 512 * 1e-5 + 2);

  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
                      200e3 * 10e-6, 0, 1024 * 512 * 1e-5 + 2);

  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
                      200e3 * 5e-6, 0, 1024 * 512 * 1e-5 + 2);

  //  //   pp
  //  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
  //                      13e6 * 10e-6, 1, 1024 * 512 * 1e-6);
  //
  //  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
  //                      13e6 * 15e-6, 0, 1024 * 512 * 1e-6);
  //  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
  //                      13e6 * 10e-6, 0, 1024 * 512 * 1e-6);
  //  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
  //                      13e6 * 5e-6, 0, 1024 * 512 * 1e-6);
  //
  //  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
  //                      13e6 * 15e-6, 0, 1024 * 512 * 1e-5);
  //  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
  //                      13e6 * 10e-6, 0, 1024 * 512 * 1e-5);
  //  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
  //                      13e6 * 5e-6, 0, 1024 * 512 * 1e-5);
  //  TriggerMultiplicity(chipMultiplicity, chipMultiplicityTrigger,
  //                      4e6 * 5e-6, 0, 1024 * 512 * 1e-5);

  //
}
