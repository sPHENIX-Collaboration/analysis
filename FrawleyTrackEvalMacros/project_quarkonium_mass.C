void project_quarkonium_mass()
{
  //SetsPhenixStyle();
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);

  TFile *fout = new TFile("root_files/ntp_quarkonium_out.root","recreate");
  
  TH1D* recomass[4];
  char name[500];
  for(int istate = 0; istate < 4; ++istate)
    {
      sprintf(name, "recomass%i",istate);
      //recomass[istate] = new TH1D(name,name,100,7.0,11.0);
      recomass[istate] = new TH1D(name,name,200,7.0,11.0);

    }

  TH2D *hdca2d = new TH2D("hdca2d","hdca2d", 200, 0.0, 50.0, 1000, -0.002, 0.002);
  TH2D *hpcaz = new TH2D("hpcaz","hpcaz",200, 0.0, 50.0, 1000,-0.1,0.1);

  Float_t event;
  Float_t px;
  Float_t py;
  Float_t pz;
  Float_t pt;
  Float_t gembed;
  Float_t quality;
  Float_t charge;
  Float_t dca2d;
  Float_t pcaz;
  Float_t gvz;
  Float_t ntpc;
  Float_t nmaps;

  Float_t decaymass=0.000511;

  // what do we have in the file?
  int events_per_file = 1;
  int num_ups_per_event = 20;
 
 // what maximum yields do we want?
  // These are pp yields for 197 /nb of pp running, with 
  //double pair_acc = 0.60 * 0.9 * 0.98;  // fraction into 1 unit of rapidity * pair eID fraction * trigger fraction 

  // pp case:
  // These are pp yields from Sasha's TN for 13,100 B pp collisions
  //int Npp[3] = {17074, 4440, 2475};  // 1S, 2S, 3S 

  // AuAu case:
  // These are yields from Sasha's TN for 24B AuAu events with 100% eID
  int Npp[3] = {27120, 7053, 3932};  // 1S, 2S, 3S unsuppressed 

  double suppr_state[3] = {0.535, 0.17, 0.035}; // suppression factor from Strickland paper 
  double eID_eff = 0.9;
  double tracking_eff = 0.95;

  double quality_cut = 10;

  bool no_suppression = true;

  int nupsmax[3] = {0,0,0};
  for(int istate = 0; istate < 3; ++istate)
    {
      if(no_suppression) suppr_state[istate] = 1.0;

      nupsmax[istate] = eID_eff * tracking_eff * suppr_state[istate] * Npp[istate];

      nupsmax[istate] = 100000;  // take all
      cout << "Requested max Upsilon counts for istate " << istate << " = " << nupsmax[istate] << endl;
    }


  int nups[3] = {0,0,0};      

  cout << "Upsilons requested = " << nupsmax << endl;

  for(int ifile=0;ifile<1000;ifile++)
    {
      if(nups[0] > nupsmax[0])
	break;
      
      char name[2000];

      //sprintf(name,"/sphenix/user/frawley/cluster_efficiency/macros/macros/g4simulations/eval_output/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      //sprintf(name,"/sphenix/user/frawley/acts_qa/macros/macros/g4simulations/eval_output_3/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      sprintf(name,"/sphenix/user/frawley/acts_qa/macros/macros/g4simulations/eval_output_2/g4svtx_eval_%i.root_g4svtx_eval.root",ifile);
      
      cout << "Adding file number " << ifile << " with name " << name << endl;     

      TChain* ntp_vertex = new TChain("ntp_vertex","reco events");
      ntp_vertex->Add(name);

      TChain* ntp_track = new TChain("ntp_track","reco tracks");
      ntp_track->Add(name);

      ntp_track->SetBranchAddress("event",&event);
      ntp_track->SetBranchAddress("px",&px);
      ntp_track->SetBranchAddress("py",&py);
      ntp_track->SetBranchAddress("pz",&pz);
      ntp_track->SetBranchAddress("pt",&pt);
      ntp_track->SetBranchAddress("charge",&charge);
      ntp_track->SetBranchAddress("dca2d",&dca2d);
      ntp_track->SetBranchAddress("pcaz",&pcaz);
      ntp_track->SetBranchAddress("gvz",&gvz);
      ntp_track->SetBranchAddress("gembed",&gembed);
      ntp_track->SetBranchAddress("quality",&quality);
      ntp_track->SetBranchAddress("ntpc",&ntpc);
      ntp_track->SetBranchAddress("nmaps",&nmaps);

      int ntracks=ntp_track->GetEntries();

      // capture the dca2d and dcaz histograms for the pions
      for(int i=0;i<ntracks;i++)
	{
	  ntp_track->GetEntry(i);
	  
	  if(gembed != 2)
	    continue;

	  hdca2d->Fill(pt, dca2d);
	  hpcaz->Fill(pt, pcaz-gvz);
	}

      //events_per_file = ntp_vertex->GetEntries();
      cout << " events in this file = " << events_per_file << endl;

      for(int iev = 0;iev<events_per_file;iev++)
	{      
	  for(int iups = 0;iups< num_ups_per_event; iups++)
	    {
	      //int istate = iups%3;
	      int istate = 0;

	      if(nups[istate] > nupsmax[istate])
		continue;

	      int nlept1 = 0;
	      int nlept2 = 0;
	      int lept1=-1;
	      int lept2 = -1;
	      Float_t px1=0;
	      Float_t px2=0;
	      Float_t py1=0;
	      Float_t py2=0;
	      Float_t pz1 = 0;
	      Float_t pz2 = 0;

	      TLorentzVector t1;
	      TLorentzVector t2;	      
	      for(int i=0;i<ntracks;i++)
		{
		  ntp_track->GetEntry(i);
		  
		  if(event != iev)
		    continue;
		  
		  if(gembed != iups+3)
		    continue;

		  if(quality > quality_cut)
		    continue;

		  if(ntpc < 20)
		    continue;

		  if(nmaps <= 1)
		    continue;

		  if(sqrt(px*px + py*py + pz*pz) < 1.0)
		    continue;

		  //		   cout << "event = " << event << " iups " << iups << " charge " << charge << " px " << px << " py " << py << " pz " << pz << " gembed " << gembed << " quality " << quality << endl;


		  if(charge == 1)
		    {
		      nlept1++;

		      lept1 = 1;	
		      px1 = px;
		      py1 = py;
		      pz1 = pz;
		      
		      Float_t E1 = sqrt( pow(px1,2) + pow(py1,2) + pow(pz1,2) + pow(decaymass,2));
		      t1.SetPxPyPzE(px1,py1,pz1,E1);	  
		    }

		  if(charge == -1)
		    {
		      nlept2++;

		      lept2 = 1;	
		      px2 = px;
		      py2 = py;
		      pz2 = pz;
		      
		      Float_t E2 = sqrt( pow(px2,2) + pow(py2,2) + pow(pz2,2) + pow(decaymass,2));
		      t2.SetPxPyPzE(px2,py2,pz2,E2);	  
		    }
		  //cout << " lept1 " << lept1 << " lept2 " << lept2 << endl;
		}

	      // calculate mass
	      if(nlept1 == 1 && nlept2 == 1)
	      //if(nlept1 > 0 && nlept2 > 0)
		{
		  TLorentzVector tsum;
		  tsum = t1+t2;
		  recomass[istate]->Fill(tsum.M());
		  recomass[3]->Fill(tsum.M()); // total spectrum
		  if(tsum.M() > 7 && tsum.M() < 11)
		    nups[istate]++;
		  //cout << " event " << event << " gembed " << gembed << endl;	  
		  //cout << " istate " << " nups[istate] " << nups[istate] << " reco mass " << tsum.M() << endl;
		}
	    }
	}
      delete ntp_track; 
      delete ntp_vertex;      
    }

  cout << " requested: " << nupsmax[0] << " reconstructed: " << nups[0] << " Upsilons " << endl; 
 
  for(int istate =0; istate < 3; ++istate)
    {
      recomass[istate]->Write();
      cout << " requested for istate: " << istate << " nupsmax = " << nupsmax[istate] << " reconstructed = " << nups[istate] << " Upsilons " << endl; 
    }
  recomass[3]->Write();

  hdca2d->Write();
  hpcaz->Write();

  fout->Close();

} 


