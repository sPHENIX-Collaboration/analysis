#include <cmath>
#include <TFile.h>
#include <TNtuple.h>
#include <TH2D.h>
#include <TCut.h>
#include <Eigen/Dense>

void kfparticle_plot_kshort()
{
  TFile fin("myKshortReco/combined_hijing.root");
  //TFile fin("hijing_eval_output/combined_ntp_mass_out.root"); TFile
  //fin("old_combined_ntp_mass_out/feb15.root");
  
  TTree *ntuple ;
  fin.GetObject("DecayTree",ntuple);

  float K0_mass;
  float K0_decayLength;
  float K0_x;
  float K0_y;
  float K0_z;
  float K0_IP_xy;
  float K0_px;
  float K0_py;
  float K0_pz;
  float K0_pT;
  float K0_pseudorapidity;
  float primary_vertex_x;
  float primary_vertex_y;
  float primary_vertex_z;


  ntuple->SetBranchAddress("K0_mass",&K0_mass);
  ntuple->SetBranchAddress("K0_decayLength",&K0_decayLength);
  ntuple->SetBranchAddress("K0_x",&K0_x);
  ntuple->SetBranchAddress("K0_y",&K0_y);
  ntuple->SetBranchAddress("K0_z",&K0_z);
  ntuple->SetBranchAddress("K0_IP_xy",&K0_IP_xy);
  ntuple->SetBranchAddress("K0_px",&K0_px);
  ntuple->SetBranchAddress("K0_py",&K0_py);
  ntuple->SetBranchAddress("K0_pz",&K0_pz);
  ntuple->SetBranchAddress("K0_pT",&K0_pT);
  ntuple->SetBranchAddress("K0_pseudorapidity",&K0_pseudorapidity);
  ntuple->SetBranchAddress("primary_vertex_x",&primary_vertex_x);
  ntuple->SetBranchAddress("primary_vertex_y",&primary_vertex_y);
  ntuple->SetBranchAddress("primary_vertex_z",&primary_vertex_z);


  /* 
 K0_mass         = 0
 K0_massErr      = 0
 K0_decayTime    = 0
 K0_decayTimeErr = 0
 K0_decayLength  = 0
 K0_decayLengthErr = 0
 K0_DIRA         = 0
 K0_FDchi2       = 0
 K0_IP           = 0
 K0_IPchi2       = 0
 K0_IPErr        = 0
 K0_IP_xy        = 0
 K0_x            = 0
 K0_y            = 0
 K0_z            = 0
 K0_px           = 0
 K0_py           = 0
 K0_pz           = 0
 K0_pE           = 0
 K0_p            = 0
 K0_pErr         = 0
 K0_pT           = 0
 K0_pTErr        = 0
 K0_charge       = 0
 K0_pseudorapidity = 0
 K0_rapidity     = 0
 K0_theta        = 0
 K0_phi          = 0
 K0_vertex_volume = 0
 K0_chi2         = 0
 K0_nDoF         = 0
 K0_PDG_ID       = 0
 K0_Covariance   = 0, 
                  0, 0, 0, 0, 0, 
                  0, 0, 0, 0, 0, 
                  0, 0, 0, 0, 0, 
                  0, 0, 0, 0
 track_1_mass    = 0
 track_1_IP      = 0
 track_1_IPchi2  = 0
 track_1_IPErr   = 0
 track_1_IP_xy   = 0
 track_1_x       = 0
 track_1_y       = 0
 track_1_z       = 0
 track_1_px      = 0
 track_1_py      = 0
 track_1_pz      = 0
 track_1_pE      = 0
 track_1_p       = 0
 track_1_pErr    = 0
 track_1_pT      = 0
 track_1_pTErr   = 0
 track_1_jT      = 0
 track_1_charge  = 0
 track_1_pseudorapidity = 0
 track_1_rapidity = 0
 track_1_theta   = 0
 track_1_phi     = 0
 track_1_chi2    = 0
 track_1_nDoF    = 0
 track_1_track_ID = 0
 track_1_PDG_ID  = 0
 track_1_Covariance = 0, 
                  0, 0, 0, 0, 0, 
                  0, 0, 0, 0, 0, 
                  0, 0, 0, 0, 0, 
                  0, 0, 0, 0
 track_2_mass    = 0
 track_2_IP      = 0
 track_2_IPchi2  = 0
 track_2_IPErr   = 0
 track_2_IP_xy   = 0
 track_2_x       = 0
 track_2_y       = 0
 track_2_z       = 0
 track_2_px      = 0
 track_2_py      = 0
 track_2_pz      = 0
 track_2_pE      = 0
 track_2_p       = 0
 track_2_pErr    = 0
 track_2_pT      = 0
 track_2_pTErr   = 0
 track_2_jT      = 0
 track_2_charge  = 0
 track_2_pseudorapidity = 0
 track_2_rapidity = 0
 track_2_theta   = 0
 track_2_phi     = 0
 track_2_chi2    = 0
 track_2_nDoF    = 0
 track_2_track_ID = 0
 track_2_PDG_ID  = 0
 track_2_Covariance = 0, 
                  0, 0, 0, 0, 0, 
                  0, 0, 0, 0, 0, 
                  0, 0, 0, 0, 0, 
                  0, 0, 0, 0
 track_1_track_2_DCA = 0
 primary_vertex_x = 0
 primary_vertex_y = 0
 primary_vertex_z = 0
 primary_vertex_nTracks = 0
 primary_vertex_volume = 0
 primary_vertex_chi2 = 0
 primary_vertex_nDoF = 0
 primary_vertex_Covariance = 0, 
                  0, 0, 0, 0, 0
 secondary_vertex_mass_pionPID = 0
 nPrimaryVertices = 0
 nEventTracks    = 0
 runNumber       = 0
 eventNumber     = 0
  */



  
  // normalize to find decay length

  TH2D *decay_length = new TH2D("decay_length","",5000,-1,1,5000,0,10);

  //TCut cut = "K0_mass>0.1&&abs(track_1_IP_xy)>0.02&&abs(track_2_IP_xy)>0.02&&abs(track_1_track_2_DCA)<0.05&&K0_pT>0.1&&abs(K0_x)>0.2&&abs(K0_y)>0.2&&abs(K0_z-primary_vertex_z)>0.1"; 

  TCut cut = "K0_mass>0.1&&abs(track_1_IP_xy)>0.02&&abs(track_2_IP_xy)>0.02&&abs(track_1_track_2_DCA)<0.05&&K0_pT>0.1&&abs(K0_x)>0.2&&abs(K0_y)>0.2";

  //TCut cut = "K0_mass>0.1&&abs(track_1_IP_xy)>0.02&&abs(track_2_IP_xy)>0.02&&abs(track_1_track_2_DCA)<0.05&&K0_pT>0.1&&abs(K0_decayLength)>0.3";

  //TCut cut = "K0_mass>0.1&&abs(track_1_IP)>0.02&&abs(track_2_IP)>0.02&&abs(track_1_track_2_DCA)<0.05&&K0_pT>0.1&&abs(K0_x)>0.1&&abs(K0_y)>0.1"; 

  //TCut cut = "K0_mass>0.1&&track_1_IP_xy>0.02&&track_2_IP_xy>0.02&&abs(track_1_track_2_DCA)<0.05&&K0_pT>0.1&& sqrt(pow(K0_x,2) + pow(K0_y,2))>0.4";
  //TCut cut = "invariant_mass>0.1&&dca3dxy1>0.02&&dca3dxy2>0.02&&abs(pair_dca)<0.05&&K0_pT>0.1"; 
  //TCut cut = "abs(track_1_IP_xy)>0.02&&abs(track_2_IP_xy)>0.02&&abs(K0_decayLength)>0.2";

  //TCut kshort_cut = "K0_mass > 0.485 && K0_mass<0.515";

  TCut kshort_cut="";
 
  int entries = ntuple->GetEntries();


  //c1->cd(2);
  TH1D *invariant_mass = new TH1D("invariant_mass","Invariant Mass",1000,0.35,0.65);
  invariant_mass->SetMinimum(0);


  TH2D *pathlengthpathlengthz = new TH2D("pathlengthpathlengthz","path v. pathlength_z",5000,-20.0,20.0,5000,0.0,10.0);

  TH2D *pathMass = new TH2D("pathMass","Invariant Mass vs. Path Length",5000,0.0,1.0,5000,0.0,5.0);


  for(int i=0; i<entries; ++i)
    {
      ntuple->GetEntry(i);
      Eigen::Vector3f K0( K0_x, K0_y, K0_z);
      Eigen::Vector3f vertex(primary_vertex_x, primary_vertex_y,  primary_vertex_z);
      Eigen::Vector3f projected_momentum (K0_px,K0_py,K0_pz);

      Eigen::Vector3f pathLength = K0-vertex;

      float costheta = pathLength.dot(projected_momentum)/(projected_momentum.norm()*pathLength.norm());
      float radius = sqrt(pow(pathLength(0),2) + pow(pathLength(1),2));

      if(costheta < 0.9995) continue;
      // if(abs(K0_x) < 0.2) continue;
      // if(abs(K0_y) < 0.2) continue;
      if(abs(pathLength(0)) < 0.2) continue;
      if(abs(pathLength(1)) < 0.2) continue;
      if(radius > 2.5) continue;
      //if(abs(K0_z- primary_vertex_z)<0.2) continue;

	 
      invariant_mass->Fill(K0_mass);
      pathlengthpathlengthz->Fill((K0_z-primary_vertex_z),pathLength.norm());
      pathMass->Fill(K0_mass,pathLength.norm());
	
    } 


  TCanvas *c1 = new TCanvas("c1","",10,10,800,800);

  TF1* f = new TF1("f","gaus(0)+[3]+[4]*x",0.45,0.55);
  f->SetParameter(0,100);
  f->SetParameter(1,0.497);
  f->SetParameter(2,0.010);
  f->SetParameter(3,0.0);
  f->SetParameter(4,0.0);
  
  invariant_mass->Fit("f","R");
  invariant_mass->DrawCopy();

  int binLow      = invariant_mass->FindBin(0.48); 
  int binHigh     = invariant_mass->FindBin(0.52);
  double integral = invariant_mass->Integral(binLow,binHigh);
  std::cout << " Integral: "<< integral << std::endl;

  TF1* fbackground  = new TF1("fbackground","[0]+[1]*x");
  fbackground->SetParameter(0,f->GetParameter(3));
  fbackground->SetParameter(1,f->GetParameter(4));

  double backgroundIntegral    = fbackground->Integral(0.485,0.51);
  double foregroundIntegral    = f->Integral(0.485,0.51);
  double signal                = (foregroundIntegral - backgroundIntegral);
  double signalBackgroundRatio = signal/backgroundIntegral;
  std::cout<< "signal: " << signal<<"  Signal to background ratio: " << signalBackgroundRatio << std::endl;


  TCanvas *c2 = new TCanvas("c2","",10,10,600,600); // path v path_z
  pathlengthpathlengthz->DrawCopy();

  TCanvas *c3 = new TCanvas("c3","",10,10,600,600); // path v mass
  pathMass->DrawCopy();



  
  //increase dca cut may reduce background more than signal


  // TH1D *invariant_mass = new TH1D("invariant_mass","Invariant Mass",200,0.47,0.53);
  // ntuple->Draw("invariant_mass>>invariant_mass",cut);
  // invariant_mass->DrawCopy();

  // TCanvas *c2 = new TCanvas("c2","",10,10,600,600);
  // TH2D *invariants = new TH2D("invariants","Invariant mass v. Invariant pt",5000,0,7,5000,0,5);
  // ntuple->Draw("invariant_mass:invariant_pt>>invariants",cut);
  // invariants->DrawCopy();

  // TH2D *pseudoInv= new TH2D("pseudoInv","Invariant Mass and pseudorapidity",5000,0,3,5000,0,7);
  // ntuple->Draw("invariant_mass:pseudorapidity>>pseudoInv",cut);
  // pseudoInv->DrawCopy()    ;

  // TH2D *pairRapidity = new TH2D("pairRapidity","Pair DCA and pseudorapidity",5000,-2,2,5000,-0.06,0.06);
  // ntuple->Draw("pair_dca:pseudorapidity>>pairRapidity",cut);
  // pairRapidity->DrawCopy();

  /*
  TCanvas *c3 = new TCanvas("c3","",10,10,600,600);
  TH2D *pathMass = new TH2D("pathMass","Invariant Mass vs. Path Length",5000,0.0,5.0,5000,0.4,0.6);
  ntuple->Draw("K0_mass:K0_decayLength>>pathMass",cut);
  pathMass->DrawCopy();

  TCanvas *c4 = new TCanvas("c4","",10,10,600,600);
  TH2D *radiusMass = new TH2D("radiusMass","Invariant Mass vs. Radius",5000,0.0,5.0,5000,0.4,0.6);
  ntuple->Draw("K0_mass:sqrt(pow(K0_x,2) + pow(K0_y,2))>>radiusMass",cut);
  radiusMass->DrawCopy();

  TCanvas *c5 = new TCanvas("c5","",10,10,600,600);
  TH2D *pathlengthpathlengthz = new TH2D("pathlengthpathlengthz","path v. pathlength_z",5000,-20.0,20.0,5000,0.0,10.0);
  ntuple->Draw("K0_decayLength:(K0_z-primary_vertex_z)>>pathlengthpathlengthz",cut+kshort_cut);
  pathlengthpathlengthz->DrawCopy();

  TCanvas *c6 = new TCanvas("c6","",10,10,600,600);
  TH2D *pathzRadius = new TH2D("pathzRadius","path v. radius",5000,-20.0,20.0,5000,0.0,10.0);
  ntuple->Draw("sqrt(pow(K0_x,2)+pow(K0_y,2)):(K0_z-primary_vertex_z)>>pathzRadius",cut+kshort_cut);
  pathzRadius->DrawCopy();
  */






  // TH2D *phiMass = new TH2D("phiMass","Phi vs. Invariant Mass", 5000,-10,10,5000,-10,10 );
  // ntuple->Draw("atan2((projected_pos1_y+projected_pos2_y)/2,(projected_pos1_x+projected_pos2_x)/2):invariant_mass");
  // phiMass->DrawCopy();

    


  // double E1 = sqrt(pow(mom1(0),2) + pow(mom1(1),2) + pow(mom1(2),2) + pow(decaymass,2));
  // double E2 = sqrt(pow(mom2(0),2) + pow(mom2(1),2) + pow(mom2(2),2) + pow(decaymass,2));

  // TLorentzVector v1 (mom1(0), mom1(1), mom1(2), E1);
  // TLorentzVector v2 (mom2(0), mom2(1), mom2(2), E2);

  // TLorentzVector tsum;
  // tsum = v1 + v2;

  // rapidity       = tsum.Rapidity();
  // pseudorapidity = tsum.Eta();
  // invariantMass  = tsum.M();
  // invariantPt    = tsum.Pt();




  //ntp_reco_info->Draw("atan2((projected_pos1_y+projected_pos2_y)/2,(projected_pos1_x+projected_pos2_x)/2):invariant_mass")
  //decay_length->DrawCopy();


  // TH2D *position = new TH2D("position","",5000,-10,10,5000,0,10);
  // ntuple->Draw("sqrt( ((x1+x2)/2) * ((x1+x2)/2)+((y1+y2)/2) * ((y1+y2)/2)) : ((z1+z2)/2)>>position" );
  
  // position->DrawCopy();




}
