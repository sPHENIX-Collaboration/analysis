#pragma once

#include <calobase/TowerInfoDefs.h>

#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>

#include <phool/recoConsts.h>

#include <TH2.h>
#include <TCanvas.h>
#include <TString.h>

#include <iostream>
#include <string>

R__LOAD_LIBRARY(libcalo_reco.so)

void EMCalDeadmap()
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");


  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "2023p003");

  const int nRuns = 6;
  int runList[] = {21796, 21615, 21599, 21598, 21518, 21520};
  TH2F* hists[nRuns];
  TCanvas *c = new TCanvas("c1", "c1",0,50,1400,1000);
  c->Divide(3, 2);
  TPad** pad_arr = new TPad*[nRuns];
  TPad* pad;
  for (int i=0; i<nRuns; i++)
  {
      c->cd(i+1);
      pad = (TPad*)gPad;
      pad_arr[i] = pad;
  }

  for (int i=0; i<nRuns; i++)
  {
      int runnumber = runList[i];
      std::cout << "Starting run " << runnumber << "; setting recoConsts\n";
      rc->set_uint64Flag("TIMESTAMP", runnumber);

      std::string url = CDBInterface::instance()->getUrl("CEMC_BadTowerMap");
      if(url.empty())
      {
	  std::cout << "Could not get Dead Map for CDB. Detector: " << "CEMC" << std::endl;
	  return;
      }

      CDBTTree* m_CDBTTree = new CDBTTree(url);

      if(!m_CDBTTree)
      {
	  std::cout << "No CDB TTree found from url " << url << std::endl;
	  return;
      }

      int etabins = 96;
      int phibins = 256;

      std::cout << "Creating hist " << Form("h_deadmap_%d", runnumber) << "\n";
      hists[i] = new TH2F(Form("h_deadmap_%d", runnumber),
	      Form("Dead Tower Map - Run %d;i#phi;i#eta", runnumber),
	      phibins, -0.5, 255.5, etabins, -0.5, 95.5);
      hists[i]->SetStats(0);

      for (int j = 0; j < etabins*phibins; j++)
      {
	  unsigned int key = TowerInfoDefs::encode_emcal(j);
	  int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);
	  int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
	  int status = m_CDBTTree->GetIntValue(j,"status");
	  float isDead = 0.001;
	  if (status > 0) isDead = 1.0;
	  hists[i]->Fill(iphi, ieta, isDead);
      }

      pad_arr[i]->cd();
      std::cout << "Drawing hist " << Form("h_deadmap_%d", runnumber) << "\n";
      hists[i]->Draw("colz");
  }

  std::cout << "Final plotting, saving plot\n";
  c->Modified();
  c->SaveAs("emcal_deadmaps_QMruns.pdf");

  gSystem->Exit(0);
}

