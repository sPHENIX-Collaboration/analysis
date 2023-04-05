void track_kinematics()
{
  int triggersample = 30;
  gStyle->SetOptStat(0);
  TFile* infile = TFile::Open(Form("jet_pythia_%igev/trees/0.root",triggersample));

  //-----------------------------------------------------------------------------------------------------------------------
  //Weight factors to enable splicing together mb, 10 GeV, and 30 GeV MC samples
  //-----------------------------------------------------------------------------------------------------------------------
  float csMB = 4.197*pow(10,9)*pow(10,-12); //I refuse to do math
  float cs10GeV = 3.210*pow(10,6)*pow(10,-12);
  float cs30GeV = 2.178*pow(10,3)*pow(10,-12);
  float weight10GeV = cs10GeV/csMB;
  float weight30GeV = cs30GeV/csMB;
  float weight = weight30GeV;

  //---------------------------------------------------------------
  //putting pi in a float for ease of use later on
  //--------------------------------------------------------------- 
  float pi = 3.1415926;


  TTree* tree = (TTree*)infile->Get("tree");
  //--------------------------------------------
  // Vectors and floats for various 
  // quantities in the stored ttree
  //--------------------------------------------
  float zvtx;
  std::vector<float> *trk_pt = {0};
  std::vector<float> *trk_phi = {0};
  std::vector<float> *trk_eta = {0};
  std::vector<float> *trk_quality = {0};
  std::vector<float> *trk_nhits = {0};

  std::vector<float> *tp_pt = {0};
  std::vector<float> *tp_phi = {0};
  std::vector<float> *tp_eta = {0};
  std::vector<int> *tp_pid = {0};

  std::vector<float> *jet_pt = {0};
  std::vector<float> *jet_phi = {0};
  std::vector<float> *jet_eta = {0};

  std::vector<float> *tjet_pt = {0};
  std::vector<float> *tjet_phi = {0};
  std::vector<float> *tjet_eta = {0};
  //------------------------------------------------------------------------
  //Set the branch address for quantities in the ttree
  //------------------------------------------------------------------------
  tree->SetBranchAddress("trk_pt",&trk_pt);
  tree->SetBranchAddress("trk_phi",&trk_phi);
  tree->SetBranchAddress("trk_eta",&trk_eta);
  tree->SetBranchAddress("trk_quality",&trk_quality);
  tree->SetBranchAddress("nmvtx_hits",&trk_nhits);

  tree->SetBranchAddress("tjet_pt",&tjet_pt);
  tree->SetBranchAddress("tjet_phi",&tjet_phi);
  tree->SetBranchAddress("tjet_eta",&tjet_eta);

  tree->SetBranchAddress("tjet_pt",&jet_pt);
  tree->SetBranchAddress("tjet_phi",&jet_phi);
  tree->SetBranchAddress("tjet_eta",&jet_eta);

  tree->SetBranchAddress("tp_pt",&tp_pt);
  tree->SetBranchAddress("tp_phi",&tp_phi);
  tree->SetBranchAddress("tp_eta",&tp_eta);
  tree->SetBranchAddress("tp_pid",&tp_pid);

  tree->SetBranchAddress("zvtx",&zvtx);

  //-----------------------------------------
  // Define histograms for filling
  //-----------------------------------------

  TH1D* h_deltar = new TH1D ("h_deltar",";delta R;",1000,0,0.4);
  h_deltar->Sumw2();
  TH2D* h_ptpart_pttrack = new TH2D("h_ptpart_pttrack",";pt_particle;pt_track/pt_particle",100,0,100,1000,0,2);
  h_ptpart_pttrack->Sumw2();

  //------------------------------------------------
  //Loop over the events in the ttree
  //------------------------------------------------

  int nentries = tree->GetEntries();
  for (int q = 0; q < nentries ;q++)
    {
      tree->GetEntry(q);
      if (abs(zvtx) > 10){continue;} // reject events at large z vertex
      int ntracks = trk_pt->size();
      int n_particles = tp_pt->size();

      for (int j = 0;j < ntracks;j++)
      	{
      	  float trkphi = trk_phi->at(j);
      	  float trketa = trk_eta->at(j);
	  //-----------------------------------------
	  //Apply some track selections
	  // to select decent tracks
	  //-----------------------------------------
	  if (trk_nhits->at(j) < 4){continue;}
	  if (trk_quality->at(j) > 6){continue;}
	  float drmax = 0.4;
	  float partpt = 0;
	  for (int k = 0; k < n_particles;k++)
	    {
	      float deltaphi = abs(trkphi - tp_phi->at(k));
	      if (deltaphi > pi) {deltaphi = 2*pi-deltaphi;}
	      float deltaeta = abs(trketa-tp_eta->at(k));
	      float deltar = TMath::Sqrt(deltaphi*deltaphi + deltaeta*deltaeta);  
	      if (deltar < drmax)
		{
		  drmax = deltar;
		  partpt = tp_pt->at(k);
		}
	    }
	  h_deltar->Fill(drmax,weight);
	  if (drmax < 0.02)
	    {
	      h_ptpart_pttrack->Fill(partpt,trk_pt->at(j)/partpt,weight);
	    }
	}
    }


  TCanvas*c1 = new TCanvas("c1","",700,500);
  h_ptpart_pttrack->Draw("COLZ");


  TCanvas*c2 = new TCanvas("c2","",700,500);
  h_deltar->Draw();




}
