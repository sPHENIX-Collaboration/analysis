#include "CaloTransverseEnergy.h"
std::vector <int> packets;
class PHCompositeNode;
class Fun4AllInputManager;
class Event;
const std::string &prdfnode="PRDF"; 
const std::string &DSTnode="DST";
const std::string &iHCALnode="HCALIN";
const std::string &oHCALnode="HCALOUT";
const std::string &EMCALnode="CEMC";

int CaloTransverseEnergy::process_event(PHCompositeNode *topNode)
{
//	if(!ApplyCuts(e)) return 1;
	n_evt++;
	std::vector<float> emcalenergy_vec, ihcalenergy_vec, ohcalenergy_vec; 
	if(isPRDF){
		if(n_evt>1000000) return 1;
		for (auto pn:packets)
		{
			std::cout<<"Looking at packet " <<pn <<std::endl;
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
		std::cout <<"Running on event " <<n_evt <<std::endl;
		std::string ihcalgeom="TOWERGEOM_HCALIN", ohcalgeom="TOWERGEOM_HCALOUT", emcalgeom="TOWERGEOM_CEMC";
		TowerInfoContainerv1* ihe=findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERINFO_CALIB_HCALIN");
		TowerInfoContainerv1* ohe=findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERINFO_CALIB_HCALOUT");
		TowerInfoContainerv1* eme=findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERINFO_CALIB_CEMC");
		TowerInfoContainerv1* ihek=findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_HCALIN");
		TowerInfoContainerv1* ohek=findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_HCALOUT");
		TowerInfoContainerv1* emek=findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_CEMC");
		RawTowerGeomContainer_Cylinderv1 *ihg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, ihcalgeom);
		RawTowerGeomContainer_Cylinderv1 *ohg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, ohcalgeom);
		RawTowerGeomContainer_Cylinderv1 *emg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, emcalgeom);
//		GlobalVertexMap *vtxmap=findNode::getClass<PHObject>(topNode, "GLOBAL_VERTEX");
//		GlobalVertex *vtx=vtxmap->begin()->second;
		std::cout<<"Getting Energies" <<std::endl;
		processDST(eme,emek,&emcalenergy_vec, emg, false, false, RawTowerDefs::CEMC);
		processDST(ihe,ihek,&ihcalenergy_vec, ihg, true, true, RawTowerDefs::HCALIN);
		processDST(ohe,ohek,&ohcalenergy_vec, ohg, true, false, RawTowerDefs::HCALOUT);
		std::cout<<"Found energy"<<std::endl; 
	}
	float emcaltotal=GetTotalEnergy(emcalenergy_vec,1)/2.26; //not sure about the calibration factor, need to check
	float ihcalcalib=1, ohcalcalib=1;
	if(isPRDF){
		ihcalcalib=1966.26;
		ohcalcalib=311.6;
	}
	float ihcaltotal=GetTotalEnergy(ihcalenergy_vec,ihcalcalib)/2.2;
	float ohcaltotal=GetTotalEnergy(ohcalenergy_vec,ohcalcalib)/2.2;
	std::cout<<"Adding the data to the histograms" <<std::endl;
	EMCALE->Fill(emcaltotal);
	IHCALE->Fill(ihcaltotal);
	OHCALE->Fill(ohcaltotal);
//	ETOTAL->Fill(emcaltotal+ihcaltotal+ohcaltotal);
	datatree->Fill();
	ihcalenergy_vec.clear();
	ohcalenergy_vec.clear();
	emcalenergy_vec.clear();
	return 1;
}
void CaloTransverseEnergy::processDST(TowerInfoContainerv1* calo_event, TowerInfoContainerv1* calo_key, std::vector<float>* energies, RawTowerGeomContainer_Cylinderv1* geom, bool hcalorem, bool inner, RawTowerDefs::CalorimeterId caloid)
{
	int maxphi=0, maxeta=0;
	geom->set_calorimeter_id(caloid);
	/*if(!hcalorem) geom->set_calorimeter_id("CEMC"); 
	else{
		if(inner) geom->set_calorimeter_id("HCALIN");
		else geom->set_calorimeter_id("HCALOUT");
	}*/
	//This processes all events in the DST in the processor 
	int ntowers=calo_event->size();
	try{
	for(int i =0;i<ntowers; i++ )
	{
		TowerInfov1* tower=calo_event->get_tower_at_channel(i);
		float energy1=tower->get_energy();
		float time=tower->get_time();
		if(inner){
			 if(time<5 || time > 7) energy1=0;
		} //using a specific timing for the relevant runs I'm using, should do the systematic from cdb when available 
		if (hcalorem && ! inner){ 
				if (time<5 || time >8) energy1=0;
		}
		else{
			 if(time < 6 || time >7 ) energy1=0; 
		}
		int key=calo_key->encode_key(i);
		int phibin=calo_key->getTowerPhiBin(key);
		int etabin=calo_key->getTowerEtaBin(key);
		//RawTowerGeom *towergeom=geom->get_tower_geometry(key);
		//assert(towergeom);
		if(energy1 <= 0){continue;}
		if(etabin>maxeta) maxeta=etabin;
		if(phibin>maxphi) maxphi=phibin;
		double eta=geom->get_etacenter(etabin);
		double phi=geom->get_phicenter(phibin);
		std::pair<double, double> etabounds=geom->get_etabounds(etabin);
		double eta_width=abs(etabounds.first - etabounds.second);
		std::pair <double, double> phibounds=geom->get_phibounds(phibin); 
		double phi_width=abs(phibounds.first - phibounds.second);
		if(!hcalorem){
			 eta=1.134/96*(1+2*etabin)-1.134;
			 eta_width=1.134/48;
		}
		else eta_width=2.2/24;
		if(!hcalorem){
			 if( n_evt == 600 && etabin_em->GetBinContent(etabin) <= 0.001 )  etabin_em->SetBinContent(etabin, eta);
			 if(n_evt == 600 && phibin_em->GetBinContent(phibin) <= 0.001) phibin_em->SetBinContent(phibin, phi_width);
		} 
		else{
			if(etabin_hc->GetBinContent(etabin) <= 0.001)  etabin_hc->SetBinContent(etabin, eta_width);
			if(phibin_hc->GetBinContent(phibin) <= 0.001) phibin_hc->SetBinContent(phibin, phi_width);			}
			//if(n_evt == 6670) std::cout<<"Eta bin " <<etabin <<" has width " <<eta_width <<" for calo " <<hcalorem <<inner <<std::endl;
		//if(hcalorem && n_evt==10) std::cout<<"eta bin number " <<etabin<<std::endl;// eta_width=tower_eta_widths[etabin];
		if(eta_width>0) energy1=energy1/eta_width;
		float et=GetTransverseEnergy(energy1, eta);
		phis->Fill(phi);
		etas->Fill(eta);
		PhiD->Fill(phi, et);
		EtaD->Fill(eta, et);		
		energy+=energy1*eta_width;
//		EtaPhi->Fill(eta, phi, et);
		tep->Fill(eta, phi, et);
		teps->Fill(eta, phi);
		if(!hcalorem){
			float rat=eEtaD->GetNbinsX();
			rat=et*rat/96;
			emcalenergy+=energy1;
			ephis->Fill(phi);
			eetas->Fill(eta);
			ePhiD->Fill(phi, et);
			eEtaD->Fill(eta,rat);
			eep->Fill(etabin, phibin, et);
			eeps->Fill(etabin, phibin);
			//try{eteeta[eta]i+=et;}
			//catch(std::exception& e){
				eteeta[eta].push_back(et);
			//}
			//try{
				etephi[phi].push_back(et);
				EtaPhi->Fill(eta, phi,et, et);	
	/*			if(n_evt==10){
					etabin_em->SetBinContent(etabin, eta);
					phibin_em->SetBinContent(phibin, phi);
				}*/
			//}
			//catch(std::exception& e){
			//	etephi[phi]=et;
			//}
				
		}
		if(hcalorem){
			hcalenergy+=energy1;
			hphis->Fill(phi);
			hetas->Fill(eta);
			if(!inner) ohPhiD->Fill(phi, et);
			else ihPhiD->Fill(phi, et);
			if(!inner) ohEtaD->Fill(etabin, et);
			else ihEtaD->Fill(etabin, et);
			//try{etheta[eta]+=et;}
			etheta[eta].push_back(et);
			EtaPhi->Fill(eta, phi, et, 0, et);
			ethphi[phi].push_back(et);
			if(inner){
				ihep->Fill(etabin, phibin, et);
				iheps->Fill(etabin, phibin);
			}
			else{
				ohep->Fill(etabin, phibin, et);
				oheps->Fill(etabin, phibin);
				/*if(n_evt==10){
					etabin_hc->SetBinContent(etabin, eta);
					phibin_hc->SetBinContent(phibin, phi);
				}*/
			}
		}
		energies->push_back(GetTransverseEnergy(energy1*eta_width, eta));
		
	}
	}
	catch (std::exception& e){std::cout<<"Exception found " <<e.what() <<std::endl;}
	//if(n_evt==10) std::cout<<"Calorimeter " <<caloid <<" max: etabin: " <<maxeta <<", phibin: " <<maxphi <<std::endl;
}
void CaloTransverseEnergy::processPacket(int packet, Event * e, std::vector<float>* energy, bool HorE)
{
	try{
		e->getPacket(packet);
	}
	catch(std::exception* e) {return;} 
	Packet *p= e->getPacket(packet);
	if(!p) return;
	std::cout<<"Have the packet with " <<p->iValue(0, "CHANNELS") <<" many channels"<<std::endl; 
	for(int c=0; c<p->iValue(0, "CHANNELS"); c++)
	{
		float eta=0, baseline=0, en=0;
		if(HorE){
		 	 int cable_channel=(c%16), cable_number=c/64;
			 cable_channel=cable_channel/2; //doubling of eta to get extra phi resolution
			 int etabin=cable_channel+8*cable_number; //24 eta bins
			 eta= etabin/12-1; //HCal
		}
		else{
			//int cable_channel=(c%16), cable_number=c/64; 
			eta=(((c%16)/2+c/4)-48)/48+1/96; //EMCal
		}
		std::cout<<"Eta value is " <<eta <<std::endl;
		for(int s=0; s<p->iValue(c, "SAMPLES"); s++)
		{
			if(s<3) baseline+=p->iValue(s,c); 
			if(s==3) baseline=baseline/3; //first 3 for baseline
			float val=p->iValue(s,c);
			if(val>en) en=val;
		}
		en=en-baseline;
		energy->push_back(GetTransverseEnergy(en,eta));
	//	std::cout<<"Energy is " <<en <<std::endl;
		//For the hcal get the phi distribution
		//take packet#, each packet has 8 phi bins
		//
		int phibin=(c%64)/16;
		phibin=phibin*2+c%2;
		phibin+=(packet%10)*8;
		float phi=phibin*PI/16;	
		float phval=PhiD->GetBinContent(phibin);
		phval=GetTransverseEnergy(en,eta);
		PhiD->Fill(phi, phval);	
	}

}
float CaloTransverseEnergy::GetTransverseEnergy(float energy, float eta)
{
	float et=energy*sin(2*atan(exp(-eta)));
	//if (eta<0.55 && eta >0.45 && et==0) std::cout<<"Zero et with energy: " <<energy <<" eta: " <<eta <<" theta: "<<sin(2*atan(exp(-eta)))<<std::endl;
	return et;
}
float CaloTransverseEnergy::GetTotalEnergy(std::vector<float> caloenergy, float calib)
{
	//This is really just a fancy sumation method
	float total_energy=0;
	for(auto e:caloenergy)
	{
		total_energy+=e/calib; //right now this is summing with the calibration factor
	}
//	total_energy=total_energy/caloenergy.size(); //normalize for number of towers, should apply a "percent of towers" but that can be a next step
	//std::cout<<"There are " <<caloenergy.size() <<"active towers in this run with energy " <<total_energy <<std::endl;
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
	return chindf;
}
bool CaloTransverseEnergy::ValidateDistro()
{
	//Just make sure that the distribution looks correct
	double eavg=0, ee=0;
	std::vector<double> et;
	int bad=0;
	for(auto e:etphi){
		ee=0;
		for(auto v:e.second){
			 eavg+=v;
			 ee+=v;
		}
		et.push_back(ee);
	}  
	eavg=eavg/etphi.size();
	for(auto e:et) if(e > eavg*1.25 || e < eavg*0.75 ) bad++; 
	if(bad > etphi.size()*0.25) return false; //if over half of the phi values are outside of the range someting is off
	else return true;
}	
void CaloTransverseEnergy::ProduceOutput()
{
	/*
 	for(auto val:eteeta){
		double eta=val.first;
		int etabin=(eta-1)/12+1;
		int ntows=val.second.size();
		float ete=eEtaD->GetBinContent(etabin);
		ete=ete/ntows;
		eEtaD->SetBinContent(etabin, ete);
	}
	for(auto val:etheta){
		double eta=val.first;
		int etabin=(eta-1)/12+1;
		int ntows=val.second.size();
		float ete=hEtaD->GetBinContent(etabin);
		ete=ete/ntows;
		hEtaD->SetBinContent(etabin, ete);
	}
	for(auto val:etephi){
		double phi=val.first;
		int phibin=(phi*PI)/32+1;
		int ntows=val.second.size();
		float ete=ePhiD->GetBinContent(phibin);
		ete=ete/ntows;
		ePhiD->SetBinContent(phibin, ete);
	}
	for(auto val:ethphi){
		double phi=val.first;
		int phibin=(phi*PI)/16+1;
		int ntows=val.second.size();
		float ete=hPhiD->GetBinContent(phibin);
		ete=ete/ntows;
		hPhiD->SetBinContent(phibin, ete);
	}*/
	TFile* outfile=new TFile(Form("../data_output/Transverse_Energy_run_%d_segment_%d.root",run_number, DST_Segment), "RECREATE");
	std::vector<float> acceptance_eta_em (96, 0);
	TH1F* acceptance_eta_em_h=new TH1F("h", "H", 96, eEtaD->GetXaxis()->GetXmin(), eEtaD->GetXaxis()->GetXmax());
	int n_emtow=0, n_ihtow=0, n_ohtow=0; 
	for(int i=0; i<eeps->GetNbinsX(); i++){
		 for(int j=0; j<eeps->GetNbinsY(); j++){
				std::cout<<"The number of towers with events is " <<n_emtow <<std::endl; 
				if(eeps->GetBinContent(i,j) > 0){
					 n_emtow++;
					 acceptance_eta_em.at(i)+=1;
					 acceptance_eta_em_h->Fill(eEtaD->GetBinCenter(i));
				}
		}
	}
	for (int i=0; i<iheps->GetNbinsX(); i++) for(int j=0; j<iheps->GetNbinsY(); j++) if(iheps->GetBinContent(i,j) > 0) n_ihtow++;
	for (int i=0; i<oheps->GetNbinsX(); i++) for(int k=0; k<oheps->GetNbinsY(); k++) if(oheps->GetBinContent(i,k) > n_evt/10) n_ohtow++;
	float accept_e=256*96/float(n_emtow), accept_ih=64*24/float(n_ihtow), accept_oh=64*24/float(n_ohtow);
	std::cout<<"acceptances " <<accept_e <<" , " <<accept_ih <<" , " <<accept_oh <<std::endl;
	eEtaD->Scale(1/n_evt);
	acceptance_eta_em_h->Scale(1/256.);
	//eEtaD->Divide(acceptance_eta_em_h);
	for(int i=0; i<eEtaD->GetNbinsX(); i++) eEtaD->SetBinContent(i+1, eEtaD->GetBinContent(i+1)*acceptance_eta_em.at(i)/256.);
	ePhiD->Scale(accept_e/n_evt);
	ohEtaD->Scale(accept_oh/n_evt);
	ohPhiD->Scale(accept_oh/n_evt);
	ihEtaD->Scale(accept_ih/n_evt);
	ihPhiD->Scale(accept_ih/n_evt);
//	for(int i=1; i<25; i++) EtaD->Fill(i, ihEtaD->GetBinContent(i)+ohEtaD->GetBinContent(i) + eEtaD->GetBinContent(4*i)+eEtaD->GetBinContent(4+i+1)+eEtaD->GetBinContent(4*i+2)+eEtaD->GetBinContent(4*i+3));
	for(int i=1; i<33; i++) PhiD->Fill(i, ihPhiD->GetBinContent(i)+ohPhiD->GetBinContent(i) + ePhiD->GetBinContent(4*i)+ePhiD->GetBinContent(4*i+1)+ePhiD->GetBinContent(4*i+2)+ePhiD->GetBinContent(4*i+3));
	//EtaD->Scale(1/n_evt);
	//PhiD->Scale(1/n_evt);
	//just make a ton of histos
	EMCALE->Scale(accept_e);
	EMCALE->Write();
	IHCALE->Scale(accept_ih);
	IHCALE->Write();
	OHCALE->Scale(accept_oh);
	OHCALE->Write();
	ETOTAL=(TH1F*)EMCALE->Clone();
	ETOTAL->SetName("total");
	ETOTAL->SetTitle("Total #frac{dE^{event}_{T}}{d#eta}; #sum_{tow} #frac{d E^{tow}_{T}}{d#eta}; N_{Evt}");
	ETOTAL->Add(IHCALE);
	ETOTAL->Add(OHCALE);;
	PhiD->Write();
	EtaD->Write();
	phis->Write();
	etas->Write();
	ePhiD->Write();
	eEtaD->Write();
	ephis->Write();
	eetas->Write();
	ohPhiD->Write();
	ohEtaD->Write();
	hphis->Write();
	ihPhiD->Write();
	ihEtaD->Write();
	hetas->Write();
	eep->Write();
	eeps->Write();
	tep->Write();
	teps->Write();
	ihep->Write();
	iheps->Write();
	ohep->Write();
	oheps->Write();
	etabin_hc->Write();
	phibin_hc->Write();
	etabin_em->Write();
	phibin_em->Write();
//	EtaPhi->Write();
//	datatree->Write();
	outfile->Write();
	outfile->Close();
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
	EtaPhi=new TNtuple("etaphi", "Eta, Phi Transverse Energy, EmCal ET, iHCal ET, oHCal ET", "eta:phi:et:eet:het");
	EMEtaPhi=new TNtuple("emetaphi", "EMCal #eta, #varphi E_{T}", "eta:phi:et");
	HEtaPhi=new TNtuple("hetaphi", "HCal #eta #varphi E_{T}", "eta, phi, et");
	datatree=new TTree("CALOET", "CALOET");
	datatree->Branch("total_energy", &energy);
	datatree->Branch("hcal_energy", &hcalenergy);
	datatree->Branch("emcal_energy", &emcalenergy);
	datatree->Branch("energy_transverse", &energy_transverse);
	datatree->Branch("Et_hcal", &et_hcal);
	datatree->Branch("Et_emcal", &et_emcal);
	/*datatree->Branch("Phi_Et",  &etphi);
	datatree->Branch("EMCal_Phi_Et", &etephi);
	datatree->Branch("HCal_Phi_Et", &ethphi);
	datatree->Branch("Eta_Et", &eteta);
	datatree->Branch("EMCal_Eta_Et", &eteeta);
	datatree->Branch("HCal_Eta_Et", &etheta);*/
	//datatree->Branch("EtaPhi", EtaPhi);
	//datatree->Branch("EM_Eta_Phi", EMEtaPhi);
	//datatree->Branch("HC_Eta_Phi", HEtaPhi);
	for(int i =0; i<8; i++)
	{
		int ih=7001+i;
		int oh=8001+i;
		packets.push_back(ih);
		packets.push_back(oh);
	}
	for(int i=0; i<128; i++){
		int em=6001+i;
		packets.push_back(em);
	}
	return 0;	 
}
