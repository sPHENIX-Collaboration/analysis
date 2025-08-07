
#ifndef LASERCLUSTERQA_H
#define LASERCLUSTERQA_H


#include <fun4all/SubsysReco.h>

#include <tpc/TpcDistortionCorrection.h>
#include <tpc/TpcDistortionCorrectionContainer.h>

#include <string>

class PHCompositeNode;
class LaserClusterContainer;
class EventHeader;

class TFile;
class TH1D;
class TH2D;

class laserClusterQA : public SubsysReco
{
	
	public:
	laserClusterQA(const std::string &name = "laserClusterQA");

	~laserClusterQA() override = default;

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

	TH2D *h;
	TH2D *hSpot[5];

	TH1D *adcSpectrum;
	TH2D *adcSpecClus;
	TH2D *adcSpecHit;

	TH1D *adcSpecSpot[5];

	int m_pedestal = 0;

	int n1700Total = 0;

	std::string m_output = "laserClusterQA.root";

	TpcDistortionCorrection m_distortionCorrection;

	TpcDistortionCorrectionContainer *m_dcc_in_module_edge{nullptr};
	TpcDistortionCorrectionContainer *m_dcc_in_static{nullptr};

};

#endif
