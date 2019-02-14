

#include "SaveCanvas.C"
#include "sPhenixStyle.C"

#include <TCanvas.h>
#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TTree.h>
#include <TVirtualFitter.h>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

using namespace std;

TFile *_file0 = NULL;

void Check()
{
  assert(_file0);

  TCanvas *c1 = new TCanvas("Check", "Check", 1800, 960);
  c1->Divide(2, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  TH1 *hNChEta = (TH1 *) _file0->GetObjectChecked("hNChEta", "TH1");
  assert(hNChEta);
  double norm = hNChEta->GetBinWidth(1) * hNormalization->GetBinContent(2)
                                              hNChEta->Scale(1. / norm)
                                                  hNChEta->Draw()
                                          //  hNormalization->Draw()
                                          //   LayerMultiplicity->ProjectionY("LayerMultiplicityX",1,1)->Draw()
                                          //   LayerMultiplicity->ProjectionY("LayerMultiplicityX2",2,2)->Draw("same")
                                          //   LayerMultiplicity->ProjectionY("LayerMultiplicityX3",3,3)->Draw("same")
                                          //  hVertexZ->Draw()

                                          hNormalization->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogz();

  SaveCanvas(c1, TString(_file0->GetName()) + TString(c1->GetName()), kTRUE);
}

void OccupancySim(const TString infile =
                      "/sphenix/user/jinhuang/HF-jet/MVTX_Multiplicity/pp200MB_30cmVZ_Iter3/pp200MB_30cmVZ_Iter3_SUM.cfg_HFMLTriggerOccupancy.root"
                  , const TString infile_trigger = "/sphenix/user/jinhuang/HF-jet/MVTX_Multiplicity/pp200MB_30cmVZ_Iter3/pp200MB_30cmVZ_Iter3_SUM.cfg_HFMLTriggerOccupancy.root" )
{
  SetsPhenixStyle();
  TVirtualFitter::SetDefaultFitter("Minuit2");

  _file0 = new TFile(infile);
  assert(_file0->IsOpen());
  _file0 = new TFile(infile);
  assert(_file0->IsOpen());

  Check();
}
