#include <TFile.h>
#include <TTree.h>
#include <TArc.h>
#include <TLine.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TH3.h>
#include <TCanvas.h>
#include <TVirtualPad.h>
#include <TMath.h>
#include <TStyle.h>

#include <iostream>
#include <vector>


TH3F     *frame3d = nullptr;
TGraph2D *gr2_xyz = nullptr;

void calcTrajectoryXY(
        float trk_x, float trk_y, float trk_pt, float trk_phi, int trk_c,
        float& R, float& cx, float& cy, float& arcphi0, float& arcphi1)
{

  const float B = 1.4; // Tesla
  const float C = 0.299792458; // m/ns
  /////////////////////////////

  float px = trk_pt*std::cos(trk_phi);
  float py = trk_pt*std::sin(trk_phi);


  R = trk_pt/(C*B) * 100.; // p=0.3BR in B field,  0.3 from C (speed of light)
                           // 100 to convert m to cm

  cx = R * ( py/trk_pt)*trk_c + trk_x;
  cy = R * (-px/trk_pt)*trk_c + trk_y;

  // phi angle calculation
  float rx = trk_x - cx; // circle origin to trackpoint
  float ry = trk_y - cy;
  float rv_x_pv = rx*py - ry*px; // direction of track relative to circle vector

  const float pi = TMath::Pi();

  //if(R>75.) 
  { 
    float arcphi = atan2(ry, rx);  // -pi ~ pi

    if(rv_x_pv<0.) { arcphi0 = arcphi-pi; arcphi1 = arcphi;}
    else           { arcphi0 = arcphi;    arcphi1 = arcphi+pi;}
  }
}

void drawDisplay(int eventnumber=10)
{
 //std::string filepath = "/sphenix/user/jaein213/tracking/SiliconSeeding/MC/macro/ana/jpsi/ana_Allpart.root";
 //std::string filepath = "/sphenix/user/jaein213/tracking/SiliconSeeding/MC/macro/ana/jpsi/ana_all.root";
 //std::string filepath = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/MC/ana_e-/merged_10k.root";
 //std::string filepath = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/MC/ana/ana_0_1kevt.root";
 //std::string filepath = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/MC/ana/merged_100k.root";
 //std::string filepath = "/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/ana_pythia_old/ana_addall.root";
 //std::string filepath = "/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/ana/all_pythia.root";
 //std::string filepath = "/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/ana/all_pythia_old.root";
 //std::string filepath = "../macro/ana_0.root";
 //std::string filepath = "/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/SiliconSeeding/macro/data/test_ana.root";
 std::string filepath = "/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/ana_em/all_em.root";

/*
  // Load ROOT libraries if not already loaded
  gSystem->Load("libTree");
  gSystem->Load("libGraf");
  gSystem->Load("libHist");
*/

  // Open the ROOT file
  TFile* file = TFile::Open(filepath.c_str(), "READ");

  // Get the trackTree and caloTree
  TTree* trackTree = (TTree*)file->Get("trackTree");
  TTree* caloTree  = (TTree*)file->Get("caloTree");
  //TTree* siClusTree= (TTree*)file->Get("SiClusTree");
  TTree* siClusTree= (TTree*)file->Get("SiClusAllTree");
  TTree* truthTree = (TTree*)file->Get("truthTree");

  if (!trackTree) {
    printf("Error: trackTree not found in file.\n");
    file->Close();
    return;
  }
  if (!siClusTree) {
    printf("Error: SiclusTree not found in file.\n");
    file->Close();
    return;
  }
  if (!caloTree) {
    printf("Error: caloTree not found in file.\n");
    file->Close();
    return;
  }
  if (!truthTree) {
    printf("No truthTree ound in file.\n");
  }

  // Set up branches for trackcluster_x, trackcluster_y, trackcluster_z, trackcluster_trackid, evt
  std::vector<float>* trackcluster_x = nullptr;
  std::vector<float>* trackcluster_y = nullptr;
  std::vector<float>* trackcluster_z = nullptr;
  std::vector<int>*   trackcluster_trackid = nullptr;

  std::vector<float>* track_x = nullptr;
  std::vector<float>* track_y = nullptr;
  std::vector<float>* track_z = nullptr;
  std::vector<float>* track_x_emc = nullptr;
  std::vector<float>* track_y_emc = nullptr;
  std::vector<float>* track_z_emc = nullptr;
  std::vector<float>* track_phi = nullptr;
  std::vector<float>* track_eta = nullptr;
  std::vector<float>* track_pt = nullptr;
  std::vector<int>*   track_charge = nullptr;

  int evt = -1;
  siClusTree->SetBranchAddress("Siclus_x", &trackcluster_x);
  siClusTree->SetBranchAddress("Siclus_y", &trackcluster_y);
  siClusTree->SetBranchAddress("Siclus_z", &trackcluster_z);
  siClusTree->SetBranchAddress("Siclus_trackid", &trackcluster_trackid);

  trackTree->SetBranchAddress("evt",    &evt);
  trackTree->SetBranchAddress("x0",     &track_x);
  trackTree->SetBranchAddress("y0",     &track_y);
  trackTree->SetBranchAddress("z0",     &track_z);
  trackTree->SetBranchAddress("phi0",   &track_phi);
  trackTree->SetBranchAddress("eta0",   &track_eta);
  trackTree->SetBranchAddress("pt0",    &track_pt);
  trackTree->SetBranchAddress("charge", &track_charge);
  // Add branch addresses for EMCAL state and track parameters
  trackTree->SetBranchAddress("x_proj_emc", &track_x_emc);
  trackTree->SetBranchAddress("y_proj_emc", &track_y_emc);
  trackTree->SetBranchAddress("z_proj_emc", &track_z_emc);

  // Set up branches for caloTree (calo_x, calo_y, calo_z, calo_evt)
  std::vector<float>* calo_x = nullptr;
  std::vector<float>* calo_y = nullptr;
  std::vector<float>* calo_z = nullptr;
  int calo_evt = -1;
  caloTree->SetBranchAddress("calo_evt", &calo_evt);
  caloTree->SetBranchAddress("x", &calo_x);
  caloTree->SetBranchAddress("y", &calo_y);
  caloTree->SetBranchAddress("z", &calo_z);

  // setup for truthTree
  std::vector<float>* truth_pt=0, *truth_eta=0, *truth_phi=0;
  std::vector<float>* truth_px=0, *truth_py=0, *truth_pz=0, *truth_e=0;
  std::vector<int>*  truth_pid=0;
  std::vector<int>*  truth_vtxid=0;
  std::vector<float>* truth_vtx_x=0, *truth_vtx_y=0, *truth_vtx_z=0; // indix vtxid, not trackid
  if(truthTree){
    truthTree->SetBranchAddress("truth_pid", &truth_pid);
    truthTree->SetBranchAddress("truth_pt", &truth_pt);
    truthTree->SetBranchAddress("truth_phi",&truth_phi);
    truthTree->SetBranchAddress("truth_px", &truth_px);
    truthTree->SetBranchAddress("truth_py", &truth_py);
    truthTree->SetBranchAddress("truth_pz", &truth_pz);
    truthTree->SetBranchAddress("truth_vtxid", &truth_vtxid);
    truthTree->SetBranchAddress("truth_vtx_x", &truth_vtx_x);
    truthTree->SetBranchAddress("truth_vtx_y", &truth_vtx_y);
    truthTree->SetBranchAddress("truth_vtx_z", &truth_vtx_z);
  }
 
  

  // Find the entry for the given eventnumber in trackTree
  Long64_t nentries = trackTree->GetEntries();

  bool found = false;
  std::vector<float> xs_track, ys_track;
  // For rz projection
  //std::vector<float> rz_trackcluster_z, rz_trackcluster_r;
  std::vector<float> rz_track_z, rz_track_r;
  std::vector<float> rz_track_z_emc, rz_track_r_emc;
  // For trajectory drawing
  //--std::vector<std::vector<float>> track_traj_xs;
  //--std::vector<std::vector<float>> track_traj_ys;
  //--std::vector<std::vector<float>> track_traj_zs;
  //--std::vector<std::vector<float>> track_traj_rs;


  TGraph* gr = NULL;
  TGraph* gr_rz = NULL;
  TGraph* gr_calo = NULL;
  TGraph* gr_rz_calo = NULL;
  TGraph* gr_phiz_calo = NULL;
  TGraph* gr_phiz_pemc = NULL;
  std::vector<TArc*> v_trk_xy;
  std::vector<TLine*> v_trk_zr;

  std::vector<TArc*> v_tr_xy;
  std::vector<TLine*> v_tr_zr;

  // frame constants
  const float si_cr[5] = {2.4, 3.2, 3.9, 7.4, 10.};
  const float si_cz[5] = {10,10,10,23.5, 23.5};
  const float calo_cr[1] = {93.5};
  const float calo_cz[1] = {120.};
  ///////////////////////////
  std::vector<TLine*>    v_line_rz;
  // Si RZ-frame
  v_line_rz.push_back(new TLine(-si_cz[0],  si_cr[0], si_cz[0],  si_cr[0]));
  v_line_rz.push_back(new TLine(-si_cz[1],  si_cr[1], si_cz[1],  si_cr[1]));
  v_line_rz.push_back(new TLine(-si_cz[2],  si_cr[2], si_cz[2],  si_cr[2]));
  v_line_rz.push_back(new TLine(-si_cz[3],  si_cr[3], si_cz[3],  si_cr[3]));
  v_line_rz.push_back(new TLine(-si_cz[4],  si_cr[4], si_cz[4],  si_cr[4]));
  v_line_rz.push_back(new TLine(-si_cz[0], -si_cr[0], si_cz[0], -si_cr[0]));
  v_line_rz.push_back(new TLine(-si_cz[1], -si_cr[1], si_cz[1], -si_cr[1]));
  v_line_rz.push_back(new TLine(-si_cz[2], -si_cr[2], si_cz[2], -si_cr[2]));
  v_line_rz.push_back(new TLine(-si_cz[3], -si_cr[3], si_cz[3], -si_cr[3]));
  v_line_rz.push_back(new TLine(-si_cz[4], -si_cr[4], si_cz[4], -si_cr[4]));
  // Calo RZ-frame
  v_line_rz.push_back(new TLine(-calo_cz[0], calo_cr[0], calo_cz[0], calo_cr[0]));
  v_line_rz.push_back(new TLine(-calo_cz[0],-calo_cr[0], calo_cz[0],-calo_cr[0]));


  std::vector<TEllipse*> v_elli_xy;
  // Si + calo X-Y frame
  v_elli_xy.push_back(new TEllipse(0,0, si_cr[0]));
  v_elli_xy.push_back(new TEllipse(0,0, si_cr[1]));
  v_elli_xy.push_back(new TEllipse(0,0, si_cr[2]));
  v_elli_xy.push_back(new TEllipse(0,0, si_cr[3]));
  v_elli_xy.push_back(new TEllipse(0,0, si_cr[4]));
  v_elli_xy.push_back(new TEllipse(0,0, calo_cr[0]));

  for (size_t il = 0; il < v_elli_xy.size(); ++il) {
    v_elli_xy[il]->SetFillStyle(0);
  }

  frame3d = new TH3F("frame3d","frame3d",1,-30, 30,1,-15, 15,1,-18, 18);  

  gStyle->SetOptStat(0);

  char input[256];
  TCanvas* can = new TCanvas("can", "Event Display XY", 1400, 700);
  can->Divide(2,1);
  TVirtualPad *p1 = can->cd(1);
  p1->Divide(2,2);

  ////////////////
  for (Long64_t i = 0; i < nentries; ++i) {
    if(truthTree) {
      truthTree->GetEntry(i);
      
      if(truth_vtx_z->size()>0 ) {
        std::cout<<"Truth Zvtx : "<<truth_vtx_z->at(0);
        if(fabs(truth_vtx_z->at(0))>5) {
          std::cout<<" : skipped"<<std::endl;
          continue;
        }
        std::cout<<" : good vtx"<<std::endl;
      }
      else {
        std::cout<<" : vtx size=0"<<std::endl;
      }

      // clear truth track vector
      for(size_t iarc=0; iarc<v_tr_xy.size(); iarc++){
        delete v_tr_xy[iarc];
        delete v_tr_zr[iarc];
      }
      v_tr_xy.clear();
      v_tr_zr.clear();

      bool ev_skip=false;
      std::cout<<"truth N: "<<truth_pid->size()<<std::endl;
      for (size_t k = 0; k < truth_pid->size(); ++k) {

        int   tr_vid = truth_vtxid->at(k)-1;
        float tr_x   = -999;
        float tr_y   = -999;
        float tr_z   = -999;
        if(0<=tr_vid&&tr_vid<truth_vtx_x->size()){
          tr_x = truth_vtx_x->at(tr_vid);
          tr_y = truth_vtx_y->at(tr_vid);
          tr_z = truth_vtx_z->at(tr_vid);
        }
        float& tr_px = truth_px->at(k);
        float& tr_py = truth_py->at(k);
        float& tr_pz = truth_pz->at(k);

        float tr_phi = truth_phi->at(k); //atan2(tr_py, tr_px);
        float tr_pt  = truth_pt->at(k); //sqrt(tr_px*tr_px + tr_py*tr_py);
        //float tr_eta = track_eta->at(k);
        int&   tr_pid = truth_pid->at(k);
        int    tr_c   = tr_pid<0 ? 1 : -1;
        float  tr_the = atan2(tr_pt, tr_pz);

        if(tr_pt>0.5) {
          cout<<"pt_excced : "<<tr_pt<<endl;
          ev_skip=true;
          break;
          //continue;
        }

        ////////////////////////
        // X-Y trajectory
        float tR, tcx, tcy, tarcphi0, tarcphi1;
        calcTrajectoryXY(tr_x, tr_y, tr_pt, tr_phi, tr_c,
                         tR, tcx, tcy, tarcphi0, tarcphi1);


         
        
        const float conv_ang = 180./TMath::Pi();
        float tarcphi0_ang = tarcphi0*conv_ang; 
        float tarcphi1_ang = tarcphi1*conv_ang; 
        
        //float phi_ang     = trk_phi*conv_ang; if(phi_ang<0) phi_ang += 360.;
        std::cout<<"px, py, R = "<<tr_pt<<" "<<tr_phi<<" "<<tR<<" "<<tcx<<" "<<tcy<<",   "
                 <<tarcphi0_ang<<" "<<tarcphi1_ang<<" "<<" "<<tr_x<<" "<<tr_y<<std::endl;
        std::cout<<" = "<<tr_vid<<" "<<tr_x<<" "<<tr_y<<" "<<tr_z<<std::endl;



        TArc *tr_xy = new TArc(tcx, tcy, tR, tarcphi0_ang, tarcphi1_ang); //, ang0*180/TMath::Pi(), ang1*180/TMath::Pi());
        tr_xy->SetFillStyle(20);
        tr_xy->SetLineStyle(5);
        tr_xy->SetLineWidth(1);
        tr_xy->SetLineColor(2);
        v_tr_xy.push_back(tr_xy);
        
        ////////////////////////
        // R-Z trajectory
        float r_end = 100.;

        float tr_r = std::sqrt(tr_x*tr_x+tr_y*tr_y);

        float slope = std::tan(tr_the);
        float z_end = (r_end-tr_r)/slope;

        //float rsign = (trk_yemc>0) ? 1 : -1;
        float rsign = (0<=tr_phi&&tr_phi<TMath::Pi()) ? 1 : -1;

        TLine *l = new TLine(tr_z, tr_r, z_end, rsign*r_end);
        l->SetLineStyle(5);
        l->SetLineColor(2);
        v_tr_zr.push_back(l);
      }
      if(ev_skip) continue;
    }


    trackTree->GetEntry(i);
    caloTree->GetEntry(i);
    siClusTree->GetEntry(i);

    

    ////////////////////////
    // clear vector
    for(size_t iarc=0; iarc<v_trk_xy.size(); iarc++){
      delete v_trk_xy[iarc];
      delete v_trk_zr[iarc];
    }
    v_trk_xy.clear();
    v_trk_zr.clear();


    ////////////////////////
    //--if (evt != eventnumber) continue;

    // SiSeed Hits
    std::vector<float> xs, ys, zs, rs; // cluster
    // Append all clusters for this event
    if (trackcluster_x && trackcluster_y && trackcluster_z) {
      for (size_t j = 0; j < trackcluster_x->size() && j < trackcluster_y->size() && j < trackcluster_z->size(); ++j) {
        xs.push_back(trackcluster_x->at(j));
        ys.push_back(trackcluster_y->at(j));
        zs.push_back(trackcluster_z->at(j));

        float sign = (trackcluster_y->at(j)>0)?1:-1;
        float r = sign* std::sqrt(trackcluster_x->at(j)*trackcluster_x->at(j) + trackcluster_y->at(j)*trackcluster_y->at(j));
        rs.push_back(r);
      }
    }

    // calo Clusters
    std::vector<float> xs_calo, ys_calo, zs_calo, rs_calo, phis_calo;
    if (calo_x && calo_y && calo_z) {
      for (size_t j = 0; j < calo_x->size() && j < calo_y->size() && j < calo_z->size(); ++j) {
        xs_calo.push_back(calo_x->at(j));
        ys_calo.push_back(calo_y->at(j));
        zs_calo.push_back(calo_z->at(j));
        phis_calo.push_back(atan2(calo_y->at(j), calo_x->at(j)));

        float sign = (calo_y->at(j)>0)?1:-1;
        float r = sign*std::sqrt(calo_x->at(j)*calo_x->at(j) + calo_y->at(j)*calo_y->at(j));
        rs_calo.push_back(r);
        
        //// Update rz_track_z_emc and rz_track_r_emc from caloTree
        //rz_track_z_emc.push_back(calo_z->at(j));
        //rz_track_r_emc.push_back(r);
      }
    }

    // track trajectory
    // Store track projection to EMC  for this event (one per track)
    std::vector<float> xs_track_pemc, ys_track_pemc, zs_track_pemc, phis_track_pemc;
    if (track_x_emc && track_y_emc && track_z_emc) {
      for (size_t j = 0; j < track_x_emc->size() && j < track_y_emc->size() && j < track_z_emc->size(); ++j) {
        xs_track_pemc.push_back(track_x_emc->at(j));
        ys_track_pemc.push_back(track_y_emc->at(j));
        zs_track_pemc.push_back(track_z_emc->at(j));

        float phi_pemc = atan2(track_y_emc->at(j), track_x_emc->at(j));
        phis_track_pemc.push_back( phi_pemc );

        if (calo_x && calo_y && calo_z) {
          for (size_t k = 0; k < calo_x->size() && k < calo_y->size(); ++k) {
            float phi_emc = atan2(calo_y->at(k), calo_x->at(k));
          }
        }
      }
    }
    
    // to show the track trajectories by using track parameters (track_x, track_y, track_z, pt, phi, eta, and field B=1.4T)
    std::vector<float> cxs_trackarc, cys_trackarc, crs_trackarc, crs_matchflag;
    if (track_x && track_y && track_z && track_phi && track_eta && track_pt) {
      size_t ntracks = std::min({track_x->size(), track_y->size(), track_z->size(), 
                                 track_pt->size()});

      for (size_t j = 0; j < ntracks; ++j) {
        float& trk_x   = track_x->at(j);
        float& trk_y   = track_y->at(j);
        float& trk_z   = track_z->at(j);
        float& trk_phi = track_phi->at(j);
        float& trk_eta = track_eta->at(j);
        float& trk_pt  = track_pt->at(j);
        int&   trk_c   = track_charge->at(j);
        float  trk_the = 2.*std::atan( std::exp(-trk_eta) );

        float& trk_yemc= (track_y_emc->at(j));

        ////////////////////////
        // X-Y trajectory
        float R, cx, cy, arcphi0, arcphi1;
        calcTrajectoryXY(trk_x, trk_y, trk_pt, trk_phi, trk_c,
                         R, cx, cy, arcphi0, arcphi1);


         
        
        const float conv_ang = 180./TMath::Pi();
        float arcphi0_ang = arcphi0*conv_ang; 
        float arcphi1_ang = arcphi1*conv_ang; 
        
        float phi_ang     = trk_phi*conv_ang; if(phi_ang<0) phi_ang += 360.;
        std::cout<<"px, py, R = "<<trk_pt<<" "<<trk_phi<<" "<<R<<" "<<cx<<" "<<cy<<",   "
                 <<phi_ang<<" "<<arcphi0_ang<<" "<<arcphi1_ang<<" "<<trk_x<<" "<<trk_y<<std::endl;



        TArc *trk_xy = new TArc(cx, cy, R, arcphi0_ang, arcphi1_ang); //, ang0*180/TMath::Pi(), ang1*180/TMath::Pi());
        trk_xy->SetFillStyle(20);
        trk_xy->SetLineWidth(1);
        trk_xy->SetLineColor(1);
        v_trk_xy.push_back(trk_xy);

        cxs_trackarc.push_back(cx);
        cys_trackarc.push_back(cy);
        crs_trackarc.push_back(R);
        
        ////////////////////////
        // R-Z trajectory
        float r_end = 100.;

        float trk_r = std::sqrt(trk_x*trk_x+trk_y*trk_y);

        float slope = std::tan(trk_the);
        float z_end = (r_end-trk_r)/slope;

        //float rsign = (trk_yemc>0) ? 1 : -1;
        float rsign = (0<=trk_phi&&trk_phi<TMath::Pi()) ? 1 : -1;

        TLine *l = new TLine(trk_z, trk_r, z_end, rsign*r_end);
        v_trk_zr.push_back(l);

        ////////////////////////////
        // check the association
        float& z_pemc   = track_z_emc->at(j);
        float  phi_pemc = atan2(track_y_emc->at(j), track_x_emc->at(j));
        phis_track_pemc.push_back( phi_pemc );

        uint flag=0;
        if (calo_x && calo_y && calo_z) {
          for (size_t k = 0; k < calo_x->size() && k < calo_y->size(); ++k) {
            float& z_emc   = calo_z->at(k);
            float  dz      = z_emc - z_pemc;
            float  phi_emc = atan2(calo_y->at(k), calo_x->at(k));
            float  dphi = phi_emc - phi_pemc;

            if( fabs(dz)<5){
              float resi = (trk_c * dphi) - (0.18*TMath::Power(trk_pt, -0.986));
              float resi2= (trk_c * dphi) - (0.67*TMath::Power(trk_pt, -0.986));
              if( fabs( (trk_c * dphi) - (0.18*TMath::Power(trk_pt, -0.986)) ) < 0.1 ) { flag|=1; }
              if( fabs( (trk_c * dphi) - (0.67*TMath::Power(trk_pt, -0.986)) ) < 0.1 ) { flag|=2; }
              std::cout<<"dphi : "<<dphi<<" "<<trk_pt<<" "<<resi<<" "<<resi2<<std::endl;
            }
          }
        }
        crs_matchflag.push_back(flag);
        std::cout<<"flag : "<<flag<<std::endl;

        if(flag>0) {
          trk_xy->SetLineColor(flag+5);
          l->SetLineColor(flag+5);
        }
      }
    }


    ///////////////////////
    // draw SiSeed Hits
    if(gr!=nullptr){ delete gr; }
    gr = new TGraph(xs.size(), &xs[0], &ys[0]);
    gr->SetTitle(Form("Track Clusters X vs Y for Event %d;X [cm];Y [cm]", eventnumber));
    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1.0);
    gr->SetMarkerColor(kBlue+1);

    if(gr_rz!=nullptr){ delete gr_rz; }
    gr_rz = new TGraph(zs.size(), &zs[0], &rs[0]);
    gr_rz->SetTitle(Form("Track Clusters R vs Z for Event %d;X [cm];Y [cm]", eventnumber));
    gr_rz->SetMarkerStyle(20);
    gr_rz->SetMarkerSize(1.0);
    gr_rz->SetMarkerColor(kBlue+1);

    if(gr2_xyz!=nullptr){ delete gr2_xyz; gr2_xyz = nullptr; }
    if(zs.size()>0){
      gr2_xyz = new TGraph2D(zs.size(), &zs[0], &xs[0], &ys[0]);
      gr2_xyz->SetTitle(Form("Track Clusters X, Y, Z for Event %d;X [cm];Y [cm]", eventnumber));
      gr2_xyz->SetMarkerStyle(20);
      gr2_xyz->SetMarkerSize(1.0);
      gr2_xyz->SetMarkerColor(kBlue+1);
    }

  
    // Draw EMCal hits if available
    if(gr_calo!=nullptr){ delete gr_calo; }
    gr_calo = new TGraph(xs_calo.size(), &xs_calo[0], &ys_calo[0]);
    gr_calo->SetMarkerStyle(20); // star marker
    gr_calo->SetMarkerSize(1.0);
    gr_calo->SetMarkerColor(kRed+1);

    if(gr_rz_calo!=nullptr){ delete gr_rz_calo; }
    gr_rz_calo = new TGraph(zs_calo.size(), &zs_calo[0], &rs_calo[0]);
    gr_rz_calo->SetMarkerStyle(20); // star marker
    gr_rz_calo->SetMarkerSize(1.0);
    gr_rz_calo->SetMarkerColor(kRed+1);

    if(gr_phiz_calo!=nullptr){ delete gr_phiz_calo; }
    gr_phiz_calo = new TGraph(zs_calo.size(), &zs_calo[0], &phis_calo[0]);
    gr_phiz_calo->SetMarkerStyle(20); // star marker
    gr_phiz_calo->SetMarkerSize(1.0);
    gr_phiz_calo->SetMarkerColor(kRed+1);

    if(gr_phiz_pemc!=nullptr){ delete gr_phiz_pemc; }
    gr_phiz_pemc = new TGraph(zs_track_pemc.size(), &zs_track_pemc[0], &phis_track_pemc[0]);
    gr_phiz_pemc->SetMarkerStyle(24); // star marker
    gr_phiz_pemc->SetMarkerSize(1.2);
    gr_phiz_pemc->SetMarkerColor(kBlue+1);

    //trajectory

    std::cout<<" Nsi : "<<xs.size()
             <<", Ncalo : "<<xs_calo.size()
             <<", Ntrk : "<<v_trk_xy.size()
             <<", Ntr : "<<v_tr_xy.size()
             <<std::endl;

    p1->cd(1);
    //frm1->Draw();
    gPad->DrawFrame(-15,-15,15,15);
    for (size_t il = 0; il < v_elli_xy.size()-1; ++il) {
      v_elli_xy[il]->Draw();
      //std::cout<<il<<" "<<v_elli_xy[il]->GetR1()<<std::endl;
    }
    if(gr->GetN()>0) gr->Draw("P");
    for(size_t iarc=0; iarc<v_trk_xy.size(); iarc++){
      v_trk_xy[iarc]->Draw("only");
    }
    for(size_t iarc=0; iarc<v_tr_xy.size(); iarc++){
      std::cout<<"aa"<<std::endl;
      v_tr_xy[iarc]->Draw("only");
    }

    p1->cd(2);
    //frm1->Draw();
    gPad->DrawFrame(-28,-15,28,15);
    for (size_t il = 0; il < v_line_rz.size()-2; ++il) {
      v_line_rz[il]->Draw();
    }
    if(gr->GetN()>0) gr_rz->Draw("P");
    for(size_t iarc=0; iarc<v_trk_zr.size(); iarc++){
      v_trk_zr[iarc]->Draw();
    }
    for(size_t iarc=0; iarc<v_tr_zr.size(); iarc++){
      v_tr_zr[iarc]->Draw();
    }

    p1->cd(3);
    //frm1->Draw();
    gPad->DrawFrame(-100,-100,100,100);
    for (size_t il = 0; il < v_elli_xy.size(); ++il) {
      v_elli_xy[il]->Draw();
    }
    if(gr->GetN()     >0) gr->Draw("P");
    if(gr_calo->GetN()>0) gr_calo->Draw("P");
    for(size_t iarc=0; iarc<v_trk_xy.size(); iarc++){
      v_trk_xy[iarc]->Draw("only");
    }
    for(size_t iarc=0; iarc<v_tr_xy.size(); iarc++){
      v_tr_xy[iarc]->Draw("only");
    }

    p1->cd(4);
    //frm1->Draw();
    gPad->DrawFrame(-150,-100,150,100);
    for (size_t il = 0; il < v_line_rz.size(); ++il) {
      v_line_rz[il]->Draw();
    }
    if(gr_rz->GetN()     >0) gr_rz->Draw("P");
    if(gr_rz_calo->GetN()>0) gr_rz_calo->Draw("P");
    for(size_t iarc=0; iarc<v_trk_zr.size(); iarc++){
      v_trk_zr[iarc]->Draw();
    }
    for(size_t iarc=0; iarc<v_tr_zr.size(); iarc++){
      v_tr_zr[iarc]->Draw();
    }
  
  
    can->cd(2);
    gPad->DrawFrame(-150, -3.2, 150, 3.2);
    gr_phiz_calo->Draw("P");
    gr_phiz_pemc->Draw("P");
    //--frame3d->Draw();
    //--if(gr2_xyz!=nullptr) {
    //--  //gr2_xyz->GetXaxis()->SetRangeUser(-30,30);
    //--  //gr2_xyz->GetYaxis()->SetRangeUser(-15,15);
    //--  //gr2_xyz->GetZaxis()->SetRangeUser(-15,15);
    //--  //gr2_xyz->Draw("Psame");
    //--  gr2_xyz->Draw("sameP");
    //--}
 

    can->Modified();
    can->Update();

    std::cin.getline(input, 256);
    if (input[0] == 'q' || input[0] == 'Q') {
      break;
    }

  }


  //can->Draw();
  //can->Update();
  file->Close();
}

/*
void LoopEventDisplay()
{
  int eventnumber = 0;
  char input[256];
  while (true) {
    DrawEventDisplay(eventnumber);
    printf("Press Enter to view next event, or 'q' then Enter to quit: ");
    std::cin.getline(input, 256);
    if (input[0] == 'q' || input[0] == 'Q') {
      break;
    }
    eventnumber++;
  }
}
*/
