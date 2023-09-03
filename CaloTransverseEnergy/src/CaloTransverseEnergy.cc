#include "CaloTransverseEnergy.h"
std::vector <int> packets;
class PHCompositeNode;
class Fun4AllInputManager;
class Event;

int CaloTransverseEnergy::processEvent(PHCompositeNode *topNode)
{
	if(!ApplyCuts(e)) return 1;
	std::vector<float> emcalenergy, ihcalenergy, ohcalenergy; 
	if(isPRDF){
		for (auto pn:packets)
		{
			Event* e= findNode::getClass<Event>(topNode, prdfnode);
			if(pn/1000 ==6 ) 
			{
			//this is the EMCal
				processPacket(pn, e, &emcalenergy, false);
			}
			else if (pn/1000 == 7) 
			{
			//inner Hcal
				processPacket(pn, e, &ihcalenergy, true);
			}
			else if (pn/1000 == 8)
			{	
			//outerhcal
				processPacket(pn, e, &ohcalenergy, true);
			}
			else
			{
			//not a calorimeter 
				packets.erase(p); //this is not quite the way to do it, but the idea is there
				continue;
			}
		}
	}
	else { // This is the DST Processor
		//
		energy=0; 
		hcalenergy=0;
		emcalenergy=0;
		energy_transeverse=0;
		et_hcal=0;
		et_emcal=0;
		std::string ihcalgoem="TOWERGEOM_HCALIN", ohcalgeom="TOWERGEOM_HCALOUT", emcalgoem="TOWERGEOM_CEMC";
		TowerInfoContainerv1* ihe=findNode::getClass<TowerInfoContainerv1>(_iHCALNode);
		TowerInfoContainerv1* ohe=findNode::getClass<TowerInfoContainerv1>(_oHCALNode);
		TowerInfoContainerv1* eme=findNode::getClass<TowerInfoContainerv1>(_EMCALNode);
		RawTowerGeomContainer_Cylinderv1 *ihg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, ihcalgeom);
		RawTowerGeomContainer_Cylinderv1 *ohg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, ohcalgeom);
		RawTowerGeomContainer_Cylinderv1 *emg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, emcalgeom);
		GlobalVertexMap *vtxmap=findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
		GlobalVertex *vtx=vtxmap->begin()->second;
		processDST(eme, &emcalenergy, emg, vtx, false);
		processDST(ihe, &ihcalenergy, ihg, vtx, true);
		processDST(ohe, &ohcalenergy, ohg, vtx, true); 
	}
	float emcaltotal=GetTotalEnergy(emcalenergy,1); //not sure about the calibration factor, need to check
	float ihcaltotal=GetTotalEnergy(ihcalenergy,1);
	float ohcaltotal=GetTotalEnergy(ohcalenergy,1);
	EMCALE->Fill(emcaltotal);
	IHCALE->Fill(ihcaltotal);
	OHCALE->Fill(ohcaltotal);
	ETOTAL->Fill(emcaltotal+ihcaltotal+ohcaltotal);
	datatree->Fill();
	return 1;
}
void CaloTransverseEnergy::processDST(TowerInfoContainer* calo_event, std::vector<float>* energies, RawTowerGeomContainer* geom, GlobalVertex* vtx)
{
	//This processes all events in the DST in the processor 
	TowerInfoContainerv1::Range tower_range=calo_event->getTowers();
	for(TowerInfoContainerv1::ConstIterator citer=tower_range.first; citer !=tower_range.second; ++citer)
	{
		TowerInfov1* tower=(TowerInfov1*)citer->second;
		float energy1=tower->get_energy();
		int phibin=calo_event->getTowerPhiBin(citer->first);
		int etabin=calo_event->getTowerEtaBin(citer->first);
		RawTowerGeom *towergeom=geom->get_tower_geometry(tower->first);
		assert(towergeom);
		if(energy1<=0) continue;
		double eta=get_etacenter(etabin);
		double phi=get_phicenter(phibin);
		energies->push_back(GetTransverseEnergy(energy1, eta);
		energy+=energy1;
		if(!hcalorem){
			emcalenergy+=energy1;
			if(eteeta.find(eta) != eteeta.end()) eteeta[eta]+=energies.at(-1);
			else{
				eteeta[eta]=energies.at(-1);
			}
			if(etephi.find(phi) != etephi.end(){
				etephi[phi]+=energies.at(-1);	
			}
			else{
				etephi[phi]=energies.at(-1);
			}
				
		}
		if(hcalored){
			hcalenergy+=energy1;
			if(etheta.find(eta) != etheta.end())etheta[eta]+=energies.at(-1);
			else etheta[eta]=energies.at(-1);
			if(ethphi.find(phi) != ethphi.end()) ethphi[phi]+=energies.at(-1);
			else ethphi[phi]=energies.at(-1);
		}
	}
	
}
void CaloTransverseEnergy::processPacket(int packet, Event * e, std::vector<float>* energy, bool HorE)
{
	try{
		e->getPacket(packet);
	}
	catch(std::exception* e) { break;} 
	Packet *p= e->getPacket(packet); 
	for(int c=0; c<p->iValue(0, "CHANNELS"); c++)
	{
		float eta=0, baseline=0, en=0;
		if(HorE) eta=(((c%16)/2+c/64*8)-12)/12+1/24; //HCal
		else eta=(((c%16)/2+c/4)-48)/48+1/96; //EMCal
		for(int s=0; s<p->iValue(c, "SAMPLES"); s++)
		{
			if(s<3) baseline+=p->iValue(s,c); 
			if(s==3) baseline=baseline/3; //first 3 for baseline
			float val=p->iValue(s,c);
			if(val>en) en=val;
		}
		en=en-baseline;
		energy->push_back(GetTransverseEnergy(en,eta));
		//For the hcal get the phi distribution
		//take packet#, each packet has 8 phi bins
		//
		int phibin=(c%64)/16;
		phibin=phibin*2+c%2;
		phibin+=(ph%10)*8;	
		float phval=PhiD->GetBinContent(phibin);
		phcal+=GetTransverseEnergy(en,eta);
		PhiD->SetBinContent(phibin, phval);	
	}

}
float CaloTransverseEnergy::GetTransverseEnergy(float energy, float eta)
{
	float et=energy*sin(2*atan(exp(-eta)));
	return et;
}
float CaloTransverseEnergy::GetTotalEnergy(std::vector<float> caloenergy, float calib)
{
	//This is really just a fancy sumation method
	float total_energy=0;
	for(auto e:caloenergy)
	{
		total_energy+=e*calib; //right now this is summing with the calibration factor
	}
	total_energy=total_energy/caloenergy.size(); //normalize for number of towers, should apply a "percent of towers" but that can be a next step
	return total_energy; 
}
float CaloTransverseEnergy::EMCaltoHCalRatio(float em, float hcal)
{
	//this is to project over to phenix data
	float ratio=em/hcal;
	return ratio; //again, first order correction, probably more to it than this
	
}
void CaloTransverseEnergy::FitToData(std::vector<float> data,int which_model) 
{
	//Apply fits from CGC, Glauber, others, meant to be generalizable
}
float CaloTransverseEnergy::Heuristic(std::vector<float> model_fit)
{
	float chindf=0;
}
bool CaloTransverseEnergy::ValidateDistro()
{
	//Just make sure that the distribution looks correct
	double eavg=0;
	int bad=0;
	for(auto e:etphi) eavg+=e;  
	eavg=eavg/etphi.size();
	for(auto e:etphi) if(e > eavg*1.25 || e < eavg*0.75 ) bad++; 
	if(bad > etphi.size()*0.25) return false; //if over half of the phi values are outside of the range someting is off
	else return true;
}	
void CaloTransverseEnergy::ProduceOutput()
{
	//just make a ton of histos
}
bool CaloTransverseEnergy::ApplyCuts(Event* e)
{
	//pass through a set of cuts
}
void CaloTransverseEnergy::GetNodes(PHCompositeNode *topNode)
{
	_topNode=topNode;
	_IHCALNode=findNode::getClass<TowerInfoContainerv1>(topNode, iHCALnode);
	_OHCALNode=findNode::getClass<TowerInfoContainerv1>(topNode, oHCALnode); 
	_EMCALNode=findNode::getClass<TowerInfoContainerv1>(topNode, EMCalnode);
}
int CaloTransverseEnergy::Init(PHCompositeNode *topNode)
{
	
	if(!isPRDF) GetNodes(topNode); //load the composite nodes into the global variables if using DST approach
	std::string outname="Transverse_Energy_"+ (std::string) run_number + "_segment_"+DST_Segment+".root";
	outfile=new TFile(outname.c_str(), "RECREATE");
	datatree=new TTree("data", "data");
	datatree->Branch("total_energy", &energy);
	datatree->Branch("hcal_energy", &hcalenergy);
	datatree->Branch("emcal_energy", &emcalenergy);
	datatree->Branch("energy_transverse", &energy_transverse);
	datatree->Branch("Et_hcal", &et_hcal);
	datatree->Branch("Et_emcal", &et_emcal);
	datatree->Branch("Phi_Et", &etphi);
	datatree->Branch("EMCal_Phi_Et", &etephi);
	datatree->Branch("HCal_Phi_Et", &ethphi);
	datatree->Branch("Eta_Et", &eteta);
	datatree->Branch("EMCal_Eta_Et", &eteeta);
	datatree->Branch("HCal_Eta_Et", &etheta);	 
}
