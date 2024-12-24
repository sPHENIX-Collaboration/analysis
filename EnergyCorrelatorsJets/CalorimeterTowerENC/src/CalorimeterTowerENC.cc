//////////////////////////////////////////////////////////////////////////////////////////////////
//												//
//												//
//		Calorimeter Tower Jets N Point-Energy Correlator Study				//
//		Author: Skadi Grossberndt							//
//		Date of First Commit: 12 July 2024						//
//		Date of Last Update:  15 Oct 2024						//
//		version: v2.0									//
//												//
//												//
//		This project is a study on energy correlators using particle jets and 		//
//	jets  definied over calorimeter towers. Feasibility study to evaluate pp prospects	//
//	for sPHENIX in 2024 with minimal tracking and momentum resolution 			//
//												//
//////////////////////////////////////////////////////////////////////////////////////////////////


#include "CalorimeterTowerENC.h"
 
CalorimeterTowerENC::CalorimeterTowerENC(int n_run, int n_segment, float jet_min, const std::string &name){
	//this is the constructor
	n_evts=0;
	this->jet_cutoff=jet_min;
	std::stringstream run, seg;
	run.width(10);
	run.fill('0');
	run <<n_run;
	seg.width(5);
	seg.fill('0');
	seg << n_segment;
	outfilename="Calorimeter_tower_ENC-"+run.str()+"-"+seg.str()+".root";
	jethits=new TH2F("jethits", "Location of particle energy deposition from truth jets; #eta; #varphi; N_{hits}", 200, -1.15, 1.05, 200, -3.1416, 3.1415);
	calojethits=new TH2F("calojethits", "Location of energy deposition from calo only anti-k_{T} jets; #eta; #varphi; N_{hits}", 200, -1.15, 1.05, 200, -3.1416, 3.1415);
	comptotows=new TH2F("comp_to_towers", "Particle energy deposition versus EMCal tower energy per EMCAL tower; E_{particle} [GeV]; E_{emcal} [GeV]; N", 50, -0.5, 49.5, 50, -0.05, 4.95 ); 
	number_of_jets=new TH1F("nJ", "Number of Jets per event; N_{jet}; events", 100, -0.5, 99.5);
	//cont_pos=new TH1F("Continuous_approx", "Monte Carlo approximation to ideal resolution; R_L; < #sum  pairlikes >", 60, -0.05, 0.85);
	EM_energy=new TH1F("emcal_energy", "Total energy in emcal; E [GeV]; events", 200, -0.25, 99.75);
	IH_energy=new TH1F("ihcal_energy", "Total energy in ihcal; E [GeV]; events", 200, -0.25, 99.75);
	OH_energy=new TH1F("ohcal_energy", "Total energy in ohcal; E [GeV]; events", 200, -0.25, 99.75);
	
	Particles=new MethodHistograms("Particles");
	EMCal=new MethodHistograms("EMCAL");
	IHCal=new MethodHistograms("IHCAL");
	OHCal=new MethodHistograms("OHCAL");
	AllCal=new MethodHistograms("ALLCAL");
	EMCalKT=new MethodHistograms("EMCAL_kt");
	IHCalKT=new MethodHistograms("IHCAL_kt");
	OHCalKT=new MethodHistograms("OHCAL_kt");
	AllCalKT=new MethodHistograms("ALLCAL_kt");
	histogram_map["parts"]=Particles;
	histogram_map["emcal"]=EMCal;
	histogram_map["ihcal"]=IHCal;
	histogram_map["ohcal"]=OHCal;
	histogram_map["emcalkt"]=EMCalKT;
	histogram_map["ihcalkt"]=IHCalKT;
	histogram_map["ohcalkt"]=OHCalKT;
	histogram_map["allcal"]=AllCal;
	histogram_map["allcalkt"]=AllCalKT;
	for(auto h:this->histogram_map){
		std::string typelabel = h.first;
		//auto hists=h.second;
		std::cout<<"Making the histograms for " <<h.first <<std::endl;
//		for(auto h1:hists->histsvector) std::cout<<"Have a histogram with that has name " <<h1->GetName()  <<std::endl;
	}
}

int CalorimeterTowerENC::Init(PHCompositeNode *topNode)
{	
	//book histograms and TTrees
	histogram_map["parts"]->E->GetName();	
	return 0; 
}

float CalorimeterTowerENC::getPt(PHG4Particle* p)
{
	float pt=0;
	pt=pow(p->get_px(), 2) + pow(p->get_py(), 2);
	pt=sqrt(pt);
	return pt;
}

float CalorimeterTowerENC::getR(std::pair<float, float> p1, std::pair<float,float> p2){
	float eta_dist=p1.first - p2.first;
	float phi_dist=p1.second - p2.second;
	phi_dist=abs(phi_dist);
	if(phi_dist > PI) phi_dist=2*PI-phi_dist;
	float R=sqrt(pow(eta_dist,2) + pow(phi_dist, 2));
	return R;
}
void CalorimeterTowerENC::GetENCCalo(std::map<int, float> allcal, RawTowerGeomContainer_Cylinderv1* geom, TowerInfoContainer* data, MethodHistograms* histograms, float jete_tot)
{
	geom->set_calorimeter_id(RawTowerDefs::HCALOUT);
	int ntow=0; 
	float jete=0.0;
	for(auto e:allcal) jete+=e.second;
	for(auto e:allcal)
	{
		if(e.second >= 0.01) ntow++; //10MeV threshold
		if(!geom) continue;
		auto key_1=data->encode_key(e.first);
//		auto tower_2 = data->get_tower_at_channel(e.first);
		int phibin_1=data->getTowerPhiBin(key_1);
		int etabin_1=data->getTowerEtaBin(key_1);
		if(etabin_1 < 0 || phibin_1 < 0 ||  phibin_1 >= geom->get_phibins() || etabin_1 >= geom->get_etabins() ) continue;
		float phi_center_1 = geom->get_phicenter(phibin_1);
		float eta_center_1 = geom->get_etacenter(etabin_1);
		for(auto f:allcal)
		{
			if(e.first >= f.first) continue;
			auto key_2 = data->encode_key(f.first);
//			auto tower_2 = data->get_tower_at_channel(f.first);
			int phibin_2 = data->getTowerPhiBin(key_2);
			int etabin_2 = data->getTowerEtaBin(key_2);
			float phi_center_2 = geom->get_phicenter(phibin_2);
			float eta_center_2 = geom->get_etacenter(etabin_2);
			if(etabin_2 < 0 || phibin_2 < 0 ||  phibin_2 >= geom->get_phibins() || etabin_2 >= geom->get_etabins() ) continue;
			std::pair<float, float> tower_center_1 {eta_center_1, phi_center_1}, tower_center_2 {eta_center_2, phi_center_2};
			float R_12=getR(tower_center_1, tower_center_2);
			histograms->R_geom->Fill(R_12);
			float jet2=pow(jete,2);
			float jet2t=pow(jete_tot,2);
			if (n_evts < 2) std::cout<<"The jet energy divsor is " <<jet2<<std::endl;
			float e2c=e.second*f.second / jet2;
			float e2c_all=e.second*f.second /jet2t; 
			if(abs(e.second) <= 0.01 )continue; //10 MeV threshold for consideration
			if(abs(f.second) <= 0.01) continue; //10 MeV threshold for consideration
			if(e2c != 0 ) histograms->R->Fill(R_12);
			e2c=e2c/((float) Nj);
			e2c_all=e2c_all/((float) Nj);
			std::cout<<"The 2 point energy correlator is " <<e2c <<std::endl;
			if(!std::isnormal(e2c) ) continue;
			histograms->E2C->Fill(R_12, e2c/*/(float) Nj*/);
			for(auto g:allcal)
			{
				if( f.first >= g.first || e.first >= g.first) continue;
				if(g.first < 0 /*|| k >= (int) data->size()*/ ) continue;
				auto key_3 = data->encode_key(g.first);
				int phibin_3 = data->getTowerPhiBin(key_3);
				int etabin_3 = data->getTowerEtaBin(key_3);
				if(etabin_3 < 0 || phibin_3 < 0 ||  phibin_3 >= geom->get_phibins() || etabin_3 >= geom->get_etabins() ) continue;
				float phi_center_3 = geom->get_phicenter(phibin_3);
				float eta_center_3 = geom->get_etacenter(etabin_3);
				std::pair<float, float> tower_center_3 {eta_center_3, phi_center_3};
				float R_13=getR(tower_center_1, tower_center_3);
				float R_23=getR(tower_center_2, tower_center_3);
				float e3c=e.second*f.second*g.second;
				float jete3=pow(jete, 3);
				float jete3t=pow(jete_tot, 3);
				float e3c_all=e3c/jete3t;
				e3c=e3c/jete3;
				float R_L = std::max(R_12, R_13);
				R_L=std::max(R_L, R_23);
				e3c=e3c/((float) Nj);
				e3c_all=e3c_all/((float) Nj);
				if(!std::isnormal(e3c)) continue;
				histograms->E3C->Fill(R_L, e3c/*/(float) Nj*/);
				if(std::isnormal(e3c_all)) histograms->E3C_pt->Fill(R_L, e3c_all);
			}
		}
	}
	histograms->N->Fill(ntow);
	histograms->E->Fill(jete);
}
		
void CalorimeterTowerENC::GetENCCalo(PHCompositeNode* topNode, std::unordered_set<int> tower_set, TowerInfoContainer* data, RawTowerGeomContainer_Cylinderv1* geom, RawTowerDefs::CalorimeterId calo, float jete, std::string caloh, int npt, float jete_tot, std::map<int, float>* allcal_e)
{
	geom->set_calorimeter_id(calo);
	MethodHistograms* histograms=histogram_map.at(caloh);
	histograms->E->Fill(jete);
	int ntow=0;
	std::cout<<"taking data on a jet size " <<tower_set.size() <<std::endl;
	for(auto i:tower_set){
		if(!geom) continue;
		if(i < 0/* || i >= (int) data->size()*/ ) continue;
		auto key_1 = data->encode_key(i);
		auto tower_1 = data->get_tower_at_channel(i);
		int phibin_1 = data->getTowerPhiBin(key_1);
		int etabin_1 = data->getTowerEtaBin(key_1);
		if(!tower_1) continue;
	//	std::cout<<"Phibin: " <<phibin_1 <<" Etabin: " <<etabin_1 <<std::endl;
		if(etabin_1 < 0 || phibin_1 < 0 ||  phibin_1 >= geom->get_phibins() || etabin_1 >= geom->get_etabins() ) continue;
		float phi_center_1 = geom->get_phicenter(phibin_1);
		float eta_center_1 = geom->get_etacenter(etabin_1);
		float energy_1= tower_1->get_energy(); 
		int ih=i;
		if(caloh.find("emcal") != std::string::npos && emcal_lookup.find(i) !=emcal_lookup.end()) ih=emcal_lookup[i];
		if(allcal_e->find(ih) == allcal_e->end()) (*allcal_e)[ih]=energy_1;
		else allcal_e->at(i)+=energy_1;
		 ntow++;
		//there is probably a smart way to collect the group of sizes n, but I can really just do it by hand for rn 
		for(auto j:tower_set){
			if (i >= j) continue;
			if(j < 0 /*|| j >= (int) data->size()*/ ) continue;
			auto key_2 = data->encode_key(j);
			auto tower_2 = data->get_tower_at_channel(j);
			int phibin_2 = data->getTowerPhiBin(key_2);
			int etabin_2 = data->getTowerEtaBin(key_2);
			float phi_center_2 = geom->get_phicenter(phibin_2);
			float eta_center_2 = geom->get_etacenter(etabin_2);
			if(etabin_2 < 0 || phibin_2 < 0 ||  phibin_2 >= geom->get_phibins() || etabin_2 >= geom->get_etabins() ) continue;
			float energy_2 = tower_2->get_energy();
			std::pair<float, float> tower_center_1 {eta_center_1, phi_center_1}, tower_center_2 {eta_center_2, phi_center_2};
			float R_12=getR(tower_center_1, tower_center_2);
			histograms->R_geom->Fill(R_12);
			if(n_evts < 2 && energy_1 >0 && energy_2 >0 ) 	std::cout<<"Seperation is R="<<R_12<<std::endl;
			float jet2=pow(jete,2);
			float jet2t=pow(jete_tot,2);
			if (n_evts < 2&& energy_1 >0 && energy_2 >0 ) std::cout<<"The jet energy divsor is " <<jet2 <<" Total is " <<jet2t <<std::endl;
			float e2c=energy_1*energy_2 / jet2;
			float e2c_all=energy_1*energy_2 /jet2t; 
			if(abs(energy_1) <= 0.001 )continue; //1 MeV threshold for consideration
			if(abs(energy_2) <= 0.001) continue; //1 MeV threshold for consideration
			if(e2c != 0 ) histograms->R->Fill(R_12);
			e2c=e2c/((float) Nj);
			e2c_all=e2c_all/((float) Nj);
			if(!std::isnormal(e2c) ) continue;
			histograms->E2C->Fill(R_12, e2c/*/(float) Nj*/);
			if(std::isnormal(e2c_all)) histograms->E2C_pt->Fill(R_12, e2c_all);
			if( npt >= 3){
				for( auto k:tower_set){
					if( j >= k || i >= k) continue;
					if(k < 0 /*|| k >= (int) data->size()*/ ) continue;
					auto key_3 = data->encode_key(k);
					auto tower_3 = data->get_tower_at_channel(k);
					int phibin_3 = data->getTowerPhiBin(key_3);
					int etabin_3 = data->getTowerEtaBin(key_3);
					if(etabin_3 < 0 || phibin_3 < 0 ||  phibin_3 >= geom->get_phibins() || etabin_3 >= geom->get_etabins() ) continue;
					float phi_center_3 = geom->get_phicenter(phibin_3);
					float eta_center_3 = geom->get_etacenter(etabin_3);
					float energy_3 = tower_3->get_energy();
					std::pair<float, float> tower_center_3 {eta_center_3, phi_center_3};
					float R_13=getR(tower_center_1, tower_center_3);
					float R_23=getR(tower_center_2, tower_center_3);
					if(abs(energy_3) <= 0.001)continue;
					float e3c=energy_3*energy_2*energy_1;
					float jete3=pow(jete, 3);
					float jete3t=pow(jete_tot, 3);
					float e3c_all=e3c/jete3t;
					e3c=e3c/jete3;
					float R_L = std::max(R_12, R_13);
					R_L=std::max(R_L, R_23);
					e3c=e3c/((float) Nj);
					e3c_all=e3c_all/((float) Nj);
					if(!std::isnormal(e3c)) continue;
					histograms->E3C->Fill(R_L, e3c/*/(float) Nj*/);
					if(std::isnormal(e3c_all)) histograms->E3C_pt->Fill(R_L, e3c_all);
				}
			}
		}
	}
	histograms->N->Fill(ntow);
}
void CalorimeterTowerENC::GetE2C(PHCompositeNode* topNode, std::unordered_set<int> em, std::unordered_set<int> ih, std::unordered_set<int> oh, float jete)
{
	//this is the processor that allows for read out of the tower energies given the set of towers in the phi-eta plane
	auto emcal_geom =  findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
	auto emcal_tower_energy =  findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
	float jete_em = 0, jete_ih=0, jete_oh=0;
	for(auto i:em)
	{
		try{
			auto tower_e=emcal_tower_energy->get_tower_at_channel(i);
			jete_em+=tower_e->get_energy();
		}
		catch(std::exception& e) { continue; }
	}
	for(auto i:ih)
	{
		try{
			auto tower_e=ihcal_tower_energy->get_tower_at_channel(i);
			jete_ih+=tower_e->get_energy();
		}
		catch(std::exception& e) { continue; }
	}
	for(auto i:oh)
	{
		try{
			auto tower_e=ohcal_tower_energy->get_tower_at_channel(i);
			jete_oh+=tower_e->get_energy();
		}
		catch(std::exception& e) { continue; }
	}
	std::map<int, float> allcal_e;
	histogram_map["emcal"]->E->Fill(jete_em);
	histogram_map["ihcal"]->E->Fill(jete_ih);
	histogram_map["ohcal"]->E->Fill(jete_oh);
	histogram_map["emcal"]->N->Fill(em.size());
	histogram_map["ihcal"]->N->Fill(ih.size());
	histogram_map["ohcal"]->N->Fill(oh.size());
	GetENCCalo(topNode,em, emcal_tower_energy, emcal_geom, RawTowerDefs::CEMC,    jete_em, "emcal", 2, jete, &allcal_e);
	GetENCCalo(topNode,ih, ihcal_tower_energy, ihcal_geom, RawTowerDefs::HCALIN,  jete_ih, "ihcal", 2, jete, &allcal_e);
	GetENCCalo(topNode,oh, ohcal_tower_energy, ohcal_geom, RawTowerDefs::HCALOUT, jete_oh, "ohcal", 2, jete, &allcal_e);
	return;
}
void CalorimeterTowerENC::GetE3C(PHCompositeNode* topNode, std::unordered_set<int> em, std::unordered_set<int> ih, std::unordered_set<int> oh, std::map<int, float>* emtowere, bool is_kt, float jete)
{
	//this is the processor that allows for read out of the tower energies given the set of towers in the phi-eta plane
	auto emcal_geom =  findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
	auto emcal_tower_energy =  findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
	float jete_em = 0, jete_ih=0, jete_oh=0;
	for(int n=0; n<(int) ohcal_tower_energy->size(); n++){
		 jete_oh+=ohcal_tower_energy->get_tower_at_channel(n)->get_energy();
	//	std::cout<<"For tower at channel " <<n <<" the energy is : " <<ohcal_tower_energy->get_tower_at_channel(n)->get_energy() <<std::endl;
	}
//	if (n_evts < 10 ) std::cout<<"Total energy found in the ohcal is " <<jete_oh <<std::endl;
	jete_oh=0;
	for(auto i:em)
	{
		try{
			if( i <= 0 ) continue;
//			if(n_evts <10 ) std::cout<<"The tower number in use is " <<i <<std::endl;
			auto tower_e=emcal_tower_energy->get_tower_at_channel(i);
			if(tower_e){
				auto et=tower_e->get_energy();
				/*if(et == 0 && i > 1) et+=emcal_tower_energy->get_tower_at_channel(i-1)->get_energy();
				if(et == 0 && i > 1) et+=emcal_tower_energy->get_tower_at_channel(i+1)->get_energy();*/
				jete_em+=et;
//				if(n_evts < 10 ) std::cout<<"The added energy is " <<et <<" total energy " <<jete_em <<std::endl;; 
				(*emtowere)[i]+=tower_e->get_energy();
			}
		}
		catch(std::exception& e) { continue; }
	}
	for(auto i:ih)
	{
		try{
			if( i <= 0 ) continue;
			auto tower_e=ihcal_tower_energy->get_tower_at_channel(i);
			if(tower_e) jete_ih+=tower_e->get_energy();
		}
		catch(std::exception& e) { continue; }
	}
	for(auto i:oh)
	{
		try{
			if( i <= 0 ) continue;
			auto tower_e=ohcal_tower_energy->get_tower_at_channel(i);
	//		std::cout<<"Jet Tower: We get the energy for the tower at channel " <<i <<" as " <<tower_e->get_energy() <<std::endl;
			if(tower_e) jete_oh+=tower_e->get_energy();
		}
		catch(std::exception& e) { continue; }
	}
	//std::cout<<"Checked " <<em.size() <<" towers in the outer hcal \n Got an energy in the ohcal part of the jet of " <<jete_oh <<std::endl; 
//	histogram_map["ohcal"]->E->Fill(jete_oh);
	std::vector<std::thread> calo_thread; 
	std::string emcal_label="emcal", ihcal_label="ihcal", ohcal_label="ohcal", all="allcal";
	if(!is_kt){
		emcal_label+="kt";
		ihcal_label+="kt";
		ohcal_label+="kt";
		all+="kt";
	}
	std::map<int, float> allcal, allcal_em, allcal_ih, allcal_oh;
	/*calo_thread.push_back(std::thread(&CalorimeterTowerENC::*/GetENCCalo(/*, this,*/topNode,em, emcal_tower_energy, emcal_geom, RawTowerDefs::CEMC, jete_em, emcal_label, 3, jete, &allcal_em)/*)*/;
/*	calo_thread.push_back(std::thread(&CalorimeterTowerENC::*/GetENCCalo(/*, this,*/ topNode,ih, ihcal_tower_energy, ihcal_geom, RawTowerDefs::HCALIN,  jete_ih, ihcal_label, 3, jete, &allcal_ih)/*)*/;
/*	calo_thread.push_back(std::thread(&CalorimeterTowerENC::*/GetENCCalo(/*, this, */topNode,oh, ohcal_tower_energy, ohcal_geom, RawTowerDefs::HCALOUT, jete_oh, ohcal_label, 3, jete, &allcal_oh/*)*/);
//	for(int t=0; t<(int)calo_thread.size(); t++) calo_thread.at(t).join();
	for(auto e:allcal_em)
	{
		if(allcal.find(e.first)==allcal.end()) allcal[e.first]=e.second;
		else allcal[e.first]+=e.second;
	}
	for(auto e:allcal_ih)
	{
		if(allcal.find(e.first)==allcal.end()) allcal[e.first]=e.second;
		else allcal[e.first]+=e.second;
	}
	for(auto e:allcal_oh)
	{
		if(allcal.find(e.first)==allcal.end()) allcal[e.first]=e.second;
		else allcal[e.first]+=e.second;
	}
	//MethodHistograms* hs=histogram_map[all];
	//GetENCCalo(allcal, ohcal_geom, ohcal_tower_energy, hs, jete);
	return;
}

void CalorimeterTowerENC::GetE2C(PHCompositeNode* topNode, std::map<PHG4Particle*, std::pair<float, float>> jet)
{
	//this is the processor that allows for particle read of jets phi-eta plane
	float jet_total_e=0;
	for(auto p1:jet) jet_total_e+=p1.first->get_e();
	histogram_map["parts"]->E->Fill(jet_total_e);
	histogram_map["parts"]->N->Fill(jet.size());
	for(auto p1it=jet.begin(); p1it != jet.end(); ++p1it )
	{
		auto p1=(*p1it);
		for(auto p2it=p1it; p2it != jet.end(); ++p2it){
			auto p2=(*p2it);
			if(p1it == p2it) continue;
			double pe1=p1.first->get_e();
			double pe2=p2.first->get_e();
			float R12=getR(p1.second, p2.second);
			histogram_map["parts"]->R->Fill(R12);
			float e2c=pe1*pe2/jet_total_e;
			e2c=e2c/(float)Nj;
			histogram_map["parts"]->E2C->Fill(R12, e2c);
		}
	}
	return;
			
}

void CalorimeterTowerENC::GetE3C(PHCompositeNode* topNode, std::map<PHG4Particle*, std::pair<float, float>> jet)
{
	//this is the processor that allows for particle read of jet in the phi-eta plane
	float jet_total_e=0;
	for(auto p1:jet) jet_total_e+=p1.first->get_e();
//	std::cout<<"The histogram map has size " <<histogram_map.size() <<std::endl;
//	for(auto h:this->histogram_map) for(auto h1:h.second->histsvector) std::cout<<"The histogram map for " <<h.first <<" has a histogram with name " <<h1->GetName() <<std::endl;
	this->histogram_map["parts"]->E->Fill(jet_total_e);
	this->histogram_map["parts"]->N->Fill(jet.size());
	for(auto p1it=jet.begin(); p1it != jet.end(); ++p1it )
	{
		auto p1=(*p1it);
		for(auto p2it=p1it; p2it != jet.end(); ++p2it){
			auto p2=(*p2it);
			if(p1it == p2it) continue;
			double pe1=p1.first->get_e();
			double pe2=p2.first->get_e();
			float R12=getR(p1.second, p2.second);
			histogram_map["parts"]->R->Fill(R12);
			float e2c=pe1*pe2/(float) pow(jet_total_e, 2);
			e2c=e2c/(float)Nj;
			histogram_map["parts"]->E2C->Fill(R12, e2c);
			for(auto p3it=p2it; p3it != jet.end() ; ++p3it){
				auto p3=(*p3it);
				if(p1it == p3it || p2it == p3it) continue;
				double pe3=p3.first->get_e();
				float R13=getR(p1.second, p3.second);
				float R23=getR(p2.second, p3.second);
				float e3c=e2c*pe3/(float)jet_total_e;
				float R_L=std::max(R12, R23);
				R_L=std::max(R_L, R13);
				histogram_map["parts"]->E3C->Fill(R_L, e3c);
			}
		}
	}
	return;
}
	
std::pair<std::map<float, std::map<float, int>>, std::pair<float, float>> CalorimeterTowerENC::GetTowerMaps(RawTowerGeomContainer_Cylinderv1* geom, RawTowerDefs::CalorimeterId caloid, TowerInfoContainer* calokey)
{
	//ge thte geometry map for the towers in the first event to may it easy to serarc
	//the idea is put the menan in and the associate a key with it 
	geom->set_calorimeter_id(caloid);
	std::pair<float, float> deltas{0,0};
	std::map<float, std::map<float, int>> bins;
	std::map<int, std::pair<float, float>> gs;
	geom->set_calorimeter_id(caloid);
	for(int i = 0; i<(int) calokey->size(); i++){
		try{
	//		std::cout<<"The channel number is " <<i <<std::endl;
			int key=calokey->encode_key(i);
			//int tower=calokey->get_tower_at_channel(key);
			int phibin=calokey->getTowerPhiBin(key);
			int etabin=calokey->getTowerEtaBin(key);
			float eta=geom->get_etacenter(etabin);
			float phi=geom->get_phicenter(phibin);
			std::pair g { eta, phi};
			if( deltas.first == 0){
				std::pair <double, double> etabounds=geom->get_etabounds(etabin);
				deltas.first=abs(etabounds.first - etabounds.second)/2.;
				std::pair <double, double> phibounds=geom->get_phibounds(etabin);
				deltas.second=abs(phibounds.first - phibounds.second)/2.;
			}
			gs[i]=g;
		}
		catch(std::exception& e){ 
			std::pair<float, float> g {-5.0, -20.0 };
			gs[i]=g;
		} 
	}
	int i=0;
	for(auto g1: gs){
		auto g=g1.second;
		if(g.first == -5  && i > 1){
			if((int)(gs.at(i-1).first*4.4/deltas.first) % 8 != 7 && (int)((3.1416/deltas.second)*gs.at(i-1).second) % 2 == 1  ){
				g.first=gs.at(i-1).first+2*deltas.first; 
				g.second=gs.at(i-1).second;
			}
			else if ((int)((3.1416/deltas.second)*gs.at(i-1).second) % 2 != 1 ){
				g.first=gs.at(i-1).first;
				g.second=gs.at(i-1).second + 2*deltas.second;
			}
			else if ((int)((4.4/deltas.first)*gs.at(i-1).first) % 8 == 7  && (int)((3.1416/deltas.second)*gs.at(i-1).second) % 2 == 1 && (4.4/deltas.first)*gs.at(i-1).first != 1.1 - deltas.first){
				g.first=gs.at(i-1).first+2*deltas.first;
				g.second=gs.at(i-1).second-16*deltas.second;
			}
			else if( (int) ((4.4/deltas.first)*gs.at(i-1).first) == 1.1 - deltas.first && (int)((3.1416/deltas.second)*gs.at(i-1).second) % 2 == 1 ) {
				gs.at(i-1).first = -1.1 + deltas.first; 
				gs.at(i-1).second = gs.at(i-1).second + 2*deltas.second;
			}
		}
		bins[g.first][g.second]=g1.first;
		i++;
	}
	return std::make_pair(bins, deltas);
}
int CalorimeterTowerENC::GetTowerNumber(std::pair<float, float> part, std::map<float, std::map< float, int>> Calomap, std::pair<float, float> delta)
{
	int keynumber=-1;
	part.second+=PI;
//	std::cout<<"The particle is centered around eta = " <<part.first <<" phi = " <<part.second <<std::endl;
	if (abs(part.first) > 1.2 ) return keynumber;
	for(auto t:Calomap)
	{
		if( part.first < t.first + delta.first && part.first >= t.first - delta.first){
			//checking the eta bounds first
			for (auto tp:t.second){
				if(part.second < tp.first + delta.second && part.second >= tp.first - delta.second){
	//				if(n_evts < 10 ) std::cout<<"Found a good bin centered in eta at " <<t.first <<" in phi at "<<tp.first <<" with bin number " <<tp.second <<std::endl;
				 	keynumber=tp.second;
					break;
				}
			}
		}
	}
//	if(n_evts < 10 ) std::cout<<" The particle is at eta= " <<part.first <<" and phi = " <<part.second <<std::endl;
	return keynumber;
}
int CalorimeterTowerENC::RecordHits(PHCompositeNode* topNode, Jet* truth_jet, std::vector<std::unordered_set<int>> kt_towers){
	//record where the particles are located and which towers are seeing hits
	std::set<std::pair<float, float>> particle_coords; //This will store the location of each particle which then I can use tho capture the relevant towe
	std::unordered_set<PHG4Particle*> jetparticles;
	std::set<std::pair<float, float>> circle_jet;
	std::map<int, float> jettowenergy, emtowerenergy, emtowerenergykt;
	auto _truthinfo=findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
//	auto _truthhits=findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_BH_1");
	auto particles=_truthinfo->GetMap();	
	//std::cout<<"Indicies for the truth map are ";
	//for(auto p:particles) std::cout<<p.first <<std::endl; 
	for( auto& iter:truth_jet->get_comp_vec()){
		Jet::SRC source = iter.first;
		unsigned int idx = iter.second;
		if( source != Jet::PARTICLE){
			std::cout<<"This jet has data that is not a particle" <<std::endl;
			return -1;
		}
			if (particles.find(idx) == particles.end()) std::cout<<"Index is not in map for index " <<idx <<std::endl;	
			else { 
				PHG4Particle* truth_part = _truthinfo->GetParticle(idx);
			//	PHG4Shower* truth_shower = _truthinfo->GetShower(idx);
				jetparticles.insert(truth_part);
			//	auto showerhits=truth_shower->g4Hit_ids()
			}
		//jethits.insert(truth_hit);
	}
	std::map<PHG4Particle*, std::pair<float, float>> jet_particle_map;
	float axis_phi=truth_jet->get_phi(), axis_eta=truth_jet->get_eta();
	float edge_R=0;
	float jet_energy=truth_jet->get_e();
	int rdivbins=Particles->R_geom->GetNbinsX();	
	float rbinsides=Particles->R_geom->GetBinWidth(5);
	for(auto p:jetparticles){
		std::pair<float, float> particle_coord; 
		particle_coord.first = atanh(p->get_pz()/sqrt(pow(p->get_py(),2) + pow(p->get_px(),2) + pow(p->get_pz(), 2)));
		particle_coord.second = atan2(p->get_py(), p->get_px());
		float R=sqrt(pow(particle_coord.second-axis_phi, 2) + pow(particle_coord.first - axis_eta, 2));
		if(edge_R < R) edge_R=R;
	}
	for(auto p:jetparticles){
	       //take in the identified particles that make up the subjet and then we overap that in the towers
		std::pair<float, float> particle_coord; 
		particle_coord.first = atanh(p->get_pz()/sqrt(pow(p->get_py(),2) + pow(p->get_px(),2) + pow(p->get_pz(), 2)));
		particle_coord.second = atan2(p->get_py(), p->get_px());
		//float angle_off=acos((particle_coord.second-axis_phi)/(particle_coord.first-axis_eta)); //this is the anglular offset of the particle circle
		particle_coords.insert(particle_coord);	
		jet_particle_map[p]=particle_coord;
		jethits->Fill(particle_coord.first, particle_coord.second);
//		if ( n_evts < 10 ) std::cout<<"The particle we are looking for is located at eta = " <<particle_coord.first 
//				<<" phi = " << particle_coord.second <<std::endl;
		int towernumberemcal=GetTowerNumber(particle_coord, EMCALMAP.first, EMCALMAP.second);
		if(jettowenergy.find(towernumberemcal) == jettowenergy.end())jettowenergy[towernumberemcal]=0; 
		jettowenergy[towernumberemcal]+=p->get_e();
		float R=sqrt(pow(particle_coord.second-axis_phi, 2) + pow(particle_coord.first - axis_eta, 2));
		float ptr=sqrt(pow(p->get_px(),2)+ pow(p->get_py(), 2))/(float) truth_jet->get_pt();
		histogram_map["parts"]->R_pt->Fill(R/(float)edge_R, ptr);
		float intersectionbase=(pow(edge_R, 2) + pow(R, 2))/(2*edge_R*R);
		for(int i=0; i < rdivbins; i++)
		{
			//this fills the "continous" case approximation
			float R_L=i*rbinsides;
			float intersection=intersectionbase+pow(R_L,2)/(2*edge_R*R);
			float circumfrence_included=0.0;
			if(intersection >= 1) {
				//this is the case of all of the ring within the jet
				circumfrence_included=2*3.14159;
			}
			else if(intersection <= -1){
				circumfrence_included=0.;
			}
			else{
				try{
					circumfrence_included=2*acos(intersection);
				}
				catch(std::exception& e) {};
				if( R_L+R > edge_R && circumfrence_included > 3.14159) circumfrence_included = 2*3.14159-circumfrence_included;
				else if(R_L +R < edge_R && circumfrence_included < 3.14159 ) circumfrence_included = 2*3.14159 - circumfrence_included;
			}
			circumfrence_included=R_L*circumfrence_included;
			if(!std::isnormal(circumfrence_included)) continue;
			Particles->R_geom->Fill(R_L, circumfrence_included);
		}
	}
	//find the center of the jet and go to R=0.4 to try to correct for particles not being in the right place
	//this is a fast way to get around the G4Hit version
	std::pair<float, float> jet_center {0,0};
	for(auto p:jet_particle_map){
		jet_center.first+=p.second.first;
		jet_center.second+=p.second.second;
	}
	jet_center.first=jet_center.first/(float)jet_particle_map.size();
	jet_center.second=jet_center.second/(float)jet_particle_map.size();
	int netabin=1+0.4/(float) EMCALMAP.second.first, nphibin=1+0.4/(float) EMCALMAP.second.second;
	for(int etabin=0; etabin< netabin; etabin++)
	{
		for(int phibin=0; phibin<nphibin; phibin++)
		{
			float deta=EMCALMAP.second.first*etabin;
			float dphi=EMCALMAP.second.second*phibin;
			float R=sqrt(pow(deta,2)+pow(dphi,2));
			if(R > 0.4 ) continue;
			std::pair<float, float> tow_center, neg_tow_center;
			tow_center.first=jet_center.first+deta;
			tow_center.second=jet_center.first+dphi;
			circle_jet.insert(tow_center);
			tow_center.first=jet_center.first+deta;
			tow_center.second=jet_center.first-dphi;
			circle_jet.insert(tow_center);
			tow_center.first=jet_center.first-deta;
			tow_center.second=jet_center.first-dphi;
			circle_jet.insert(tow_center);
			tow_center.first=jet_center.first-deta;
			tow_center.second=jet_center.first+dphi;
			circle_jet.insert(tow_center);
			}
	}//this gets the jet centers using emcal binning, using the undorded set allows for double writes to be ok
	//std::cout<<"The circluar jet should encompass " <<circle_jet.size() <<" bins in the emcal" <<std::endl;	
//	getE2C(jet_particle_map, topNode); 
	std::unordered_set<int> jet_towers_ihcal, jet_towers_ohcal, jet_towers_emcal; 
	for(auto pc:/*particle_coords*/ circle_jet)
	{
		if(abs(pc.first) > 1.2) continue;
//		if ( n_evts < 10 ) std::cout<<"The particle we are looking for is located at eta = " <<pc.first 
//				<<" phi = " << pc.second <<std::endl;
		int tne=GetTowerNumber(pc, EMCALMAP.first, EMCALMAP.second);
		if(tne != -1 ){ 
			jet_towers_emcal.insert(tne);
//			if(n_evts < 10 ) std::cout<<"The identified tower number for the emcal is " <<tne <<std::endl;
		}
		emtowerenergy[tne]=0;
		emtowerenergykt[tne]=0;
		int tni=GetTowerNumber(pc, IHCALMAP.first, IHCALMAP.second);
		if(tni != -1) jet_towers_ihcal.insert(tni);
//		if(n_evts < 10) std::cout<<"Particle added to the ihcal in bin " <<tni <<std::endl;
		int tno=GetTowerNumber(pc, OHCALMAP.first, OHCALMAP.second);
		if (tno != -1) jet_towers_ohcal.insert(tno);
//		if (n_evts < 10 ) std::cout<<"Particle added to the ohcal in bin " <<tno <<std::endl;
	}
//	if(n_evts < 10)	std::cout<<"We have loaded in particles " <<particle_coords.size() <<std::endl;
//	getE2C(topNode, jet_towers_ihcal, jet_towers_ohcal, jet_towers_emcal);
	bool kt_tows_good=false;
	if(jet_towers_emcal.size() == 0 || jet_towers_ohcal.size() == 0 ) return 1;
	try{
		std::cout<<" The kt map has size " <<kt_towers.size() <<std::endl;
	}
	catch(std::exception& e) {std::cout<<"Couldn't access the map to get size" <<std::endl;}
	if(kt_towers.size() > 0){
	try{
		std::cout<<"The tows emcal bit has size " <<kt_towers[0].size() <<std::endl;
	}
	catch(std::exception& e){std::cout<<"Nothing in the 0th element " <<std::endl;}
	try{
		std::cout<<"The tows ihcal bit has size " <<kt_towers[1].size() <<std::endl;
	}
	catch(std::exception& e){std::cout<<"Nothing in the 1st element " <<std::endl;}
	try{
		std::cout<<"The tows ohcal bit has size " <<kt_towers[2].size() <<std::endl;
		kt_tows_good=true;
	}
	catch(std::exception& e){std::cout<<"Nothing in the 2nd element " <<std::endl;}
	}
try{	GetE3C(topNode, jet_particle_map); }//get both in one call
	catch(std::exception& e) {std::cout<<"unable to run the particle map, \n Exception " <<e.what() <<std::endl;}
	try{GetE3C(topNode, jet_towers_emcal, jet_towers_ihcal, jet_towers_ohcal, &emtowerenergy, false, jet_energy); }//get both values filled in one 
	catch(std::exception& e) {std::cout<<"unable to run the tower map, \n Exception " <<e.what() <<std::endl;}
	try{if(kt_towers.size() > 0 && kt_tows_good) GetE3C(topNode, kt_towers[0], kt_towers[1], kt_towers[2], &emtowerenergykt, true, jet_energy); }
	catch(std::exception& e) {std::cout<<"unable to run the kt map, \n Exception " <<e.what() <<std::endl;}
	for(auto m:jettowenergy) comptotows->Fill(m.second, emtowerenergy[m.first]);
	return 1;
}
std::map<std::pair<float, float>, std::vector<std::unordered_set<int>>> CalorimeterTowerENC::FindAntiKTTowers(PHCompositeNode* topNode)
{
		auto emcal_geom =  findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
		auto emcal_tower_energy =  findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
		auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
		auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
		auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
		auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
		//This is a fake anti-kt process, really using E_T instead to allow for jet ID without tracking 
		//For this I will just mao all the energy onto a "single" calo, with the Get Tower number to squash 
		//emcal down to hcal 
		std::map<int, float> emcal_to_hcal_energy;
		std::map<int, std::pair<float, float>> hcal_tower_pos;
		std::map<std::pair<float, float>, std::vector<std::unordered_set<int>>> tower_jets; 	
		for(int i=0; i<(int)emcal_tower_energy->size(); i++)
		{
			auto tower =emcal_tower_energy->get_tower_at_channel(i);
			float energy=tower->get_energy();
			if(energy==0) continue;
			auto key=emcal_tower_energy->encode_key(i);
			int phibin = emcal_tower_energy->getTowerPhiBin(key);
			int etabin = emcal_tower_energy->getTowerEtaBin(key);
			auto phi=emcal_geom->get_phicenter(phibin);
			auto eta=emcal_geom->get_etacenter(etabin);	
			energy=energy/cosh(eta);
			std::pair<float, float> coords {eta, phi};
			int hcal_tower_n=-1;
			if(emcal_lookup.find(i) == emcal_lookup.end()){
				int t=GetTowerNumber(coords, OHCALMAP.first, OHCALMAP.second);
				emcal_lookup[i]=t;
 				hcal_lookup[t].push_back(i);
			}
			hcal_tower_n=emcal_lookup[i]; 
			if(emcal_to_hcal_energy.find(hcal_tower_n) == emcal_to_hcal_energy.end())
				emcal_to_hcal_energy[hcal_tower_n]=energy;
			else emcal_to_hcal_energy[hcal_tower_n]+=energy;
		}
		for(int i=0; i<(int)ihcal_tower_energy->size(); i++)
		{
			auto tower =ihcal_tower_energy->get_tower_at_channel(i);
			float energy=tower->get_energy();
			if(energy==0) continue;
			auto key=ihcal_tower_energy->encode_key(i);
			int etabin = ihcal_tower_energy->getTowerEtaBin(key);
			auto eta=ihcal_geom->get_etacenter(etabin);	
			energy=energy/cosh(eta);
			if(emcal_to_hcal_energy.find(i) == emcal_to_hcal_energy.end())
				emcal_to_hcal_energy[i]=energy;
			else emcal_to_hcal_energy[i]+=energy;
		}	
		for(int i=0; i<(int)ohcal_tower_energy->size(); i++)
		{
			auto tower =ohcal_tower_energy->get_tower_at_channel(i);
			float energy=tower->get_energy();
			if(energy==0) continue;
			auto key=ohcal_tower_energy->encode_key(i);
			int phibin = ohcal_tower_energy->getTowerPhiBin(key);
			int etabin = ohcal_tower_energy->getTowerEtaBin(key);
			auto phi=ohcal_geom->get_phicenter(phibin);
			auto eta=ohcal_geom->get_etacenter(etabin);	
			energy=energy/cosh(eta);
			std::pair<float, float> coords {eta, phi};
			if(emcal_to_hcal_energy.find(i) == emcal_to_hcal_energy.end())
				emcal_to_hcal_energy[i]=energy;
			else emcal_to_hcal_energy[i]+=energy;
			hcal_tower_pos[i]= coords;

		}	
		float etmin=1.0; //10 MeV threshold chosen because
		float max_found=0;
		int max_bin=0;
		std::cout<<"The energy output is non zero in how many towers?: " <<emcal_to_hcal_energy.size() <<std::endl;
		for(auto e:emcal_to_hcal_energy){
			if(e.second > max_found){
				max_found=e.second;
				max_bin=e.first;
			}
		}
		std::cout<<"The maximum energy is " <<max_found <<std::endl;
		while( max_found > etmin) 
		{
			//now I need to find the jets that look correct 
			//auto tower=ohcal_tower_energy->get_tower_at_channel(max_bin);
			//auto key=ohcal_tower_energy->encode_key(max_bin);
			std::map<int, float> candidates;
//			if(n_evts < 5) std::cout<<"Looking for jet number " <<tower_jets.size() <<std::endl;
			std::unordered_set<int> jettows;
			auto central=hcal_tower_pos[max_bin];
			float jet_pt=emcal_to_hcal_energy[max_bin];
			for(auto c:hcal_tower_pos){
				float R=getR(central, c.second);
		//		std::cout<<" The seperation is " <<R <<" for a particle at " <<c.second.second <<std::endl;
				if(R < 0.5 && emcal_to_hcal_energy[c.first] > 0.001 ){ candidates[c.first]=R;
				jet_pt+=emcal_to_hcal_energy[c.first];
				}
			}
			candidates[max_bin]=0.;
		//	if(candidates.size() > 2) std::cout<<"The number of candidate towers is " <<candidates.size() <<" with total E_T " <<jet_pt  <<std::endl;
			if(jet_pt < 0.005*jet_cutoff){
				for(auto c:candidates){
		//			std::cout<<"The Number of bins still in play is now " <<emcal_to_hcal_energy.size() <<std::endl;
		//			std::cout<<"The tower needing to leave has bin number=" <<c.first <<" and R=" <<c.second <<std::endl;
					if(emcal_to_hcal_energy.find(c.first) != emcal_to_hcal_energy.end()) emcal_to_hcal_energy.erase(c.first);
		//			std::cout<<"Now the energy bins should have decreased by 1? " <<emcal_to_hcal_energy.size() <<std::endl;
				}
		//		std::cout<<"Jet Energy was below half a percent of the cutoff" <<std::endl; 
				max_found=0.0;
				for(auto e:emcal_to_hcal_energy){
				if(e.second > max_found){
					max_found=e.second;
					max_bin=e.first;
				}
			}
				
				continue;
			} 
			bool found_all_parts=false;
			jet_pt=0.0;
			while(!found_all_parts)
			{
				//to recombine, need to make an assumption that all the particles are massless
				//I think this is close enough for right now 
				if(candidates.size() < 2){
					for(auto c:candidates){
						if(emcal_to_hcal_energy.find(c.first) !=emcal_to_hcal_energy.end()){
							 emcal_to_hcal_energy.erase(c.first);
						}
					}
					 break;
				}
				std::map<int, float> di;
				for( auto c:candidates) di[c.first] = pow(emcal_to_hcal_energy[c.first], -2);
				std::map<std::pair<int, int>, float> dij;
				for(auto c:candidates){
					for(auto d:candidates){
						if(c.first >= d.first)continue; //avoid double counting
						std::pair <int, int> couple {c.first, d.first};
						float aktc=pow(emcal_to_hcal_energy[c.first], -2);
						float aktd=pow(emcal_to_hcal_energy[d.first], -2);
						float delta=getR(hcal_tower_pos[c.first], hcal_tower_pos[d.first]);
						delta=pow(delta/0.4, 2);
						dij[couple]=(std::min(aktc, aktd) * delta);
					}
				}
				std::pair<int,int> mergecoords {-1,-1};
				float minmerge=10000000.;
				for(auto d:di){
					 if(d.second < minmerge){
						minmerge=d.second; 
						mergecoords.first=d.first;
					}
				}
				for(auto d:dij){
					if(d.second < minmerge){
						minmerge=d.second;
						mergecoords=d.first;
					}
				}
				if(mergecoords.second != -1){
					//this means that we have to merge particles 
					int c = mergecoords.first, d = mergecoords.second;
					int newparticleindex=((--candidates.end())->first) + mergecoords.first*mergecoords.second;
					float E=0., px=0., py=0.,pz=0.;
					E=emcal_to_hcal_energy[c]*cosh(hcal_tower_pos[c].first);
					E+=emcal_to_hcal_energy[d]*cosh(hcal_tower_pos[d].first);
					px=emcal_to_hcal_energy[c]*cos(hcal_tower_pos[c].second);
					px+=emcal_to_hcal_energy[d]*cos(hcal_tower_pos[d].second);
					py=emcal_to_hcal_energy[c]*sin(hcal_tower_pos[c].second);
					py+=emcal_to_hcal_energy[d]*sin(hcal_tower_pos[d].second);
					E=emcal_to_hcal_energy[c]*sinh(hcal_tower_pos[c].first);
					E+=emcal_to_hcal_energy[d]*sinh(hcal_tower_pos[d].first);
					float eta=0., phi=0.;
					eta=atanh(pz/E);
					phi=atan(py/px);
					std::pair<float, float> coords {eta, phi};
					candidates[newparticleindex]=getR(central, coords);
					candidates.erase(c);
					candidates.erase(d);
					//only record the tower indexs
					if(c < (int)ohcal_tower_energy->size()) jettows.insert(c);
					if(d < (int)ohcal_tower_energy->size()) jettows.insert(d);
					emcal_to_hcal_energy[newparticleindex]=sqrt(abs(pow(E,2)-pow(pz, 2)));
					emcal_to_hcal_energy.erase(c);
					emcal_to_hcal_energy.erase(d);
					hcal_tower_pos[newparticleindex]=coords;
					jet_pt=emcal_to_hcal_energy[newparticleindex];
//					std::cout<<"Merged particles have E_{T} = " <<jet_pt <<std::endl;
				}
				else{
					found_all_parts=true; 
					for(auto c:candidates){
						if(emcal_to_hcal_energy.find(c.first) != emcal_to_hcal_energy.end()) 
							emcal_to_hcal_energy.erase(c.first);
						}
					continue;
				}
				if(candidates.size() <= 1){
					found_all_parts=true;
					continue;
				}
				continue;
			}
			for(auto e:emcal_to_hcal_energy){
				if(e.second > max_found){
					max_found=e.second;
					max_bin=e.first;
				}
			}
			std::unordered_set<int> emcaltowers;
			for(auto c:jettows)
			{
				for(auto b:hcal_lookup[c]) emcaltowers.insert(b);
			}
			std::vector<std::unordered_set<int>> tows {emcaltowers, jettows, jettows};
			if(jet_pt > 0 ) std::cout<<"Identified a Jet with a E_t of " <<jet_pt <<std::endl;
			if(jet_pt > 0.01*jet_cutoff){ //trying a much lower cutoff, and use a qucik cluster trick above to decrease the number of trys it takes
				tower_jets[central]=tows;
				calojethits->Fill(central.first, central.second);
			}
				
			continue;
		}

		std::cout<<"Identified jets: " <<tower_jets.size() <<std::endl;	
		return tower_jets;
	
}
std::map<Jet*, std::pair<float,float>> CalorimeterTowerENC::MatchKtTowers(std::map<std::pair<float, float>, std::vector<std::unordered_set<int>>> jettow, JetContainer* jets){
	std::map<Jet*, std::pair<float, float>> coords;
	for(auto jet:*jets){
		float jeteta=jet->get_eta();
		float jetphi=jet->get_phi();
		std::pair<float, float> jetcoord {jeteta, jetphi};
		float rmin=10.0;
		std::pair<float, float> closest {0.0, 0.0};
		for(auto t:jettow){
			float R=getR(jetcoord, t.first);
			if(R < rmin){
				rmin=R;
				closest=t.first;
			}
		}
		coords[jet]=closest;
	}
	std::cout<<"Matched the jets in the calo to the nearest jet axis" <<std::endl;
	return coords;
}
int CalorimeterTowerENC::process_event(PHCompositeNode *topNode){
	//need to process the events, this is really going to be a minor thing, need to pull my existing ENC code to do it better
	//for the first event build the  tower number map that is needed
	n_evts++;
	std::cout<<"Running on event " <<n_evts<<std::endl;
	try{
		Nj=0;
		auto emcal_geom =  findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
		auto emcal_tower_energy =  findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
		auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
		auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
		auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
		auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
		if(EMCALMAP.first.size() == 0 )  EMCALMAP=GetTowerMaps(emcal_geom, RawTowerDefs::CEMC, emcal_tower_energy);
		if(IHCALMAP.first.size() == 0 )  IHCALMAP=GetTowerMaps(ihcal_geom, RawTowerDefs::HCALIN, ihcal_tower_energy);
		if(OHCALMAP.first.size() == 0 )  OHCALMAP=GetTowerMaps(ohcal_geom, RawTowerDefs::HCALOUT, ohcal_tower_energy);
		auto jets = findNode::getClass<JetContainer> (topNode, "AntiKt_Truth_r04");
		float emcal_energy=0, ihcal_energy=0, ohcal_energy=0;	
		try{
		for(int i=0; i<(int)emcal_tower_energy->size(); i++ ){
			auto tower = emcal_tower_energy->get_tower_at_channel(i);
			float energy=tower->get_energy();
			emcal_energy+=energy;
		}
		for(int i=0; i<(int)ihcal_tower_energy->size(); i++ ){
			auto tower = ihcal_tower_energy->get_tower_at_channel(i);
			float energy=tower->get_energy();
			ihcal_energy+=energy;
		}
		for(int i=0; i<(int)ohcal_tower_energy->size(); i++ ){
			auto tower = ohcal_tower_energy->get_tower_at_channel(i);
			float energy=tower->get_energy();
			ohcal_energy+=energy;
		}
		EM_energy->Fill(emcal_energy);
		OH_energy->Fill(ohcal_energy);
		IH_energy->Fill(ihcal_energy);
		try{Nj=jets->size();
		auto towerktjets=FindAntiKTTowers(topNode);
		try{auto coordinatedjets=MatchKtTowers(towerktjets, jets);
		std::vector<std::thread> jetThreads;
		try{for(auto j:*jets){
			if(j->get_pt() < jet_cutoff){ //put in a 10 GeV cut on the jets
				Nj--;
				continue;
			}
			this->histogram_map["parts"]->pt->Fill(j->get_pt());
			/*jetThreads.push_back(std::thread(&*/CalorimeterTowerENC::RecordHits(/*, this,*/ topNode, j, towerktjets[coordinatedjets[j]]);
		}
	//	for(int t=0; t<(int)jetThreads.size(); t++) jetThreads.at(t).join();
		number_of_jets->Fill(Nj);
		}
		catch(std::exception& e4) {std::cout<<"The threads objects are the problems \n Exception: "<<e4.what() <<std::endl;}
		}
		catch(std::exception& e3) {std::cout<<"The matches are the problems \n Exception: "<<e3.what() <<std::endl;}
		}
		catch(std::exception& e2) {std::cout<<"The anti-kt objects are the problems \n Exception: "<<e2.what() <<std::endl;}
		}
		catch(std::exception& e1) {std::cout<<"The energy objects are the problems \n Exception: "<<e1.what() <<std::endl;}
	}
	catch(std::exception& e ) {
		std::cout<<"Failed to run on event " <<n_evts <<"\n Skipping Event" <<" \n Exception was " <<e.what() <<std::endl;	
		n_evts--;
	}
	return Fun4AllReturnCodes::EVENT_OK;
	
	
}
int CalorimeterTowerENC::End(PHCompositeNode *topNode){
	return 1;
}
void CalorimeterTowerENC::Print(const std::string &what) const
{
	std::cout<<"Printing the files out now to a file named: " <<outfilename <<std::endl;
	TFile* f=new TFile(outfilename.c_str(), "RECREATE");
	//printing the stuff out 
	for(auto hs:histogram_map){
		//hs.second->R->Scale(1/(float)hs.second->N->GetEntries()); //average over number of jets
		hs.second->E2C->Scale(1/(float)hs.second->E2C->GetBinWidth(5)); //correct for binning
		hs.second->E3C->Scale(1/(float)hs.second->E3C->GetBinWidth(5)); //correct for binning
		//hs.second->E2C->Scale(1/(float)n_evts); //average over events
		//hs.second->E3C->Scale(1/(float)n_evts); //average over events
		for(auto h:hs.second->histsvector){
			h->Write();
		}
		hs.second->R_pt->Write();
	}
	jethits->Write();
	comptotows->Write();
	number_of_jets->Write();
	EM_energy->Write();
	IH_energy->Write();
	OH_energy->Write();
	f->Write();
	f->Close();
	return;
}
