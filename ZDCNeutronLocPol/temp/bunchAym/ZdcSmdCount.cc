#include "ZdcSmdCount.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>


#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

#include <ffarawobjects/Gl1Packetv1.h>
#include <ffarawobjects/CaloPacketContainerv1.h>
#include <ffarawobjects/CaloPacketv1.h>
//spin database stuff
#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <TFile.h>
#include <TH2.h>
#include <TH1.h>
#include <TGraphErrors.h>

//____________________________________________________________________________..
ZdcSmdCount::ZdcSmdCount(const std::string &name,const int runnumber,float cutvalue,int roller):
 SubsysReco(name)
{
  runnumber_ = runnumber;
  cut_value_ =  cutvalue;
  char outfile_c[200];
  sprintf(outfile_c,"result_count_%08d-%d-%d.root",runnumber_,static_cast<int>(cut_value_),roller);
  outfile_name_.assign(outfile_c);
  event_counter_ = 0;
  std::cout << "ZdcSmdCount::ZdcSmdCount(const std::string &name) Calling ctor" << std::endl;
  std::cout << "SImpleZdcSmdAna::Output file : " << outfile_name_ << std::endl;
}

//____________________________________________________________________________..
ZdcSmdCount::~ZdcSmdCount()
{
  std::cout << "ZdcSmdCount::~ZdcSmdCount() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int ZdcSmdCount::Init(PHCompositeNode *topNode)
{
  std::cout << "ZdcSmdCount::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  outfile_ = new TFile(outfile_name_.c_str(),"RECREATE");
  outfile_ -> cd();

  InitHist();
  const char *zdccalib = "/sphenix/u/jaein213";
    if (!zdccalib)
  {
    std::cout << "ZDCCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string mappingfilename = std::string(zdccalib) + "/" + "ChannelMapping.dat";
  std::ifstream mapping(mappingfilename);

  if (!mapping)
  {
  std::cout<< " could not be opened."<<std::endl;
    exit(1);
  }
  int adc, array;
  std::string ChannelName;
  for (int i = 0; i < 128; i++)
  {
    mapping >> adc >> array >> ChannelName;
    Chmapping[adc] = array;
    std::cout << adc << " \t" << array << std::endl;
  }

  GetSpinPatterns();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ZdcSmdCount::InitRun(PHCompositeNode *topNode)
{
//  std::cout << "ZdcSmdCount::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int ZdcSmdCount::process_event(PHCompositeNode *topNode)
{
  if (event_counter_ % 1000 == 0)
    std::cout << "event : " << event_counter_ << " started" << std::endl;
  CaloPacketContainer *zdc_cont = findNode::getClass<CaloPacketContainerv1>(topNode, "ZDCPackets");
  CaloPacket *p = zdc_cont->getPacket(0);
  Gl1Packetv1 *gl1 = findNode::getClass<Gl1Packetv1>(topNode, "GL1Packet");
  if (gl1)
  {
    bunchNum_ = gl1->getBunchNumber();
  }
  if (zdc_cont->get_npackets() != 1)
  {
    std::cout << "No packet! skip this event : " << event_counter_ << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }
  float south_zdc[3] = {0.};
  float north_zdc[3] = {0.};
  float south_smd[16] = {0.};
  float north_smd[16] = {0.};
  float south_veto[2] = {0.};
  float north_veto[2] = {0.};
  if (false)
  {

    std::cout << south_zdc[0]
              << north_zdc[0]
              << south_smd[0]
              << north_smd[0]
              << south_veto[0]
              << north_veto[0]
              << std::endl;
  }
  // unsigned int north_smd_h_count = 0; // horizontal smd
  // unsigned int north_smd_v_count = 0; // vertical smd
  // unsigned int south_smd_h_count = 0; // horizontal smd
  // unsigned int south_smd_v_count = 0; // vertical smd
  for (std::map<int, int>::iterator it = Chmapping.begin(); it != Chmapping.end(); ++it) // new mapping for ZDC/SMD/Veto with 2 ADC boards//May 13th 2024
  {
    if (it->second < 0)
      continue;
    if (it->second > 51)
      continue;
    int j = it->second;
    // std::cout<<j<<std::endl;
    double baseline = 0.;
    double baseline_low = 0.;
    double baseline_high = 0.;
    for (int s = 0; s < 3; s++)
    {
      baseline_low += p->iValue(s, it->first);
    }

    baseline_low /= 3.;

    for (int s = p->iValue(0, "SAMPLES") - 3; s < p->iValue(0, "SAMPLES"); s++)
    {
      baseline_high += p->iValue(s, it->first);
    }

    baseline_high /= 3.;
    baseline = baseline_low;

    if (baseline_high < baseline_low)
      baseline = baseline_high;

    if (baseline_high < baseline_low)
      baseline = baseline_high;

    for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
    {
      // h_waveformAll->Fill(s, p->iValue(s, it->first) - baseline);
      if (j < 8)
      {
        h2_SouthZDC_->Fill(s, p->iValue(s, it->first) - baseline);
      }
      else if (j < 16) //-->[0,15]
      {
        h2_NorthZDC_->Fill(s, p->iValue(s, it->first) - baseline);
      }

      else if (j < 32) //-->[16,31]
      {
        h2_NorthSMD_->Fill(s, p->iValue(s, it->first) - baseline);
      }

      else if (j < 48) //-->[32,47]
      {
        h2_SouthSMD_->Fill(s, p->iValue(s, it->first) - baseline);
      }

      else if (j < 50) //-->[48,49]
      {
        h2_NorthVeto_->Fill(s, p->iValue(s, it->first) - baseline);
      }

      else if (j < 52) //-->[50,51]
      {
        h2_SouthVeto_->Fill(s, p->iValue(s, it->first) - baseline);
      }
    }

    ////////////////////////////////////////////////////////

    if (j == 0 || j == 2 || j == 4) // ZDC South High Gain
    {
      float max_adc = -99999;
      float max_t = -1;
      int time_min = 6;
      int time_max = 9;
      for (int s = time_min; s < time_max + 1; s++)
      {
        float temp = p->iValue(s, it->first);
        if (max_adc < temp)
        {
          max_adc = temp;
          max_t = s;
        }
      }
      south_zdc[j / 2] = max_adc - baseline;
      h2_SouthZDC_cut_->Fill(max_t, south_zdc[j / 2]);
      h1_SouthZDC_adc_->Fill(south_zdc[j / 2]);
      h1_SouthZDC_adc_origin_->Fill(max_adc);
      h1_SouthZDC_adc_ped_->Fill(baseline);
    }
    if (j == 8 || j == 10 || j == 12) // ZDC North High Gain
    {
      float max_adc = -99999;
      float max_t = -1;
      int time_min = 6;
      int time_max = 9;
      for (int s = time_min; s < time_max + 1; s++)
      {
        float temp = p->iValue(s, it->first);
        if (max_adc < temp)
        {
          max_adc = temp;
          max_t = s;
        }
      }
      north_zdc[(j - 8) / 2] = max_adc - baseline;
      h2_NorthZDC_cut_->Fill(max_t, north_zdc[(j - 8) / 2]);
      h1_NorthZDC_adc_->Fill(north_zdc[(j - 8) / 2]);
      h1_NorthZDC_adc_origin_->Fill(max_adc);
      h1_NorthZDC_adc_ped_->Fill(baseline);
    }

    if (j > 15 && j < 31) // j == 31 : N SMD ADC SUM
    {
      float max_adc = -99999;
      float max_t = -1;
      int time_min = 10;
      int time_max = 13;
      for (int s = time_min; s < time_max + 1; s++)
      {
        float temp = p->iValue(s, it->first);
        if (max_adc < temp)
        {
          max_adc = temp;
          max_t = s;
        }
      }
      //smd[0] = h1 smd[7] = h8, smd[8] = v1 smd[14] = v7
      north_smd[j - 16] = max_adc - baseline;
      h2_NorthSMD_cut_->Fill(max_t, north_smd[j - 16]);
      h1_NorthSMD_adc_->Fill(north_smd[j - 16]);
      h1_NorthSMD_adc_origin_->Fill(max_adc);
      h1_NorthSMD_adc_ped_->Fill(baseline);
    }
    if (j > 31 && j < 47) // j == 47 : S SMD ADC SUM
    {
      float max_adc = -99999;
      float max_t = -1;
      int time_min = 7;
      int time_max = 12;
      for (int s = time_min; s < time_max + 1; s++)
      {
        float temp = p->iValue(s, it->first);
        if (max_adc < temp)
        {
          max_adc = temp;
          max_t = s;
        }
      }
      //smd[0] = h1 smd[7] = h8, smd[8] = v1 smd[14] = v7
      south_smd[j - 32] = max_adc - baseline;
      h2_SouthSMD_cut_->Fill(max_t, south_smd[j - 32]);
      h1_SouthSMD_adc_->Fill(south_smd[j - 32]);
      h1_SouthSMD_adc_origin_->Fill(max_adc);
      h1_SouthSMD_adc_ped_->Fill(baseline);
    }
    if (j > 47 && j < 50)
    {
      float max_adc = -99999;
      float max_t = -1;
      int time_min = 7;
      int time_max = 12;
      for (int s = time_min; s < time_max + 1; s++)
      {
        float temp = p->iValue(s, it->first);
        if (max_adc < temp)
        {
          max_adc = temp;
          max_t = s;
        }
      }
      north_veto[j - 48] = max_adc - baseline;
      h2_NorthVeto_cut_->Fill(max_t, max_adc - baseline);
      h1_NorthVeto_adc_->Fill(max_adc-baseline);
      h1_NorthVeto_adc_origin_->Fill(max_adc);
      h1_NorthVeto_adc_ped_->Fill(baseline);
    }
    if (j > 49 && j < 52)
    {
      float max_adc = -99999;
      float max_t = -1;
      int time_min = 6;
      int time_max = 8;
      for (int s = time_min; s < time_max + 1; s++)
      {
        float temp = p->iValue(s, it->first);
        if (max_adc < temp)
        {
          max_adc = temp;
          max_t = s;
        }
      }

      south_veto[j - 50] = max_adc - baseline;
      h2_SouthVeto_cut_->Fill(max_t, max_adc - baseline);
      h1_SouthVeto_adc_->Fill(max_adc - baseline);
      h1_SouthVeto_adc_origin_->Fill(max_adc);
      h1_SouthVeto_adc_ped_->Fill(baseline);
    }
  }
  // North side
  h1_NorthZDC1_adc_->Fill(north_zdc[0]);
  h1_NorthZDC2_adc_->Fill(north_zdc[1]);
  h1_NorthZDC3_adc_->Fill(north_zdc[2]);
  if (north_zdc[0] > 100 && north_zdc[1] > 15 && north_veto[0]<150 && north_veto[1] < 150)
 // if (north_zdc[0] > 100 && north_zdc[1] > 75)
  {
    if (north_smd[8] > cut_value_ && north_smd[9] > cut_value_) // v1 v2 Left
    {
      h1_NorthSMD_Vertical_12_->Fill(bunchNum_);
    }
    if (north_smd[13] > cut_value_ && north_smd[14] > cut_value_) // v6 v7 Right
    {
      h1_NorthSMD_Vertical_67_->Fill(bunchNum_);
    }
    if (north_smd[0] > cut_value_ && north_smd[1] > cut_value_) // h1 h2 bottom
    {
      h1_NorthSMD_Horizontal_12_->Fill(bunchNum_);
    }
    if (north_smd[6] > cut_value_ && north_smd[7] > cut_value_) // h7 h8 top
    {
      h1_NorthSMD_Horizontal_78_->Fill(bunchNum_);
    }
  }
  
  h1_SouthZDC1_adc_->Fill(south_zdc[0]);
  h1_SouthZDC2_adc_->Fill(south_zdc[1]);
  h1_SouthZDC3_adc_->Fill(south_zdc[2]);
  if (south_zdc[0] > 100 && south_zdc[1] > 15 && south_veto[0]<150 && south_veto[1]<150)
  //if (south_zdc[0] > 100 && south_zdc[1] > 75)
  {
    if (south_smd[8] > cut_value_ && south_smd[9] > cut_value_) // v1 v2 Left
    {
      h1_SouthSMD_Vertical_12_->Fill(bunchNum_);
    }
    if (south_smd[13] > cut_value_ && south_smd[14] > cut_value_) // v6 v7 Right
    {
      h1_SouthSMD_Vertical_67_->Fill(bunchNum_);
    }
    if (south_smd[0] > cut_value_ && south_smd[1] > cut_value_) // h1 h2 Bottom
    {
      h1_SouthSMD_Horizontal_12_->Fill(bunchNum_);
    }
    if (south_smd[6] > cut_value_ && south_smd[7] > cut_value_) // h7 h8 Top
    {
      h1_SouthSMD_Horizontal_78_->Fill(bunchNum_);
    }
  }
  event_counter_++;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ZdcSmdCount::ResetEvent(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "ZdcSmdCount::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ZdcSmdCount::EndRun(const int runnumber)
{
  std::cout << "ZdcSmdCount::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ZdcSmdCount::End(PHCompositeNode *topNode)
{

  std::cout << "ZdcSmdCount::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  outfile_->Write(); 
  outfile_->Close(); 
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ZdcSmdCount::Reset(PHCompositeNode *topNode)
{
 std::cout << "ZdcSmdCount::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void ZdcSmdCount::Print(const std::string &what) const
{
  if (Verbosity() > 5)
  {
    std::cout << "ZdcSmdCount::Print(const std::string &what) const Printing info for " << what << std::endl;
  }
}
void ZdcSmdCount::SetZDCAdcCut(unsigned int side, unsigned int module, float value)
{
  //not used 
  if (side < kSide_num_ && module < kZdc_num_)
    zdc_adc_cut_[side][module] = value;
  else
  {
    std::cout << "ZdcSmdCount::setZdcAdcCut : side value is unsuitable! Use default value.. " << std::endl;
  }
}

void ZdcSmdCount::GetSpinPatterns()
{
  // Get the spin patterns from the spin DB

  //  0xffff is the qa_level from XingShiftCal //////
  unsigned int qa_level = 0xffff;
  SpinDBContent spin_cont;
  SpinDBOutput spin_out("phnxrc");
  runNum = runnumber_;
  spin_out.StoreDBContent(runNum, runNum, qa_level);
  spin_out.GetDBContentStore(spin_cont, runNum);

  // Get crossing shift
  crossingShift = spin_cont.GetCrossingShift();
  std::cout << "Crossing shift: " << crossingShift << std::endl;

  std::cout << "Blue spin pattern: [";
  for (int i = 0; i < 120; i++)
  {
    spinPatternBlue[i] = spin_cont.GetSpinPatternBlue(i);
    std::cout << spinPatternBlue[i];
    if (i < 119)
      std::cout << ", ";
  }
  std::cout << "]" << std::endl;

  std::cout << "Yellow spin pattern: [";
  for (int i = 0; i < 120; i++)
  {
    spinPatternYellow[i] = spin_cont.GetSpinPatternYellow(i);
    std::cout << spinPatternYellow[i];
    if (i < 119)
      std::cout << ", ";
  }
  std::cout << "]" << std::endl;

  if (false)
  {
    // for testing -- if we couldn't find the spin pattern, fill it with a dummy pattern
    // +-+-+-+- ...
    std::cout << "Could not find spin pattern packet for blue beam! Using dummy pattern" << std::endl;
    for (int i = 0; i < NBUNCHES; i++)
    {
      int mod = i % 2;
      if (mod == 0)
        spinPatternBlue[i] = 1;
      else
        spinPatternBlue[i] = -1;
    }
    // for testing -- if we couldn't find the spin pattern, fill it with a dummy pattern
    // ++--++-- ,,,
    std::cout << "Could not find spin pattern packet for yellow beam! Using dummy pattern" << std::endl;
    for (int i = 0; i < NBUNCHES; i++)
    {
      int mod = i % 4;
      if (mod == 0 || mod == 1)
        spinPatternYellow[i] = 1;
      else
        spinPatternYellow[i] = -1;
    }
  }
}

void ZdcSmdCount::InitHist()
{
  h1_NorthSMD_Vertical_12_ = new TH1I("h1_NSMD_v_12", "h1_NSMD_v_12", 128, 0, 128);
  h1_NorthSMD_Vertical_67_ = new TH1I("h1_NSMD_v_67", "h1_NSMD_v_67", 128, 0, 128);
  h1_NorthSMD_Horizontal_12_ = new TH1I("h1_NSMD_h_12", "h1_NSMD_h_12", 128, 0, 128);
  h1_NorthSMD_Horizontal_78_ = new TH1I("h1_NSMD_h_78", "h1_NSMD_h_78", 128, 0, 128);

  h1_SouthSMD_Vertical_12_ = new TH1I("h1_SSMD_v_12", "h1_SSMD_v_12", 128, 0, 128);
  h1_SouthSMD_Vertical_67_ = new TH1I("h1_SSMD_v_67", "h1_SSMD_v_67", 128, 0, 128);
  h1_SouthSMD_Horizontal_12_ = new TH1I("h1_SSMD_h_12", "h1_SSMD_h_12", 128, 0, 128);
  h1_SouthSMD_Horizontal_78_ = new TH1I("h1_SSMD_h_78", "h1_SSMD_h_78", 128, 0, 128);

  h1_NorthZDC_adc_ = new TH1D("h1_NZDC_adc", "h1_NZDC_adc", 200, 0, 500);
  h1_SouthZDC_adc_ = new TH1D("h1_SZDC_adc", "h1_SZDC_adc", 200, 0, 500);
  h1_NorthZDC_adc_origin_ = new TH1D("h1_NZDC_adc_or", "h1_NZDC_adc_or", 2000, 0, 5000);
  h1_SouthZDC_adc_origin_ = new TH1D("h1_SZDC_adc_or", "h1_SZDC_adc_or", 2000, 0, 5000);
  h1_NorthZDC_adc_ped_ = new TH1D("h1_NZDC_adc_ped", "h1_NZDC_adc_ped", 2000, 0, 5000);
  h1_SouthZDC_adc_ped_ = new TH1D("h1_SZDC_adc_ped", "h1_SZDC_adc_ped", 2000, 0, 5000);
  h1_NorthZDC1_adc_ = new TH1D("h1_NZDC1_adc", "h1_NZDC1_adc", 200, 0, 500);
  h1_SouthZDC1_adc_ = new TH1D("h1_SZDC1_adc", "h1_SZDC1_adc", 200, 0, 500);
  h1_NorthZDC2_adc_ = new TH1D("h1_NZDC2_adc", "h1_NZDC2_adc", 200, 0, 500);
  h1_SouthZDC2_adc_ = new TH1D("h1_SZDC2_adc", "h1_SZDC2_adc", 200, 0, 500);
  h1_NorthZDC3_adc_ = new TH1D("h1_NZDC3_adc", "h1_NZDC3_adc", 200, 0, 500);
  h1_SouthZDC3_adc_ = new TH1D("h1_SZDC3_adc", "h1_SZDC3_adc", 200, 0, 500);
  
  
  h1_NorthSMD_adc_ = new TH1D("h1_NSMD_adc", "h1_NSMD_adc", 200, 0, 500);
  h1_SouthSMD_adc_ = new TH1D("h1_SSMD_adc", "h1_SSMD_adc", 200, 0, 500);
  h1_NorthSMD_adc_origin_ = new TH1D("h1_NSMD_adc_or", "h1_NSMD_adc_or", 2000, 0, 5000);
  h1_SouthSMD_adc_origin_ = new TH1D("h1_SSMD_adc_or", "h1_SSMD_adc_or", 2000, 0, 5000);
  h1_NorthSMD_adc_ped_ = new TH1D("h1_NSMD_adc_ped", "h1_NSMD_adc_ped", 2000, 0, 5000);
  h1_SouthSMD_adc_ped_ = new TH1D("h1_SSMD_adc_ped", "h1_SSMD_adc_ped", 2000, 0, 5000);
  h2_SouthZDC_ = new TH2F("h2_SouthZDC", "h2_SouthZDC", 19, -0.5, 18.5, 512, -500, 16000);
  h2_NorthZDC_ = new TH2F("h2_NorthZDC", "h2_NorthZDC", 19, -0.5, 18.5, 512, -500, 16000);
  h2_SouthSMD_ = new TH2F("h2_SouthSMD", "h2_SouthSMD", 19, -0.5, 18.5, 512, -500, 16000);
  h2_NorthSMD_ = new TH2F("h2_NorthSMD", "h2_NorthSMD", 19, -0.5, 18.5, 512, -500, 16000);
  h2_SouthVeto_ = new TH2F("h2_SouthVeto", "h2_SouthVeto", 19, -0.5, 18.5, 512, -500, 16000);
  h2_NorthVeto_ = new TH2F("h2_NorthVeto", "h2_NorthVeto", 19, -0.5, 18.5, 512, -500, 16000);
  h1_SouthVeto_adc_ = new TH1D("h1_SouthVeto_adc", "h1_SouthVeto_adc", 512, -500, 16000);
  h1_NorthVeto_adc_ = new TH1D("h1_NorthVeto_adc", "h1_NorthVeto_adc", 512, -500, 16000);
  h1_SouthVeto_adc_origin_ = new TH1D("h1_SouthVeto_adc_or", "h1_SouthVeto_or", 512, -500, 16000);
  h1_NorthVeto_adc_origin_ = new TH1D("h1_NorthVeto_adc_or", "h1_NorthVeto_or", 512, -500, 16000);
  h1_SouthVeto_adc_ped_ = new TH1D("h1_SouthVeto_adc_ped", "h1_SouthVeto_ped", 512, -500, 16000);
  h1_NorthVeto_adc_ped_ = new TH1D("h1_NorthVeto_adc_ped", "h1_NorthVeto_ped", 512, -500, 16000);
  h2_SouthZDC_cut_ = new TH2F("h2_SouthZDC_cut", "h2_SouthZDC_cut", 19, -0.5, 18.5, 512, -500, 16000);
  h2_NorthZDC_cut_ = new TH2F("h2_NorthZDC_cut", "h2_NorthZDC_cut", 19, -0.5, 18.5, 512, -500, 16000);
  h2_SouthSMD_cut_ = new TH2F("h2_SouthSMD_cut", "h2_SouthSMD_cut", 19, -0.5, 18.5, 512, -500, 16000);
  h2_NorthSMD_cut_ = new TH2F("h2_NorthSMD_cut", "h2_NorthSMD_cut", 19, -0.5, 18.5, 512, -500, 16000);
  h2_SouthVeto_cut_ = new TH2F("h2_SouthVeto_cut", "h2_SouthVeto_cut", 19, -0.5, 18.5, 512, -500, 16000);
  h2_NorthVeto_cut_ = new TH2F("h2_NorthVeto_cut", "h2_NorthVeto_cut", 19, -0.5, 18.5, 512, -500, 16000);
}
