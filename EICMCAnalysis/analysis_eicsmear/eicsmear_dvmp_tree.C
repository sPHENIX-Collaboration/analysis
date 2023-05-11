#include <vector>
int eicsmear_dvmp_tree(TString filename_mc = "/sphenix/user/gregtom3/data/Fall2018/JPsi_reco_studies/reconstructedQ2/18x275_DVMP_1M_ascii_converted.root",
		       TString filename_mc_smeared = "/sphenix/user/gregtom3/data/Fall2018/JPsi_reco_studies/reconstructedQ2/18x275_DVMP_1M_ascii_converted.smear.root")
{

  /* PRELIMINARY ROOT STUFF */

  /* Loading libraries and setting sphenix style */
  gSystem->Load("libeicsmear");
  gROOT->LoadMacro("./sPHENIXStyle/sPhenixStyle.C");
  SetsPhenixStyle();



  /* INPUT */



  /* Input files */
  TFile *file_mc = new TFile(filename_mc, "OPEN");
  TFile *file_mc_smeared = new TFile(filename_mc_smeared, "OPEN");
  
  /* Trees */
  TTree *tree = (TTree*)file_mc->Get("EICTree");
  Double32_t Q2fromEICTree;
  tree->SetBranchAddress("trueQ2",&Q2fromEICTree);
  TTree *tree_smeared= (TTree*)file_mc_smeared->Get("Smeared");

  /* Output File */
  TFile *myfile = new TFile("/sphenix/user/gregtom3/data/Fall2018/JPsi_reco_studies/reconstructedQ2/analysisTree.root","RECREATE");
  /* Add friend */
  tree->AddFriend(tree_smeared);


  /* Create data tree because this is getting hectic */
  TTree *theTree = new TTree("DVMP_Tree","");
  Float_t de_phi_truth, de_eta_truth, de_pt_truth, de_p_truth;
  Float_t dp_phi_truth, dp_eta_truth, dp_pt_truth, dp_p_truth;
  Float_t sp_phi_truth, sp_eta_truth, sp_pt_truth, sp_p_truth;
  Float_t se_phi_truth, se_eta_truth, se_pt_truth, se_p_truth;
  Float_t jpsi_phi_truth, jpsi_eta_truth, jpsi_pt_truth, jpsi_p_truth;
  Float_t de_phi_reco, de_eta_reco, de_pt_reco, de_p_reco;
  Float_t dp_phi_reco, dp_eta_reco, dp_pt_reco, dp_p_reco;
  Float_t sp_phi_reco, sp_eta_reco, sp_pt_reco, sp_p_reco;
  Float_t se_phi_reco, se_eta_reco, se_pt_reco, se_p_reco;
  Float_t jpsi_phi_reco, jpsi_eta_reco, jpsi_pt_reco, jpsi_p_reco;
  Float_t invariant_reco_decay;
  Float_t invariant_reco_scatter;
  Float_t jpsi_px_truth, jpsi_py_truth, jpsi_pz_truth;
  Float_t jpsi_px_reco, jpsi_py_reco, jpsi_pz_reco;
  Double_t Q2;
  
  Float_t t;
  theTree->Branch("Q2",&Q2,"Event QSquared");
  theTree->Branch("t",&t,"Event t");
  theTree->Branch("de_phi_truth",&de_phi_truth,"Decay Electron Truth Phi");
  theTree->Branch("de_eta_truth",&de_eta_truth,"Decay Electron Truth Eta");
  theTree->Branch("de_pt_truth",&de_pt_truth,"Decay Electron Truth Pt");
  theTree->Branch("de_p_truth",&de_p_truth,"Decay Electron Truth P");
  theTree->Branch("dp_phi_truth",&dp_phi_truth,"Decay Positron Truth Phi");
  theTree->Branch("dp_eta_truth",&dp_eta_truth,"Decay Positron Truth Eta");
  theTree->Branch("dp_pt_truth",&dp_pt_truth,"Decay Positron Truth Pt");
  theTree->Branch("dp_p_truth",&dp_p_truth,"Decay Positron Truth P");
  theTree->Branch("sp_phi_truth",&sp_phi_truth,"Scattered Proton Truth Phi");
  theTree->Branch("sp_eta_truth",&sp_eta_truth,"Scattered Proton Truth Eta");
  theTree->Branch("sp_pt_truth",&sp_pt_truth,"Scattered Proton Truth Pt");
  theTree->Branch("sp_p_truth",&sp_p_truth,"Scattered Proton Truth P");
  theTree->Branch("se_phi_truth",&se_phi_truth,"Scattered Electron Truth Phi");
  theTree->Branch("se_eta_truth",&se_eta_truth,"Scattered Electron Truth Eta");
  theTree->Branch("se_pt_truth",&se_pt_truth,"Scattered Electron Truth Pt");
  theTree->Branch("se_p_truth",&se_p_truth,"Scattered Electron Truth P");
  theTree->Branch("jpsi_phi_truth",&jpsi_phi_truth,"Jpsi Truth Phi");
  theTree->Branch("jpsi_eta_truth",&jpsi_eta_truth,"Jpsi Truth Eta");
  theTree->Branch("jpsi_pt_truth",&jpsi_pt_truth,"Jpsi Truth Pt");
  theTree->Branch("jpsi_p_truth",&jpsi_p_truth,"Jpsi Truth P");
  theTree->Branch("de_phi_reco",&de_phi_reco,"Decay Electron Reco Phi");
  theTree->Branch("de_eta_reco",&de_eta_reco,"Decay Electron Reco Eta");
  theTree->Branch("de_pt_reco",&de_pt_reco,"Decay Electron Reco Pt");
  theTree->Branch("de_p_reco",&de_p_reco,"Decay Electron Reco P");
  theTree->Branch("dp_phi_reco",&dp_phi_reco,"Decay Positron Reco Phi");
  theTree->Branch("dp_eta_reco",&dp_eta_reco,"Decay Positron Reco Eta");
  theTree->Branch("dp_pt_reco",&dp_pt_reco,"Decay Positron Reco Pt");
  theTree->Branch("dp_p_reco",&dp_p_reco,"Decay Positron Reco P");
  theTree->Branch("sp_phi_reco",&sp_phi_reco,"Scattered Proton Reco Phi");
  theTree->Branch("sp_eta_reco",&sp_eta_reco,"Scattered Proton Reco Eta");
  theTree->Branch("sp_pt_reco",&sp_pt_reco,"Scattered Proton Reco Pt");
  theTree->Branch("sp_p_reco",&sp_p_reco,"Scattered Proton Reco P");
  theTree->Branch("se_phi_reco",&se_phi_reco,"Scattered Electron Reco Phi");
  theTree->Branch("se_eta_reco",&se_eta_reco,"Scattered Electron Reco Eta");
  theTree->Branch("se_pt_reco",&se_pt_reco,"Scattered Electron Reco Pt");
  theTree->Branch("se_p_reco",&se_p_reco,"Scattered Electron Reco P");
  theTree->Branch("jpsi_phi_reco",&jpsi_phi_reco,"Jpsi Reco Phi");
  theTree->Branch("jpsi_eta_reco",&jpsi_eta_reco,"Jpsi Reco Eta");
  theTree->Branch("jpsi_pt_reco",&jpsi_pt_reco,"Jpsi Reco Pt");
  theTree->Branch("jpsi_p_reco",&jpsi_p_reco,"Jpsi Reco P");
  theTree->Branch("jpsi_px_truth",&jpsi_px_truth,"Jpsi Truth Px");
  theTree->Branch("jpsi_py_truth",&jpsi_py_truth,"Jpsi Truth Py");
  theTree->Branch("jpsi_pz_truth",&jpsi_pz_truth,"Jpsi Truth Pz");
  theTree->Branch("jpsi_px_reco",&jpsi_px_reco,"Jpsi Reco Px");
  theTree->Branch("jpsi_py_reco",&jpsi_py_reco,"Jpsi Reco Py");
  theTree->Branch("jpsi_pz_reco",&jpsi_pz_reco,"Jpsi Reco Pz");
  theTree->Branch("invariant_reco_decay",&invariant_reco_decay,"Invariant Mass Decay Particles");
  theTree->Branch("invariant_reco_scatter",&invariant_reco_scatter,"Invariant Mass Scattered Electron");
  
  /* Momentum cut for plotting */
  TCut pcut("Smeared.particles.p<=50&&Smeared.particles.p>1");
  
  /* Eta cut for plotting */
  TCut ecut("-log(tan(Smeared.particles.theta/2))<=4&&-log(tan(Smeared.particles.theta/2))>=-4");

  /* Scattered lepton cut */
  TCut cut_scattered_lepton("particles.id==11&&particles.KS==2&&Smeared.particles.p<=50&&Smeared.particles.p>1");

  /* Decay electron cut */
  TCut cut_decay_electron("particles.id==11&&particles.KS==1&&Smeared.particles.p<=50&&Smeared.particles.p>1");

  /* Decay positron cut */
  TCut cut_decay_positron("particles.id==-11&&particles.KS==1&&Smeared.particles.p<=50&&Smeared.particles.p>1");



  /* PLOTTING */

  erhic::EventMilou *event  = NULL;
  Smear::Event       *eventS = NULL;

  tree->SetBranchAddress("event",&event);
  tree_smeared->SetBranchAddress("eventS", &eventS);
  
  unsigned max_event = tree_smeared->GetEntries();
  std::vector<float> particle_eta;
      particle_eta.push_back(0);
      particle_eta.push_back(0);
      particle_eta.push_back(0);
      particle_eta.push_back(0);
      particle_eta.push_back(0);
      std::vector<float> particle_theta;
      particle_theta.push_back(0);
      particle_theta.push_back(0);
      particle_theta.push_back(0);
      particle_theta.push_back(0);
      particle_theta.push_back(0);
      std::vector<float> particle_phi;
      particle_phi.push_back(0);
      particle_phi.push_back(0);
      particle_phi.push_back(0);
      particle_phi.push_back(0);
      particle_phi.push_back(0);
      std::vector<float> particle_pt;
      particle_pt.push_back(0);
      particle_pt.push_back(0);
      particle_pt.push_back(0);
      particle_pt.push_back(0);
      particle_pt.push_back(0);
      std::vector<float> particle_p;
      particle_p.push_back(0);
      particle_p.push_back(0);
      particle_p.push_back(0);
      particle_p.push_back(0);
      particle_p.push_back(0);
      
      std::vector<float> true_particle_eta;
      true_particle_eta.push_back(0);
      true_particle_eta.push_back(0);
      true_particle_eta.push_back(0);
      true_particle_eta.push_back(0);
      true_particle_eta.push_back(0);
      std::vector<float> true_particle_theta;
      true_particle_theta.push_back(0);
      true_particle_theta.push_back(0);
      true_particle_theta.push_back(0);
      true_particle_theta.push_back(0);
      true_particle_theta.push_back(0);
      std::vector<float> true_particle_phi;
      true_particle_phi.push_back(0);
      true_particle_phi.push_back(0);
      true_particle_phi.push_back(0);
      true_particle_phi.push_back(0);
      true_particle_phi.push_back(0);
      std::vector<float> true_particle_p;
      true_particle_p.push_back(0);
      true_particle_p.push_back(0);
      true_particle_p.push_back(0);
      true_particle_p.push_back(0);
      true_particle_p.push_back(0);
      std::vector<float> true_particle_pt;
      true_particle_pt.push_back(0);
      true_particle_pt.push_back(0);
      true_particle_pt.push_back(0);
      true_particle_pt.push_back(0);
      true_particle_pt.push_back(0);
  for ( unsigned ievent = 0; ievent < max_event/1000; ievent++ )
    {
      if ( ievent%1000 == 0 )
        cout << "Processing event " << ievent << endl;

      /* load event */
      tree->GetEntry(ievent);
      tree_smeared->GetEntry(ievent);
      cout << Q2fromEICTree << endl;
     
      unsigned ntracks = eventS->GetNTracks();
      
      

      
      for ( unsigned itrack = 0; itrack < ntracks; itrack++ )
        {
	  
	  Smear::ParticleMCS * sparticle = eventS->GetTrack( itrack );
	  erhic::ParticleMC * tparticle = event->GetTrack(itrack);
	  int type_of_particle = -1;

	  // Decay Electron
	  if(tparticle->Id().Code()==11&&tparticle->GetStatus()==1)
	    {
	      type_of_particle = 0;
	    }

	  // Scattered Electron
	  if(tparticle->Id().Code()==11&&tparticle->GetStatus()==2)
	    {
	      type_of_particle = 1;
	    }
	  
	  // Decay Positron
	  if(tparticle->Id().Code()==-11&&tparticle->GetStatus()==1)
	    {
	      type_of_particle = 2;
	    }

	  // Scattered Proton
	  if(tparticle->Id().Code()==2212&&tparticle->GetStatus()==1)
	    {
	      type_of_particle = 3;
	    }
	  // JPsi
	  if(tparticle->Id().Code()==443)
	    {
	      type_of_particle = 4;
	    }
	  
	  // Test that the smear particle was found
	  if(sparticle==NULL&&type_of_particle!=4)
	    {
	      particle_eta.at(type_of_particle)=NULL;
	      particle_theta.at(type_of_particle)=NULL;
	      particle_phi.at(type_of_particle)=NULL;
	      particle_pt.at(type_of_particle)=NULL;
	      particle_p.at(type_of_particle)=NULL;
	      
	      true_particle_eta.at(type_of_particle)=tparticle->GetEta();
	      true_particle_theta.at(type_of_particle)=tparticle->GetTheta();
	      true_particle_phi.at(type_of_particle)=tparticle->GetPhi();
	      true_particle_p.at(type_of_particle)=tparticle->GetP();
	      true_particle_pt.at(type_of_particle)=tparticle->GetPt();

	      //true_particle_eta.at(type_of_particle)=NULL;
	      //true_particle_phi.at(type_of_particle)=NULL;
	      //true_particle_p.at(type_of_particle)=NULL;
	      //true_particle_pt.at(type_of_particle)=NULL;
	    }
	  else if(type_of_particle!=4)
	    {
	      
	      particle_eta.at(type_of_particle)=sparticle->GetEta();
	      particle_theta.at(type_of_particle)=2*atan(exp(-sparticle->GetEta()));
	      particle_phi.at(type_of_particle)=sparticle->GetPhi();
	      particle_pt.at(type_of_particle)=sparticle->GetPt();
	      particle_p.at(type_of_particle)=sparticle->GetP();
	      
	      true_particle_eta.at(type_of_particle)=tparticle->GetEta();
	      true_particle_theta.at(type_of_particle)=tparticle->GetTheta();
	      true_particle_phi.at(type_of_particle)=tparticle->GetPhi();
	      true_particle_p.at(type_of_particle)=tparticle->GetP();
	      true_particle_pt.at(type_of_particle)=tparticle->GetPt();
	    }
	  else if(type_of_particle==4)
	    {
	      true_particle_eta.at(type_of_particle)=tparticle->GetEta();
	      true_particle_theta.at(type_of_particle)=tparticle->GetTheta();
	      true_particle_phi.at(type_of_particle)=tparticle->GetPhi();
	      true_particle_p.at(type_of_particle)=tparticle->GetP();
	      true_particle_pt.at(type_of_particle)=tparticle->GetPt();
	    }
	  
	} // end loop over particles

      
      bool can_we_reconstruct_3_final_state_leptons = true;
      if((particle_p.at(0)<1||particle_p.at(1)<1)||particle_p.at(2)<1)
	{
	  can_we_reconstruct_3_final_state_leptons = false;
	}
      
      bool can_we_reconstruct_scattered_lepton = true;
      if(particle_p.at(1)<1)
	{
	  can_we_reconstruct_scattered_lepton = false;
	}

      bool can_we_reconstruct_decay_particles = true;
      if(particle_p.at(0)<1||particle_p.at(2)<1)
	{
	  can_we_reconstruct_decay_particles = false;
	}

      // From the eta, phi and p arrays, calculate the invariant mass
      // First, try the decay electron and decay positron
      if(can_we_reconstruct_3_final_state_leptons)
	{
	  float m = sqrt(2*particle_pt.at(0)*particle_pt.at(2)*(cosh(particle_eta.at(0)-particle_eta.at(2))-cos(particle_phi.at(0)-particle_phi.at(2))));
	  invariant_reco_decay=m;
	}

      // Now try the scattered electron and decay positron
      
      if(can_we_reconstruct_3_final_state_leptons)
	{
	  float m = sqrt(2*particle_pt.at(1)*particle_pt.at(2)*(cosh(particle_eta.at(1)-particle_eta.at(2))-cos(particle_phi.at(1)-particle_phi.at(2))));
	  invariant_reco_scatter=m;
	}
      // cout << particle_theta.at(0) << endl;
      // Next, fill in reco J/Psi stuff 
      if(can_we_reconstruct_3_final_state_leptons)
	{
	  float sum_px = particle_pt.at(0)*cos(particle_phi.at(0))+particle_pt.at(2)*cos(particle_phi.at(2));
	  float sum_py = particle_pt.at(0)*sin(particle_phi.at(0))+particle_pt.at(2)*sin(particle_phi.at(2));
	  float sum_pz = particle_p.at(0)*cos(particle_theta.at(0))+particle_p.at(2)*cos(particle_theta.at(2));
	  float sum_p = sqrt(sum_px*sum_px+sum_py*sum_py+sum_pz*sum_pz);
	  jpsi_px_reco=sum_px;
	  jpsi_py_reco=sum_py;
	  jpsi_pz_reco=sum_pz;
	  
	  
	  float sum_px_true = true_particle_pt.at(0)*cos(true_particle_phi.at(0))+true_particle_pt.at(2)*cos(true_particle_phi.at(2));
	  float sum_py_true = true_particle_pt.at(0)*sin(true_particle_phi.at(0))+true_particle_pt.at(2)*sin(true_particle_phi.at(2));
	  float sum_pz_true = true_particle_p.at(0)*cos(true_particle_theta.at(0))+true_particle_p.at(2)*cos(true_particle_theta.at(2));	  
	  jpsi_px_truth=sum_px_true;
	  jpsi_py_truth=sum_py_true;
	  jpsi_pz_truth=sum_pz_true;

	  
	  
	  // if(sum_p>4.8&&sum_p<5)
	  //cout << ievent << " - " << true_particle_p.at(4) << " : " << sum_p << " : " << sum_pz << " : " << sum_px << " : " << sum_py << " : " << endl;
	  //float jpsi_eta = -log(tan( (acos( (particle_p.at(0)*cos(2*atan(exp(-particle_eta.at(0))))+particle_p.at(2)*cos(2*atan(exp(-particle_eta.at(2))))/(sum_p) )))/2));
	  //float jpsi_eta = -log(tan((acos(particle_p.at(0)*cos(particle_theta.at(0))+particle_p.at(2)*cos(particle_theta.at(2))))
	  // float jpsi_eta = -log(tan(atan( (particle_pt.at(0)+particle_pt.at(2))/(particle_p.at(0)*cos(particle_theta.at(0))+particle_p.at(2)*cos(particle_theta.at(2))))/2));
	  //	  float jpsi_phi = acos( (particle_pt.at(0)*cos((particle_phi.at(0)))+particle_pt.at(2)*cos((particle_phi.at(2))))/(sqrt(sum_px*sum_px+sum_py*sum_py)) );
	  float jpsi_eta = -log(tan( (acos(jpsi_pz_reco/sum_p))/2 ) );
	  float jpsi_phi = atan(jpsi_py_reco/jpsi_px_reco);
	  if(jpsi_py_reco>0&&jpsi_px_reco<0)
	    jpsi_phi=3.14159265+jpsi_phi;
	  else if(jpsi_py_reco<0&&jpsi_px_reco<0)
	    jpsi_phi=3.14159265+jpsi_phi;
	  else if(jpsi_py_reco<0&&jpsi_px_reco>0)
	    jpsi_phi=2*3.14159265+jpsi_phi;
	    
	  //cout << true_particle_phi.at(4) << " : " << jpsi_px_truth << " : " << jpsi_py_truth << " : " << atan(jpsi_py_truth/jpsi_px_truth)<< endl;
	  //cout << true_particle_phi.at(4) << " : " << jpsi_phi<< endl;
	    //cout << jpsi_phi << " : " << true_particle_phi.at(4) << endl;
	  particle_p.at(4)=sum_p;
	  particle_pt.at(4)=sum_p*sin(jpsi_phi);
	  particle_eta.at(4)=jpsi_eta;
	  particle_phi.at(4)=jpsi_phi; 
	  

	  // Event Kinematics
	  
	  
	  t = 2*275*true_particle_p.at(3)*(1-cos(2*atan(exp(-true_particle_eta.at(3)))));
	}
      
      de_phi_truth=true_particle_phi.at(0);
      de_eta_truth=true_particle_eta.at(0);
      de_pt_truth=true_particle_pt.at(0);
      de_p_truth=true_particle_p.at(0);
      dp_phi_truth=true_particle_phi.at(2);
      dp_eta_truth=true_particle_eta.at(2);
      dp_pt_truth=true_particle_pt.at(2);
      dp_p_truth=true_particle_p.at(2);
      sp_phi_truth=true_particle_phi.at(3);
      sp_eta_truth=true_particle_eta.at(3);
      sp_pt_truth=true_particle_pt.at(3);
      sp_p_truth=true_particle_p.at(3);
      se_phi_truth=true_particle_phi.at(1);
      se_eta_truth=true_particle_eta.at(1);
      se_pt_truth=true_particle_pt.at(1);
      se_p_truth=true_particle_p.at(1);
      jpsi_phi_truth=true_particle_phi.at(4);
      jpsi_eta_truth=true_particle_eta.at(4);
      jpsi_pt_truth=true_particle_pt.at(4);
      jpsi_p_truth=true_particle_p.at(4);
      de_phi_reco=particle_phi.at(0);
      de_eta_reco=particle_eta.at(0);
      de_pt_reco=particle_pt.at(0);
      de_p_reco=particle_p.at(0);
      dp_phi_reco=particle_phi.at(2);
      dp_eta_reco=particle_eta.at(2);
      dp_pt_reco=particle_pt.at(2);
      dp_p_reco=particle_p.at(2);
      sp_phi_reco=particle_phi.at(3);
      sp_eta_reco=particle_eta.at(3);
      sp_pt_reco=particle_pt.at(3);
      sp_p_reco=particle_p.at(3);
      se_phi_reco=particle_phi.at(1);
      se_eta_reco=particle_eta.at(1);
      se_pt_reco=particle_pt.at(1);
      se_p_reco=particle_p.at(1);
      jpsi_phi_reco=particle_phi.at(4);
      jpsi_eta_reco=particle_eta.at(4);
      jpsi_pt_reco=particle_pt.at(4);
      jpsi_p_reco=particle_p.at(4);
      // if(ievent==770)
      //	cout << jpsi_p_truth << " : " << jpsi_p_reco << endl;
      theTree->Fill();
    } // end loop over events
  
  /////

 

  myfile->cd();
  theTree->Write("Tree");
  myfile->Write();
  myfile->Close();
  return 0;
}

float eta_to_theta(float eta)
{
  return 2*atan(exp(-1*eta));
}

float theta_to_eta(float theta)
{
  return -log(tan(theta/2));
}
