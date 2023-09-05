#include "CaloTransverseEnergy.h"
std::vector <int> packets;
class PHCompositeNode;
class Fun4AllInputManager;
class Event;

int CaloTransverseEnergy::processEvent(PHCompositeNode *topNode)
{
//	if(!ApplyCuts(e)) return 1;
	std::vector<float> emcalenergy_vec, ihcalenergy_vec, ohcalenergy_vec; 
	if(isPRDF){
		for (auto pn:packets)
		{
			Event* e= findNode::getClass<Event>(topNode, prdfnode);
			if(pn/1000 ==6 ) 
			{
			//this is the EMCal
				processPacket(pn, e, &emcalenergy_vec, false);
			}
			else if (pn/1000 == 7) 
			{
			//inner Hcal
				processPacket(pn, e, &ihcalenergy_vec, true);
			}
			else if (pn/1000 == 8)
			{	
			//outerhcal
				processPacket(pn, e, &ohcalenergy_vec, true);
			}
			else
			{
			//not a calorimeter 
				//packets.erase(&pn); //this is not quite the way to do it, but the idea is there
				continue;
			}
		}
	}
	else { // This is the DST Processor
		//
		energy=0; 
		hcalenergy=0;
		emcalenergy=0;
		energy_transverse=0;
		et_hcal=0;
		et_emcal=0;
		std::string ihcalgeom="TOWERGEOM_HCALIN", ohcalgeom="TOWERGEOM_HCALOUT", emcalgeom="TOWERGEOM_CEMC";
		TowerInfoContainerv1* ihe=findNode::getClass<TowerInfoContainerv1>(_IHCALNode, "TOWERS_HCALIN");
		TowerInfoContainerv1* ohe=findNode::getClass<TowerInfoContainerv1>(_OHCALNode, "TOWERS_HCALOUT");
		TowerInfoContainerv1* eme=findNode::getClass<TowerInfoContainerv1>(_EMCALNode, "TOWERS_CEMC");
		RawTowerGeomContainer_Cylinderv1 *ihg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, ihcalgeom);
		RawTowerGeomContainer_Cylinderv1 *ohg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, ohcalgeom);
		RawTowerGeomContainer_Cylinderv1 *emg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, emcalgeom);
//		GlobalVertexMap *vtxmap=findNode::getClass<PHObject>(topNode, "GLOBAL_VERTEX");
//		GlobalVertex *vtx=vtxmap->begin()->second;
		processDST(eme, &emcalenergy_vec, emg, false, false);
		processDST(ihe, &ihcalenergy_vec, ihg, true, true);
		processDST(ohe, &ohcalenergy_vec, ohg, true, false); 
	}
	float emcaltotal=GetTotalEnergy(emcalenergy_vec,1); //not sure about the calibration factor, need to check
	float ihcaltotal=GetTotalEnergy(ihcalenergy_vec,1);
	float ohcaltotal=GetTotalEnergy(ohcalenergy_vec,1);
	EMCALE->Fill(emcaltotal);
	IHCALE->Fill(ihcaltotal);
	OHCALE->Fill(ohcaltotal);
	ETOTAL->Fill(emcaltotal+ihcaltotal+ohcaltotal);
	datatree->Fill();
	return 1;
}
void CaloTransverseEnergy::processDST(TowerInfoContainerv1* calo_event, std::vector<float>* energies, RawTowerGeomContainer_Cylinderv1* geom, bool hcalorem, bool inner)
{
//	if(!hcalorem) geom->set_calorimeter_id("CEMC"); 
//	else{
//		if(inner) geom->set_calorimeter_id("HCALIN");
//		else geom->set_calorimeter_id("HCALOUT");
//	}
	//This processes all events in the DST in the processor 
	int ntowers=calo_event->size();
	for(int i =0;i<ntowers; i++ )
	{
		TowerInfov1* tower=calo_event->get_tower_at_channel(i);
		float energy1=tower->get_energy();
		int key=calo_event->encode_key(i);
		int phibin=calo_event->getTowerPhiBin(key);
		int etabin=calo_event->getTowerEtaBin(key);
		RawTowerGeom *towergeom=geom->get_tower_geometry(key);
		assert(towergeom);
		if(energy1<=0) continue;
		double eta=geom->get_etacenter(etabin);
		double phi=geom->get_phicenter(phibin);
		energies->push_back(GetTransverseEnergy(energy1, eta));
		energy+=energy1;
		if(!hcalorem){
			emcalenergy+=energy1;
			if(eteeta.find(eta) != eteeta.end()) eteeta[eta]+=energies->at(-1);
			else{
				eteeta[eta]=energies->at(-1);
			}
			if(etephi.find(phi) != etephi.end()){
				etephi[phi]+=energies->at(-1);	
			}
			else{
				etephi[phi]=energies->at(-1);
			}
				
		}
		if(hcalorem){
			hcalenergy+=energy1;
			if(etheta.find(eta) != etheta.end())etheta[eta]+=energies->at(-1);
			else etheta[eta]=energies->at(-1);
			if(ethphi.find(phi) != ethphi.end()) ethphi[phi]+=energies->at(-1);
			else ethphi[phi]=energies->at(-1);
		}
	}
	
}
void CaloTransverseEnergy::processPacket(int packet, Event * e, std::vector<float>* energy, bool HorE)
{
	try{
		e->getPacket(packet);
	}
	catch(std::exception* e) {} 
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
		phibin+=(packet%10)*8;	
		float phval=PhiD->GetBinContent(phibin);
		phval+=GetTransverseEnergy(en,eta);
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
	for(auto e:etphi) eavg+=e.second;  
	eavg=eavg/etphi.size();
	for(auto e:etphi) if(e.second > eavg*1.25 || e.second < eavg*0.75 ) bad++; 
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
	return true;
}
void CaloTransverseEnergy::GetNodes(PHCompositeNode *topNode)
{
	_topNode=topNode;
	_IHCALNode=findNode::getClass<PHCompositeNode>(topNode, iHCALnode);
	_OHCALNode=findNode::getClass<PHCompositeNode>(topNode, oHCALnode); 
	_EMCALNode=findNode::getClass<PHCompositeNode>(topNode, EMCALnode);
}
int CaloTransverseEnergy::Init(PHCompositeNode *topNode)
{
	
	if(!isPRDF) GetNodes(topNode); //load the composite nodes into the global variables if using DST approach
	//std::string outname=("Transverse_Energy_%i_segment_%i.root", run_number, DST_Segment);
	outfile=new TFile(Form("Transverse_Energy%d_segment_%d.root",run_number, DST_Segment), "RECREATE");
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
