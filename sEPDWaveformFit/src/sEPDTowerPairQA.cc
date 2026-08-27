#include "sEPDTowerPairQA.h"

#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

#include <cdbobjects/CDBTTree.h>

#include <ffarawobjects/CaloPacket.h>
#include <ffarawobjects/CaloPacketContainer.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TString.h>
#include <TTree.h>

#include <algorithm>
#include <cmath>
#include <iostream>

sEPDTowerPairQA::sEPDTowerPairQA(const std::string &name)
  : SubsysReco(name)
{
}

bool sEPDTowerPairQA::sepd_skip_channel(int ich, int pid)
{
  int sector = (ich + 1) / 32;
  int emptych = -999;
  if (sector == 0 && pid == 9001)
  {
    emptych = 1;
  }
  else
  {
    emptych = 14 + 32 * sector;
  }
  return (ich == emptych);
}

void sEPDTowerPairQA::build_index_map()
{
  m_idxpid.clear();
  m_idxch.clear();
  for (int pid = 9001; pid <= 9006; pid++)
  {
    for (int ch = 0; ch < 128; ch++)
    {
      if (sepd_skip_channel(ch, pid))
      {
        continue;
      }
      m_idxpid.push_back(pid);
      m_idxch.push_back(ch);
    }
  }
}

TProfile *sEPDTowerPairQA::clone_template(const std::string &file, const char *newname)
{
  if (file.empty())
  {
    return nullptr;
  }
  TFile *ft = TFile::Open(file.c_str(), "READ");
  if (!ft || ft->IsZombie())
  {
    std::cout << "sEPDTowerPairQA: WARNING could not open template file for cloning: " << file << std::endl;
    delete ft;
    m_file->cd();
    return nullptr;
  }
  TProfile *src = dynamic_cast<TProfile *>(ft->Get("waveform_template"));
  TProfile *clone = nullptr;
  if (src)
  {
    m_file->cd();
    clone = dynamic_cast<TProfile *>(src->Clone(newname));
    if (clone)
    {
      clone->SetDirectory(m_file);
    }
  }
  else
  {
    std::cout << "sEPDTowerPairQA: WARNING no waveform_template in " << file << std::endl;
  }
  ft->Close();
  delete ft;
  m_file->cd();
  return clone;
}

int sEPDTowerPairQA::Init(PHCompositeNode * )
{
  m_file = new TFile(m_outputFile.c_str(), "RECREATE");

  const std::string tA = m_labelA;
  const std::string tB = m_labelB;

  h_dchi2 = new TH1F("h_dchi2",
                     Form("Paired #Delta#chi^{2} = #chi^{2}_{%s} - #chi^{2}_{%s};#Delta#chi^{2} [ADC^{2}];towers",
                          tA.c_str(), tB.c_str()),
                     1000, -5000, 5000);
  h_dchi2_frac = new TH1F("h_dchi2_frac",
                          Form("Fractional improvement (#chi^{2}_{%s}-#chi^{2}_{%s})/#chi^{2}_{%s};fraction;towers",
                               tA.c_str(), tB.c_str(), tA.c_str()),
                          300, -2.0, 1.0);
  h_chi2_scatter = new TH2F("h_chi2_scatter",
                            Form("#chi^{2} per tower;#chi^{2}_{%s} [ADC^{2}];#chi^{2}_{%s} [ADC^{2}]",
                                 tA.c_str(), tB.c_str()),
                            300, 0, 3000, 300, 0, 3000);
  p_dchi2_vs_amp = new TProfile("p_dchi2_vs_amp",
                                Form("<#Delta#chi^{2}> vs amplitude;amplitude [ADC];<#chi^{2}_{%s}-#chi^{2}_{%s}>",
                                     tA.c_str(), tB.c_str()),
                                100, 0, 3000);
  p_dchi2frac_vs_amp = new TProfile("p_dchi2frac_vs_amp",
                                    "<fractional improvement> vs amplitude;amplitude [ADC];<fraction>",
                                    100, 0, 3000);
  p_dchi2frac_map = new TProfile2D("p_dchi2frac_map",
                                   "<fractional improvement> vs arm/ring;arm;ring;<fraction>",
                                   2, -0.5, 1.5, NRING, -0.5, NRING - 0.5);

  h_dtime = new TH1F("h_dtime",
                     Form("Paired #Delta t = t_{%s} - t_{%s};#Delta t [samples];towers", tA.c_str(), tB.c_str()),
                     300, -3, 3);
  h_amp_scatter = new TH2F("h_amp_scatter",
                           Form("amplitude per tower;amp_{%s} [ADC];amp_{%s} [ADC]", tA.c_str(), tB.c_str()),
                           300, 0, 3000, 300, 0, 3000);

  m_tree = new TTree("pair_tree", "sEPD paired template-fit comparison");
  m_tree->Branch("ich", &t_ich);
  m_tree->Branch("arm", &t_arm);
  m_tree->Branch("ring", &t_ring);
  m_tree->Branch("ampA", &t_ampA);
  m_tree->Branch("ampB", &t_ampB);
  m_tree->Branch("chi2A", &t_chi2A);
  m_tree->Branch("chi2B", &t_chi2B);
  m_tree->Branch("timeA", &t_timeA);
  m_tree->Branch("timeB", &t_timeB);

  if (m_captureEvents > 0)
  {
    build_index_map();
    if (!m_channelMapFile.empty())
    {
      m_sepdmap = new CDBTTree(m_channelMapFile);
    }
    else
    {
      std::cout << "sEPDTowerPairQA: WARNING capture requested but no channel-map file set "
                << "- raw-sample capture will be skipped" << std::endl;
    }
    m_tmplA = clone_template(m_templateAFile, "template_A");
    m_tmplB = clone_template(m_templateBFile, "template_B");

    m_file->cd();
    m_dtree = new TTree("display_tree", "sEPD raw waveform + both production template fits");
    m_dtree->Branch("evt", &d_evt);
    m_dtree->Branch("ich", &d_ich);
    m_dtree->Branch("arm", &d_arm);
    m_dtree->Branch("ring", &d_ring);
    m_dtree->Branch("ns", &d_ns);
    m_dtree->Branch("raw", d_raw, "raw[16]/F");
    m_dtree->Branch("ampA", &d_ampA);
    m_dtree->Branch("timeA", &d_timeA);
    m_dtree->Branch("chi2A", &d_chi2A);
    m_dtree->Branch("ampB", &d_ampB);
    m_dtree->Branch("timeB", &d_timeB);
    m_dtree->Branch("chi2B", &d_chi2B);
    std::cout << "sEPDTowerPairQA::Init - example-fit capture ENABLED for first " << m_captureEvents
              << " events (index map " << m_idxpid.size() << " channels)" << std::endl;
  }

  std::cout << "sEPDTowerPairQA::Init - comparing node A=" << m_nodeA << " (" << m_labelA
            << ") vs node B=" << m_nodeB << " (" << m_labelB << "), amp cut " << m_amp_cut
            << ", output " << m_outputFile << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int sEPDTowerPairQA::process_event(PHCompositeNode *topNode)
{
  m_event++;

  TowerInfoContainer *towersA = findNode::getClass<TowerInfoContainer>(topNode, m_nodeA);
  TowerInfoContainer *towersB = findNode::getClass<TowerInfoContainer>(topNode, m_nodeB);
  if (!towersA || !towersB)
  {
    if (m_event <= 3)
    {
      std::cout << "sEPDTowerPairQA::process_event - missing node "
                << (towersA ? m_nodeB : m_nodeA) << "; skipping event" << std::endl;
    }
    return Fun4AllReturnCodes::EVENT_OK;
  }

  unsigned int nA = towersA->size();
  unsigned int nB = towersB->size();
  if (nA != nB && m_event <= 3)
  {
    std::cout << "sEPDTowerPairQA::process_event - WARNING size mismatch " << m_nodeA << "=" << nA
              << " " << m_nodeB << "=" << nB << std::endl;
  }
  unsigned int n = std::min(nA, nB);

  const bool capturing = (m_dtree != nullptr && m_sepdmap != nullptr && m_event <= m_captureEvents);
  CaloPacket *pkts[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
  if (capturing)
  {
    if (!m_mapValidated)
    {
      m_mapValidated = true;
      m_mapOK = (m_idxpid.size() == n);
      if (!m_mapOK)
      {
        std::cout << "sEPDTowerPairQA: WARNING index-map size " << m_idxpid.size()
                  << " != container size " << n << " - example-fit capture disabled" << std::endl;
      }
    }
    if (m_mapOK)
    {
      CaloPacketContainer *cont = findNode::getClass<CaloPacketContainer>(topNode, "SEPDPackets");
      for (int pid = 9001; pid <= 9006; pid++)
      {
        pkts[pid - 9001] = cont ? cont->getPacketbyId(pid) : findNode::getClass<CaloPacket>(topNode, pid);
      }
    }
  }

  for (unsigned int i = 0; i < n; i++)
  {
    TowerInfo *tA = towersA->get_tower_at_channel(i);
    TowerInfo *tB = towersB->get_tower_at_channel(i);
    if (!tA || !tB)
    {
      continue;
    }

    float ampA = tA->get_energy();
    float ampB = tB->get_energy();
    float chi2A = tA->get_chi2();
    float chi2B = tB->get_chi2();
    float timeA = tA->get_time();
    float timeB = tB->get_time();

    if (!std::isfinite(chi2A) || !std::isfinite(chi2B) || chi2A <= 0 || ampA < m_amp_cut)
    {
      continue;
    }

    unsigned int key = towersA->encode_key(i);
    int arm = TowerInfoDefs::get_epd_arm(key);
    int ring = TowerInfoDefs::get_epd_rbin(key);

    double dchi2 = chi2A - chi2B;
    double frac = dchi2 / chi2A;

    h_dchi2->Fill(dchi2);
    h_dchi2_frac->Fill(frac);
    h_chi2_scatter->Fill(chi2A, chi2B);
    p_dchi2_vs_amp->Fill(ampA, dchi2);
    p_dchi2frac_vs_amp->Fill(ampA, frac);
    p_dchi2frac_map->Fill(arm, ring, frac);
    h_dtime->Fill(timeA - timeB);
    h_amp_scatter->Fill(ampA, ampB);

    t_ich = (int) i;
    t_arm = arm;
    t_ring = ring;
    t_ampA = ampA;
    t_ampB = ampB;
    t_chi2A = chi2A;
    t_chi2B = chi2B;
    t_timeA = timeA;
    t_timeB = timeB;
    m_tree->Fill();

    if (capturing && m_mapOK)
    {
      int idx = m_sepdmap->GetIntValue((int) i, m_mapFieldName);
      if (idx >= 0 && idx < (int) m_idxpid.size())
      {
        CaloPacket *cp = pkts[m_idxpid[idx] - 9001];
        int ch = m_idxch[idx];
        if (cp && !cp->iValue(ch, "SUPPRESSED"))
        {
          int ns = cp->iValue(0, "SAMPLES");
          if (ns > MAXSAMP)
          {
            ns = MAXSAMP;
          }
          for (int s = 0; s < MAXSAMP; s++)
          {
            d_raw[s] = 0.f;
          }
          for (int s = 0; s < ns; s++)
          {
            d_raw[s] = (float) cp->iValue(s, ch);
          }
          d_evt = m_event;
          d_ich = (int) i;
          d_arm = arm;
          d_ring = ring;
          d_ns = ns;
          d_ampA = ampA;
          d_timeA = timeA;
          d_chi2A = chi2A;
          d_ampB = ampB;
          d_timeB = timeB;
          d_chi2B = chi2B;
          m_dtree->Fill();
        }
      }
    }

    m_sum_dchi2 += dchi2;
    m_sum_dchi2_frac += frac;
    m_npaired++;
    if (chi2B < chi2A)
    {
      m_nimproved++;
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int sEPDTowerPairQA::End(PHCompositeNode * )
{
  if (m_npaired > 0)
  {
    double mean_dchi2 = m_sum_dchi2 / (double) m_npaired;
    double mean_frac = m_sum_dchi2_frac / (double) m_npaired;
    double frac_improved = (double) m_nimproved / (double) m_npaired;
    std::cout << "sEPDTowerPairQA::End - paired towers (" << m_labelA << " vs " << m_labelB << ", amp>"
              << m_amp_cut << "): " << m_npaired << std::endl;
    std::cout << "  mean Delta(chi2) = " << mean_dchi2 << " ADC^2" << std::endl;
    std::cout << "  mean fractional improvement = " << mean_frac << std::endl;
    std::cout << "  fraction of towers improved = " << frac_improved << std::endl;
  }
  else
  {
    std::cout << "sEPDTowerPairQA::End - WARNING no paired towers above amp cut " << m_amp_cut << std::endl;
  }

  m_file->cd();
  h_dchi2->Write();
  h_dchi2_frac->Write();
  h_chi2_scatter->Write();
  p_dchi2_vs_amp->Write();
  p_dchi2frac_vs_amp->Write();
  p_dchi2frac_map->Write();
  h_dtime->Write();
  h_amp_scatter->Write();
  m_tree->Write();

  if (m_dtree)
  {
    if (m_tmplA)
    {
      m_tmplA->Write();
    }
    if (m_tmplB)
    {
      m_tmplB->Write();
    }
    m_dtree->Write();
    std::cout << "sEPDTowerPairQA::End - captured " << m_dtree->GetEntries()
              << " example waveforms (display_tree) + template_A/template_B" << std::endl;
  }

  m_file->Close();
  delete m_file;
  m_file = nullptr;

  delete m_sepdmap;
  m_sepdmap = nullptr;

  return Fun4AllReturnCodes::EVENT_OK;
}
