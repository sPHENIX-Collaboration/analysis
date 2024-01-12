#include "HCalCalibTree.h"

#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfov2.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>

#include <TFile.h>
#include <TH1F.h>

#include <Event/Event.h>
#include <Event/packet.h>
#include <cassert>
#include <sstream>
#include <string>

using namespace std;

HCalCalibTree::HCalCalibTree(const std::string &name, const std::string &filename, const std::string &prefix)
    : SubsysReco(name), detector("HCALIN"), prefix(prefix), outfilename(filename)
{
}

HCalCalibTree::~HCalCalibTree() {
  delete hm;
}

int HCalCalibTree::Init(PHCompositeNode *) {
  std::cout << std::endl << "HCalCalibTree::Init" << std::endl;
  hm = new Fun4AllHistoManager(Name());
  outfile = new TFile(outfilename.c_str(), "RECREATE");

  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      std::string channel_histname = "h_channel_" + std::to_string(ieta) + "_" + std::to_string(iphi);
      h_channel_hist[ieta][iphi] = new TH1F(channel_histname.c_str(), "", 200, 0, 10000);
    }
  }
  h_waveformchi2 = new TH2F("h_waveformchi2", "", 1000, 0, 10000, 1000, 0, 100000);
  h_waveformchi2->GetXaxis()->SetTitle("peak (ADC)");
  h_waveformchi2->GetYaxis()->SetTitle("chi2");

  h_check = new TH1F("h_check", "", 1000, 0, 0.01);
  h_eventnumber_record = new TH1F("h_eventnumber_record", "", 2, 0, 2);

  if (prefix == "TOWERINFO_SIM_") {
    tower_threshold = tower_threshold_sim;
    vert_threshold = vert_threshold_sim;
    veto_threshold = veto_threshold_sim;
  } else {
    tower_threshold = tower_threshold_data;
    vert_threshold = vert_threshold_data;
    veto_threshold = veto_threshold_data;
  }
  std::cout << "Offline cut applied: " << std::endl;
  std::cout << "tower_threshold = " << tower_threshold << std::endl;
  std::cout << "vert_threshold = " << vert_threshold << std::endl;
  std::cout << "veto_threshold = " << veto_threshold << std::endl;

  Fun4AllServer *se = Fun4AllServer::instance();
  se -> registerHistoManager(hm);

  event = 0;
  goodevent = 0;
  return 0;
}

int HCalCalibTree::process_event(PHCompositeNode *topNode) {
  if (event % 100 == 0) std::cout << "HCalCalibTree::process_event " << event << std::endl;
  goodevent_check = 0;
  process_towers(topNode);
  event++;
  if (goodevent_check == 1) goodevent++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int HCalCalibTree::process_towers(PHCompositeNode *topNode) {
  ostringstream nodenamev2;
  nodenamev2.str("");
  nodenamev2 << prefix << detector;

  TowerInfoContainer *towers = findNode::getClass<TowerInfoContainer>(topNode, nodenamev2.str());
  if (!towers ) {
     std::cout << std::endl << "Didn't find node " << nodenamev2.str() << std::endl;
     return Fun4AllReturnCodes::EVENT_OK;
  }

  int size = towers->size();
  for (int channel = 0; channel < size; channel++) {
    TowerInfo *tower = towers->get_tower_at_channel(channel);
    float energy = tower->get_energy();
    float chi2 = tower->get_chi2();
    unsigned int towerkey = towers->encode_key(channel);
    int ieta = towers->getTowerEtaBin(towerkey);
    int iphi = towers->getTowerPhiBin(towerkey);
    m_peak[ieta][iphi] = energy;
    m_chi2[ieta][iphi] = chi2;
    h_waveformchi2->Fill(m_peak[ieta][iphi], m_chi2[ieta][iphi]);
    h_check->Fill(m_peak[ieta][iphi]);
    if (m_chi2[ieta][iphi] > 10000) m_peak[ieta][iphi] = 0;
  }

  // Apply cut
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      if (m_peak[ieta][iphi] < tower_threshold) continue; //tower cut
      int up = iphi + 1;
      int down = iphi - 1;
      if (up > 63) up -= 64;
      if (down < 0) down += 64;
      if (m_peak[ieta][up] < vert_threshold || m_peak[ieta][down] < vert_threshold) continue; //vert cut
      if (ieta != 0 && (m_peak[ieta-1][up] > veto_threshold || m_peak[ieta-1][iphi] > veto_threshold || m_peak[ieta-1][down] > veto_threshold)) continue; // left veto cut
      if (ieta != 23 && (m_peak[ieta+1][up] > veto_threshold || m_peak[ieta+1][iphi] > veto_threshold || m_peak[ieta+1][down] > veto_threshold)) continue; //right veto cut
      h_channel_hist[ieta][iphi]->Fill(m_peak[ieta][iphi]);
      goodevent_check = 1;
    }
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

int HCalCalibTree::ResetEvent(PHCompositeNode *topNode) {
  return Fun4AllReturnCodes::EVENT_OK;
}

int HCalCalibTree::End(PHCompositeNode * /*topNode*/) {
  std::cout << "HCalCalibTree::End" << std::endl;
  std::cout << "Number of events: " << event << std::endl;
  std::cout << "Number of good events: " << goodevent << std::endl;
  h_eventnumber_record->SetBinContent(1, event);
  h_eventnumber_record->SetBinContent(2, goodevent);
  outfile->cd();
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      h_channel_hist[ieta][iphi]->Write();
      delete h_channel_hist[ieta][iphi];
    }
  }
  h_waveformchi2->Write();
  h_check->Write();
  h_eventnumber_record->Write();
  outfile->Close();
  delete outfile;
  hm->dumpHistos(outfilename, "UPDATE");
  return 0;
}
