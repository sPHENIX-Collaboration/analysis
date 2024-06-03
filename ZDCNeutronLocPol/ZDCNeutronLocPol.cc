#include "ZDCNeutronLocPol.h"

#include <calobase/TowerInfoDefs.h>
#include <caloreco/CaloWaveformFitting.h>

#include <calobase/TowerInfoDefs.h>
#include <caloreco/CaloWaveformFitting.h>
#include <ffarawobjects/CaloPacketContainerv1.h>
#include <ffarawobjects/CaloPacketv1.h>
#include <ffarawobjects/Gl1Packetv1.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <Event/packet.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/recoConsts.h>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <phool/PHCompositeNode.h>
#include <cmath>
#include <fstream>

R__LOAD_LIBRARY(libuspin.so)

//____________________________________________________________________________..
ZDCNeutronLocPol::ZDCNeutronLocPol(const std::string &name)
  : SubsysReco(name)
{
  std::cout << "ZDCNeutronLocPol::ZDCNeutronLocPol(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
ZDCNeutronLocPol::~ZDCNeutronLocPol()
{
  std::cout << "ZDCNeutronLocPol::~ZDCNeutronLocPol() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int ZDCNeutronLocPol::Init(PHCompositeNode * /*topNode*/)
{
  std::cout << "ZDCNeutronLocPol::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  smdHits = new TTree();
  smdHits = new TTree("smdHits", "smdHits");
  smdHits->SetDirectory(0);
  smdHits->Branch("bunchnumber", &bunchnumber, "bunchnumber/I");
  // smdHits -> Branch("evtseq_gl1",  &evtseq_gl1, "evtseq_gl1/I");
  // smdHits -> Branch("evtseq_zdc",  &evtseq_zdc, "evtseq_zdc/I");
  // smdHits -> Branch("BCO_gl1",  &BCO_gl1, "BCO_gl1/l");
  // smdHits -> Branch("BCO_zdc",  &BCO_zdc, "BCO_zdc/l");
  smdHits->Branch("showerCutN", &showerCutN, "showerCutN/I");
  smdHits->Branch("showerCutS", &showerCutS, "showerCutS/I");
  smdHits->Branch("n_x", &n_x, "n_x/F");
  smdHits->Branch("n_y", &n_y, "n_y/F");
  smdHits->Branch("s_x", &s_x, "s_x/F");
  smdHits->Branch("s_y", &s_y, "s_y/F");
  smdHits->Branch("zdcN1_adc", &zdcN1_adc, "zdcN1_adc/F");
  smdHits->Branch("zdcN2_adc", &zdcN2_adc, "zdcN2_adc/F");
  smdHits->Branch("zdcN3_adc", &zdcN3_adc, "zdcN3_adc/F");
  smdHits->Branch("zdcS1_adc", &zdcS1_adc, "zdcS1_adc/F");
  smdHits->Branch("zdcS2_adc", &zdcS2_adc, "zdcS2_adc/F");
  smdHits->Branch("zdcS3_adc", &zdcS3_adc, "zdcS3_adc/F");
  smdHits->Branch("veto_NF", &veto_NF, "veto_NF/F");
  smdHits->Branch("veto_NB", &veto_NB, "veto_NB/F");
  smdHits->Branch("veto_SF", &veto_SF, "veto_SF/F");
  smdHits->Branch("veto_SB", &veto_SB, "veto_SB/F");
  smdHits->Branch("smdS1_adc", &smdS1_adc, "smdS1_adc/F");
  smdHits->Branch("smdS2_adc", &smdS2_adc, "smdS2_adc/F");
  smdHits->Branch("smdS6_adc", &smdS6_adc, "smdS6_adc/F");
  smdHits->Branch("smdS7_adc", &smdS7_adc, "smdS7_adc/F");

  smdHits->Branch("smdN1_adc", &smdN1_adc, "smdN1_adc/F");
  smdHits->Branch("smdN2_adc", &smdN2_adc, "smdN2_adc/F");
  smdHits->Branch("smdN6_adc", &smdN6_adc, "smdN6_adc/F");
  smdHits->Branch("smdN7_adc", &smdN7_adc, "smdN7_adc/F");

  smdHits->Branch("smdS1_v_adc", &smdS1_v_adc, "smdS1_v_adc/F");
  smdHits->Branch("smdS2_v_adc", &smdS2_v_adc, "smdS2_v_adc/F");
  smdHits->Branch("smdS7_v_adc", &smdS7_v_adc, "smdS7_v_adc/F");
  smdHits->Branch("smdS8_v_adc", &smdS8_v_adc, "smdS8_v_adc/F");

  smdHits->Branch("smdN1_v_adc", &smdN1_v_adc, "smdN1_v_adc/F");
  smdHits->Branch("smdN2_v_adc", &smdN2_v_adc, "smdN2_v_adc/F");
  smdHits->Branch("smdN7_v_adc", &smdN7_v_adc, "smdN7_v_adc/F");
  smdHits->Branch("smdN8_v_adc", &smdN8_v_adc, "smdN8_v_adc/F");

  // waveform
  h_waveformZDC = new TH2F("h_waveformZDC", "h_waveformZDC", 17, -0.5, 16.5, 512, -500, 20000);
  h_waveformSMD_North = new TH2F("h_waveformSMD_North", "h_waveformSMD_North", 17, -0.5, 16.5, 512, -500, 20000);
  h_waveformSMD_South = new TH2F("h_waveformSMD_South", "h_waveformSMD_South", 17, -0.5, 16.5, 512, -500, 20000);
  h_waveformVeto_North = new TH2F("h_waveformVeto_North", "h_waveformVeto_North", 17, -0.5, 16.5, 512, -500, 20000);
  h_waveformVeto_South = new TH2F("h_waveformVeto_South", "h_waveformVeto_South", 17, -0.5, 16.5, 512, -500, 20000);
  h_waveformAll = new TH2F("h_waveformAll", "h_waveformAll", 17, -0.5, 16.5, 512, -500, 20000);

  for (int i = 0; i < 32; i++)
  {
    h_rawADC[i] = new TH1F(Form("rawADC%d", i), Form("rawADC%d", i), 128, 0, 4000);
    h_pedADC[i] = new TH1F(Form("pedADC%d", i), Form("pedADC%d", i), 128, 0, 4000);
    h_signalADC[i] = new TH1F(Form("signalDC%d", i), Form("signalADC%d", i), 128, 0, 4000);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ZDCNeutronLocPol::InitRun(PHCompositeNode * /*topNode*/)
{
  std::cout << "ZDCNeutronLocPol::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;

  evtcnt = 0;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ZDCNeutronLocPol::process_event(PHCompositeNode *topNode)
{
  // std::cout << "ZDCNeutronLocPol::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  if (evtcnt % 10000 == 0)
  {
    std::cout << "Event: " << evtcnt << std::endl;
  }

  p_gl1 = findNode::getClass<Gl1Packetv1>(topNode, "GL1Packet");
  zdc_cont = findNode::getClass<CaloPacketContainerv1>(topNode, "ZDCPackets");

  showerCutN = 0;
  showerCutS = 0;

  if (p_gl1)
  {
    bunchnumber = p_gl1->getBunchNumber();
    if (evtcnt % 1000 == 0)
    {
      std::cout << bunchnumber << std::endl;
    }
    if (zdc_cont->get_npackets() != 1)
    {
      return Fun4AllReturnCodes::EVENT_OK;
    }

    CaloPacket *p_zdc = zdc_cont->getPacket(0);

    if (p_zdc)
    {
      int nThresholdNVer = 0;
      int nThresholdNHor = 0;
      int nThresholdSVer = 0;
      int nThresholdSHor = 0;

      // evtseq_gl1 = p_gl1->getEvtSequence();
      // evtseq_zdc = p_zdc->getEvtSequence();
      // BCO_gl1 = p_gl1->getBCO();
      // BCO_zdc = p_zdc->getBCO();

      // in this for loop we get: zdc_adc and smd_adc
      for (int c = 0; c < p_zdc->iValue(0, "CHANNELS"); c++)
      {
        /*
        double baseline = 0.;
        double baseline_low = 0.;
        double baseline_high = 0.;

        for(int s = 0; s < 3; s++)
        {
          baseline_low += p_zdc->iValue(s, c);
        }

        baseline_low /= 3.;

        for (int s = p_zdc->iValue(0, "SAMPLES")-3; s < p_zdc->iValue(0, "SAMPLES"); s++)
        {
          baseline_high += p_zdc->iValue(s,c);
        }

        baseline_high /=3.;

        baseline = baseline_low;

        if(baseline_high < baseline_low) {baseline = baseline_high;}

        for (int s = 0; s < p_zdc->iValue(0, "SAMPLES"); s++)
        {
          h_waveformAll->Fill(s, p_zdc->iValue(s, c) - baseline);

          if (c < 16) //-->[0,15]
          {
            h_waveformZDC->Fill(s, p_zdc->iValue(s, c) - baseline);
          }

          if ((c > 47) && (c < 64)) //-->[48,63]
          {
            h_waveformSMD_North->Fill(s, p_zdc->iValue(s, c) - baseline);
          }

          if ((c > 111) && (c < 128)) //-->[112,127]
          {
            h_waveformSMD_South->Fill(s, p_zdc->iValue(s, c) - baseline);
          }

          if ((c > 15) && (c < 18)) //-->[16,17]
          {
            h_waveformVeto_North->Fill(s, p_zdc->iValue(s, c) - baseline);
          }

          if ((c > 79) && (c < 82)) //-->[80,81]
          {
            h_waveformVeto_South->Fill(s, p_zdc->iValue(s, c) - baseline);
          }

        }

        */

        std::vector<float> resultFast = anaWaveformFast(p_zdc, c);  // fast waveform fitting
        float signalFast = resultFast.at(0);
        float timingFast = resultFast.at(1);
        float pedFast = resultFast.at(2);

        ////////// ZDC channels ///////
        if (c >= 0 && c < 16)
        {
          signalFast = (4 < timingFast && timingFast < 10) ? signalFast : 0;
          unsigned int towerkey = TowerInfoDefs::decode_zdc(c);
          int zdc_side = TowerInfoDefs::get_zdc_side(towerkey);
          if (zdc_side == 0)
          {
            if (c == 0)
            {
              zdcS1_adc = signalFast;
              continue;
            }
            if (c == 2)
            {
              zdcS2_adc = signalFast;
              continue;
            }
            if (c == 4)
            {
              zdcS3_adc = signalFast;
              continue;
            }
          }
          else if (zdc_side == 1)
          {
            if (c == 8)
            {
              zdcN1_adc = signalFast;
              continue;
            }
            if (c == 10)
            {
              zdcN2_adc = signalFast;
              continue;
            }
            if (c == 12)
            {
              zdcN3_adc = signalFast;
              continue;
            }
          }
        }
        /////////////end ZDC channels//////////////////

        //////////// North Veto counters //////////////////
        if (c == 16)
        {
          signalFast = (6 < timingFast && timingFast < 11) ? signalFast : 0;
          veto_NF = signalFast;
          continue;
        }
        if (c == 17)
        {
          signalFast = (6 < timingFast && timingFast < 11) ? signalFast : 0;
          veto_NB = signalFast;
          continue;
        }
        //////////// end North veto counters /////////

        ///////// North SMD ////////
        if (c >= 48 && c < 63)
        {
          signalFast = (9 < timingFast) ? signalFast : 0;

          rawADC[c - 48] = signalFast + pedFast;
          pedADC[c - 48] = pedFast;
          signalADC[c - 48] = signalFast;

          h_rawADC[c - 48]->Fill(signalFast + pedFast);
          h_pedADC[c - 48]->Fill(pedFast);
          h_signalADC[c - 48]->Fill(signalFast);
          if (c == 48)
          {
            smdN1_v_adc = signalFast;
          }
          if (c == 49)
          {
            smdN2_v_adc = signalFast;
          }
          if (c == 54)
          {
            smdN7_v_adc = signalFast;
          }
          if (c == 55)
          {
            smdN8_v_adc = signalFast;
          }
          if (c == 56)
          {
            smdN1_adc = signalFast;
          }
          if (c == 57)
          {
            smdN2_adc = signalFast;
          }
          if (c == 61)
          {
            smdN6_adc = signalFast;
          }
          if (c == 62)
          {
            smdN7_adc = signalFast;
          }
          if (c < 56 && signalFast > 5.)
          {
            nThresholdNHor++;
          }
          if (c >= 56 && signalFast > 5.)
          {
            nThresholdNVer++;
          }
          continue;
        }
        //////// end North SMD///////////////

        //////////// South Veto counters //////////////////
        if (c == 80)
        {
          signalFast = (6 < timingFast && timingFast < 11) ? signalFast : 0;
          veto_SF = signalFast;
          continue;
        }
        if (c == 81)
        {
          signalFast = (6 < timingFast && timingFast < 11) ? signalFast : 0;
          veto_SB = signalFast;
          continue;
        }
        //////////// end South veto counters /////////
        //// South SMD ////////
        if (c >= 112 && c < 127)
        {
          signalFast = (7 < timingFast && timingFast < 12) ? signalFast : 0;
          rawADC[c - 96] = signalFast + pedFast;
          pedADC[c - 96] = pedFast;
          signalADC[c - 96] = signalFast;

          h_rawADC[c - 96]->Fill(signalFast + pedFast);
          h_pedADC[c - 96]->Fill(pedFast);
          h_signalADC[c - 96]->Fill(signalFast);
          if (c == 112)
          {
            smdS1_v_adc = signalFast;
          }
          if (c == 113)
          {
            smdS2_v_adc = signalFast;
          }
          if (c == 118)
          {
            smdS7_v_adc = signalFast;
          }
          if (c == 119)
          {
            smdS8_v_adc = signalFast;
          }
          if (c == 120)
          {
            smdS1_adc = signalFast;
          }
          if (c == 121)
          {
            smdS2_adc = signalFast;
          }
          if (c == 125)
          {
            smdS6_adc = signalFast;
          }
          if (c == 126)
          {
            smdS7_adc = signalFast;
          }
          if (c < 120 && signalFast > 5.)
          {
            nThresholdSHor++;
          }
          if (c >= 120 && signalFast > 5.)
          {
            nThresholdSVer++;
          }
          continue;
        }
        /////// end South SMD //////////////

      }  // end channel loop

      // shower cut (SMD noise threshold cut)
      if (nThresholdNHor > 1 && nThresholdNVer > 1)
      {
        showerCutN = 1;
      }

      if (nThresholdSHor > 1 && nThresholdSVer > 1)
      {
        showerCutS = 1;
      }

      ////////// Compute SMD position and fill hists
      CompSmdAdc();
      CompSmdPos();
      n_y = smd_pos[0];
      n_x = smd_pos[1];
      s_y = smd_pos[2];

      s_x = smd_pos[3];
      smdHits->Fill();
    }  // end if p_zdc good
  }  // end if p_gl1 good

  evtcnt++;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ZDCNeutronLocPol::End(PHCompositeNode * /*topNode*/)
{
  std::cout << "ZDCNeutronLocPol::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  TFile *ofile = new TFile(outfile.c_str(), "RECREATE");

  smdHits->Write();

  // h_waveformZDC->Write();
  // h_waveformSMD_North->Write();
  // h_waveformSMD_South->Write();
  // h_waveformVeto_North->Write();
  // h_waveformVeto_South->Write();
  // h_waveformAll->Write();
  /*
  for (int i = 0; i < 32; i++)
  {
    h_rawADC[i]->Write();
    h_pedADC[i]->Write();
    h_signalADC[i]->Write();
  }
  */
  ofile->Write();
  ofile->Close();
  delete (smdHits);

  return Fun4AllReturnCodes::EVENT_OK;
}

std::vector<float> ZDCNeutronLocPol::anaWaveformFast(CaloPacket *p, const int channel)
{
  std::vector<float> waveform;
  // Chris: preallocation = speed improvement
  waveform.reserve(p->iValue(0, "SAMPLES"));
  for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
  {
    waveform.push_back(p->iValue(s, channel));
  }
  std::vector<std::vector<float>> multiple_wfs;
  multiple_wfs.push_back(waveform);

  std::vector<std::vector<float>> fitresults_zdc;
  fitresults_zdc = WaveformProcessingFast->calo_processing_fast(multiple_wfs);

  std::vector<float> result;
  result = fitresults_zdc.at(0);
  return result;
}

void ZDCNeutronLocPol::CompSmdAdc()  // mulitplying by relative gains
{
  for (int i = 0; i < 15; i++)  // last one is reserved for analog sum
  {
    // multiply SMD channels with their gain factors
    // to get the absolute ADC values in the same units
    // rgains come from CompSmdAdc()
    signalADC[i] = signalADC[i] * smd_north_rgain[i];
    signalADC[i + 16] = signalADC[i + 16] * smd_south_rgain[i];
  }
}

void ZDCNeutronLocPol::CompSmdPos()  // computing position with weighted averages
{
  float w_ave[4];  // 0 -> north hor (y); 1 -> noth vert (x); 2 -> south hor (y); 3 -> south vert. (x)
  float weights[4] = {0};
  memset(weights, 0, sizeof(weights));  // memset float works only for 0
  float w_sum[4];
  memset(w_sum, 0, sizeof(w_sum));

  float smd_threshold = 5.;

  // these constants convert the SMD channel number into real dimensions (cm's)
  const float hor_scale = 2.0 * 11.0 / 10.5 * sin(3.1415 / 4);  // from gsl_math.h
  const float ver_scale = 1.5 * 11.0 / 10.5;
  float hor_offset = (hor_scale * 8 / 2.0) * (7.0 / 8.0);
  float ver_offset = (ver_scale * 7 / 2.0) * (6.0 / 7.0);

  for (int i = 0; i < 8; i++)
  {
    if (signalADC[i] > smd_threshold)
    {
      weights[0] += signalADC[i];            // summing weights
      w_sum[0] += (float) i * signalADC[i];  // summing for the average
    }

    if (signalADC[i + 16] > smd_threshold)
    {
      weights[2] += signalADC[i + 16];
      w_sum[2] += ((float) i + 16.) * signalADC[i + 16];
    }
  }
  for (int i = 0; i < 7; i++)
  {
    if (signalADC[i + 8] > smd_threshold)
    {
      weights[1] += signalADC[i + 8];
      w_sum[1] += ((float) i + 8.) * signalADC[i + 8];
    }

    if (signalADC[i + 24] > smd_threshold)
    {
      weights[3] += signalADC[i + 24];
      w_sum[3] += ((float) i + 24.) * signalADC[i + 24];
    }
  }

  if (weights[0] > 0.0)
  {
    w_ave[0] = w_sum[0] / weights[0];  // average = sum / sumn of weights...
    smd_pos[0] = hor_scale * w_ave[0] - hor_offset;
  }
  else
  {
    smd_pos[0] = 0;
  }

  if (weights[1] > 0.0)
  {
    w_ave[1] = w_sum[1] / weights[1];
    smd_pos[1] = ver_scale * (w_ave[1] - 8.0) - ver_offset;
  }
  else
  {
    smd_pos[1] = 0;
  }

  if (weights[2] > 0.0)
  {
    w_ave[2] = w_sum[2] / weights[2];
    smd_pos[2] = hor_scale * (w_ave[2] - 16.0) - hor_offset;
  }
  else
  {
    smd_pos[2] = 0;
  }

  if (weights[3] > 0.0)
  {
    w_ave[3] = w_sum[3] / weights[3];
    smd_pos[3] = ver_scale * (w_ave[3] - 24.0) - ver_offset;
  }
  else
  {
    smd_pos[3] = 0;
  }
}

void ZDCNeutronLocPol::setFileName(const std::string &fname)
{
  outfile = fname;
}

void ZDCNeutronLocPol::setGainMatch(const std::string &gainfile)
{
  // getting gains
  float col1;
  std::string col2;
   //std::string gainfile = gainname;
   std::ifstream gain_infile(gainfile);

  if (!gain_infile)
  {
    std::cout << "No relative gain file: All relative gains set to 1." << std::endl;
    return;
  }

  std::cout << "SMD Relative Gains: " << std::endl;

  for (float &i : gain)
  {
    gain_infile >> col1 >> col2;
    i = col1;
  }

  for (int i = 0; i < 16; i++)  // relative gains of SMD north channels
  {
    smd_north_rgain[i] = gain[i];  // 0-7: y channels, 8-14: x channels, 15: analog sum
    std::cout << "N" << i << " " << gain[i] << std::endl;
  }

  for (int i = 0; i < 16; i++)  // relative gains of SMD south channels
  {
    smd_south_rgain[i] = gain[i + 16];  // 0-7: y channels, 8-14: x channels, 15: analog sum
    std::cout << "S" << i << " " << gain[i] << std::endl;
  }

  gain_infile.close();
}
