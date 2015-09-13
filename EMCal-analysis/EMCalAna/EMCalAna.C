#include "EMCalAna.h"

#include <g4eval/CaloEvalStack.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/getClass.h>
#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Particle.h>
#include <g4hough/SvtxVertexMap.h>
#include <g4cemc/RawTowerContainer.h>
#include <g4cemc/RawTowerGeom.h>
#include <g4cemc/RawTower.h>
#include <g4cemc/RawClusterContainer.h>
#include <g4cemc/RawCluster.h>

#include <TNtuple.h>
#include <TFile.h>

#include <iostream>
//#include <vector>
//#include <algorithm>
#include <cmath>

using namespace std;

EMCalAna::EMCalAna(const string &name, const string &caloname,
    const string &filename) :
    SubsysReco(name), _caloname(caloname), _ievent(0)
{
  verbosity = 0;
}

int
EMCalAna::Init(PHCompositeNode *topNode)
{

  _ievent = 0;

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalAna::fillOutputNtuples() entered" << endl;

  CaloEvalStack caloevalstack(topNode, _caloname);
  CaloRawClusterEval* clustereval = caloevalstack.get_rawcluster_eval();
  CaloRawTowerEval* towereval = caloevalstack.get_rawtower_eval();
  CaloTruthEval* trutheval = caloevalstack.get_truth_eval();

  ++_ievent;

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

