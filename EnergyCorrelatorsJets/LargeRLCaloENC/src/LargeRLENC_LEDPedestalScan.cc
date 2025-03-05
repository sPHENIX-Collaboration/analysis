#include "LargeRLENC_LEDPedestalScan.h"
// Just run a version of the LargeRL enc that only calls the processing and no cuts
int LEDPedestalScan::process_event(PHCompositeNode* topNode)
{
	std::array<float, 3> vertex {0., 0., 0.};
	//try to build a jet like object
	JetContainerv1* jets=enc_Calc->getJets("TowerInfo", "r04", vertex, 0.5, topNode);
	if(jets->size() > 0)
	{
		std::vector<float> fake_ratio;
		for(auto j:*jets)fake_ratio.push_back(0.5);
		enc_Calc->eventCut->passesTheCut(jets, fake_ratio, fake_ratio, 0.5, vertex, fake_ratio);
		jets
		
