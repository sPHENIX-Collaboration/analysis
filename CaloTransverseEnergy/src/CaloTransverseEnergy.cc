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
std::vector <int> n_evt_eta;
int CaloTransverseEnergy::process_event(PHCompositeNode *topNode)
{
//	if(!ApplyCuts(e)) return 1;
	n_evt++;
	std::vector<float> emcalenergy_vec, ihcalenergy_vec, ohcalenergy_vec; 
	if(isPRDF){
	//	if(n_evt>1000000) return 1;
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
		TowerInfoContainerv1* ihek=findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_HCALIN");
		TowerInfoContainerv1* ohek=findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_HCALOUT");
		TowerInfoContainerv1* emek=findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_CEMC");
		RawTowerGeomContainer_Cylinderv1 *ihg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, ihcalgeom);
		RawTowerGeomContainer_Cylinderv1 *ohg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, ohcalgeom);
		RawTowerGeomContainer_Cylinderv1 *emg=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, emcalgeom);
		TowerInfoContainerv1* eme=findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERINFO_CALIB_CEMC");
		float z_vtx=0; 
		MbdVertexMap* mbdvtxmap=findNode::getClass<MbdVertexMap>(topNode,"MbdVertexMap");
		//get the z vertex of the event here
		if(mbdvtxmap->empty()) std::cout<<"empty mbdmap" <<std::endl;
		MbdVertex* mbdvtx=nullptr;
		if(mbdvtxmap ){ 
			for(MbdVertexMap::ConstIter mbditer = mbdvtxmap->begin(); mbditer != mbdvtxmap->end(); ++mbditer)
			{
				mbdvtx=mbditer->second;
			}
			if(mbdvtx) z_vtx=mbdvtx->get_z();
		}	
		std::cout<<"Getting Energies" <<std::endl;
		processDST(eme,emek,&emcalenergy_vec, emg, false, false, RawTowerDefs::CEMC, z_vtx);
		processDST(ihe,ihek,&ihcalenergy_vec, ihg, true, true, RawTowerDefs::HCALIN, z_vtx);
		processDST(ohe,ohek,&ohcalenergy_vec, ohg, true, false, RawTowerDefs::HCALOUT, z_vtx);
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
	PLTS.em->ET->Fill(emcaltotal*2.26);
	PLTS.em->dET->Fill(emcaltotal);
	PLTS.ihcal->ET->Fill(ihcaltotal*2.2);
	PLTS.ihcal->dET->Fill(ihcaltotal);
	PLTS.ohcal->ET->Fill(ohcaltotal*2.2);
	PLTS.ohcal->dET->Fill(ohcaltotal);
	evt_data.emcal_et=emcaltotal;
	evt_data.ohcal_et=ohcaltotal;
	evt_data.ihcal_et=ihcaltotal;
	std::map<std::string, float> data {{"EMCAL", emcaltotal}, {"OHCAL", ohcaltotal}, {"IHCAL", ihcaltotal}};
	PLTS.event_data.push_back(data);
//	PLTS.event_data.push_back(std::make_pair("OHCAL", ohcaltotal));
//	PLTS.event_data.push_back(std::make_pair("IHCAL", ihcaltotal));
//	ETOTAL->Fill(emcaltotal+ihcaltotal+ohcaltotal);
	datatree->Fill();
	evt_data.emcal_et=0;
	evt_data.ihcal_et=0;
	evt_data.ohcal_et=0;
	evt_data.emcal_tower_et.clear();
	evt_data.ihcal_tower_et.clear();
	evt_data.ohcal_tower_et.clear();
	
	ihcalenergy_vec.clear();
	ohcalenergy_vec.clear();
	emcalenergy_vec.clear();
	return 1;
}
void CaloTransverseEnergy::processDST(TowerInfoContainerv1* calo_event, TowerInfoContainerv1* calo_key, std::vector<float>* energies, RawTowerGeomContainer_Cylinderv1* geom, bool hcalorem, bool inner, RawTowerDefs::CalorimeterId caloid, float z_vtx)
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
		float radius, z;
		radius=geom->get_radius(); 
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
		z=radius*(2*eta/(1-eta*eta));	
		z+=z_vtx;
		eta=radius/(z+sqrt(z*z+radius*radius));
		if(!hcalorem){
			 if( n_evt == 600 && etabin_em->GetBinContent(etabin) <= 0.001 )  etabin_em->SetBinContent(etabin, eta);
			 if(n_evt == 600 && phibin_em->GetBinContent(phibin) <= 0.001) phibin_em->SetBinContent(phibin, phi_width);
			PLTS.em->z_val->Fill(z_vtx);
		} 
		else{
			if(etabin_hc->GetBinContent(etabin) <= 0.001)  etabin_hc->SetBinContent(etabin, eta_width);
			if(phibin_hc->GetBinContent(phibin) <= 0.001) phibin_hc->SetBinContent(phibin, phi_width);	
			if(inner) PLTS.ihcal->z_val->Fill(z_vtx);
			else PLTS.ohcal->z_val->Fill(z_vtx);
		}
			//if(n_evt == 6670) std::cout<<"Eta bin " <<etabin <<" has width " <<eta_width <<" for calo " <<hcalorem <<inner <<std::endl;
		//if(hcalorem && n_evt==10) std::cout<<"eta bin number " <<etabin<<std::endl;// eta_width=tower_eta_widths[etabin];
		float et=GetTransverseEnergy(energy1, eta);
		float et_div=et;
		if(eta_width>0) et_div=et/eta_width;
/*		phis->Fill(phi);
		etas->Fill(eta);
		PhiD->Fill(phi, et_div);
		EtaD->Fill(eta, et_div);		
		energy+=energy1;
//		EtaPhi->Fill(eta, phi, et);*/
//		tep->Fill(eta, phi, et);
//		teps->Fill(eta, phi);
		if(!hcalorem){
			float rat=PLTS.em->dET_eta->GetNbinsX();
			rat=et_div*96.0/rat;
			emcalenergy+=energy1;
			PLTS.em->phi->Fill(phibin);
			PLTS.em->eta->Fill(etabin);
			PLTS.em->E_phi->Fill(phi, et_div);
			PLTS.em->dET_eta->Fill(eta,rat);
			PLTS.em->ET_eta_phi->Fill(eta, phi, et_div);
			PLTS.em->Hits2D->Fill(etabin, phibin);
			PLTS.em->ET_z_eta->Fill(z_vtx, eta, et_div);
			PLTS.em->ET_z->Fill(z, et);
			try{evt_data.emcal_tower_et[eta]+=et_div;}
			catch(std::exception& e){evt_data.emcal_tower_et[eta]=et_div;}
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
/*			hphis->Fill(phi);
			hetas->Fill(eta);
			if(!inner) ohPhiD->Fill(phi, et);
			else ihPhiD->Fill(phi, et);
			if(!inner) ohEtaD->Fill(etabin, et);
			else ihEtaD->Fill(etabin, et);
			*/
			if(inner){
				PLTS.ihcal->phi->Fill(phibin);
				PLTS.ihcal->eta->Fill(etabin);
				PLTS.ihcal->E_phi->Fill(phi, et);
				PLTS.ihcal->dET_eta->Fill(eta, et_div);
				PLTS.ihcal->ET_eta_phi->Fill(eta, phi, et_div);
				PLTS.ihcal->Hits2D->Fill(etabin, phibin);
				try{evt_data.ihcal_tower_et[eta]+=et_div;}
				catch(std::exception& e){evt_data.ihcal_tower_et[eta]=et_div;}
			}
					
			else{
				try{evt_data.ohcal_tower_et[eta]+=et_div;}
				catch(std::exception& e){evt_data.ohcal_tower_et[eta]=et_div;}
				PLTS.ohcal->phi->Fill(phibin);
				PLTS.ohcal->eta->Fill(etabin);
				PLTS.ohcal->E_phi->Fill(phi, et);
				PLTS.ohcal->dET_eta->Fill(eta, et_div);
				PLTS.ohcal->ET_eta_phi->Fill(eta, phi, et_div);
				PLTS.ohcal->Hits2D->Fill(etabin, phibin);
			}
			etheta[eta].push_back(et);
			EtaPhi->Fill(eta, phi, et, 0, et);
			ethphi[phi].push_back(et);
			/*if(inner){
				ihep->Fill(etabin, phibin, et);
				iheps->Fill(etabin, phibin);
			}
			else{
				try{evt_data.ohcal_tower_et[eta]+=et;}
				catch(std::exception& e){evt_data.ohcal_tower_et[eta]=et_div;}
				ohep->Fill(etabin, phibin, et);
				oheps->Fill(etabin, phibin);
				if(n_evt==10){
					etabin_hc->SetBinContent(etabin, eta);
					phibin_hc->SetBinContent(phibin, phi);
				}*/
			}
		
		energies->push_back(GetTransverseEnergy(energy1, eta));
		
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
		int etabin=0;
		float eta=0, baseline=0, en=0;
		if(HorE){
		 	 int cable_channel=(c%16), cable_number=c/64;
			 cable_channel=cable_channel/2; //doubling of eta to get extra phi resolution
			 etabin=cable_channel+8*cable_number; //24 eta bins
			 eta= (float) etabin/12.0-1.1; //HCal
		}
		else{
			//int cable_channel=(c%16), cable_number=c/64; 
			eta=(((c%16)/2+c/4)-48)/48+1/96; //EMCal
		}
	//	std::cout<<"Eta value is " <<eta <<std::endl;
		for(int s=0; s<p->iValue(c, "SAMPLES"); s++)
		{
			if(s<3) baseline+=p->iValue(s,c); 
			if(s==3) baseline=baseline/3; //first 3 for baseline
			float val=p->iValue(s,c);
			if(val>en) en=val;
		}
		en=en-baseline;
		if(packet/1000 == 7 ) en=en/1966.26;
		if(packet/1000 == 8 ) en=en/311.6;
		if(energy->size() >= 2) if(GetTransverseEnergy(en,eta)>2*energy->at(energy->size()-1)) continue; 
		if(en <=0 ) continue;
		//control for high channels and 0 energy conditions
		energy->push_back(GetTransverseEnergy(en,eta));
	//	std::cout<<"Energy is " <<en <<std::endl;
		//For the hcal get the phi distribution
		//take packet#, each packet has 8 phi bins
		//
		int phibin=(c%64)/16;
		phibin=phibin*2+c%2;
		phibin+=(packet%10)*8;
		float phi=phibin*PI/16;	
		float phval=GetTransverseEnergy(en,eta);
		PhiD->Fill(phi, phval);	
		EtaD->Fill(etabin, phval*24.0/2.2);
		tep->Fill(etabin, phibin, phval);
		teps->Fill(etabin, phibin, 1);
		if(packet/1000 == 7) 
		{
			ihPhiD->Fill(phi, phval);
			ihEtaD->Fill(etabin,phval*24/2.2);
			ihep->Fill(etabin, phibin, phval);
			iheps->Fill(etabin, phibin, 1);
		}
		if(packet/1000 == 8) 
		{
			ohPhiD->Fill(phi, phval);
			ohEtaD->Fill(etabin,phval*24/2.2);
			ohep->Fill(etabin, phibin, phval);
			oheps->Fill(etabin, phibin, 1);
		}
			
		//n_evt_eta.at(etabin)++;
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
	TFile* outfile=new TFile(Form("../data_output/Transverse_Energy_run_%d_segment_%d.root",run_number, DST_Segment), "RECREATE");
	PLTS.em->getAcceptance();
	PLTS.ihcal->getAcceptance();
	PLTS.ohcal->getAcceptance();
	PLTS.em->scaleThePlots(n_evt, PLTS.event_data);
	PLTS.ihcal->scaleThePlots(n_evt, PLTS.event_data);
	PLTS.ohcal->scaleThePlots(n_evt, PLTS.event_data);
	for(auto data_point:PLTS.event_data){
		float total_val=0;
		total_val+=data_point["EMCAL"];
		total_val+=data_point["IHCAL"];
		total_val+=data_point["OHCAL"];
		PLTS.total->dET->Fill(total_val);
	}
	for(unsigned int i=0; i<PLTS.total->hists_1->size(); i++){
		 PLTS.total->hists_1->at(i)->Add(PLTS.em->hists_1->at(i));
		 PLTS.total->hists_1->at(i)->Add(PLTS.ihcal->hists_1->at(i));
		 PLTS.total->hists_1->at(i)->Add(PLTS.ohcal->hists_1->at(i));
	}

	for(unsigned int i=0; i<PLTS.total->hists_2->size(); i++){
		 PLTS.total->hists_2->at(i)->Add(PLTS.em->hists_2->at(i));
		 PLTS.total->hists_2->at(i)->Add(PLTS.ihcal->hists_2->at(i));
		 PLTS.total->hists_2->at(i)->Add(PLTS.ohcal->hists_2->at(i));
	}
//	PLTS.total->BuildTotal(PLTS);
	for(auto h:*(PLTS.em->hists_1)) h->Write();
	for(auto h:*(PLTS.em->hists_2))h->Write();
	for(auto h:*(PLTS.ihcal->hists_1))h->Write();
	for(auto h:*(PLTS.ihcal->hists_2))h->Write();
	for(auto h:*(PLTS.ohcal->hists_1))h->Write();
	for(auto h:*(PLTS.ohcal->hists_2))h->Write();
	for(auto h:*(PLTS.total->hists_1))h->Write();
	for(auto h:*(PLTS.total->hists_2))h->Write();
	/*TH1F* acceptance_eta_em_h=new TH1F("h", "H", 96, eEtaD->GetXaxis()->GetXmin(), eEtaD->GetXaxis()->GetXmax());
	
	int n_emtow=0, n_ihtow=0, n_ohtow=0; 
	seg_acceptance.em_eta_acceptance=std::vector<float> (96,0);
	seg_acceptance.ihcal_eta_acceptance=std::vector<float> (96,0);
	seg_acceptance.ohcal_eta_acceptance=std::vector<float> (96,0);
	for(int i=0; i<eeps->GetNbinsX(); i++){
		 for(int j=0; j<eeps->GetNbinsY(); j++){
				std::cout<<"The number of towers with events is " <<n_emtow <<std::endl; 
				if(eeps->GetBinContent(i,j) > 0){
					 n_emtow++;
					 seg_acceptance.em_eta_acceptance.at(i)+=1;
					 acceptance_eta_em_h->Fill(eEtaD->GetBinCenter(i));
				}
		}
	}
	for (int i=0; i<iheps->GetNbinsX(); i++) for(int j=0; j<iheps->GetNbinsY(); j++){
			 if(iheps->GetBinContent(i,j) > 0){
					 n_ihtow++;
					 seg_acceptance.ihcal_eta_acceptance.at(i)+=1;
			}
	}
	for (int i=0; i<oheps->GetNbinsX(); i++) for(int k=0; k<oheps->GetNbinsY(); k++){
			 if(oheps->GetBinContent(i,k) > 0)
			{
	 			n_ohtow++;
				seg_acceptance.ohcal_eta_acceptance.at(i)+=1;
			}
	}
	seg_acceptance.emcal_acceptance=256*96/float(n_emtow);
	seg_acceptance.ihcal_acceptance=64*24/float(n_ihtow);
	seg_acceptance.ohcal_acceptance=64*24/float(n_ohtow);
	//std::cout<<"acceptances " <<accept_e <<" , " <<accept_ih <<" , " <<accept_oh <<std::endl;
//	eEtaD->Scale(1/n_evt);
	acceptance_eta_em_h->Scale(1/256.);
	//eEtaD->Divide(acceptance_eta_em_h);
//	for(int i=0; i<eEtaD->GetNbinsX(); i++) eEtaD->SetBinContent(i+1, eEtaD->GetBinContent(i+1)*acceptance_eta_em.at(i)/256.);
	ePhiD->Scale(accept_e/n_evt);
	ohEtaD->Scale(accept_oh/n_evt);
	ohPhiD->Scale(accept_oh/n_evt);
	ihEtaD->Scale(accept_ih/n_evt);
	ihPhiD->Scale(accept_ih/n_evt);
//	for(int i=1; i<25; i++) EtaD->Fill(i, ihEtaD->GetBinContent(i)+ohEtaD->GetBinContent(i) + eEtaD->GetBinContent(4*i)+eEtaD->GetBinContent(4+i+1)+eEtaD->GetBinContent(4*i+2)+eEtaD->GetBinContent(4*i+3));
//	for(int i=1; i<33; i++) PhiD->Fill(i, ihPhiD->GetBinContent(i)+ohPhiD->GetBinContent(i) + ePhiD->GetBinContent(4*i)+ePhiD->GetBinContent(4*i+1)+ePhiD->GetBinContent(4*i+2)+ePhiD->GetBinContent(4*i+3));
	//EtaD->Scale(1/n_evt);
	//PhiD->Scale(1/n_evt);
	EtaD->Scale(1/(float)n_evt);
	//just make a ton of histos
	EMCALE->Scale(seg_acceptance.emcal_acceptance);
	EMCALE->Write();
	IHCALE->Scale(seg_acceptance.ihcal_acceptance);
	IHCALE->Write();
	OHCALE->Scale(seg_acceptance.ohcal_acceptance);
	OHCALE->Write();
	ETOTAL=(TH1F*)EMCALE->Clone();
	ETOTAL->SetName("total");
	ETOTAL->SetTitle("Total #frac{dE^{event}_{T}}{d#eta}; #sum_{tow} #frac{d E^{tow}_{T}}{d#eta}; N_{Evt}");
	ETOTAL->Add(IHCALE);
	ETOTAL->Add(OHCALE);
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
//	EtaPhi->Write();*/
	datatree->Write();
	headertree->Fill();
	headertree->Write();
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
	try{
		_EMCALNode=findNode::getClass<PHCompositeNode>(topNode, EMCALnode);
		led=false;
	}
	catch(std::exception& e){led=true;}
}
int CaloTransverseEnergy::Init(PHCompositeNode *topNode)
{
	std::cout<<"initializing analysis on run " <<run_number <<" segement " <<DST_Segment <<std::endl;
	if(!isPRDF) GetNodes(topNode); //load the composite nodes into the global variables if using DST approach
	else{
		for(int i=0; i<24; i++) n_evt_eta.push_back(0);//std::string outname=("Transverse_Energy_%i_segment_%i.root", run_number, DST_Segment);
	}
	EtaPhi=new TNtuple("etaphi", "Eta, Phi Transverse Energy, EmCal ET, iHCal ET, oHCal ET", "eta:phi:et:eet:het");
	EMEtaPhi=new TNtuple("emetaphi", "EMCal #eta, #varphi E_{T}", "eta:phi:et");
	HEtaPhi=new TNtuple("hetaphi", "HCal #eta #varphi E_{T}", "eta, phi, et");
	datatree=new TTree("CALOET", "CALOET");
	datatree->Branch("total_energy", &energy);
	datatree->Branch("hcal_energy", &hcalenergy);
	datatree->Branch("emcal_energy", &emcalenergy);
	datatree->Branch("energy_transverse", &energy_transverse);
	datatree->Branch("angular_data_em", &evt_data);
	headertree=new TTree("HEADER", "RUN DATA");
	headertree->Branch("run number", &run_number);
	headertree->Branch("segment number", &DST_Segment);
	headertree->Branch("acceptances", &seg_acceptance);
	headertree->Branch("number of events", &n_evt);
	headertree->Branch("cuts", &kinematiccuts);
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
	//for(int i=0; i<128; i++){
	//	int em=6001+i;
	//	packets.push_back(em);
	//}
	return 0;	 
}
