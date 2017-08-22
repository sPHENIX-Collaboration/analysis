#include "ClusterPeeler.h"

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <g4cemc/RawClusterContainer.h>
#include <g4cemc/RawCluster.h>

#include <TMath.h>

#include <iostream>
#include <cstdio>

using namespace std;

ClusterPeeler::ClusterPeeler(const std::string &name, const std::string &calorimetername, TFile *file_rc):
	SubsysReco(name),
	event(NULL)
{
	/// create the TTree, saving it in the passed-in file.
	/// be sure the call TFile::Write() after Fun4AllServer::End()
	
	file_rc->cd();
	string treename = "CLUSTER_" + calorimetername;
	string treetitle = calorimetername + " Cluster Containers";
	event = new TTree(treename.c_str(), treetitle.c_str());
	event->Branch("ncluster", &ncluster, "ncluster/I");
	event->Branch("energy", energy, "energy[ncluster]/F");
	event->Branch("eta", eta, "eta[ncluster]/F");
	event->Branch("phi", phi, "phi[ncluster]/F");
}

int ClusterPeeler::process_event(PHCompositeNode *topNode) {
	/// find the correct node
	
	string node_name = event->GetName();
	RawClusterContainer *event_raw = findNode::getClass<RawClusterContainer>(topNode, node_name.c_str());
	if (!event_raw) {
		cout << PHWHERE << "ERROR: no node " << node_name << " found. Aborting event processing.\n";
		return 0;
	}
	
	/// read off the important cluster data
	
	RawClusterContainer::ConstRange begin_end = event_raw->getClusters();
	RawClusterContainer::ConstIterator id_and_cluster;
	ncluster = 0;
	for (id_and_cluster = begin_end.first; id_and_cluster != begin_end.second; ++id_and_cluster) {
		RawCluster *cluster = id_and_cluster->second;
		
		/// the number of noise clusters per event is pretty huge.
		/// cut out everything with "pT < 0.2GeV/c".
		/// ..joe made that threshold up; i am copying him.
		
		//Float_t theta = 2. * TMath::ATan(TMath::Exp(-1.*cluster->get_eta()));
		//Float_t pt = cluster->get_energy() * TMath::Sin(theta);
		if (cluster->get_energy() < 0.2)
			continue;
		
		energy[ncluster] = cluster->get_energy();
		eta[ncluster] = cluster->get_eta();
		phi[ncluster] = cluster->get_phi();
		++ncluster;
	}
	event->Fill();
	return 0;
}
