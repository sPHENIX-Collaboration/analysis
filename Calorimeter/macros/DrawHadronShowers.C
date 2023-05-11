#include <TFile.h>
#include <TLine.h>
#include <TString.h>
#include <TTree.h>
#include <cassert>
#include <cmath>
#include "SaveCanvas.C"
#include "sPhenixStyle.C"
using namespace std;

class lin_res
{
 public:
  TString name;
  TGraphErrors *linearity;
  TGraphErrors *resolution;
  TF1 *f_res;
};



//TString base_dataset = "/phenix/u/jinhuang/links/sPHENIX_work/prod_analysis/hadron_shower_res_nightly/";
//TString base_dataset = "/phenix/u/jinhuang/links/sPHENIX_work/prod_analysis/hadron_shower_res_hcaldigi/";
//TString base_dataset = "/phenix/u/jinhuang/links/sPHENIX_work/prod_analysis/hadron_shower_res_hcaldigi_sampling/";
//TString base_dataset ="/phenix/u/jinhuang/links/ePHENIX_work/sPHENIX_work/production_analysis_updates/spacal1d/fieldmap";
TString base_dataset = "/sphenix/u/weihuma/analysis/Calorimeter/macros/";

void DrawHadronShowers(void)
{
  SetsPhenixStyle();
  TVirtualFitter::SetDefaultFitter("Minuit2");

  ClusterSizeScan("eta0", "0<|#eta|<0.1");
  //  ClusterSizeScan("eta0.60","0.6<|#eta|<0.7");
 // PIDScan();
  //EtaScan();
}

void EtaScan(void)
{
  const TString config = "Single #pi^{-}, Track-based 5x5 clusterizer";

  lin_res *QA_Eta0 =
      GetResolution("pi-", "eta0", "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP");
  QA_Eta0->name = "0<|eta|<0.1";

  lin_res *QA_Eta3 =
      GetResolution("pi-", "eta0.30", "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP");
  QA_Eta3->name = "0.3<|eta|<0.4";

  lin_res *QA_Eta6 =
      GetResolution("pi-", "eta0.60", "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP");
  QA_Eta6->name = "0.6<|eta|<0.7";

  lin_res *QA_Eta9 =
      GetResolution("pi-", "eta0.90", "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP");
  QA_Eta9->name = "0.9<|eta|<1.0";

  // ------------
  QA_Eta0->resolution->SetMarkerStyle(kFullCircle);
  QA_Eta0->resolution->SetMarkerColor(kRed + 3);
  QA_Eta0->resolution->SetLineColor(kRed + 3);

  QA_Eta0->linearity->SetMarkerStyle(kFullCircle);
  QA_Eta0->linearity->SetMarkerColor(kRed + 3);
  QA_Eta0->linearity->SetLineColor(kRed + 3);

  QA_Eta0->f_res->SetLineColor(kRed + 3);

  // ------------
  QA_Eta3->resolution->SetMarkerStyle(kFullSquare);
  QA_Eta3->resolution->SetMarkerColor(kBlue + 3);
  QA_Eta3->resolution->SetLineColor(kBlue + 3);

  QA_Eta3->linearity->SetMarkerStyle(kFullSquare);
  QA_Eta3->linearity->SetMarkerColor(kBlue + 3);
  QA_Eta3->linearity->SetLineColor(kBlue + 3);

  QA_Eta3->f_res->SetLineColor(kBlue + 3);

  // ------------
  QA_Eta6->resolution->SetMarkerStyle(kFullCross);
  QA_Eta6->resolution->SetMarkerColor(kMagenta + 3);
  QA_Eta6->resolution->SetLineColor(kMagenta + 3);

  QA_Eta6->linearity->SetMarkerStyle(kFullCross);
  QA_Eta6->linearity->SetMarkerColor(kMagenta + 3);
  QA_Eta6->linearity->SetLineColor(kMagenta + 3);

  QA_Eta6->f_res->SetLineColor(kMagenta + 3);
  QA_Eta6->f_res->SetLineStyle(kDashed);

  // ------------
  QA_Eta9->resolution->SetMarkerStyle(kStar);
  QA_Eta9->resolution->SetMarkerColor(kCyan + 3);
  QA_Eta9->resolution->SetLineColor(kCyan + 3);

  QA_Eta9->linearity->SetMarkerStyle(kStar);
  QA_Eta9->linearity->SetMarkerColor(kCyan + 3);
  QA_Eta9->linearity->SetLineColor(kCyan + 3);

  QA_Eta9->f_res->SetLineColor(kCyan + 3);
  QA_Eta9->f_res->SetLineStyle(kDashed);

  TCanvas *c1 = new TCanvas(Form("EtaScan"),
                            Form("EtaScan"), 1300, 600);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  p->DrawFrame(0, 0, 60, 1.2,
               Form("Linearity;Truth energy [GeV];Measured Energy / Truth energy"));
  TLine *l = new TLine(0, 1, 60, 1);
  l->SetLineWidth(2);
  l->SetLineColor(kGray);
  l->Draw();

  QA_Eta0->linearity->Draw("ep");
  QA_Eta3->linearity->Draw("ep");
  QA_Eta6->linearity->Draw("ep");
  QA_Eta9->linearity->Draw("ep");

  TLegend *leg = new TLegend(.2, .2, .85, .45);
  leg->SetHeader("#splitline{#it{#bf{sPHENIX}}  Geant4 Simulation}{" + config + "}");
  leg->AddEntry("", "", "");
  leg->AddEntry(QA_Eta0->linearity, QA_Eta0->name, "ep");
  leg->AddEntry(QA_Eta3->linearity, QA_Eta3->name, "ep");
  leg->AddEntry(QA_Eta6->linearity, QA_Eta6->name, "ep");
  leg->AddEntry(QA_Eta9->linearity, QA_Eta9->name, "ep");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  TF1 *f_calo_t1044 = new TF1("f_calo_t1044", "sqrt([0]*[0]+[1]*[1]/x)/100", 6,
                              32);
  f_calo_t1044->SetParameters(13.5, 64.9);
  f_calo_t1044->SetLineWidth(3);
  f_calo_t1044->SetLineColor(kGreen + 2);

  TH1 *hframe = p->DrawFrame(0, 0, 60, 0.7,
                             Form("Resolution;Truth energy [GeV];#DeltaE/<E>"));

  QA_Eta0->f_res->Draw("same");
  QA_Eta0->resolution->Draw("ep");

  QA_Eta3->f_res->Draw("same");
  QA_Eta3->resolution->Draw("ep");

  QA_Eta6->f_res->Draw("same");
  QA_Eta6->resolution->Draw("ep");

  QA_Eta9->f_res->Draw("same");
  QA_Eta9->resolution->Draw("ep");

  f_calo_t1044->Draw("same");

  TLegend *leg = new TLegend(.2, .6, .85, .9);
  leg->SetHeader("#splitline{#it{#bf{sPHENIX}}  Geant4 Simulation}{" + config + "}");
  leg->AddEntry("", "", "");
  leg->AddEntry(QA_Eta0->linearity,
                QA_Eta0->name + "      " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QA_Eta0->f_res->GetParameter(0),
                         QA_Eta0->f_res->GetParameter(1)),
                "lpe");
  leg->AddEntry(f_calo_t1044,
                TString("T-1044 (#eta=0)") + "   " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         f_calo_t1044->GetParameter(0),
                         f_calo_t1044->GetParameter(1)),
                "l");
  leg->AddEntry(QA_Eta3->linearity,
                QA_Eta3->name + "  " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QA_Eta3->f_res->GetParameter(0),
                         QA_Eta3->f_res->GetParameter(1)),
                "lpe");
  leg->AddEntry(QA_Eta6->linearity,
                QA_Eta6->name + "  " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QA_Eta6->f_res->GetParameter(0),
                         QA_Eta6->f_res->GetParameter(1)),
                "lpe");
  leg->AddEntry(QA_Eta9->linearity,
                QA_Eta9->name + "  " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QA_Eta9->f_res->GetParameter(0),
                         QA_Eta9->f_res->GetParameter(1)),
                "lpe");
  leg->Draw();

  SaveCanvas(c1,
             base_dataset + "DrawHadronShowers_" + TString(c1->GetName()), kTRUE);
}

void PIDScan(void)
{
  const TString config = "Single Particles, 0<|#eta|<0.1, Track-based 5x5 clusterizer";

  lin_res *QA_electron =
      GetResolution("e-", "eta0", "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP");
  QA_electron->name = "Electron";

  lin_res *QA_PiNeg =
      GetResolution("pi-", "eta0", "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP");
  QA_PiNeg->name = "#pi^{-}";

  lin_res *QA_PiPos =
      GetResolution("pi\\+", "eta0", "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP");
  QA_PiPos->name = "#pi^{+}";

  lin_res *QA_Proton =
      GetResolution("proton", "eta0", "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP");
  QA_Proton->name = "Proton";

  // ------------
  QA_electron->resolution->SetMarkerStyle(kFullCircle);
  QA_electron->resolution->SetMarkerColor(kRed + 3);
  QA_electron->resolution->SetLineColor(kRed + 3);

  QA_electron->linearity->SetMarkerStyle(kFullCircle);
  QA_electron->linearity->SetMarkerColor(kRed + 3);
  QA_electron->linearity->SetLineColor(kRed + 3);

  QA_electron->f_res->SetLineColor(kRed + 3);

  // ------------
  QA_PiNeg->resolution->SetMarkerStyle(kFullSquare);
  QA_PiNeg->resolution->SetMarkerColor(kBlue + 3);
  QA_PiNeg->resolution->SetLineColor(kBlue + 3);

  QA_PiNeg->linearity->SetMarkerStyle(kFullSquare);
  QA_PiNeg->linearity->SetMarkerColor(kBlue + 3);
  QA_PiNeg->linearity->SetLineColor(kBlue + 3);

  QA_PiNeg->f_res->SetLineColor(kBlue + 3);

  // ------------
  QA_PiPos->resolution->SetMarkerStyle(kFullCross);
  QA_PiPos->resolution->SetMarkerColor(kMagenta + 3);
  QA_PiPos->resolution->SetLineColor(kMagenta + 3);

  QA_PiPos->linearity->SetMarkerStyle(kFullCross);
  QA_PiPos->linearity->SetMarkerColor(kMagenta + 3);
  QA_PiPos->linearity->SetLineColor(kMagenta + 3);

  QA_PiPos->f_res->SetLineColor(kMagenta + 3);
  QA_PiPos->f_res->SetLineStyle(kDashed);

  // ------------
  QA_Proton->resolution->SetMarkerStyle(kStar);
  QA_Proton->resolution->SetMarkerColor(kCyan + 3);
  QA_Proton->resolution->SetLineColor(kCyan + 3);

  QA_Proton->linearity->SetMarkerStyle(kStar);
  QA_Proton->linearity->SetMarkerColor(kCyan + 3);
  QA_Proton->linearity->SetLineColor(kCyan + 3);

  QA_Proton->f_res->SetLineColor(kCyan + 3);
  QA_Proton->f_res->SetLineStyle(kDashed);

  TCanvas *c1 = new TCanvas(Form("PIDScan"),
                            Form("PIDScan"), 1300, 600);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  p->DrawFrame(0, 0, 60, 1.2,
               Form("Linearity;Truth energy [GeV];Measured Energy / Truth energy"));
  TLine *l = new TLine(0, 1, 60, 1);
  l->SetLineWidth(2);
  l->SetLineColor(kGray);

  l->Draw();

  QA_electron->linearity->Draw("ep");
  QA_PiNeg->linearity->Draw("ep");
  QA_PiPos->linearity->Draw("ep");
  QA_Proton->linearity->Draw("ep");

  TLegend *leg = new TLegend(.2, .2, 1, .45);
  leg->SetHeader("#splitline{#it{#bf{sPHENIX}}  Geant4 Simulation}{" + config + "}");
  leg->AddEntry("", "", "");
  leg->AddEntry(QA_electron->linearity, QA_electron->name, "ep");
  leg->AddEntry(QA_PiNeg->linearity, QA_PiNeg->name, "ep");
  leg->AddEntry(QA_PiPos->linearity, QA_PiPos->name, "ep");
  leg->AddEntry(QA_Proton->linearity, QA_Proton->name, "ep");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  TF1 *f_calo_t1044 = new TF1("f_calo_t1044", "sqrt([0]*[0]+[1]*[1]/x)/100", 6,
                              32);
  f_calo_t1044->SetParameters(13.5, 64.9);
  f_calo_t1044->SetLineWidth(3);
  f_calo_t1044->SetLineColor(kGreen + 2);

  TH1 *hframe = p->DrawFrame(0, 0, 60, 0.7,
                             Form("Resolution;Truth energy [GeV];#DeltaE/<E>"));

  QA_electron->f_res->Draw("same");
  QA_electron->resolution->Draw("ep");

  QA_PiNeg->f_res->Draw("same");
  QA_PiNeg->resolution->Draw("ep");

  QA_PiPos->f_res->Draw("same");
  QA_PiPos->resolution->Draw("ep");

  QA_Proton->f_res->Draw("same");
  QA_Proton->resolution->Draw("ep");

  f_calo_t1044->Draw("same");

  TLegend *leg = new TLegend(.2, .6, 1, .9);
  leg->SetHeader("#splitline{#it{#bf{sPHENIX}}  Geant4 Simulation}{" + config + "}");
  leg->AddEntry("", "", "");
  leg->AddEntry(QA_electron->linearity,
                QA_electron->name + "      " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QA_electron->f_res->GetParameter(0),
                         QA_electron->f_res->GetParameter(1)),
                "lpe");
  leg->AddEntry(f_calo_t1044,
                TString("T-1044 Data, #pi^{-}") + "  " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         f_calo_t1044->GetParameter(0),
                         f_calo_t1044->GetParameter(1)),
                "l");
  leg->AddEntry(QA_PiNeg->linearity,
                QA_PiNeg->name + "      " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QA_PiNeg->f_res->GetParameter(0),
                         QA_PiNeg->f_res->GetParameter(1)),
                "lpe");
  leg->AddEntry(QA_PiPos->linearity,
                QA_PiPos->name + "  " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QA_PiPos->f_res->GetParameter(0),
                         QA_PiPos->f_res->GetParameter(1)),
                "lpe");
  leg->AddEntry(QA_Proton->linearity,
                QA_Proton->name + "  " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QA_Proton->f_res->GetParameter(0),
                         QA_Proton->f_res->GetParameter(1)),
                "lpe");
  leg->Draw();

  SaveCanvas(c1,
             base_dataset + "DrawHadronShowers_" + TString(c1->GetName()), kTRUE);
}

void ClusterSizeScan(TString seta = "eta0", TString eta_desc = "0<|#eta|<0.1")
{
  const TString config = "Single #pi^{-}, " + eta_desc;

  lin_res *QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP =
      GetResolution("pi-", seta, "h_QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP");
  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->name = "3x3 cluster";

  lin_res *QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP =
      GetResolution("pi-", seta, "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP");
  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->name = "5x5 cluster";

  lin_res *QAG4SimJet_AntiKt_Tower_r02_Leading_Et =
      GetResolution("pi-", seta, "h_QAG4SimJet_AntiKt_Tower_r02_Leading_Et");
  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->name = "Anti-k_{T} R=0.2";

  lin_res *QAG4SimJet_AntiKt_Tower_r04_Leading_Et =
      GetResolution("pi-", seta, "h_QAG4SimJet_AntiKt_Tower_r04_Leading_Et");
  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->name = "Anti-k_{T} R=0.4";

  // ------------
  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->resolution->SetMarkerStyle(kFullCircle);
  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->resolution->SetMarkerColor(kRed + 3);
  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->resolution->SetLineColor(kRed + 3);

  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->linearity->SetMarkerStyle(kFullCircle);
  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->linearity->SetMarkerColor(kRed + 3);
  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->linearity->SetLineColor(kRed + 3);

  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->f_res->SetLineColor(kRed + 3);

  // ------------
  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->resolution->SetMarkerStyle(kFullSquare);
  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->resolution->SetMarkerColor(kBlue + 3);
  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->resolution->SetLineColor(kBlue + 3);

  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->linearity->SetMarkerStyle(kFullSquare);
  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->linearity->SetMarkerColor(kBlue + 3);
  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->linearity->SetLineColor(kBlue + 3);

  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->f_res->SetLineColor(kBlue + 3);

  // ------------
  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->resolution->SetMarkerStyle(kFullCross);
  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->resolution->SetMarkerColor(kMagenta + 3);
  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->resolution->SetLineColor(kMagenta + 3);

  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->linearity->SetMarkerStyle(kFullCross);
  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->linearity->SetMarkerColor(kMagenta + 3);
  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->linearity->SetLineColor(kMagenta + 3);

  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->f_res->SetLineColor(kMagenta + 3);
  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->f_res->SetLineStyle(kDashed);

  // ------------
  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->resolution->SetMarkerStyle(kStar);
  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->resolution->SetMarkerColor(kCyan + 3);
  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->resolution->SetLineColor(kCyan + 3);

  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->linearity->SetMarkerStyle(kStar);
  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->linearity->SetMarkerColor(kCyan + 3);
  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->linearity->SetLineColor(kCyan + 3);

  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->f_res->SetLineColor(kCyan + 3);
  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->f_res->SetLineStyle(kDashed);

  TCanvas *c1 = new TCanvas(Form("ClusterSizeScan_") + seta,
                            Form("ClusterSizeScan_") + seta, 1300, 600);
  c1->Divide(2, 1);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  //  p->SetLogy();

  p->DrawFrame(0, 0, 60, 1.2,
               Form("Linearity;Truth energy [GeV];Measured Energy / Truth energy"));
  TLine *l = new TLine(0, 1, 60, 1);
  l->SetLineWidth(2);
  l->SetLineColor(kGray);
  l->Draw();

  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->linearity->Draw("ep");
  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->linearity->Draw("ep");
  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->linearity->Draw("ep");
  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->linearity->Draw("ep");

  TLegend *leg = new TLegend(.2, .2, .85, .45);
  leg->SetHeader("#splitline{#it{#bf{sPHENIX}}  Geant4 Simulation}{" + config + "}");
  leg->AddEntry("", "", "");
  leg->AddEntry(QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->linearity, QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->name, "ep");
  leg->AddEntry(QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->linearity, QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->name, "ep");
  leg->AddEntry(QAG4SimJet_AntiKt_Tower_r02_Leading_Et->linearity, QAG4SimJet_AntiKt_Tower_r02_Leading_Et->name, "ep");
  leg->AddEntry(QAG4SimJet_AntiKt_Tower_r04_Leading_Et->linearity, QAG4SimJet_AntiKt_Tower_r04_Leading_Et->name, "ep");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  TF1 *f_calo_t1044 = new TF1("f_calo_t1044", "sqrt([0]*[0]+[1]*[1]/x)/100", 6,
                              32);
  f_calo_t1044->SetParameters(13.5, 64.9);
  f_calo_t1044->SetLineWidth(3);
  f_calo_t1044->SetLineColor(kGreen + 2);

  TH1 *hframe = p->DrawFrame(0, 0, 60, 0.7,
                             Form("Resolution;Truth energy [GeV];#DeltaE/<E>"));

  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->f_res->Draw("same");
  QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->resolution->Draw("ep");

  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->f_res->Draw("same");
  QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->resolution->Draw("ep");

  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->f_res->Draw("same");
  QAG4SimJet_AntiKt_Tower_r02_Leading_Et->resolution->Draw("ep");

  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->f_res->Draw("same");
  QAG4SimJet_AntiKt_Tower_r04_Leading_Et->resolution->Draw("ep");

  f_calo_t1044->Draw("same");

  TLegend *leg = new TLegend(.2, .6, .85, .9);
  leg->SetHeader("#splitline{#it{#bf{sPHENIX}}  Geant4 Simulation}{" + config + "}");
  leg->AddEntry("", "", "");
  leg->AddEntry(QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->linearity,
                QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->name + "      " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->f_res->GetParameter(0),
                         QAG4Sim_CalorimeterSum_TrackProj_3x3Tower_EP->f_res->GetParameter(1)),
                "lpe");
  leg->AddEntry(f_calo_t1044,
                TString("T-1044 (4x4, #eta=0)") + " " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         f_calo_t1044->GetParameter(0),
                         f_calo_t1044->GetParameter(1)),
                "l");
  leg->AddEntry(QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->linearity,
                QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->name + "      " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->f_res->GetParameter(0),
                         QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP->f_res->GetParameter(1)),
                "lpe");
  leg->AddEntry(QAG4SimJet_AntiKt_Tower_r02_Leading_Et->linearity,
                QAG4SimJet_AntiKt_Tower_r02_Leading_Et->name + "  " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QAG4SimJet_AntiKt_Tower_r02_Leading_Et->f_res->GetParameter(0),
                         QAG4SimJet_AntiKt_Tower_r02_Leading_Et->f_res->GetParameter(1)),
                "lpe");
  leg->AddEntry(QAG4SimJet_AntiKt_Tower_r04_Leading_Et->linearity,
                QAG4SimJet_AntiKt_Tower_r04_Leading_Et->name + "  " +
                    Form("%.1f%% #oplus %.1f%%/#sqrt{E}",
                         QAG4SimJet_AntiKt_Tower_r04_Leading_Et->f_res->GetParameter(0),
                         QAG4SimJet_AntiKt_Tower_r04_Leading_Et->f_res->GetParameter(1)),
                "lpe");
  leg->Draw();

  SaveCanvas(c1,
             base_dataset + "DrawHadronShowers_" + TString(c1->GetName()), kTRUE);
}

lin_res *
GetResolution(TString PID = "pi-", TString eta = "eta0", TString qa_histo = "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP")
{
  TString dataset_name = PID + "_" + eta + "_" + qa_histo;

  const int N = 6;
  const double Ps[] = {8, 12, 16, 32, 40, 50, -1, -1};

  vector<double> mean;
  vector<double> mean_err;
  vector<double> res;
  vector<double> res_err;

  TCanvas *c1 = new TCanvas("GetResolution_LineShape_" + dataset_name,
                            "GetResolution_LineShape_" + dataset_name, 1800, 900);

  c1->Divide(3, 2);
  int idx = 1;
  TPad *p;

 // for (int i = 0; i < N; ++i)
  for (int i = 2; i < 3; ++i) //for 16GeV
  {
  // if(i==2){ //for 16GeV

    p = (TPad *) c1->cd(idx++);
    c1->Update();

    const double P = Ps[i];

    TString sEnergy = Form("%.0fGeV", P);

    TH1 *h = NULL;

    //if (qa_histo.Length() == 0)
      h = GetEvalHisto(PID, eta, sEnergy);
  //  else
   //   h = GetQAHisto(PID, eta, sEnergy, qa_histo);

    TF1 *fgaus_g = new TF1("fgaus_LG_g_" + dataset_name, "gaus",
                           h->GetMean() - 4 * h->GetRMS(), h->GetMean() + 4 * h->GetRMS());
    fgaus_g->SetParameters(1, h->GetMean(),
                           h->GetRMS());
    h->Fit(fgaus_g, "MR0N");
    fgaus_g->SetLineColor(kGray);

    TF1 *fgaus = new TF1("fgaus_LG_" + dataset_name, "gaus",
                         fgaus_g->GetParameter(1) - 1.5 * fgaus_g->GetParameter(2),
                         fgaus_g->GetParameter(1) + 1.5 * fgaus_g->GetParameter(2));
    fgaus->SetParameters(fgaus_g->GetParameter(0), fgaus_g->GetParameter(1),
                         fgaus_g->GetParameter(2));
    fgaus->SetLineColor(kBlue);
    h->Fit(fgaus, "MR0N");

    h->Draw();
    fgaus_g->Draw("same");
    fgaus->Draw("same");

    double scale = 1;
    if (qa_histo.Contains("Leading_Et")) scale = 1. / P;

    mean.push_back(fgaus->GetParameter(1) * scale);
    mean_err.push_back(fgaus->GetParError(1) * scale);
    res.push_back(fgaus->GetParameter(2) / fgaus->GetParameter(1));
    res_err.push_back(fgaus->GetParError(2) / fgaus->GetParameter(1));


  //}// for 16GeV
  }

  TGraphErrors *ge_linear = new TGraphErrors(N, Ps,
                                             &mean[0], 0, &mean_err[0]);

  TGraphErrors *ge_res = new TGraphErrors(N, Ps,
                                          &res[0], 0, &res_err[0]);
  ge_res->GetHistogram()->SetStats(0);
  ge_res->Print();

  lin_res *ret = new lin_res;
  ret->name = dataset_name;
  ret->linearity = ge_linear;
  ret->resolution = ge_res;
  ret->f_res = new TF1("f_calo_r_" + dataset_name, "sqrt([0]*[0]+[1]*[1]/x)/100",
                       6, 60);
  ge_res->Fit(ret->f_res, "RM0N");

  SaveCanvas(c1,
             base_dataset + "DrawHadronShowers_" + TString(c1->GetName()), kTRUE);

  new TCanvas;

  ret->f_res->Draw();
  ge_res->Draw("*e");

  return ret;
}

TH1 *GetEvalHisto(TString PID = "pi-", TString eta = "eta0", TString energy = "16GeV")
{
  TString dataset_name = PID + "_" + eta + "_" + energy;
  TH1 *h_E = new TH1F("h_ESumOverp_" + dataset_name, dataset_name + " h_ESumOverp;E/p;Count per bin", 100, 0, 2);
  //TString infile = base_dataset + "G4Hits_sPHENIX_" + dataset_name + "_*.root_g4svtx_eval.root";
  TString infile = base_dataset + "G4Hits_sPHENIX_" + dataset_name + ".root_g4svtx_eval.root";

  TChain *ntp_track = new TChain("ntp_track");
  const int n = ntp_track->Add(infile);

  cout << "GetEvalHisto - loaded " << n << " files from " << infile << endl;

  ntp_track->Draw("(cemce3x3 + hcaloute3x3 + hcaline3x3)/sqrt(gpx**2+gpy**2+gpz**2)>>h_ESumOverp_" + dataset_name,
                  "nfromtruth>50 && gtrackID == 1", "goff");

  delete ntp_track;

  return h_E;
}

TH1 *GetQAHisto(TString PID = "pi-", TString eta = "eta0", TString energy = "16GeV", TString qa_histo = "h_QAG4Sim_CalorimeterSum_TrackProj_5x5Tower_EP")
{
  cout << "Weihu Ma run test " << endl; 
 
  TString dataset_name = PID + "_" + eta + "_" + energy;
  TH1 *h = NULL;
  //TString infile = base_dataset + "G4Hits_sPHENIX_" + dataset_name + "_*.root_qa.root";
  TString infile = base_dataset + "G4Hits_sPHENIX_" + dataset_name + ".root_qa.root";
  cout << "GetQAHisto - searching " << infile << endl;

  TChain *T = new TChain("T");
  const int n = T->Add(infile);

  TObjArray *fileElements = T->GetListOfFiles();
  TIter next(fileElements);
  TChainElement *chEl = 0;
  while ((chEl = (TChainElement *) next()))
  {
    cout << "GetQAHisto - processing " << chEl->GetTitle() << endl;

    TFile f(chEl->GetTitle());

    if (f.IsOpen())
    {
      TH1 *hqa = (TH1 *) f.GetObjectChecked(qa_histo, "TH1");
      cout << "Weihu Ma run test1 " << endl; 
      if (hqa)
      {
        cout << "Weihu Ma run test4 " << endl; 
        if (h)
        {
          cout << "Weihu Ma run test2 " << endl; 
          h->Add(hqa);
        }
        else
        {
          cout << "Weihu Ma run test3 " << endl; 
          h = (TH1 *) (hqa->Clone(TString(hqa->GetName()) + "_" + dataset_name));
          h->SetDirectory(NULL);
          assert(h);
        }
      }
    }
  }
  //assert(h);

  return h;
}
