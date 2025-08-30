#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>

#include <TLorentzVector.h>

#include <cmath>
#include <string>
#include <vector>

const float electron_mass = 0.000511; // GeV/c^2
const float pion_mass     = 0.1396; // GeV/c^2

void analyze_SiSeedPair(
     //const std::string& filename="/sphenix/user/jaein213/tracking/SiliconSeeding/MC/macro/ana/jpsi/ana_all.root", 
//     const std::string& filename="/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/MC/ana_jpsi/merged_200k.root", 
//     //const std::string& filename="/sphenix/user/jaein213/tracking/SiliconSeeding/MC/macro/ana/e+/inner_ana_0601_all.root", 
//     const std::string& filename="/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/MC/ana_e-/merged_10k.root", 
//     const std::string& filename="/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/MC/ana/ana_0_1kevt.root", 
//     const std::string& filename="/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/MC/ana/merged_100k.root", // pythia
     const std::string& filename="/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/ana_em/all_em.root", // pythia
//     const std::string& filename="/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/ana_ep/all_ep.root", // pythia
//     const std::string& filename="/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/ana_jpsi/all_jpsi.root", // pythia
//     const std::string& filename="/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/SiliconSeeding/macro/data/test_ana2k.root", // pythia
     //const std::string& filename="/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/SiliconSeeding/macro/data/test_ana5k.root", // pythia
     //const std::string& filename="/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/SiliconSeeding/macro/data/test_ana10k.root", // pythia
     //const std::string& filename="/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/SiliconSeeding/macro/data/test_ana50k.root", // pythia
     //const std::string& filename="/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/SiliconSeeding/macro/data/test_ana.root", // pythia
     //const std::string& filename="/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/ana_pythia/ana_addall.root",
     //const std::string& filename="/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/ana/all_pythia.root",
     float phi_threshold = 0.05
                       )
{
  TFile *f = TFile::Open(filename.c_str(), "READ");

  TTree *trackTree = (TTree *)f->Get("trackTree");
  TTree *caloTree  = (TTree *)f->Get("caloTree");
  TTree *siClusTree  = (TTree *)f->Get("SiClusTree");
  TTree *truthTree = (TTree *)f->Get("truthTree");

  int evt;
  std::vector<float> *track_phi    = 0, *track_pt = 0, *track_pz = 0, *track_eta = 0, *track_z = 0;
  std::vector<float> *track_chi2ndf= 0;
  std::vector<int>   *track_charge = 0, *track_nmaps = 0, *track_nintt = 0;
  std::vector<float> *track_phi_emc= 0, *track_z_emc = 0;

  int calo_evt;
  std::vector<float> *calo_phi = 0, *calo_energy = 0;
  std::vector<float> *calo_x = 0, *calo_y = 0, *calo_z = 0;
  std::vector<float> *calo_chi2 = 0, *calo_prob = 0;


  std::vector<float>* truth_pt=0, *truth_eta=0, *truth_phi=0;
  std::vector<float> *truth_pz = 0;
  //std::vector<float>* truth_px, *truth_py, *truth_pz, *truth_e;

  // SiClus
  std::vector<int> *SiClus_trackid=0;
  std::vector<int> *SiClus_layer=0;
  std::vector<float> *SiClus_x=0;
  std::vector<float> *SiClus_y=0;
  std::vector<float> *SiClus_z=0;



  trackTree->SetBranchAddress("evt",     &evt);
  trackTree->SetBranchAddress("chi2ndf", &track_chi2ndf);
  trackTree->SetBranchAddress("charge",  &track_charge);
  trackTree->SetBranchAddress("nmaps",   &track_nmaps);
  trackTree->SetBranchAddress("nintt",   &track_nintt);
  trackTree->SetBranchAddress("pt0",     &track_pt);
  trackTree->SetBranchAddress("pz0",     &track_pz);
  trackTree->SetBranchAddress("eta0",    &track_eta);
  trackTree->SetBranchAddress("phi0",    &track_phi);
  trackTree->SetBranchAddress("z0",      &track_z);
  trackTree->SetBranchAddress("phi_proj_emc", &track_phi_emc);
  trackTree->SetBranchAddress("z_proj_emc",   &track_z_emc);
/*
  trackTree->SetBranchAddress("pt0",      &track_pt);
  trackTree->SetBranchAddress("eta0",     &track_eta);
  trackTree->SetBranchAddress("phi0",     &track_phi);
  trackTree->SetBranchAddress("z0",       &track_z);
  trackTree->SetBranchAddress("phi_proj_emc", &track_phi_emc);
  trackTree->SetBranchAddress("z_proj_emc",   &track_z_emc);
*/

///////////
   // Truth tree and branches
  if(truthTree!=nullptr){
    truthTree->SetBranchAddress("truth_pt",  &truth_pt);
    truthTree->SetBranchAddress("truth_pz",  &truth_pz);
    truthTree->SetBranchAddress("truth_eta", &truth_eta);
    truthTree->SetBranchAddress("truth_phi", &truth_phi);
  }

  //--truthTree->SetBranchAddress("truth_pid", &truth_pid);
  //--truthTree->SetBranchAddress("truth_px",  &truth_px);
  //--truthTree->SetBranchAddress("truth_py",  &truth_py);
  //--truthTree->SetBranchAddress("truth_e",   &truth_e);
  //--truthTree->SetBranchAddress("truth_pt",  &truth_pt);

///////////
  siClusTree->SetBranchAddress("Siclus_trackid", &SiClus_trackid);
  siClusTree->SetBranchAddress("Siclus_layer",   &SiClus_layer);
  siClusTree->SetBranchAddress("Siclus_x",       &SiClus_x);
  siClusTree->SetBranchAddress("Siclus_y",       &SiClus_y);
  siClusTree->SetBranchAddress("Siclus_z",       &SiClus_z);


///////////
  caloTree->SetBranchAddress("calo_evt", &calo_evt);
  caloTree->SetBranchAddress("phi",      &calo_phi);
  caloTree->SetBranchAddress("x",        &calo_x);
  caloTree->SetBranchAddress("y",        &calo_y);
  caloTree->SetBranchAddress("z",        &calo_z);
  caloTree->SetBranchAddress("energy",   &calo_energy);
  caloTree->SetBranchAddress("chi2",     &calo_chi2);
  caloTree->SetBranchAddress("prob",     &calo_prob);

///////////

  //TFile *outFile = new TFile("dphi_distribution_em.root", "RECREATE");
  TFile *outFile = new TFile("dphi_distribution.root", "RECREATE");
  TH1F *h_dphi = new TH1F("h_dphi", "Track - Calo #Delta#phi;#Delta#phi;Counts", 200, -0.3, 0.3);
  TH1F *h_dphi_emc = new TH1F("h_dphi (EMCal Proj)", "Track - Calo #Delta#phi;#Delta#phi;Counts", 200, -0.3, 0.3);
  TH2F *h_dphi_emc_pt = new TH2F("h_dphi_pt", "Track - Calo #Delta#phi;pT;#Delta#phi", 200, 0, 20, 200, -0.3, 0.3);
  TH2F *h_dphi_emc_pt_truth = new TH2F("h_dphi_pt_truth", "Track - Calo #Delta#phi;pT{true};#Delta#phi", 200, 0, 20, 200, -0.3, 0.3);
  TH1F *h_EoverP_all = new TH1F("h_EoverP_all", "E/p of matched track-calo;E/p;Counts", 100, 0, 5);
  TH1F *h_EoverP_cut = new TH1F("h_EoverP_cut", "E/p of matched track-calo phi cut;E/p;Counts", 100, 0, 5);
  TH1F *h_dz = new TH1F("h_dz", "Track - Calo #Delta z;#Delta z (cm);Counts", 200, -50, 50);
  TH1F *h_dz_emc = new TH1F("h_dz_emc", "Track(EMCal Proj) - Calo #Delta z;#Delta z (cm);Counts", 200, -50, 50);

  TH1F *h_mass     = new TH1F("h_mass", "Invariant mass of matched track pairs (e^{+}e^{-});Mass (GeV/c^{2});Counts", 200, 0, 5);
  // Invariant mass histograms for different J/psi pT bins
  TH1F *h_mass_pt0_1 = new TH1F("h_mass_pt0_1", "Invariant mass (pT 0-1 GeV/c);Mass (GeV/c^{2});Counts", 200, 0, 5);
  TH1F *h_mass_pt1_2 = new TH1F("h_mass_pt1_2", "Invariant mass (pT 1-2 GeV/c);Mass (GeV/c^{2});Counts", 200, 0, 5);
  TH1F *h_mass_pt2_3 = new TH1F("h_mass_pt2_3", "Invariant mass (pT 2-3 GeV/c);Mass (GeV/c^{2});Counts", 200, 0, 5);
  TH1F *h_mass_pt3_4 = new TH1F("h_mass_pt3_4", "Invariant mass (pT 3-4 GeV/c);Mass (GeV/c^{2});Counts", 200, 0, 5);
  TH1F *h_mass_pt4up = new TH1F("h_mass_pt4up", "Invariant mass (pT >4 GeV/c);Mass (GeV/c^{2});Counts", 200, 0, 5);
  TH1F *h_track_chi2ndf_matched = new TH1F("h_track_chi2ndf_matched", "Chi2/NDF of matched tracks;#chi^{2}/ndf;Counts", 100, 0, 10);

  TH2F *h_reco_vs_truth_pt = new TH2F("h_reco_vs_truth_pt", "Reconstructed pT vs Truth pT;Truth pT (GeV/c);Reconstructed pT (GeV/c)", 100, 0, 20, 100, 0, 20);

  TNtuple *h_ntp_sicalo = new TNtuple("ntp_sicalo", "SiSeed + Calo combination", "pt:pz:c:chi2:nintt:nmaps:hitbit:eta:the:phi:z:phi_pemc:z_pemc:phi_intt:pt_tr:pz_tr:phi_tr:phi_emc:z_emc:e_emc:chi2_emc:phi_calo:pt_calo");

  TNtuple *h_ntp_sicalotrk = new TNtuple("ntp_sicalotrk", "SiSeed + Calo combination", "pt:pz:c:chi2:nintt:nmaps:hitbit:eta:the:phi:z:phi_pemc:z_pemc:phi_intt:pt_tr:pz_tr:phi_tr:phi_emc:z_emc:e_emc:chi2_emc:phi_calo:pt_calo");

  TNtuple *h_ntp_pair = new TNtuple("ntp_pair", "pair", "mass:pt:phi:eta:massc:ptc:ptp:pzp:eopp:dpp:dzp:ptm:pzm:eopm:dpm:dzm");


  // Define a struct to hold matched track info
  struct MatchedTrack {
    size_t track_idx;
    size_t calo_idx;
    float  min_dphi;
    float  min_dphi_emc;
    float  min_dz;
    float  min_dz_emc;
    float  eop;
    float  pt_calo;
    float  eop_calo;
    int    nmaps;
    int    nintt;
    int    charge;
    float  chi2ndf;
  };

  struct stCluster {
    int   m_clsTrkid;
    int   m_clslyr  ;
    float m_clsx    ;
    float m_clsy    ;
    float m_clsz    ;
  };
  ////////////////////////////////////////////////////

  Long64_t nentries = trackTree->GetEntries();

  int nskip=0;

  for (Long64_t i = 0; i < nentries; ++i) // Event Loop
  {

    trackTree->GetEntry(i);
    caloTree->GetEntry(i);
    siClusTree->GetEntry(i);
    if(truthTree!=nullptr) truthTree->GetEntry(i);

    if (evt != calo_evt)
    {
      std::cerr << "Warning: evt mismatch at entry " << i << ": track evt = " << evt << ", calo evt = " << calo_evt << std::endl;
      continue;
    }

    if((evt%1000)==0) {
       std::cout << "Matching evt = " << evt << std::endl;
    }

    std::vector<stCluster> vClus;



    int nclusSize = SiClus_trackid->size();

    cout<<evt <<" : ntrk, nemc : "<<track_phi->size()<<", "<< calo_phi->size()<< " "<<nclusSize<<endl;
    std::vector<MatchedTrack> matched_tracks;

    int itotalClus=0;
    ///////////////////////////
    // First pass: find matched tracks and store info
    for (size_t it = 0; it < track_phi->size(); ++it)
    {
      int nmaps = (*track_nmaps)[it];
      int nintt = (*track_nintt)[it];

      float& pt     = ((*track_pt)[it]);
      float& pz     = ((*track_pz)[it]);
      float& eta    = ((*track_eta)[it]);
      int&   charge = ((*track_charge)[it]);

      int nclus = nmaps+nintt;
      //std::cout << "charge : " << charge<<" "<<pt<<" "<<nmaps<<" "<<nintt<<" "<<calo_phi->size()<<" "<<nclus<<std::endl;

      // cluster:
      int hitbit=0;
      stCluster iCl, oCl;
      for(int ic=0; ic<nclus; ic++) {
        int   clsid    = ic+itotalClus;

        int layer =  ((*SiClus_layer)[clsid]);
        stCluster Cl {
          ((*SiClus_trackid)[clsid]),
          layer,
          ((*SiClus_x)[clsid]),
          ((*SiClus_y)[clsid]),
          ((*SiClus_z)[clsid]) 
        };
        vClus.push_back(Cl);

        if(0<=Cl.m_clslyr&&Cl.m_clslyr<5) iCl = Cl;
        else                              oCl = Cl;

        hitbit |= (1<<layer);

        //cout<<"cls : "<<clsid<<" "<<Cl.m_clsTrkid<<" "<<Cl.m_clslyr<<" "<<Cl.m_clsx<<" "<<Cl.m_clsy<<" "<<Cl.m_clsz<<endl;
      }
      itotalClus+=nclus;

      // truth track
      float pt_tr=0, pz_tr=0, phi_tr=0, eta_tr=0;

      if(truthTree!=nullptr){
        int ntruth = truth_pt->size();
        for (size_t itr = 0; itr < (ntruth>0?1:0); ++itr)
        {
          pt_tr  = ((*truth_pt)[itr]);
          pz_tr  = ((*truth_pz)[itr]);
          phi_tr = ((*truth_phi)[itr]);
          eta_tr = ((*truth_eta)[itr]);
          cout<<"tru : "<<itr<<" "<<pt_tr<<" "<<pz_tr<<" "<<phi_tr<<" "<<eta_tr<<endl;
        }
      }

      if (pt < 0.3) continue;

      if (nmaps < 1 || nintt < 1)
      {
        if(nskip<1000) {
          std::cout << "Skipping track with nmaps = " << nmaps << " and nintt = " << nintt << std::endl;
        } else if(nskip==1000) {
          std::cout << "exceed nskip. comment suppressed" << std::endl;
        }
        nskip++;

        continue;
      }

      //cout<<"cls-in : "<<iCl.m_clsTrkid<<" "<<iCl.m_clslyr<<" "<<iCl.m_clsx<<" "<<iCl.m_clsy<<" "<<iCl.m_clsz<<endl;
      //cout<<"cls-out: "<<oCl.m_clsTrkid<<" "<<oCl.m_clslyr<<" "<<oCl.m_clsx<<" "<<oCl.m_clsy<<" "<<oCl.m_clsz<<endl;
      //std::cout << "charge : " << charge<<" "<<pt<<std::endl;

      float phi_intt = atan2(oCl.m_clsy-iCl.m_clsy, oCl.m_clsx-iCl.m_clsx);


      // Find calo cluster with minimum dphi_emc for this track
      float ntp_value[30] = {
                pt,                   //0
                pz,                   //1
                (float)(charge),      //2
                (*track_chi2ndf)[it], //3
                (float)nintt,         //4
                (float)nmaps,         //5
                (float)hitbit,        //6
                (*track_eta)[it],     //7
                (float)(2.* std::atan( std::exp(-eta) )),//8
                (*track_phi)[it],     //9   
                (*track_z)[it],       //10
                (*track_phi_emc)[it], //11
                (*track_z_emc)[it],   //12
                phi_intt,             //13
                pt_tr,                //14
                pz_tr,                //15
                phi_tr                //16
              };


      const float par[2] = {0.21, -0.986}; // par for pT_calo calculation

      float min_dr = 1e9;
      size_t min_ic = calo_phi->size();
      float min_dphi = 0, min_dphi_emc = 0, min_dz = 0, min_dz_emc = 0;
      float match_pt_calo=0;
      //cout<<"aaaa"<<endl;
      for (size_t ic = 0; ic < calo_phi->size(); ++ic)
      {
      //--cout<<"bbb"<<endl;
       
        float dphi_emc = (*track_phi_emc)[it] - (*calo_phi)[ic];
        if (dphi_emc >  M_PI) dphi_emc -= 2 * M_PI;
        if (dphi_emc < -M_PI) dphi_emc += 2 * M_PI;

        const float p0 = -0.181669;
        const float p1 =  0.00389827;
        //float dphi_emc_corr = dphi_emc - charge*(p0/pt + p1);
        float dphi_emc_corr = charge*dphi_emc - 0.18*std::pow(pt, -0.986);

        float x_calo = (*calo_x)[ic];
        float y_calo = (*calo_y)[ic];
        float z_calo = (*calo_z)[ic];

        float dz_emc = (*track_z_emc)[it] - z_calo;


        float phi_calo = atan2(y_calo - oCl.m_clsy,  x_calo - oCl.m_clsx);

        float dphi = phi_calo - phi_intt;
        float pt_calo = par[0]*pow(-charge*dphi, par[1]);//cal_CaloPt(dphi);

        ntp_value[17]  = (*calo_phi)[ic];
        ntp_value[18] = z_calo;
        ntp_value[19] = (*calo_energy)[ic];
        ntp_value[20] = (*calo_chi2)[ic];
        ntp_value[21] = phi_calo;
        ntp_value[22] = pt_calo;
        
        h_ntp_sicalo->Fill(ntp_value);
        //--std::cout << "   energy : " << ntp_value[10]<<std::endl;


        if (fabs(dphi_emc) < min_dr)
        //if (fabs(dphi_emc_corr) < min_dr)
        //if (fabs(dz_emc) < min_dr)
        {
          //min_dr = fabs(dphi_emc);
          //min_dr = fabs(dphi_emc_corr);
          
          min_dr = fabs(dz_emc);
          min_ic = ic;

          //min_dphi = (*track_phi)[it] - (*calo_phi)[ic];
          //if (min_dphi >  M_PI) min_dphi -= 2 * M_PI;
          //if (min_dphi < -M_PI) min_dphi += 2 * M_PI;

          //min_dphi_emc = dphi_emc_corr;
          //min_dz = (*track_z)[it] - (*calo_z)[ic];
          //min_dz_emc = (*track_z_emc)[it] - (*calo_z)[ic];

          //match_pt_calo = pt_calo;
        }
      }

      if (min_ic != calo_phi->size())
      { // for matched track
        float x_calo = (*calo_x)[min_ic];
        float y_calo = (*calo_y)[min_ic];
        float z_calo = (*calo_z)[min_ic];

        float phi_calo = atan2(y_calo - oCl.m_clsy,  x_calo - oCl.m_clsx);

        float dphi = phi_calo - phi_intt;
        float pt_calo = par[0]*pow(-charge*dphi, par[1]);//cal_CaloPt(dphi);

        ntp_value[17]  = (*calo_phi)[min_ic];
        ntp_value[18] = z_calo;
        ntp_value[19] = (*calo_energy)[min_ic];
        ntp_value[20] = (*calo_chi2)[min_ic];
        ntp_value[21] = phi_calo;
        ntp_value[22] = pt_calo;
      }
      else {
        ntp_value[17] = -9999.;
        ntp_value[18] = -9999.;
        ntp_value[19] = -9999.;
        ntp_value[20] = -9999.;
        ntp_value[21] = -9999.;
        ntp_value[22] = -9999.;
      }
      h_ntp_sicalotrk->Fill(ntp_value);

      if (min_ic == calo_phi->size()) {
        continue; // no match
      }

      float p = (*track_pt)[it] * std::cosh((*track_eta)[it]);
      float E = (*calo_energy)[min_ic];
      float eop = -999;
      if (p > 0)
      {
        eop = E / p;
        h_EoverP_all->Fill(eop);
      }
      float p_calo = match_pt_calo * std::cosh((*track_eta)[it]);
      float eop_calo = E/p_calo;

      h_dphi->Fill(min_dphi);
      h_dphi_emc->Fill(min_dphi_emc);
      h_dphi_emc_pt->Fill((*track_pt)[it], min_dphi_emc);
      //h_dphi_emc_pt_truth->Fill((*truth_pt)[0], min_dphi_emc);
      h_dz->Fill(min_dz);
      h_dz_emc->Fill(min_dz_emc);
     // std::cout << "  ➤ Δφ = " << min_dphi << ", Δz = " << min_dz << std::endl;

      h_track_chi2ndf_matched->Fill((*track_chi2ndf)[it]);
      if ((*track_chi2ndf)[it] > 5)
        continue; // Skip tracks with high chi2/ndf
      if (min_dz_emc > 4)
        continue;
     // h_reco_vs_truth_pt->Fill((*truth_pt)[0], (*track_pt)[it]);

      matched_tracks.push_back({it, min_ic, min_dphi, min_dphi_emc, min_dz, min_dz_emc, eop, match_pt_calo, eop_calo, nmaps, nintt, (*track_charge)[it], (*track_chi2ndf)[it]});
    }

    //cout<<"nclus-end : "<<itotalClus<<endl;

    /////////////////////////
    //--// 1.5 pass: split positive and negative to different array;
    //--std::vector<MatchedTrack> v_trkp, v_trkm;
    //--for (size_t idx = 0; idx < matched_tracks.size(); ++idx)
    //--{
    //--  const auto& mt = matched_tracks[idx];
    //--  if(mt.charge>0) v_trkp.push_back(mt);
    //--  else            v_trkm.push_back(mt);
    //--}

    /////////////////////////
    // Second pass: loop through matched tracks and fill histograms and compute invariant mass
    cout<<"matched track : "<<matched_tracks.size()<<endl;
    float v_pair[20];
    for (size_t idx = 0; idx < matched_tracks.size(); ++idx)
    {
      const auto& mt = matched_tracks[idx];

      if (mt.nmaps > 2 && mt.nintt > 1 && std::abs(mt.min_dz_emc) < 4)
      {
        h_EoverP_cut->Fill(mt.eop);

        if (!(mt.eop > 0.6 && mt.eop < 2)) continue;

        for (size_t jdx = idx+1; jdx < matched_tracks.size(); ++jdx)
        {
          const auto& mt2 = matched_tracks[jdx];

          //if (mt2.nmaps < 2 || mt2.nintt < 1) continue;
          //if (std::abs(mt2.min_dz_emc) > 4) continue;

          if (mt2.nmaps > 2 && mt2.nintt > 1 && std::abs(mt2.min_dz_emc) < 4) {

            if (mt.charge * mt2.charge >= 0) continue; // opposite sign only

            //float eop2 = -999;
            //float p2 = (*track_pt)[mt2.track_idx] * std::cosh((*track_eta)[mt2.track_idx]);
            //float E2_raw = (*calo_energy)[mt2.calo_idx];
            //if (p2 > 0) eop2 = E2_raw / p2;
            //if (!(eop2 > 0.8 && eop2 < 2)) continue;

            if (!(mt2.eop > 0.6 && mt2.eop < 2)) continue;

            float pt1 = (*track_pt)[mt.track_idx],  eta1 = (*track_eta)[mt.track_idx],  phi1 = (*track_phi)[mt.track_idx];
            float pt2 = (*track_pt)[mt2.track_idx], eta2 = (*track_eta)[mt2.track_idx], phi2 = (*track_phi)[mt2.track_idx];
            TLorentzVector v1, v2;
            v1.SetPtEtaPhiM(pt1, eta1, phi1, electron_mass);
            v2.SetPtEtaPhiM(pt2, eta2, phi2, electron_mass);
            //v1.SetPtEtaPhiM(pt1, eta1, phi1, pion_mass);
            //v2.SetPtEtaPhiM(pt2, eta2, phi2, pion_mass);

            TLorentzVector vpair = v1 + v2;
 
            //float mass = (v1 + v2).M();
            //float total_pt = pt1 + pt2;
            float mass     = vpair.M();
            float total_pt = vpair.Pt();
        
            // mass with pt_calo
            float ptc1 = mt.pt_calo;
            float ptc2 = mt2.pt_calo;
            TLorentzVector vc1, vc2;
            vc1.SetPtEtaPhiM(ptc1, eta1, phi1, electron_mass);
            vc2.SetPtEtaPhiM(ptc2, eta2, phi2, electron_mass);
            //vc1.SetPtEtaPhiM(ptc1, eta1, phi1, pion_mass);
            //vc2.SetPtEtaPhiM(ptc2, eta2, phi2, pion_mass);
            TLorentzVector vpairc = vc1 + vc2;
 
            float massc     = vpairc.M();
            float total_ptc = vpairc.Pt();

            //if ((0.8 < mt.eop  && mt.eop  < 2) &&
            //    (0.8 < mt2.eop && mt2.eop < 2)) 
            {
              h_mass->Fill(mass);

              // Fill the appropriate mass histogram based on total pT
              if (total_pt < 1.0) {
                h_mass_pt0_1->Fill(mass);
              } else if (total_pt < 2.0) {
                h_mass_pt1_2->Fill(mass);
              } else if (total_pt < 3.0) {
                h_mass_pt2_3->Fill(mass);
              } else if (total_pt < 4.0) {
                h_mass_pt3_4->Fill(mass);
              } else {
                h_mass_pt4up->Fill(mass);
              }
            }

    //TNtuple *h_ntp_pair = new TNtuple("ntp_pair", "SiSeed + Calo combination", 
    //"mass:pt:phi:eta:ptp:pzp:phip:dpp:dzp:ptm:pzm:phim:dpm:dzm");
 
            v_pair[ 0] = mass;
            v_pair[ 1] = total_pt;
            v_pair[ 2] = vpair.Phi();
            v_pair[ 3] = vpair.Eta();
            v_pair[ 4] = massc; // mass with pt_calo
            v_pair[ 5] = total_ptc; // pair-pt with pt_calo
            v_pair[ 6] = pt1;
            v_pair[ 7] = v1.Pz();
            v_pair[ 8] = mt.eop_calo;
            v_pair[ 9] = mt.min_dphi_emc;
            v_pair[10] = mt.min_dz_emc;
            v_pair[11] = pt2;
            v_pair[12] = v2.Pz();
            v_pair[13] = mt2.eop_calo;
            v_pair[15] = mt2.min_dphi_emc;
            v_pair[15] = mt2.min_dz_emc;

            h_ntp_pair->Fill(v_pair);
          }
        }
      }
    }
  }

  std::cout<<"total nskip : "<< nskip <<std::endl;

 // TFile *outFile = new TFile("dphi_distribution_e-.root", "RECREATE");
  //TFile *outFile = new TFile("dphi_distribution_PYTHIA_temp.root", "RECREATE");
  std::cout << "Saving histogram to dphi_distribution.root" << std::endl;
  h_dphi->Write();
  h_dphi_emc->Write();
  h_dphi_emc_pt->Write();
  h_dphi_emc_pt_truth->Write();
  h_EoverP_all->Write();
  h_EoverP_cut->Write();
  h_dz->Write();
  h_dz_emc->Write();
  h_mass->Write();
  h_mass_pt0_1->Write();
  h_mass_pt1_2->Write();
  h_mass_pt2_3->Write();
  h_mass_pt3_4->Write();
  h_mass_pt4up->Write();
  h_track_chi2ndf_matched->Write();
  h_reco_vs_truth_pt->Write();
  h_ntp_sicalo->Write();
  h_ntp_sicalotrk->Write();
  h_ntp_pair->Write();

  outFile->Close();

  std::cout << "Δφ histogram saved to 'dphi_distribution.root'" << std::endl;
  f->Close();
}
