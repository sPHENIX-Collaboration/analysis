#include "sEPDWaveform.h"

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

#include <Event/Event.h>
#include <Event/packet.h>

#include <ffarawobjects/CaloPacket.h>
#include <ffarawobjects/CaloPacketContainer.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>

#include <vector>
#include <map>
#include <iomanip>

#include <TFile.h>
#include <TGraph.h>
#include <TProfile2D.h>

using namespace std;

sEPDWaveform::sEPDWaveform(const std::string &name) : SubsysReco(name)
{
}

sEPDWaveform::~sEPDWaveform()
{
}

int sEPDWaveform::Init(PHCompositeNode *topNode)
{
  WaveformProcessing = nullptr;

  WaveformProcessing2 = new CaloWaveformProcessing();
  WaveformProcessing2->set_processing_type(CaloWaveformProcessing::FAST);
  WaveformProcessing2->initialize_processing();

  m_file = new TFile(m_outputFileName.c_str(), "RECREATE");

  if (m_fill_geometry_histos)
  {

    for (int arm = 0; arm < NARM; arm++)
    {
      for (int ring = 0; ring < NRING; ring++)
      {
        hpwaveform_1[arm][ring] = new TProfile(Form("hpwaveform_1_arm%d_ring%d", arm, ring),
                                               Form("Low amplitude waveform arm%d ring%d", arm, ring),
                                               300, 0, 30);
        hpwaveform_2[arm][ring] = new TProfile(Form("hpwaveform_2_arm%d_ring%d", arm, ring),
                                               Form("Medium amplitude waveform arm%d ring%d", arm, ring),
                                               300, 0, 30);
        hpwaveform_3[arm][ring] = new TProfile(Form("hpwaveform_3_arm%d_ring%d", arm, ring),
                                               Form("High amplitude waveform arm%d ring%d", arm, ring),
                                               300, 0, 30);

        h2waveform_1[arm][ring] = new TH2F(Form("h2waveform_1_arm%d_ring%d", arm, ring),
                                           Form("Low amplitude waveform 2D arm%d ring%d;Sample;ADC", arm, ring),
                                           m_n_samples, 0, m_n_samples, 200, -100, 1500);
        h2waveform_2[arm][ring] = new TH2F(Form("h2waveform_2_arm%d_ring%d", arm, ring),
                                           Form("Medium amplitude waveform 2D arm%d ring%d;Sample;ADC", arm, ring),
                                           m_n_samples, 0, m_n_samples, 200, -100, 1500);
        h2waveform_3[arm][ring] = new TH2F(Form("h2waveform_3_arm%d_ring%d", arm, ring),
                                           Form("High amplitude waveform 2D arm%d ring%d;Sample;ADC", arm, ring),
                                           m_n_samples, 0, m_n_samples, 200, -100, 1500);
      }
    }

    hpwaveform_1_sum = new TProfile("hpwaveform_1_sum", "Low amplitude sum", 300, 0, 30);
    hpwaveform_2_sum = new TProfile("hpwaveform_2_sum", "Medium amplitude sum", 300, 0, 30);
    hpwaveform_3_sum = new TProfile("hpwaveform_3_sum", "High amplitude sum", 300, 0, 30);

    h2waveform_1_sum = new TH2F("h2waveform_1_sum", "Low amplitude sum 2D;Sample;ADC", m_n_samples, 0, m_n_samples, 200, -100, 1500);
    h2waveform_2_sum = new TH2F("h2waveform_2_sum", "Medium amplitude sum 2D;Sample;ADC", m_n_samples, 0, m_n_samples, 200, -100, 1500);
    h2waveform_3_sum = new TH2F("h2waveform_3_sum", "High amplitude sum 2D;Sample;ADC", m_n_samples, 0, m_n_samples, 200, -100, 1500);
  }

  if (m_fill_global_histos)
  {
    hpwaveform_global_1 = new TProfile("hpwaveform_global_1", "Global Low Amplitude Template", 300, 0, 30);
    hpwaveform_global_2 = new TProfile("hpwaveform_global_2", "Global Medium Amplitude Template", 300, 0, 30);
    hpwaveform_global_3 = new TProfile("hpwaveform_global_3", "Global High Amplitude Template", 300, 0, 30);

    h2waveform_global_1 = new TH2F("h2waveform_global_1", "Global Low Amplitude 2D;Sample;ADC", m_n_samples, 0, m_n_samples, 200, -100, 1500);
    h2waveform_global_2 = new TH2F("h2waveform_global_2", "Global Medium Amplitude 2D;Sample;ADC", m_n_samples, 0, m_n_samples, 200, -100, 1500);
    h2waveform_global_3 = new TH2F("h2waveform_global_3", "Global High Amplitude 2D;Sample;ADC", m_n_samples, 0, m_n_samples, 200, -100, 1500);
  }

  waveform_template = new TProfile("waveform_template", "sEPD Waveform Template", m_template_nbins, 0, 30);

  h_sig_chi2 = new TH2F("h_sig_chi2","Signal vs Chi2",5500,-100,1e4,5000,0,5e6);
  h_sig_chi2_nt = new TH2F("h_sig_chi2_nt","Signal vs Chi2 (no template)",1100,-100,1e4,1000,0,1e5);
  h_sig1_sig2 = new TH2F("h_sig1_sig2","Signal1 vs Signal2",1100,-100,1e3,1100,-100,1e3);
  h_sigP1_sigP2 = new TH2F("h_sigP1_sigP2","Peak1 vs Peak2",320,-1,31,320,-1,31);
  h_chi1_chi2 = new TH2F("h_chi1_chi2","Chi2_1 vs Chi2_2",1e3,0,1e4,1e3,0,1e4);
  h_ped1_ped2 = new TH2F("h_ped1_ped2","Pedestal1 vs Pedestal2",1e3,0,1e4,1e3,0,1e4);
  h_dif_peak_time = new TH2F("h_dif_peak_time","Peak time difference",100,-5,5,100,-5,5);
  h_chi2 = new TH1F("h_chi2","Chi2 distribution",1000,0,1e5);

  p_chi2 = new TProfile2D("p_chi2","Chi2 vs arm/ring",NARM,0,NARM,NRING,0,NRING);
  h_etaphi_ped = new TProfile2D("h_etaphi_ped","Pedestal vs arm/ring",NARM,0,NARM,NRING,0,NRING);
  h_etaphi_ped2 = new TProfile2D("h_etaphi_ped2","Pedestal2 vs arm/ring",NARM,0,NARM,NRING,0,NRING);

  h_time1 = new TH1F("h_time1","Peak time distribution 1",100,0,30);
  h_time2 = new TH1F("h_time2","Peak time distribution 2",100,0,30);

  m_tree = new TTree("waveform_tree", "sEPD Waveform Tree");
  m_tree->Branch("arm", &m_arm);
  m_tree->Branch("ring", &m_ring);
  m_tree->Branch("phibin", &m_phibin);
  m_tree->Branch("peak", &m_peak);
  m_tree->Branch("peaktime", &m_peaktime);
  m_tree->Branch("pedestal", &m_pedestal);
  m_tree->Branch("ADC_peak", &m_ADC_peak);
  m_tree->Branch("ADC_peaktime", &m_ADC_peaktime);
  m_tree->Branch("waveform", &m_waveform);

  return Fun4AllReturnCodes::EVENT_OK;
}

int sEPDWaveform::process_event(PHCompositeNode *topNode)
{

  m_arm.clear();
  m_ring.clear();
  m_phibin.clear();
  m_peak.clear();
  m_peaktime.clear();
  m_pedestal.clear();
  m_ADC_peak.clear();
  m_ADC_peaktime.clear();
  m_waveform.clear();

  auto processPacket = [&](auto *packet, int pid)
  {
    if (!packet)
    {
      return;
    }

    int nchannels = packet->iValue(0, "CHANNELS");
    if (nchannels > m_nchannels)
    {
      std::cout << "Skipping packet " << pid << " - nchannels " << nchannels
                << " exceeds expected " << m_nchannels << std::endl;
      return;
    }

    for (int channel = 0; channel < nchannels; channel++)
    {
      if (packet->iValue(channel, "SUPPRESSED"))
      {
        continue;
      }

      std::vector<double> waveform;
      for (int samp = 0; samp < m_n_samples; samp++)
      {
        waveform.push_back(packet->iValue(samp, channel));
      }

      if (waveform.size() != (unsigned int)m_n_samples)
      {
        continue;
      }

      int global_channel = (pid - m_packet_low) * m_nchannels + channel;

      unsigned int tower_key = TowerInfoDefs::encode_epd(global_channel);

      int arm = TowerInfoDefs::get_epd_arm(tower_key);
      int ring = TowerInfoDefs::get_epd_rbin(tower_key);
      int phi = TowerInfoDefs::get_epd_phibin(tower_key);

      m_arm.push_back(arm);
      m_ring.push_back(ring);
      m_phibin.push_back(phi);
      m_waveform.push_back(waveform);

      double peak = 0;
      double peaktime = 0;
      double pedestal = 0;

      int peakbin = 0;
      for (int i = 0; i < m_n_samples; i++)
      {
        if (waveform[i] > peak)
        {
          peak = waveform[i];
          peakbin = i;
        }
      }
      peaktime = peakbin;

      if (peakbin > 0 && peakbin < m_n_samples - 1)
      {
        double ym1 = waveform[peakbin - 1];
        double y0 = waveform[peakbin];
        double yp1 = waveform[peakbin + 1];
        double denom = ym1 - (2.0 * y0) + yp1;
        if (denom < 0)
        {
          double delta = 0.5 * (ym1 - yp1) / denom;
          if (delta > -0.5 && delta < 0.5)
          {
            peaktime = peakbin + delta;
          }
        }
      }

      for (int i = 0; i < 3 && i < m_n_samples; i++)
      {
        pedestal += waveform[i];
      }
      pedestal /= 3.0;

      m_peak.push_back(peak);
      m_peaktime.push_back(peaktime);
      m_pedestal.push_back(pedestal);
      m_ADC_peak.push_back(peak - pedestal);
      m_ADC_peaktime.push_back(peaktime);

      if (arm < NARM && ring < NRING)
      {

        int max_phi = (ring == 0) ? NPHI_RING0 : NPHI;
        if (phi >= max_phi)
        {
          std::cout << "Warning: Invalid phi bin " << phi << " for ring " << ring << std::endl;
          continue;
        }

        double amplitude = peak - pedestal;

        if (amplitude > 50)
        {

          double thr = pedestal + 0.5 * amplitude;
          double t_hm = -1.0;
          for (int i = 1; i <= peakbin; i++)
          {
            if (waveform[i - 1] < thr && waveform[i] >= thr && waveform[i] != waveform[i - 1])
            {
              t_hm = (i - 1) + (thr - waveform[i - 1]) / (waveform[i] - waveform[i - 1]);
              break;
            }
          }

          double area = 0.0;
          for (int i = 0; i < m_n_samples; i++)
          {
            double y = waveform[i] - pedestal;
            if (y > 0) { area += y; }
          }
          if (t_hm >= 0 && area > 0)
          {
            double shift = m_target_hm - t_hm;
            for (int i = 0; i < m_n_samples; i++)
            {
              double ttmpl = i + shift;
              if (ttmpl > 0 && ttmpl < 30)
              {
                waveform_template->Fill(ttmpl, (waveform[i] - pedestal) / area);
              }
            }
          }
        }

        for (int i = 0; i < m_n_samples; i++)
        {
          double time = i - peaktime + 6.0;

          if (amplitude > 50 && amplitude < 500)
          {

            if (m_fill_geometry_histos)
            {
              h2waveform_1[arm][ring]->Fill(i, waveform[i] - pedestal);
              h2waveform_1_sum->Fill(i, waveform[i] - pedestal);

              if (time > 0 && time < 30)
              {
                hpwaveform_1[arm][ring]->Fill(time, waveform[i] - pedestal);
                hpwaveform_1_sum->Fill(time, waveform[i] - pedestal);
              }
            }

            if (m_fill_global_histos)
            {
              h2waveform_global_1->Fill(i, waveform[i] - pedestal);
              if (time > 0 && time < 30)
              {
                hpwaveform_global_1->Fill(time, waveform[i] - pedestal);
              }
            }
          }
          else if (amplitude >= 500 && amplitude < 1000)
          {

            if (m_fill_geometry_histos)
            {
              h2waveform_2[arm][ring]->Fill(i, waveform[i] - pedestal);
              h2waveform_2_sum->Fill(i, waveform[i] - pedestal);

              if (time > 0 && time < 30)
              {
                hpwaveform_2[arm][ring]->Fill(time, waveform[i] - pedestal);
                hpwaveform_2_sum->Fill(time, waveform[i] - pedestal);
              }
            }

            if (m_fill_global_histos)
            {
              h2waveform_global_2->Fill(i, waveform[i] - pedestal);
              if (time > 0 && time < 30)
              {
                hpwaveform_global_2->Fill(time, waveform[i] - pedestal);
              }
            }
          }
          else if (amplitude >= 1000)
          {

            if (m_fill_geometry_histos)
            {
              h2waveform_3[arm][ring]->Fill(i, waveform[i] - pedestal);
              h2waveform_3_sum->Fill(i, waveform[i] - pedestal);

              if (time > 0 && time < 30)
              {
                hpwaveform_3[arm][ring]->Fill(time, waveform[i] - pedestal);
                hpwaveform_3_sum->Fill(time, waveform[i] - pedestal);
              }
            }

            if (m_fill_global_histos)
            {
              h2waveform_global_3->Fill(i, waveform[i] - pedestal);
              if (time > 0 && time < 30)
              {
                hpwaveform_global_3->Fill(time, waveform[i] - pedestal);
              }
            }
          }
        }
      }
    }

  };

  if (m_offline)
  {

    CaloPacketContainer *cont = findNode::getClass<CaloPacketContainer>(topNode, "SEPDPackets");
    bool found = false;
    for (int pid = m_packet_low; pid <= m_packet_high; pid++)
    {
      CaloPacket *cp = cont ? cont->getPacketbyId(pid) : findNode::getClass<CaloPacket>(topNode, pid);
      if (cp)
      {
        found = true;

        if (eventCount == 0)
        {
          int nch = cp->iValue(0, "CHANNELS");
          int nunsupp = 0;
          for (int c = 0; c < nch; c++)
          {
            if (!cp->iValue(c, "SUPPRESSED")) { nunsupp++; }
          }
          std::cout << "[sEPDWaveform] packet " << pid
                    << ": SAMPLES=" << cp->iValue(0, "SAMPLES")
                    << " CHANNELS=" << nch
                    << " unsuppressed=" << nunsupp << std::endl;
        }
      }
      processPacket(cp, pid);
    }
    if (!found)
    {
      if (eventCount < 3)
      {
        std::cout << "sEPDWaveform::process_event - no sEPD CaloPacket nodes (9001-9006) found" << std::endl;
      }
      return Fun4AllReturnCodes::DISCARDEVENT;
    }
  }
  else
  {

    Event *_event = findNode::getClass<Event>(topNode, "PRDF");
    if (!_event)
    {
      std::cout << "sEPDWaveform::process_event - Event not found" << std::endl;
      return Fun4AllReturnCodes::DISCARDEVENT;
    }
    for (int pid = m_packet_low; pid <= m_packet_high; pid++)
    {
      Packet *packet = _event->getPacket(pid);
      processPacket(packet, pid);
      delete packet;
    }
  }

  if (m_arm.size() > 0)
  {
    m_tree->Fill();
  }

  eventCount++;

  if (m_show_progress && eventCount % m_progress_interval == 0)
  {

    std::cout << "\rProcessed " << eventCount << " events...";
    std::cout.flush();
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int sEPDWaveform::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int sEPDWaveform::End(PHCompositeNode *topNode)
{
  if (m_show_progress)
  {
    std::cout << std::endl;
  }

  std::cout << "sEPDWaveform::End - Writing output to " << m_outputFileName << std::endl;

  m_file->cd();

  for (int arm = 0; arm < NARM; arm++)
  {
    for (int ring = 0; ring < NRING; ring++)
    {
      if (hpwaveform_1[arm][ring]) hpwaveform_1[arm][ring]->Write();
      if (hpwaveform_2[arm][ring]) hpwaveform_2[arm][ring]->Write();
      if (hpwaveform_3[arm][ring]) hpwaveform_3[arm][ring]->Write();
      if (h2waveform_1[arm][ring]) h2waveform_1[arm][ring]->Write();
      if (h2waveform_2[arm][ring]) h2waveform_2[arm][ring]->Write();
      if (h2waveform_3[arm][ring]) h2waveform_3[arm][ring]->Write();
    }
  }

  if (hpwaveform_1_sum) hpwaveform_1_sum->Write();
  if (hpwaveform_2_sum) hpwaveform_2_sum->Write();
  if (hpwaveform_3_sum) hpwaveform_3_sum->Write();
  if (h2waveform_1_sum) h2waveform_1_sum->Write();
  if (h2waveform_2_sum) h2waveform_2_sum->Write();
  if (h2waveform_3_sum) h2waveform_3_sum->Write();

  if (hpwaveform_global_1) hpwaveform_global_1->Write();
  if (hpwaveform_global_2) hpwaveform_global_2->Write();
  if (hpwaveform_global_3) hpwaveform_global_3->Write();
  if (h2waveform_global_1) h2waveform_global_1->Write();
  if (h2waveform_global_2) h2waveform_global_2->Write();
  if (h2waveform_global_3) h2waveform_global_3->Write();

  if (waveform_template && waveform_template->GetMaximum() > 0)
  {
    std::cout << "sEPDWaveform::End - Normalizing template from max=" << waveform_template->GetMaximum() << " to 1.0" << std::endl;
    waveform_template->Scale(1.0 / waveform_template->GetMaximum());
    waveform_template->Write();
  }
  else if (waveform_template)
  {
    std::cout << "Warning: waveform_template has zero maximum, writing unnormalized" << std::endl;
    waveform_template->Write();
  }

  h_sig_chi2->Write();
  h_sig_chi2_nt->Write();
  h_sig1_sig2->Write();
  h_sigP1_sigP2->Write();
  h_chi1_chi2->Write();
  h_ped1_ped2->Write();
  h_dif_peak_time->Write();
  h_chi2->Write();
  p_chi2->Write();
  h_etaphi_ped->Write();
  h_etaphi_ped2->Write();
  h_time1->Write();
  h_time2->Write();

  m_tree->Write();

  m_file->Close();
  delete m_file;

  if (WaveformProcessing) delete WaveformProcessing;
  if (WaveformProcessing2) delete WaveformProcessing2;

  std::cout << "sEPDWaveform::End - Processed " << eventCount << " events" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

void sEPDWaveform::Print(const std::string &what) const
{
}

void sEPDWaveform::ShowProgress(int current, int total)
{
  if (total <= 0) return;

  float percent = (float)current / (float)total * 100.0;
  int barWidth = 50;
  int pos = barWidth * current / total;

  std::cout << "\r[";
  for (int i = 0; i < barWidth; ++i)
  {
    if (i < pos) std::cout << "=";
    else if (i == pos) std::cout << ">";
    else std::cout << " ";
  }
  std::cout << "] " << std::fixed << std::setprecision(1) << percent << "% ("
            << current << "/" << total << " events)";
  std::cout.flush();

  if (current == total)
  {
    std::cout << std::endl;
  }
}
