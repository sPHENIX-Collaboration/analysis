#include "../corrections/GeoAcceptanceCorrection.h"

std::string combine_name(std::string base, std::string suffix)
{
  if(suffix!="")
  {
    return base+"_"+suffix;
  }
  else
  {
    return base;
  }
}

std::string combine_title(std::string base, std::string suffix, std::string axislabel)
{
  if(suffix!="")
  {
    return base+" "+suffix+";"+axislabel;
  }
  else
  {
    return base+";"+axislabel;
  }
}

struct HistContainer
{
  TH1F* lifetime_truth;
  TH1F* lifetime_reco;

  TH1F* decaylength_truth;
  TH1F* decaylength_reco;

  TH1F* lifetime_truth_signal;
  TH1F* lifetime_truth_background;
  TH1F* lifetime_reco_signal;
  TH1F* lifetime_reco_background;

  HistContainer(std::string name_suffix, std::string title_suffix,
                int nBins_t, float min_t, float max_t,
                int nBins_L, float min_L, float max_L)
  {
    const std::string lifetime_label = "#tau [ps]";
    const std::string decaylength_label = "decay length [cm]";

    lifetime_truth = new TH1F(combine_name("lifetime_truth",name_suffix).c_str(),
                              combine_title("Truth lifetime",title_suffix,lifetime_label).c_str(),
                              nBins_t,min_t,max_t);
    lifetime_reco = new TH1F(combine_name("lifetime_reco",name_suffix).c_str(),
                             combine_title("Reco lifetime",title_suffix,lifetime_label).c_str(),
                             nBins_t,min_t,max_t);
    decaylength_truth = new TH1F(combine_name("decaylength_truth",name_suffix).c_str(),
                                 combine_title("Truth decay length",title_suffix,decaylength_label).c_str(),
                                 nBins_L,min_L,max_L);
    decaylength_reco = new TH1F(combine_name("decaylength_reco",name_suffix).c_str(),
                                combine_title("Reco decay length",title_suffix,decaylength_label).c_str(),
                                nBins_L,min_L,max_L);
    lifetime_truth_signal = new TH1F(combine_name("lifetime_truth_signal",name_suffix).c_str(),
                                     combine_title("Signal truth lifetime",title_suffix,lifetime_label).c_str(),
                                     nBins_t,min_t,max_t);
    lifetime_truth_background = new TH1F(combine_name("lifetime_truth_background",name_suffix).c_str(),
                                         combine_title("Background truth lifetime",title_suffix,lifetime_label).c_str(),
                                         nBins_t,min_t,max_t);
    lifetime_reco_signal = new TH1F(combine_name("lifetime_reco_signal",name_suffix).c_str(),
                                     combine_title("Signal reco lifetime",title_suffix,lifetime_label).c_str(),
                                     nBins_t,min_t,max_t);
    lifetime_reco_background = new TH1F(combine_name("lifetime_reco_background",name_suffix).c_str(),
                                         combine_title("Background reco lifetime",title_suffix,lifetime_label).c_str(),
                                         nBins_t,min_t,max_t);
  }

  void Write()
  {
    lifetime_truth->Write();
    lifetime_reco->Write();
    decaylength_truth->Write();
    decaylength_reco->Write();
    lifetime_truth_signal->Write();
    lifetime_truth_background->Write();
    lifetime_reco_signal->Write();
    lifetime_reco_background->Write();
  }
};

void KFParticle_truth_lifetime()
{
  const bool verbose = false;
  const float min_DIRA = 0.99;
  const float min_track_IPxy = 0.05;
  const float min_decayLength = 0.05;
  const float min_mother_pt = 0.8;
  const int min_nMVTX = 1;
  const int min_nINTT = 1;
  const int min_nTPC = 1;
  const float max_pv_quality = 20.;
  const float max_daughter_DCA = 0.5;

  const std::string particle_name = "Lambda0";
  const int mother_pdgid = 3122;
  const float mother_mass = 1.115;

  const float speed = 2.99792458e-2;

  GeoAcceptanceCorrection geoacc_corr("/sphenix/u/cdean/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots/Lambda0_geometric_acceptance_ratio_pT.root","Lambda0_inGeo_pT");

  //kshort file: /gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Kshort_reco.root
  TFile* f = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/outputKFParticle_Lambda_reco_swum_weighted.root");
  TTree* t = (TTree*)f->Get("DecayTree");

  TFile* fout = new TFile("KFParticle_truth_h.root","RECREATE");

  const int nBins_lifetime = 100;
  const float min_lifetime = 0.;
  const float max_lifetime = 500.;

  const int nBins_decaylength = 100;
  const float min_decaylength = 0.;
  const float max_decaylength = 10.;

  // starting distributions
  HistContainer h("","",
                  nBins_lifetime,min_lifetime,max_lifetime,nBins_decaylength,min_decaylength,max_decaylength);
  // applying mother pt cut
  HistContainer h_motherpt("motherptcut","with mother pT>800MeV",
                           nBins_lifetime,min_lifetime,max_lifetime,nBins_decaylength,min_decaylength,max_decaylength);
  // applying acceptance corrections
  HistContainer h_geoacc("geoacc","with mother pT>800MeV, geo. acc. corrected",
                         nBins_lifetime,min_lifetime,max_lifetime,nBins_decaylength,min_decaylength,max_decaylength);
  // applying KFParticle candidate selections
  HistContainer h_kfcut("kfcut","with mother pT>800MeV, geo. acc. corrected, with KFParticle selection cuts",
                         nBins_lifetime,min_lifetime,max_lifetime,nBins_decaylength,min_decaylength,max_decaylength);
  // applying swimming corrections
  HistContainer h_swim("swim","with mother pT>800 MeV, geo. acc. corrected, with KFParticle selection cuts and swimming correction",
                       nBins_lifetime,min_lifetime,max_lifetime,nBins_decaylength,min_decaylength,max_decaylength);

  TH1F* lifetime_diff = new TH1F("lifetime_diff","lifetime_diff",100,-20.,20.);
  TH1F* decaylength_diff = new TH1F("decaylength_diff","decaylength_diff",100,-2.,2.);

  float decayTime;
  float decayLength;
  float mother_p;
  float mother_pt;
  float mother_DIRA;
  float mother_PV_x;
  float mother_PV_y;
  float mother_PV_z;

  std::vector<int>* track1_pdgid_history = nullptr;
  float track1_EV_x;
  float track1_EV_y;
  float track1_EV_z;
  float track1_PV_x;
  float track1_PV_y;
  float track1_PV_z;
  float track1_gpx;
  float track1_gpy;
  float track1_gpz;
  float track1_IP_xy;
  unsigned int track1_nMVTX;
  unsigned int track1_nINTT;
  unsigned int track1_nTPC;

  std::vector<int>* track2_pdgid_history = nullptr;
  float track2_EV_x;
  float track2_EV_y;
  float track2_EV_z;
  float track2_PV_x;
  float track2_PV_y;
  float track2_PV_z;
  float track2_gpx;
  float track2_gpy;
  float track2_gpz;
  float track2_IP_xy;
  unsigned int track2_nMVTX;
  unsigned int track2_nINTT;
  unsigned int track2_nTPC;

  float pv_chi2;
  unsigned int pv_ndof;
  float daughter_DCA;

  std::vector<float>* turningPoints_TAU = nullptr;
  float swim_weight;

  t->SetBranchAddress((particle_name+"_decayTime").c_str(),&decayTime);
  t->SetBranchAddress((particle_name+"_decayLength").c_str(),&decayLength);
  t->SetBranchAddress((particle_name+"_p").c_str(),&mother_p);
  t->SetBranchAddress((particle_name+"_pT").c_str(),&mother_pt);
  t->SetBranchAddress((particle_name+"_DIRA").c_str(),&mother_DIRA);
  t->SetBranchAddress("primary_vertex_x",&mother_PV_x);
  t->SetBranchAddress("primary_vertex_y",&mother_PV_y);
  t->SetBranchAddress("primary_vertex_z",&mother_PV_z);

  t->SetBranchAddress("track_1_true_track_history_PDG_ID",&track1_pdgid_history);
  t->SetBranchAddress("track_1_true_EV_x",&track1_EV_x);
  t->SetBranchAddress("track_1_true_EV_y",&track1_EV_y);
  t->SetBranchAddress("track_1_true_EV_z",&track1_EV_z);
  t->SetBranchAddress("track_1_true_PV_x",&track1_PV_x);
  t->SetBranchAddress("track_1_true_PV_y",&track1_PV_y);
  t->SetBranchAddress("track_1_true_PV_z",&track1_PV_z);
  t->SetBranchAddress("track_1_true_px",&track1_gpx);
  t->SetBranchAddress("track_1_true_py",&track1_gpy);
  t->SetBranchAddress("track_1_true_pz",&track1_gpz);
  t->SetBranchAddress("track_1_IP_xy",&track1_IP_xy);
  t->SetBranchAddress("track_1_MVTX_nHits",&track1_nMVTX);
  t->SetBranchAddress("track_1_INTT_nHits",&track1_nINTT);
  t->SetBranchAddress("track_1_TPC_nHits",&track1_nTPC);

  t->SetBranchAddress("track_2_true_track_history_PDG_ID",&track2_pdgid_history);
  t->SetBranchAddress("track_2_true_EV_x",&track2_EV_x);
  t->SetBranchAddress("track_2_true_EV_y",&track2_EV_y);
  t->SetBranchAddress("track_2_true_EV_z",&track2_EV_z);
  t->SetBranchAddress("track_2_true_PV_x",&track2_PV_x);
  t->SetBranchAddress("track_2_true_PV_y",&track2_PV_y);
  t->SetBranchAddress("track_2_true_PV_z",&track2_PV_z);
  t->SetBranchAddress("track_2_true_px",&track2_gpx);
  t->SetBranchAddress("track_2_true_py",&track2_gpy);
  t->SetBranchAddress("track_2_true_pz",&track2_gpz);
  t->SetBranchAddress("track_2_IP_xy",&track2_IP_xy);
  t->SetBranchAddress("track_2_MVTX_nHits",&track2_nMVTX);
  t->SetBranchAddress("track_2_INTT_nHits",&track2_nINTT);
  t->SetBranchAddress("track_2_TPC_nHits",&track2_nTPC);

  t->SetBranchAddress("primary_vertex_chi2",&pv_chi2);
  t->SetBranchAddress("primary_vertex_ndof",&pv_ndof);
  t->SetBranchAddress("track_1_track_2_DCA",&daughter_DCA);

  t->SetBranchAddress("swim_weight",&swim_weight);
  t->SetBranchAddress("turningPoints_TAU",&turningPoints_TAU);

  for(int i=0;i<t->GetEntries();i++)
  {
    if(i % 1000 == 0) std::cout << i << std::endl;
    t->GetEntry(i);

    if(track1_nMVTX<min_nMVTX || track2_nMVTX<min_nMVTX ||
       track1_nINTT<min_nINTT || track2_nINTT<min_nINTT ||
       track1_nTPC<min_nTPC || track2_nTPC<min_nTPC ||
       pv_chi2/pv_ndof>max_pv_quality ||
       daughter_DCA>max_daughter_DCA)
    {
      continue;
    }

    if(verbose)
    {
      std::cout << "--------------------------------------------" << std::endl;
      std::cout << "entry " << i << std::endl;
      std::cout << "reco decayLength: " << decayLength << " decayTime: " << decayTime << std::endl;
      std::cout << "p: " << mother_p << std::endl;
      std::cout << "reco PV: (" << mother_PV_x << ", " << mother_PV_y << ", " << mother_PV_z << ")" << std::endl;
    }

    const bool is_signal = (track1_pdgid_history && track2_pdgid_history &&
                            track1_pdgid_history->size()>0 && track2_pdgid_history->size()>0 && 
                            ((track1_pdgid_history->at(0)==mother_pdgid && track2_pdgid_history->at(0)==mother_pdgid) || 
                             (track1_pdgid_history->at(0)==-mother_pdgid && track2_pdgid_history->at(0)==-mother_pdgid)));

    if(verbose)
    {
      if(is_signal)
      {
        std::cout << "is signal" << std::endl;
      }
      else
      {
        std::cout << "is not signal" << std::endl;
        if(track1_pdgid_history == nullptr)
        {
          std::cout << "history unavailable for track 1" << std::endl;
        }
        if(track2_pdgid_history == nullptr)
        {
          std::cout << "history unavailable for track 2" << std::endl;
        }
        if(track1_pdgid_history && track2_pdgid_history && track1_pdgid_history->size()==0)
        {
          std::cout << "history size is zero for track 1" << std::endl;
        }
        if(track1_pdgid_history && track2_pdgid_history && track2_pdgid_history->size()==0)
        {
          std::cout << "history size is zero for track 2" << std::endl;
        }
        if(track1_pdgid_history && track2_pdgid_history && track1_pdgid_history->size()>0 && track2_pdgid_history->size()>0)
        {
          std::cout << "direct descendant PIDs are " << track1_pdgid_history->at(0) << " and " << track2_pdgid_history->at(0) << std::endl;
        }
      }
    }

    std::array<float,3> true_SV;

    if(fabs(track1_PV_x+99.)>0.001)
    {
      true_SV = {track1_PV_x, track1_PV_y, track1_PV_z};
    }
    else if(fabs(track2_PV_x+99.)>0.001)
    {
      true_SV = {track2_PV_x, track2_PV_y, track2_PV_z};
    }
    else
    {
      true_SV = {track1_EV_x,track1_EV_y,track1_EV_z};
    }

    if(verbose)
    {
      std::cout << "SV: (" << true_SV[0] << ", " << true_SV[1] << ", " << true_SV[2] << ")" << std::endl;
      std::cout << "PV: (" << mother_PV_x << ", " << mother_PV_y << ", " << mother_PV_z << ")" << std::endl;
    }

    const float true_decayLength = sqrt(pow(true_SV[0]-mother_PV_x,2)+pow(true_SV[1]-mother_PV_y,2)+pow(true_SV[2]-mother_PV_z,2));
    const float true_p = sqrt(pow(track1_gpx+track2_gpx,2)+pow(track1_gpy+track2_gpy,2)+pow(track1_gpz+track2_gpz,2));
    const float true_decayTime = 1./speed * mother_mass*true_decayLength/true_p;

    if(verbose)
    {
      std::cout << "true decay length: " << true_decayLength << std::endl;
      std::cout << "true p: " << true_p << std::endl;
      std::cout << "true decay time: " << true_decayTime << std::endl;
    }

    h.lifetime_reco->Fill(decayTime);
    h.decaylength_reco->Fill(decayLength);
    h.decaylength_truth->Fill(true_decayLength);
    decaylength_diff->Fill(decayLength-true_decayLength);
    h.lifetime_truth->Fill(true_decayTime);
    lifetime_diff->Fill(decayTime-true_decayTime);

    if(is_signal)
    {
      h.lifetime_truth_signal->Fill(true_decayTime);
      h.lifetime_reco_signal->Fill(decayTime);
    }
    else
    {
      h.lifetime_truth_background->Fill(true_decayTime);
      h.lifetime_reco_background->Fill(decayTime);
    }

    if(mother_pt>min_mother_pt)
    {
      h_motherpt.lifetime_truth->Fill(true_decayTime);
      h_motherpt.decaylength_truth->Fill(true_decayLength);
      h_motherpt.lifetime_reco->Fill(decayTime);
      h_motherpt.decaylength_reco->Fill(decayLength);

      const float geoacc_weight = 1./geoacc_corr.h_corr->Interpolate(mother_pt);
      h_geoacc.lifetime_truth->Fill(true_decayTime,geoacc_weight);
      h_geoacc.decaylength_truth->Fill(true_decayLength,geoacc_weight);
      h_geoacc.lifetime_reco->Fill(decayTime,geoacc_weight);
      h_geoacc.decaylength_reco->Fill(decayLength,geoacc_weight);

      if(is_signal)
      {
        h_motherpt.lifetime_truth_signal->Fill(true_decayTime);
        h_motherpt.lifetime_reco_signal->Fill(decayTime);

        h_geoacc.lifetime_truth_signal->Fill(true_decayTime,geoacc_weight);
        h_geoacc.lifetime_reco_signal->Fill(decayTime,geoacc_weight);
      }
      else
      {
        h_motherpt.lifetime_truth_background->Fill(true_decayTime);
        h_motherpt.lifetime_reco_background->Fill(decayTime);

        h_geoacc.lifetime_truth_background->Fill(true_decayTime,geoacc_weight);
        h_geoacc.lifetime_reco_background->Fill(decayTime,geoacc_weight);
      }

      if(mother_DIRA>min_DIRA && decayLength>min_decayLength && fabs(track1_IP_xy)>min_track_IPxy && fabs(track2_IP_xy)>min_track_IPxy)
      {
        h_kfcut.lifetime_truth->Fill(true_decayTime,geoacc_weight);
        h_kfcut.decaylength_truth->Fill(true_decayLength,geoacc_weight);
        h_kfcut.lifetime_reco->Fill(decayTime,geoacc_weight);
        h_kfcut.decaylength_reco->Fill(decayLength,geoacc_weight);

        h_swim.lifetime_truth->Fill(true_decayTime,geoacc_weight*swim_weight);
        h_swim.decaylength_truth->Fill(true_decayLength,geoacc_weight*swim_weight);
        h_swim.lifetime_reco->Fill(decayTime,geoacc_weight*swim_weight);
        h_swim.decaylength_reco->Fill(decayLength,geoacc_weight*swim_weight);

        if(is_signal)
        {
          h_kfcut.lifetime_truth_signal->Fill(true_decayTime,geoacc_weight);
          h_kfcut.lifetime_reco_signal->Fill(decayTime,geoacc_weight);

          h_swim.lifetime_truth_signal->Fill(true_decayTime,geoacc_weight*swim_weight);
          h_swim.lifetime_reco_signal->Fill(decayTime,geoacc_weight*swim_weight);
        }
        else
        {
          h_kfcut.lifetime_truth_background->Fill(true_decayTime,geoacc_weight);
          h_kfcut.lifetime_reco_background->Fill(decayTime,geoacc_weight);

          h_swim.lifetime_truth_background->Fill(true_decayTime,geoacc_weight*swim_weight);
          h_swim.lifetime_reco_background->Fill(decayTime,geoacc_weight*swim_weight);
        }
      }
    }
  }

  h.Write();
  h_motherpt.Write();
  h_geoacc.Write();
  h_kfcut.Write();
  h_swim.Write();

  lifetime_diff->Write();
  decaylength_diff->Write();
}
