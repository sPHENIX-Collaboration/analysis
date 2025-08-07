
#ifndef LASERQAMULTIPLE_H
#define LASERQAMULTIPLE_H


#include <fun4all/SubsysReco.h>

#include <string>

#include <TTree.h>
#include <TFile.h>
#include <TH3.h>

class PHCompositeNode;
class LaserClusterContainer;
class LaserCluster;
class EventHeader;

class TFile;
class TH1D;
class TH2D;
class TTree;

class laserQA_multiple : public SubsysReco
{
	
	public:
	laserQA_multiple(const std::string &name = "laserQA");

	~laserQA_multiple() override = default;

	void setOutfile(const std::string &outname)
	{
		m_output = outname;
	}

	void setPedestal(int ped)
	{
		m_pedestal = ped;
	}

	int InitRun(PHCompositeNode *topNode) override;

	int process_event(PHCompositeNode *topNode) override;

	int End(PHCompositeNode *topNode) override;

private:

  
  int m_pedestal = 0;
  
  std::string m_output = "laserQA.root";

  TFile *outputFile = nullptr;
  TTree *T = nullptr;
  int event = 0;
  //LaserCluster *lc = nullptr;
  TH3D *hitHist = nullptr;
  TH3D *clustHist = nullptr;
  float layer = 0;
  float iphi = 0;
  float it = 0;
  float adc = 0;
  float phi = 0;
  float R = 0;
  bool fitMode = false;
  
  
  
  TH3D *tmpHitHist = nullptr;

};

#endif
