//Creates a huge list of plots procedurally, allowing for me to be properly lazy 
//need to test implementation and check in rcalo values as of 4-12-2023
//Have to check on the eta shift with Joey
class caloplots{ //creates a relevant plot for each calorimeter type
	public: 
		std::string calo="EMCAL";
		TH1F* Energy, *E_phi, *ET, *dET_eta, *ET_phi, *ET_z, *dET, *acceptance_eta;
		std::vector<TH1F*> *hists_1;
		TH2F* ET_eta_phi, *E_eta_phi, *E_T_z_eta;
		std::vector<TH2F*> *hists_2;
		float acceptance=0.0; 
		bool ok_phi=false;
		float z=0, zl, zh, zmin=1, zmax=1, rcalo=1;
		int phibins=256, etabins=96, etamin=-1.13, etamax=1.13;
		caloplots(std::string caloname="EMCAL", float zlow=-30, float zhigh=30){
			zl=zlow; 
			zh=zhigh;
			calo=caloname;
			z=(zl+zh)/2;
			if(calo.find("OHCAL") != std::string::npos || calo.find("ALL") != std::string::npos) rcalo=3;
			if(calo.find("IHCAL") != std::string::npos) rcalo=2;
			if(calo.find("HCAL") != std::string::npos || calo.find("ALL") != std::string::npos){
					zmax=rcalo*(1-etamax*etamax)/(2*etamax);
					zmin=rcalo*(1-etamin*etamin)/(2*etamin);
				}
			AdjustEtaEdge();
			MakePlots();
		}
		~caloplots()
		void validatePhi(){
			int good=0, nbins=E_phi->GetNbinsX();
			float avg=0;
			for(int i=0; i<nbins; i++) avg+=E_phi->GetBinContent(i+1);
			avg=avg/nbins;
			for(int j=0; j<nbins; j++) if(E_phi->GetBinContent(j+1) >0.5*avg && E_phi->GetBinContent(j+1) < 1.5*avg) good++;
			if(good>= 0.75*nbins) ok_phi=true;
		}
		getAcceptance(){
			for(int i=0; i<etabins; i++){
				int live_towers=0;
				for(int j=0; j<phibins; j++){
					if(E_T_eta_phi->GetBinContent(i+1, j+1) >= 0){
						 live_towers++;
						 acceptance++;
					}
				}
				if(live_towers==0) livetowers=1;
				float pct=(float)livetowers/(float)phibins;
				acceptance_eta->Fill(i, pct);
			}
			acceptance=acceptance/(phibins*etabins);
		}
		scaleThePlots(int n_evt=1, std::vector<std::map<std::string, float>> calo_data){
			dET_eta->Scale(1/n_evt);
			dET_eta->Divide(acceptance_eta)
			ET_phi->Scale(1/(n_evt*acceptance));
			ET_z->Scale(1/(n_evt*acceptance));
			dET->Scale(1/acceptance);
			for(auto cd:calo_data) cd[calo_name]=cd[calo_name]/acceptance;
			
		}
		BuildTotal(std::vector<std::map<std::string, float>> calo_data)
		{
			for(auto data_point:calo_data){
				float total_val=0;
				total_val+=data_point["EMCAL"];
				total_val+=data_point["IHCAL"];
				total_val+=data_point["OHCAL"];
				dET->Fill(total_val);
			}
		}
	private:
		void AdjustEtaEdge()
		{
			//shifts min and max eta value on z boundries.
			float deltaeta=0;
			etamax=-2/etamax-zh/rcalo+sqrt(pow((etamax*zh-rcalo)/(etamax*rcalo), 2)+ 1);
			etamin=-2/etamin-zl/rcalo+sqrt(pow((etamin*zl-rcalo)/(etamin*rcalo), 2)+ 1);
		}
		MakePlots(){
			if(calo.find("HCAL") != std::string::npos || calo.find("ALL") != std::string::npos){
				etabins=24;
				phibins=64;
			}

			Energy=new TH1F*(Form("Enengy_%s_z_%d", calo, z), Form("E_{event} in %s with vertex z=%d; E [GeV]", calo, z), 1000, 0.5, 1000.5);
			
			ET=new TH1F*(Form("ET_%s_z_%d", calo, z), Form("E_{T}^{event} in %s with vertex z=%d; E [GeV]", calo, z), 1000, 0.5, 1000.5);
			E_phi=new TH1F*(Form("E_phi_%s_z_%d", calo, z), Form("E(#varphi) in %s with vertex z=%d, #varphi, E [GeV]",calo,z), phinbins, -0.01, 6.3);
			
			ET_phi=new TH1F*(Form("ET_phi_%s_z_%d", calo, z), Form("E_{T}(#varphi) in %s with vertex z=%d, #varphi, #E_{T} [GeV]",calo,z), phinbins, -0.01, 6.3);
			
			dET_eta=new TH1F*(Form("dET_eta_%s_z_%d", calo, z), Form("dE_{T}/d#eta(#eta) in %s with vertex z=%d, #eta, #frac{dE_{T}}{d#eta} [GeV]",calo,z), petabins, etamin, etamax);
			
			ET_z=new TH1F*(Form("ET_z_%s_z_%d", calo, z), Form("E_{T}(z) in %s with vertex z=%d, z, #E_{T} [GeV]",calo,z), 40, zmin,zmax);

			dET=new TH1F*(Form("dET_%s_z_%d", calo, z), Form("#frac{dE_{T}}{d#eta} in %s with vertex z=%d, #frac{dE_{T}}{d#eta} [GeV]",calo,z), 1000, 0.5, 100.5);
			acceptance_eta=new TH1F*(Form("acceptance_%s_z_%d", calo, z), Form("Acceptance in physical #eta slice in %s with vertex z=%d, #eta, percent towers responding", calo,z), etabins, etamin, etamax);
			hists_1->push_back(Energy);
			hists_1->push_back(ET);
			hists_1->push_back(E_phi);
			hists_1->push_back(ET_phi);
			hists_1->push_back(dET_eta);
			hists_1->push_back(ET_z);
			hists_1->push_back(dET);
			hists_1->push_back(acceptance_eta);
			ET_eta_phi=new TH2F*(Form("ET_eta_phi_%s_z%d", calo, z), Form("E_{T}(#eta, #varphi) in %S with vertex z=%d, #eta, #varphi, E_{T} [GeV]", calo, z), etabins, etamin, etamax, phibins, phimax);
		       	E_eta_phi=new TH2F*(Form("E_eta_phi_%s_Z_%d", calo, z), Form("E(#eta, #varphi)	physical binning in %s with vertex centered at z=%d, #eta, #varphi, E [GeV]", calo, z), etabins, -1.13, 1.13, phibins, phimax);
			ET_z_eta=new TH2F*(Form("ET_z_eta_%s_%z_%d", calo, z), Form("E_{T}(z_{vertex}, #eta) in %s with vertex centered at z=%d, z_{vertex}, #eta, E_{T} [GeV]", calo, z), 40, zl, zh, etabins, etamin, etamax);
			hists_2->push_back(ET_eta_phi);
			hists_2->push_back(E_eta_phi);
			hists_2->push_back(ET_z_eta);
		}
}
struct plots{
		int z_bin=0;
		float zl=0, zh=1;
		caloplots* em=new caloplots("EMCAL",zl,zh);
		caloplots* ihcal=new caloplots("IHCAL", zl, zh);
		caloplots* ohcal=new caloplots("OHCAL", zl, zh);
		caloplots* total=new caloplots("ALL", zl, zh);
		std::vector<std::map<std::string, float>> event_data; 
}
		
