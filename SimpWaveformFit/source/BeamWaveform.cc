//____________________________________________________________________________..
//
//____________________________________________________________________________..

#include "BeamWaveform.h"
#include "PROTOTYPE4_FEM.h"

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

#include <Event/Event.h>
#include <Event/packet.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>   // for PHIODataNode
#include <phool/PHNode.h>         // for PHNode
#include <phool/PHNodeIterator.h> // for PHNodeIterator
#include <phool/PHObject.h>       // for PHObject
#include <phool/getClass.h>



#include <vector>
#include <map>

#include <TFile.h>
#include <TFile.h>
#include <TGraph.h>
#include <TProfile2D.h>

using namespace std;

//____________________________________________________________________________..
BeamWaveform::BeamWaveform(const std::string &name) : SubsysReco(name)
{
  std::cout << "BeamWaveform::BeamWaveform(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
BeamWaveform::~BeamWaveform()
{
  std::cout << "BeamWaveform::~BeamWaveform() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int BeamWaveform::Init(PHCompositeNode *topNode)
{
  std::cout << "BeamWaveform::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  WaveformProcessing = new CaloWaveformProcessing();
  WaveformProcessing->set_template_name("CEMC_TEMPLATE");
  //WaveformProcessing->set_template_url("/sphenix/u/bseidlitz/work/waveform_cemc_gen_template/macro/beam_template_waveform_cemc.root");
  WaveformProcessing->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  WaveformProcessing->initialize_processing();

  WaveformProcessing2 = new CaloWaveformProcessing();
  //WaveformProcessing2->set_template_name("CEMC_TEMPLATE");
  //WaveformProcessing2->set_template_url("/sphenix/u/bseidlitz/work/waveform_cemc_gen_template/macro/beam-00021796/new_temp.root");
  WaveformProcessing2->set_processing_type(CaloWaveformProcessing::FAST);
  WaveformProcessing2->initialize_processing();

  if (m_Detector == "CEMC")
  {
    encode_tower = TowerInfoDefs::encode_emcal;
  }
  else if (m_Detector == "HCALIN" || m_Detector == "HCALOUT")
  {
    encode_tower = TowerInfoDefs::encode_hcal;
  }

  // outfile
  m_file = new TFile(Form("%s.root", m_Detector.c_str()), "RECREATE");

  // these are the TProfiles which become templates
  for (int i = 0; i < netabins; i++)
  {
    hpwaveform_1[i] = new TProfile(Form("hpwaveform_1_%d", i), Form("hpwaveform_1_%d", i), 300, 0, 30);
    hpwaveform_2[i] = new TProfile(Form("hpwaveform_2_%d", i), Form("hpwaveform_2_%d", i), 300, 0, 30);
    hpwaveform_3[i] = new TProfile(Form("hpwaveform_3_%d", i), Form("hpwaveform_3_%d", i), 300, 0, 30);
  }
  
  hpwaveform_1_sum = new TProfile("hpwaveform_1_sum", "", 300, 0, 30);
  hpwaveform_2_sum = new TProfile("hpwaveform_2_sum", "", 300, 0, 30);
  hpwaveform_3_sum = new TProfile("hpwaveform_3_sum", "", 300, 0, 30);


  // these hists are just for looking at properties of the fit
  h_sig_chi2 = new TH2F("h_sig_chi2","",5500,-100,1e4,5000 ,0,5e6);
  h_sig_chi2_nt = new TH2F("h_sig_chi2_nt","",1100,-100,1e4,1000 ,0,1e5);

  h_sig1_sig2 = new TH2F("h_sig1_sig2","",1100,-100,1e3,1100 ,-100,1e3);
  h_sigP1_sigP2 = new TH2F("h_sigP1_sigP2","",320,-1,31,320 ,-1,31);
  h_chi1_chi2 = new TH2F("h_chi1_chi2","",1e3,0,1e4,1e3 ,0,1e4);
  h_ped1_ped2 = new TH2F("h_ped1_ped2","",1e3,0,1e4,1e3 ,0,1e4);
  h_chi2 = new TH1F("h_chi2","",1e4,0,1e7);
  h_etaphi_ped = new TProfile2D("h_etaphi_ped","",96,0,96,256,0,256,-500,500,"s");
  h_etaphi_ped2 = new TProfile2D("h_etaphi_ped2","",96,0,96,256,0,256,-500,500,"s");

  h_time1 = new TH1F("h_time1","",100,-10,50);
  h_time2 = new TH1F("h_time2","",100,-10,50);

  p_chi2 = new TProfile2D("p_chi2","",96,0,96,256,0,256,0,1e8);
  h_dif_peak_time = new TH2F("h_dif_peak_time","",500,-50,50,500,-1,1);

  for(int i=0; i<NWFs; i++){
    h_wf[i] = new TGraph();
    h_wf[i]->SetName(Form("h_wf%d",i));
  }

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");
  hm = new Fun4AllHistoManager("MYHISTOS");

  se->registerHistoManager(hm);
  //se->registerHisto(m_tree->GetName(), m_tree);


  return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
int BeamWaveform::process_event(PHCompositeNode *topNode)
{
  if (eventCount%1000==0) std::cout << "processing event " << eventCount << std::endl;
  eventCount++;

  // open an event in the prdf
  Event *_event = findNode::getClass<Event>(topNode, "PRDF");
  if (_event == nullptr)
  {
    std::cout << "CaloUnpackPRDF::Process_Event - Event not found" << std::endl;
    return -1;
  }
  if (_event->getEvtType() >= 8) /// special event where we do not read out the calorimeters
  {
    return Fun4AllReturnCodes::DISCARDEVENT;
  }

  int ichannel = 0;
  for (int pid = m_packet_low; pid <= m_packet_high; pid++)
  {
    Packet *packet = _event->getPacket(pid);
    if (!packet){ ichannel += 192; cout << "no packet " << pid << endl;} 
    if (packet)
    {
      int nchannels = packet->iValue(0, "CHANNELS");
      for (int ich = 0; ich < nchannels; ich++)
      {
        unsigned int key = encode_tower(ichannel);
        ichannel++;
        int phibin = TowerInfoDefs::getCaloTowerPhiBin(key);
        int etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
        // check if eta phi bin is in the mask
        std::pair<unsigned int, unsigned int> etaphi = std::make_pair(etabin, phibin);
        if (m_masktower.find(etaphi) != m_masktower.end())
        {
          continue;
        }

        // fill a vector with samples for input to wavefrom fitting
        std::vector<double> vec_signal_samples;
        std::vector<float> temp;
        int n_samples = packet->iValue(ich, "SAMPLES");
        n_samples = 10;

        int max = 0;
        int maxx = 0;
        for (int isample = 0; isample < n_samples; isample++)
        {
          vec_signal_samples.push_back(packet->iValue(isample, ich));
          temp.push_back(packet->iValue(isample, ich));
          if (max < packet->iValue(isample, ich))
          {
            max = packet->iValue(isample, ich);
            maxx = isample;
          }
        }
        std::vector<std::vector<float>> vecvec_signal_samples;
        vecvec_signal_samples.push_back(temp);
         
        float pedDif = vec_signal_samples[0] - vec_signal_samples[1]; 
        float pedDif2 = vec_signal_samples[0] - vec_signal_samples[30]; 
        h_etaphi_ped->Fill(etabin,phibin,pedDif);
        h_etaphi_ped2->Fill(etabin,phibin,pedDif2);

        double fpeak        =0;// NAN;
        double fpeak_sample =0;// NAN;
        double fpedestal    =0;// NAN;
        //double fchi2        =0;// NAN;
        double peak        =0;// NAN;
        double peak_sample =0;// NAN;
        double pedestal    =0;// NAN;
        double chi2        =0;// NAN;
        double peak2        =0;// NAN;
        double peak2_sample =0;// NAN;
        double pedestal2    =0;// NAN;
        double xchi2        =0;// NAN;
        std::map<int, double> parameters_io;
        if (max - vec_signal_samples[0] > -10 && maxx < 13)
        {

            std::vector<std::vector<float>> processed_waveforms = WaveformProcessing->process_waveform(vecvec_signal_samples);
            peak        = processed_waveforms[0][0];
            peak_sample = processed_waveforms[0][1] + 6;
            pedestal    = processed_waveforms[0][2]; 
            chi2        = processed_waveforms[0][3]; 
            
             // this fit is slow so I comment it out if I don't want to use it
            //int verb = 0;
            //if (peak > 700 && chi2 < 20) verb =1;
            //PROTOTYPE4_FEM::SampleFit_PowerLawDoubleExp(vec_signal_samples, peak,
            //                                          fpeak_sample, fpedestal,
            //                                          parameters_io,fchi2
            //                                          , verb);
            fpeak_sample = peak_sample;
            fpeak = peak;
            fpedestal = pedestal;
            processed_waveforms = WaveformProcessing2->process_waveform(vecvec_signal_samples);
            peak2        = processed_waveforms[0][0];
            peak2_sample = processed_waveforms[0][1] ;
            pedestal2    = processed_waveforms[0][2]; 
            xchi2        = processed_waveforms[0][3]; 
        }
        else
        {
          continue;
        }


        h_time1->Fill(peak_sample);
        h_time2->Fill(peak2_sample);
        h_sigP1_sigP2->Fill(peak_sample,peak2_sample);
        h_chi1_chi2->Fill(chi2,xchi2);
        h_ped1_ped2->Fill(pedestal,pedestal2);
        h_sig_chi2->Fill(peak,chi2);
        h_sig_chi2_nt->Fill(peak2,xchi2);
        h_chi2->Fill(chi2);
        p_chi2->Fill(etabin,phibin,chi2);
        float difPeak = peak - peak2;
        float difTime = peak_sample - peak2_sample;
        h_dif_peak_time->Fill(difPeak,difTime);

        // save some waveforms of particular interest 
        if(peak > 1000 && peak < 2000 && chi2 > 5e4 && chi2 < 1e5 && CWFs < NWFs){
           for(int i=0;i<n_samples; i++){
             h_wf[CWFs]->SetPoint(i,i,vec_signal_samples[i]);
           }
           CWFs++;
        }
        

        // ccreate new template
        if (fpeak_sample < 9 && fpeak_sample > 3 )
        {
          h_sig1_sig2->Fill(peak,peak2);
          double offsettime = 6 - peak_sample;

          if (fpeak > 50 && fpeak < 200) {
            for (int i = 0; i < n_samples; i++)
            {
              hpwaveform_1[etabin]->Fill(i + offsettime, (vec_signal_samples[i] - fpedestal) / fpeak);
              hpwaveform_1_sum->Fill(i + offsettime, (vec_signal_samples[i] - fpedestal) / fpeak);
            }
          } else if (fpeak > 200 && fpeak < 700) {
            for (int i = 0; i < n_samples; i++)
            {
              hpwaveform_2[etabin]->Fill(i + offsettime, (vec_signal_samples[i] - fpedestal) / fpeak);
              hpwaveform_2_sum->Fill(i + offsettime, (vec_signal_samples[i] - fpedestal) / fpeak);
            }
          } else if (fpeak > 7000 && fpeak < 7500) {
            for (int i = 0; i < n_samples; i++)
            {
              hpwaveform_3[etabin]->Fill(i + offsettime, (vec_signal_samples[i] - fpedestal) / fpeak);
              hpwaveform_3_sum->Fill(i + offsettime, (vec_signal_samples[i] - fpedestal) / fpeak);
            }
          } else {
            continue;
          }
        }
      } // channel loop 
      ichannel += 192-nchannels;
    }// if packet
    delete packet;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int BeamWaveform::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int BeamWaveform::End(PHCompositeNode *topNode)
{
  std::cout << "BeamWaveform::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  m_file->cd();
  for (int i = 0; i < netabins; i++)
  {
    hpwaveform_1[i]->Write();
    hpwaveform_2[i]->Write();
    hpwaveform_3[i]->Write();
  }
  for(int i=0; i<NWFs; i++) h_wf[i]->Write();

  hpwaveform_1_sum->Write();
  hpwaveform_2_sum->Write();
  hpwaveform_3_sum->Write();
  h_sig_chi2->Write();
  h_chi2->Write();
  h_sig_chi2_nt->Write();
  h_sig1_sig2->Write();
  h_sigP1_sigP2->Write();
  h_time1->Write();
  h_time2->Write();
  h_chi1_chi2->Write();
  h_ped1_ped2->Write();
  p_chi2->Write();
  h_dif_peak_time->Write();
  h_etaphi_ped->Write();
  h_etaphi_ped2->Write();
  m_file->Close();
  hm->dumpHistos(Form("%s.root", m_Detector.c_str()), "MYHISTOS");

  delete m_file;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..

void BeamWaveform::Print(const std::string &what) const
{
  std::cout << "BeamWaveform::Print(const std::string &what) const Printing info for " << what << std::endl;
}
