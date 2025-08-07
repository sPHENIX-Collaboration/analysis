#include "CentralityValid.h"

#include <centrality/CentralityInfo.h>
#include <calotrigger/MinimumBiasInfo.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/PHRandomSeed.h>
#include <phool/getClass.h>

#include <TF1.h>
#include <TEfficiency.h>
#include <TFile.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TSystem.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

CentralityValid::CentralityValid(const std::string &name, const std::string &hist_name)
  : SubsysReco(name)
{
  _hist_filename = hist_name;
}

CentralityValid::~CentralityValid()
{
  delete hm;
}
int CentralityValid::Init(PHCompositeNode * /*unused*/)
{
  // Histograms

  hm = new Fun4AllHistoManager("CENTRALITY_VALIDATION_HIST");

  _h_centrality_bin = new TH1D("h_centrality_bin", ";Centrality Bin; 1/N{_Events}",20,0, 1.0);
  _h_centrality_bin_mb = new TH1D("h_centrality_bin_mb", ";Centrality Bin; 1/N{_Events}",20,0, 1.0);

  _he_min_bias = new TEfficiency("he_min_bias",";Min Bias?; Fraction of Events", 1, 0, 1);
  hm->registerHisto(_h_centrality_bin);
  hm->registerHisto(_h_centrality_bin_mb);
  hm->registerHisto(_he_min_bias);
  return Fun4AllReturnCodes::EVENT_OK;
}

int CentralityValid::InitRun(PHCompositeNode * /*unused*/)
{
  if (Verbosity())
  {
    std::cout << __FILE__ << " :: " << __FUNCTION__ << std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}


int CentralityValid::process_event(PHCompositeNode *topNode)
{
  if (Verbosity())
  {
    std::cout << __FILE__ << " :: " << __FUNCTION__ << " :: " << __LINE__ << std::endl;
  }

  _central = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  
  if (!_central)
  {
    std::cout << "no centrality node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  _minimumbiasinfo = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  
  if (!_minimumbiasinfo)
  {
    std::cout << "no minimumbias node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  float centile = (_central->has_centile(CentralityInfo::PROP::mbd_NS)?_central->get_centile(CentralityInfo::PROP::mbd_NS) : -999.99);

  _h_centrality_bin->Fill(centile);

  bool isMinBias = _minimumbiasinfo->isAuAuMinimumBias();

  if (isMinBias)
    {
      _h_centrality_bin_mb->Fill(centile);
    }
  _he_min_bias->Fill(isMinBias,0);
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int CentralityValid::End(PHCompositeNode * /* topNode*/)
{
  hm->dumpHistos(_hist_filename.c_str(), "RECREATE");

  return 0;
}
