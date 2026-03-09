#include "laserClusterQA.h"

#include <trackbase/LaserClusterContainerv1.h>
#include <trackbase/LaserClusterv1.h>
#include <trackbase/TpcDefs.h>

#include <tpc/TpcDistortionCorrection.h>
#include <tpc/TpcDistortionCorrectionContainer.h>

#include <ffaobjects/EventHeader.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TVector3.h>

#include <string>



laserClusterQA::laserClusterQA(const std::string& name)
: SubsysReco(name)
{
}

int laserClusterQA::InitRun(PHCompositeNode* topNode)
{

	h = new TH2D("h",Form("Pedestal %d;#phi_{reco};R_{reco} [cm]",m_pedestal),1000,0.036,0.044,1000,43,45);


	adcSpectrum = new TH1D("adcSpectrum",Form("Pedestal %d;Hit ADC",m_pedestal),1001,-0.5,1000.5);

	adcSpecClus = new TH2D("adcSpecClus",Form("Pedestal %d;Hit ADC;Cluster R_{reco} [cm]",m_pedestal),1001,-0.5,1000.5,1000,43,45);
	adcSpecHit = new TH2D("adcSpecHit",Form("Pedestal %d;Hit ADC;Hit R_{reco} [cm]",m_pedestal),1001,-0.5,1000.5,1000,43,45);

	for(int i=0; i<5; i++){
		hSpot[i] = new TH2D(Form("h_%d",i),Form("Spot %d Pedestal %d;#phi_{reco};R_{reco} [cm]",i,m_pedestal),1000,0.036,0.044,1000,43,45);
		adcSpecSpot[i] = new TH1D(Form("adcSpecSpot_%d",i),Form("Spot %d Pedestal %d;Hit ADC",i,m_pedestal),1001,-0.5,1000.5);
	}

	return Fun4AllReturnCodes::EVENT_OK;

}


int laserClusterQA::process_event(PHCompositeNode* topNode)
{
	
	EventHeader *eventHeader = findNode::getClass<EventHeader>(topNode, "EventHeader");
	if(!eventHeader)
	{
		std::cout << PHWHERE << " EventHeader Node missing, abort" << std::endl;
		return Fun4AllReturnCodes::ABORTRUN;
	}

	LaserClusterContainer *lcc = findNode::getClass<LaserClusterContainer>(topNode, "LASER_CLUSTER");
	if (!lcc)
	{
		std::cout << PHWHERE << "LASER_CLUSTER Node missing, abort." << std::endl;
		return Fun4AllReturnCodes::ABORTRUN;
	}

	m_dcc_in_module_edge = findNode::getClass<TpcDistortionCorrectionContainer>(topNode, "TpcDistortionCorrectionContainerModuleEdge");
	if (m_dcc_in_module_edge)
	{
		std::cout << "TpcCentralMembraneMatching:   found TPC distortion correction container module edge" << std::endl;
	}

	m_dcc_in_static = findNode::getClass<TpcDistortionCorrectionContainer>(topNode, "TpcDistortionCorrectionContainerStatic");
	if (m_dcc_in_static)
	{
		std::cout << "TpcCentralMembraneMatching:   found TPC distortion correction container static" << std::endl;
	}

	std::cout << "working on event " << eventHeader->get_EvtSequence() << std::endl;

	int n1700Event = 0;

	auto clusrange = lcc->getClusters();
	for (auto cmitr = clusrange.first;
		cmitr != clusrange.second;
		++cmitr)
	{
		const auto& [cmkey, cmclus_orig] = *cmitr;
		LaserCluster* cmclus = cmclus_orig;
		bool side = (bool) TpcDefs::getSide(cmkey);
		const unsigned int nhits = cmclus->getNhits();

		bool clus1700 = false;

		if(cmclus->getNLayers()<=1 || cmclus->getNIPhi()<=2)
		{
			continue;
		}

		Acts::Vector3 tmp_pos(cmclus->getX(), cmclus->getY(), cmclus->getZ());
		if (m_dcc_in_module_edge)
		{
			tmp_pos = m_distortionCorrection.get_corrected_position(tmp_pos, m_dcc_in_module_edge);
		}
		if (m_dcc_in_static)
		{
			tmp_pos = m_distortionCorrection.get_corrected_position(tmp_pos, m_dcc_in_static);
		}

		TVector3 pos(tmp_pos[0], tmp_pos[1], tmp_pos[2]);

		double R = pos.Perp();
		double phi = pos.Phi();
		int spot = -1;

		if(side && R > 43.0 && R < 45.0 && phi > 0.036 && phi < 0.044)
		{
			n1700Event++;
			n1700Total++;
			clus1700 = true;
			h->Fill(phi,R);

			if(R > 200*phi + 36.14)
			{
				spot = 0;
			}
			else if(R > -200*phi + 51.96 && phi > 0.0398)
			{
				spot = 1;
			}
			else if(R < -175.44*phi + 50.701)
			{
				spot = 3;
			}
			else if(R < 152.6*phi + 37.661)
			{
				spot = 4;
			}
			else
			{
				spot = 2;
			}

			if(spot != -1)
			{
				hSpot[spot]->Fill(phi,R);
			}
		}

		for(int h=0; h<(int)nhits; h++)
		{
			adcSpectrum->Fill(cmclus->getHitAdc(h));

			if(!clus1700)
			{
				continue;
			}

			adcSpecClus->Fill(cmclus->getHitAdc(h),R);
			adcSpecHit->Fill(cmclus->getHitAdc(h), sqrt(pow(cmclus->getHitX(h),2) + pow(cmclus->getHitY(h),2)) );

			if(spot != -1)
			{
				adcSpecSpot[spot]->Fill(cmclus->getHitAdc(h));
			}
		}

	}

	std::cout << "Found " << n1700Event << " in this event and " << n1700Total << " total" << std::endl;

	return Fun4AllReturnCodes::EVENT_OK;
}

int laserClusterQA::End(PHCompositeNode* /*topNode*/)
{
	TFile *outfile = new TFile(m_output.c_str(),"RECREATE");
	outfile->cd();

	h->Write();
	for(int i=0; i<5; i++)
	{
		hSpot[i]->Write();
	}

	adcSpectrum->Write();
	adcSpecClus->Write();
	adcSpecHit->Write();

	for(int i=0; i<5; i++)
	{
		adcSpecSpot[i]->Write();
	}

	outfile->Close();

	return Fun4AllReturnCodes::EVENT_OK;
}
