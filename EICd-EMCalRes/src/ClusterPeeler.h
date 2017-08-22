#ifndef __CLUSTERPEELER_H_
#define __CLUSTERPEELER_H__

#include <fun4all/SubsysReco.h>

#include <TFile.h>
#include <TNtuple.h>
#include <TTree.h>

class PHCompositeNode;

/// MAX_NCLUSTER comes from ../macro/globals.h
/// i could not compile ClusterPeeler if i #include'd that file.
/// so, i am redefining it here.

const int MAX_NCLUSTER = 1024;

struct ClusterPeeler : public SubsysReco
{
	/// ClusterPeeler creates and fills a TTree.
	/// each entry holds info on all clusters gathered...
	/// 	
	/// 	for a given event,
	/// 	for a given calorimeter.
	/// 
	/// specifically, a la TTree::Show(), we collect...
	/// 	
	/// 	-- blah output of Show() blah --
	/// 
	/// calorimeter names should be like "CEMC", "FEMC", "HCALIN", et cetera.
	
	/// ClusterPeeler **does not** create its own TFile.
	/// it creates its TTree in the c'tor and saves it in the passed-in TFile.
	/// be sure to call TFile::Write() when this task is all done.
	
	ClusterPeeler(const std::string &name, const std::string &calorimetername, TFile *file_rc);
	
	int process_event(PHCompositeNode *topNode);
	
	TTree *event;
	Int_t ncluster;
	Float_t energy[MAX_NCLUSTER];
	Float_t eta[MAX_NCLUSTER];
	Float_t phi[MAX_NCLUSTER];
};

#endif // __CLUSTERPEELER_H__
