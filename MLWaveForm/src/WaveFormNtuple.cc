
#include "WaveFormNtuple.h"

#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <TFile.h>
#include <TNtuple.h>

//____________________________________________________________________________..
WaveFormNtuple::WaveFormNtuple(const std::string &name):
 SubsysReco(name)
{
}

//____________________________________________________________________________..
int WaveFormNtuple::InitRun(PHCompositeNode * /*topNode*/)
{
  outfile = TFile::Open("ntuple.root","RECREATE");
  ntup = new TNtuple("ntup","waveform analysis","e:t:chi2:ped:status:s5:s6:s7");
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int WaveFormNtuple::process_event(PHCompositeNode *topNode)
{
  TowerInfoContainer *towerinfocontainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERS_HCALOUT");
  if (towerinfocontainer)
  {
    unsigned int nchannels = towerinfocontainer->size();
    for (unsigned int channel = 0; channel < nchannels; channel++)
    {
      TowerInfo *rawtwr = towerinfocontainer->get_tower_at_channel(channel);
      
      // std::cout << "energy: " << rawtwr->get_energy() << std::endl;
      // std::cout << "time: " << rawtwr->get_time_float() << std::endl;
      // for (int j = 0; j < 12; j++)
      // {
      // 	std::cout << "waveform_value[" << j << "]: " << rawtwr->get_waveform_value(j) << std::endl;
      // }
      ntup->Fill(rawtwr->get_energy(), rawtwr->get_time_float(),rawtwr->get_chi2(), rawtwr->get_pedestal(),rawtwr->get_status(), rawtwr->get_waveform_value(5), rawtwr->get_waveform_value(6), rawtwr->get_waveform_value(7));
    }
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int WaveFormNtuple::End(PHCompositeNode * /*topNode*/)
{
  std::cout << "WaveFormNtuple::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  outfile->cd();
  ntup->Write();
  outfile->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}
