class caloplots{ //creates a relevant plot for each calorimeter type
	public: 
		std::string calo="EMCAL";
		TH1F* Energy, E_phi, *ET, *dET_eta, *ET_phi, *ET_z, *dET, *acceptance_eta;
		TH2F* ET_eta_phi, *E_eta_phi, *E_T_z_eta;
		float acceptance=0.0; 
		bool ok_phi=false;
		float z=0, zl, zh;
		caloplots::caloplots(float zlow, float zhigh, std::string caloname){
			zl=zlow; 
			zh=zhigh;
			calo=caloname;
			z=(zl+zh)/2;
			MakePlots();
		}

		void validatePhi(){
			int good=0, nbins=E_phi->GetNbinsX();
			float avg=0;
			for(int i=0; i<nbins; i++) avg+=E_phi->GetBinContent(i+1);
			avg=avg/nbins;
			for(int j=0; j<nbins; j++) if(E_phi->GetBinContent(j+1) >0.5*avg && E_phi->GetBinContent(j+1) < 1.5*avg) good++;
			if(good>= 0.75*nbins) ok_phi=true;
		}

	private:
		MakePlots()

struct plots{
		int z_bin=0;
		
