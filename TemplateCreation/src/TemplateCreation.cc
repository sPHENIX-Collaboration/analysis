
#include "TemplateCreation.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>  // for SubsysReco
#include <fun4all/PHTFileServer.h>


#include <Event/Event.h>
#include <Event/packet.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

#include <TProfile.h>
#include <TMath.h>
#include <TF1.h>
#include <TH2.h>

#include <phool/PHCompositeNode.h>







double TemplateCreation::rising_shape(double *val, double *par){
  double ped = par[0];
  double N_lan = par[1];
  double peak = par[2];
  double width = par[3];
  double x = val[0];
  double result = ped + N_lan*TMath::Landau(x, peak, width);
  return result;
}

//____________________________________________________________________________..
TemplateCreation::TemplateCreation(const std::string &name):
 SubsysReco(name)
 , s_outfilename("testout.root")
 , nsamples(31)
 , m_packet_low(21351)
 , m_packet_high(21352)
 , m_nchannels(192)
 , target_hm(5)
{
}

//____________________________________________________________________________..
TemplateCreation::~TemplateCreation()
{
}

//____________________________________________________________________________..
int TemplateCreation::Init(PHCompositeNode *topNode)
{
  PHTFileServer::get().open(s_outfilename, "RECREATE");
  hp_all_particle_fine = new TProfile("hp_all_particle_fine","", nsamples*binscale, -0.5, nsamples - .5);
  h2_all_particle_fine = new TH2D("h2_all_particle_fine","", nsamples*binscale, -0.5, nsamples - .5, 100, -0.1, 0.5);
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TemplateCreation::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TemplateCreation::process_event(PHCompositeNode *topNode)
{
  TH1F *h_waveform = new TH1F("h_waveform","", nsamples, -0.5, nsamples - 0.5);
  TF1 *f_rising_func = new TF1("f_rising_func",this,&TemplateCreation::rising_shape, 0, 31, 4,"TemplateCreation","rising_shape");

  Event *_event = findNode::getClass<Event>(topNode, "PRDF");
  if (_event == nullptr)
    {
      std::cout << "CaloUnpackPRDF::Process_Event - Event not found" << std::endl;
      return -1;
    }
    if (_event->getEvtType() >= 8)  /// special event where we do not read out the calorimeters
    {
      return Fun4AllReturnCodes::DISCARDEVENT;
    }
    for (int pid = m_packet_low; pid <= m_packet_high; pid++)
    {
      Packet *packet = _event->getPacket(pid);
      if (packet)
	{
	  int nchannels = packet->iValue(0, "CHANNELS");
	  if (nchannels > m_nchannels) // packet is corrupted and reports too many channels
	    {
	      return Fun4AllReturnCodes::DISCARDEVENT;
	    }
	  for (int channel = 0; channel < nchannels; channel++)
	    {
	      float hm = 0;
	      float sum = 0;
	      int maxbin = 0;
	      float shift = 0;

	      std::vector<float> waveform;
	      waveform.reserve(nsamples);
	      for (int samp = 0; samp < nsamples; samp++)
		{
		  h_waveform->SetBinContent(samp + 1, packet->iValue(samp, channel));
		}
	      
	      maxbin = h_waveform->GetMaximumBin();
	      if (maxbin > 12){continue;} //Selection region really only needed for test beam data
	      if (maxbin <= 5) {continue;}
	      if (h_waveform->GetMaximum() < 2000){continue;} 

	      float pedestal = 1500; 
	      if (maxbin > 5)
		{
		  pedestal = 0.5 * (h_waveform->GetBinContent(maxbin - 6) + h_waveform->GetBinContent(maxbin - 5));
		}

	      for (int j = 1; j <= nsamples; j++)
		{
		  if (h_waveform->GetBinContent(j) - pedestal  > 0)
		    {
		      sum += h_waveform->GetBinContent(j) - pedestal;
		    }
		}
	      for (int j = 0; j <= nsamples; j++)
		{
		  if (((h_waveform->GetBinContent(j+1)) - pedestal) > 0)
		    {
		      h_waveform->SetBinContent(j + 1, ((h_waveform->GetBinContent(j+1)) - pedestal)/sum);
		    }
		  else
		    {
		      h_waveform->SetBinContent(j + 1,0);
		    }
		}
	      
	      if (h_waveform->GetBinContent(h_waveform->GetMaximumBin()) > 0.33){continue;}

	      bool pileup = false;
	      for (int k = maxbin+1; k <= nsamples;k++)
		{
		  if ( h_waveform->GetBinContent(k) > 1.05 *  h_waveform->GetBinContent(k-1))
		    {
		      pileup = true;
		    }
		}
	     if (pileup){continue;}

	      f_rising_func->SetParameters(0, 0.1, static_cast<double>(h_waveform->GetBinCenter(maxbin)), 1);//, p3, p4, p5);
	      f_rising_func->FixParameter(0, 0);
	      f_rising_func->SetParLimits(2,static_cast<double>(h_waveform->GetBinCenter(maxbin - 1)), static_cast<double>(h_waveform->GetBinCenter(maxbin + 1)));
	      f_rising_func->SetParLimits(3, 0.5, 3);
	      f_rising_func->SetRange(static_cast<float>(h_waveform->GetBinCenter(maxbin-3)), static_cast<float>(h_waveform->GetBinCenter(maxbin+1)));
	      h_waveform->Fit("f_rising_func","NRQ");
	      hm = f_rising_func->GetParameter(2) - 1.5*f_rising_func->GetParameter(3);
	      shift = target_hm - hm;

	      bool skipwaveform = false;
	      for (int i = h_waveform->GetMaximumBin() + 5; i < nsamples ;i++)
		{
		  if (h_waveform->GetMaximum() < binscale*h_waveform->GetBinContent(i))
		    {
		      skipwaveform = true;
		      continue;
		    }
		}
	      if (skipwaveform){continue;}
	      for (int j = 0; j < nsamples; j++)
		{
		  hp_all_particle_fine->Fill(j + shift, h_waveform->GetBinContent(j+1));
		  h2_all_particle_fine->Fill(j + shift, h_waveform->GetBinContent(j+1));
		}
	    }
	}
    }


    delete h_waveform;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TemplateCreation::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TemplateCreation::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TemplateCreation::End(PHCompositeNode *topNode)
{
  hp_all_particle_fine->Scale(1/hp_all_particle_fine->GetMaximum());
  


  for (int i = 0 ; i < hp_all_particle_fine->GetNbinsX();i++)
    {
      if ( hp_all_particle_fine->GetBinContent(i+1) < 0.001)
	{
	  hp_all_particle_fine->SetBinContent(i+1,0);
	}
    }
  

  
  PHTFileServer::get().cd(s_outfilename);
  hp_all_particle_fine->SetName("h_template");
  hp_all_particle_fine->Write();
  h2_all_particle_fine->Write();
  
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TemplateCreation::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

