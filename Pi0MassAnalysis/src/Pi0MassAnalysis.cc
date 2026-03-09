//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in Pi0MassAnalysis.h.
//
// Pi0MassAnalysis(const std::string &name = "Pi0MassAnalysis")
// everything is keyed to Pi0MassAnalysis, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// Pi0MassAnalysis::~Pi0MassAnalysis()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int Pi0MassAnalysis::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int Pi0MassAnalysis::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int Pi0MassAnalysis::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT;
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT;
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int Pi0MassAnalysis::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int Pi0MassAnalysis::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int Pi0MassAnalysis::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int Pi0MassAnalysis::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void Pi0MassAnalysis::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "Pi0MassAnalysis.h"

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <boost/format.hpp>

#include "TLorentzVector.h"

//____________________________________________________________________________..
Pi0MassAnalysis::Pi0MassAnalysis(const std::string &name)
  : SubsysReco(name)
{
  std::cout << "Pi0MassAnalysis::Pi0MassAnalysis(const std::string &name) Calling ctor" << std::endl;
  _foutname = name;
}

//____________________________________________________________________________..
Pi0MassAnalysis::~Pi0MassAnalysis()
{
  std::cout << "Pi0MassAnalysis::~Pi0MassAnalysis() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int Pi0MassAnalysis::Init(PHCompositeNode *topNode)
{
  std::cout << "Pi0MassAnalysis::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  counter = 0;

  _f = new TFile(_foutname.c_str(), "RECREATE");

  _tree = new TTree("ttree", "");

  _tree->Branch("nclus", &nclus, "nclus/I");
  _tree->Branch("cluster_energy", cluster_energy, "cluster_energy[nclus]/F");
  _tree->Branch("cluster_eta", cluster_eta, "cluster_eta[nclus]/F");
  _tree->Branch("cluster_phi", cluster_phi, "cluster_phi[nclus]/F");
  _tree->Branch("cluster_prob", cluster_prob, "cluster_prob[nclus]/F");
  _tree->Branch("cluster_chi2", cluster_chi2, "cluster_chi2[nclus]/F");

  _tree->Branch("npi0", &npi0, "npi0/I");
  _tree->Branch("pi0cand_pt", pi0cand_pt, "pi0cand_pt[npi0]/F");
  _tree->Branch("pi0cand_eta", pi0cand_eta, "pi0cand_eta[npi0]/F");
  _tree->Branch("pi0cand_phi", pi0cand_phi, "pi0cand_phi[npi0]/F");
  _tree->Branch("pi0cand_mass", pi0cand_mass, "pi0cand_mass[npi0]/F");

  pi0MassHist = new TH1F("pi0mass", ";m_{#gamma#gamma} [GeV];Entries", 50, 0.0, 0.3);

  for (int i = 0; i < 24; i++)
  {
    boost::format formatting("pi0mass %.1lf < eta < %.1lf");
    std::string histName = (formatting % (-1.2 + i * 0.1) % (-1.1 + i * 0.1)).str();
    pi0MassHistEtaDep[i] = new TH1F(histName.c_str(), ";m_{#gamma#gamma} [GeV];Entries", 50, 0.0, 0.3);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Pi0MassAnalysis::InitRun(PHCompositeNode *topNode)
{
  std::cout << "Pi0MassAnalysis::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Pi0MassAnalysis::process_event(PHCompositeNode *topNode)
{
  nclus = 0;

  // Get Vertex
  float vx = 0;
  float vy = 0;
  float vz = 0;
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (vertexmap)
  {
    if (!vertexmap->empty())
    {
      GlobalVertex *vtx = (vertexmap->begin()->second);
      vx = vtx->get_x();
      vy = vtx->get_y();
      vz = vtx->get_z();
    }
  }

  CLHEP::Hep3Vector vertex(vx, vy, vz);

  RawClusterContainer *clusterEM = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if (clusterEM)
  {
    RawClusterContainer::Range begin_end = clusterEM->getClusters();
    for (RawClusterContainer::Iterator iter = begin_end.first; iter != begin_end.second; ++iter)
    {
      RawCluster *clust = iter->second;

      if (clust->get_energy() > minClusterEnergy && clust->get_prob() > photonClusterProbability)
      {
        // CLHEP::Hep3Vector origin(0, 0, 0);
        CLHEP::Hep3Vector cluster_vector = RawClusterUtility::GetECoreVec(*clust, vertex);

        cluster_energy[nclus] = clust->get_energy();
        cluster_eta[nclus] = cluster_vector.pseudoRapidity();
        cluster_phi[nclus] = clust->get_phi();
        cluster_prob[nclus] = clust->get_prob();
        cluster_chi2[nclus] = clust->get_chi2();

        nclus++;
      }
    }
  }

  npi0 = 0;

  for (int i = 0; i < nclus; i++)
  {
    TLorentzVector v1;
    v1.SetPtEtaPhiM(cluster_energy[i] / cosh(cluster_eta[i]), cluster_eta[i], cluster_phi[i], 0.0);
    for (int j = i + 1; j < nclus; j++)
    {
      float alpha = fabs(cluster_energy[i] - cluster_energy[j]) / (cluster_energy[i] + cluster_energy[j]);

      if (alpha > 0.5)
        continue;

      TLorentzVector v2;
      v2.SetPtEtaPhiM(cluster_energy[j] / cosh(cluster_eta[j]), cluster_eta[j], cluster_phi[j], 0.0);

      if (v1.DeltaR(v2) > 0.8)
        continue;

      TLorentzVector res;
      res = v1 + v2;

      pi0cand_pt[npi0] = res.Pt();
      pi0cand_eta[npi0] = res.Eta();
      pi0cand_phi[npi0] = res.Phi();
      pi0cand_mass[npi0] = res.M();
      npi0++;

      pi0MassHist->Fill(res.M());

      int eta_index = floor((res.Eta() + 1.2) * 10);

      if (eta_index >= 0 && eta_index < 24)
      {
        pi0MassHistEtaDep[eta_index]->Fill(res.M());
      }
    }
  }

  _tree->Fill();

  counter++;

  if (counter % 100 == 0)
    std::cout << counter << " events are processed" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Pi0MassAnalysis::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Pi0MassAnalysis::EndRun(const int runnumber)
{
  std::cout << "Pi0MassAnalysis::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Pi0MassAnalysis::End(PHCompositeNode *topNode)
{
  std::cout << "Pi0MassAnalysis::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  _f->Write();
  _f->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Pi0MassAnalysis::Reset(PHCompositeNode *topNode)
{
  std::cout << "Pi0MassAnalysis::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void Pi0MassAnalysis::Print(const std::string &what) const
{
  std::cout << "Pi0MassAnalysis::Print(const std::string &what) const Printing info for " << what << std::endl;
}
