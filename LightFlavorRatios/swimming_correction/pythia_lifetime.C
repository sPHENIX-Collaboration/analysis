#include <phool/PHRandomSeed.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1F.h"

const float mass = 1.115;
const float mass_resolution = 0.01; // absolute uncertainty [GeV]
const float p_resolution_coef = 0.002; // sigma_p/p = coef*p
const float vertex_resolution = 0.01; // absolute uncertainty [cm]

void pythia_lifetime(int process=1)
{
  const uint seed = PHRandomSeed();
  std::unique_ptr<gsl_rng> m_rng;
  m_rng.reset(gsl_rng_alloc(gsl_rng_mt19937));
  gsl_rng_set(m_rng.get(), seed);

  TChain* t = new TChain("ntp_gtrack");
  if(process==0)
  {
    t->Add("/sphenix/tg/tg01/hf/frawley/sims/pythia_fromhits_detroit/pythia_reco_3122_[0-9]_g4svtx_eval.root");
    t->Add("/sphenix/tg/tg01/hf/frawley/sims/pythia_fromhits_detroit/pythia_reco_3122_[0-9][0-9]_g4svtx_eval.root");
  }
  else
  {
    t->Add(("/sphenix/tg/tg01/hf/frawley/sims/pythia_fromhits_detroit/pythia_reco_3122_"+std::to_string(process)+"[0-9][0-9]_g4svtx_eval.root").c_str());
  }

  TFile* fout = new TFile(("/sphenix/user/mjpeters/analysis/LightFlavorRatios/swimming_correction/output/fout_"+std::to_string(process)+".root").c_str(),"RECREATE");

  int segment;
  int event_out;

  float lifetime_out;
  float lifetime_labframe_out;

  float mother_vx;
  float mother_vy;
  float mother_vz;
  float mother_vt;

  float track1_vx;
  float track1_vy;
  float track1_vz;
  float track1_vt;

  float track2_vx;
  float track2_vy;
  float track2_vz;
  float track2_vt;

  float mother_pt;
  float mother_pz;
  float track1_pt;
  float track1_pz;
  float track2_pt;
  float track2_pz;

  float decaylength_out;
  float decaylength_err_out;
  float p_err_out;
  float decaytime_err_out;
  float smear_amount_out;

  bool mother_isReco;
  bool track1_isReco;
  bool track2_isReco;
  bool mother_isPrimary;

  TTree* t_out = new TTree("lifetime_study","truth candidate lifetime");
  t_out->Branch("segment",&segment_out);
  t_out->Branch("event",&event_out);
  t_out->Branch("lifetime",&lifetime_out);
  t_out->Branch("lifetime_labframe",&lifetime_labframe_out);

  t_out->Branch("mother_vx",&mother_vx);
  t_out->Branch("mother_vy",&mother_vy);
  t_out->Branch("mother_vz",&mother_vz);
  t_out->Branch("mother_vt",&mother_vt);

  t_out->Branch("track1_vx",&track1_vx);
  t_out->Branch("track1_vy",&track1_vy);
  t_out->Branch("track1_vz",&track1_vz);
  t_out->Branch("track1_vt",&track1_vt);

  t_out->Branch("track2_vx",&track2_vx);
  t_out->Branch("track2_vy",&track2_vy);
  t_out->Branch("track2_vz",&track2_vz);
  t_out->Branch("track2_vt",&track2_vt);

  t_out->Branch("mother_pt",&mother_pt);
  t_out->Branch("mother_pz",&mother_pz);
  t_out->Branch("track1_pt",&track1_pt);
  t_out->Branch("track1_pz",&track1_pz);
  t_out->Branch("track2_pt",&track2_pt);
  t_out->Branch("track2_pz",&track2_pz);

  t_out->Branch("decaylength",&decaylength_out);
  t_out->Branch("decaylength_err",&decaylength_err_out);
  t_out->Branch("p_err",&p_err_out);
  t_out->Branch("lifetime_err",&decaytime_err_out);
  t_out->Branch("smear_amount",&smear_amount_out);

  t_out->Branch("mother_isReco",&mother_isReco);
  t_out->Branch("track1_isReco",&track1_isReco);
  t_out->Branch("track2_isReco",&track2_isReco);
  t_out->Branch("mother_isPrimary",&mother_isPrimary);
  
  //std::string indir = "/sphenix/tg/tg01/hf/frawley/sims/pythia_fromhits_detroit/";
  TH1F* lifetime = new TH1F("lifetime","Truth lifetime;#tau [ns]",100,0.,0.5);
  TH1F* lifetime_ptcut = new TH1F("lifetime_ptcut","Truth lifetime after 160MeV daughter pT cut;#tau [ns]",100,0.,0.5);
  TH1F* lifetime_motherptcut = new TH1F("lifetime_motherptcut","Truth lifetime after 800MeV mother pT cut;#tau [ns]",100,0.,0.5);
  TH1F* lifetime_bothptcut = new TH1F("lifetime_bothptcut","Truth lifetime after both 160MeV daughter pT cut and 800MeV mother pT cut;#tau [ns]",100,0.,0.5);
  TH1F* lifetime_reco = new TH1F("lifetime_reco","Truth lifetime for reconstructible mothers;#tau [ns]",100,0.,0.5);
  TH1F* lifetime_fromlength = new TH1F("lifetime_fromlength","Truth lifetime from decay length;#tau [ns]",100,0.,0.5);
  TH1F* lifetime_diffmethods = new TH1F("lifetime_diffmethods","Truth lifetime difference from decay time - decay length;#Delta#tau [ns]",100,-0.02,0.02);
  TH1F* lifetime_smeared = new TH1F("lifetime_smeared","Truth lifetime after resolution smearing;#tau [ns]",100,0.,0.5);
  TH1F* lifetime_smear_amount = new TH1F("lifetime_smear_amount","Amount of smearing applied;#Delta#tau [ns]",100,-0.05,0.05);
  TH1F* lifetime_resolution = new TH1F("lifetime_resolution","Lifetime resolution;#sigma_{#tau} [ns]",100,0.,0.1);

  TH1F* lifetime_labframe = new TH1F("lifetime_labframe","Lab-frame truth lifetime;t [ns]",100,0.,0.5);
  TH1F* lifetime_labframe_ptcut = new TH1F("lifetime_labframe_ptcut","Lab-frame truth lifetime after 160MeV daughter pT cut",100,0.,0.5);
  TH1F* lifetime_labframe_motherptcut = new TH1F("lifetime_labframe_motherptcut","Lab-frame truth lifetime after 800MeV mother pT cut",100,0.,0.5);
  TH1F* lifetime_labframe_bothptcut = new TH1F("lifetime_labframe_bothptcut","Lab-frame truth lifetime after both 160MeV daughter pT cut and 800MeV mother pT cut",100,0.,0.5);

  TH2F* lifetime_vsmotherpt = new TH2F("lifetime_vsmotherpt","Truth lifetime vs. mother pT;t [ns];mother pT [GeV]",100,0.,0.5,100,0.,3.);
  TH2F* lifetime_vstrack1pt = new TH2F("lifetime_vstrack1pt","Truth lifetime vs. track 1 pT;t [ns];track 1 pT [GeV]",100,0.,0.5,100,0.,3.);
  TH2F* lifetime_vstrack2pt = new TH2F("lifetime_vstrack2pt","Truth lifetime vs. track 2 pT;t [ns];track 2 pT [GeV]",100,0.,0.5,100,0.,3.);

  TH2F* lifetime_labframe_vsmotherpt = new TH2F("lifetime_labframe_vsmotherpt","Lab-frame truth lifetime vs. mother pT;t [ns];mother pT [GeV]",100,0.,0.5,100,0.,3.);
  TH2F* lifetime_labframe_vstrack1pt = new TH2F("lifetime_labframe_vstrack1pt","Lab-frame truth lifetime vs. track 1 pT;t [ns];track 1 pT [GeV]",100,0.,0.5,100,0.,3.);
  TH2F* lifetime_labframe_vstrack2pt = new TH2F("lifetime_labframe_vstrack2pt","Lab-frame truth lifetime vs. track 2 pT;t [ns];track 2 pT [Gev]",100,0.,0.5,100,0.,3.);

  //for(int i=0; i<100; i++)
  //{
  //  std::cout << "file " << i << std::endl;
  //  std::string filename = indir+"pythia_reco_3122_"+std::to_string(i)+"_g4svtx_eval.root";
  //  TFile* f = TFile::Open(filename.c_str());
  //  TTree* t = (TTree*)f->Get("ntp_gtrack");
    float event;
    float gtrackid;
    float gflavor;
    float gparentflavor;
    float gparentid;
    float gvx;
    float gvy;
    float gvz;
    float gvt;
    float trackid;
    float gpt;
    float gpz;
    t->SetBranchAddress("event",&event);
    t->SetBranchAddress("gtrackID",&gtrackid);
    t->SetBranchAddress("gflavor",&gflavor);
    t->SetBranchAddress("gparentflavor",&gparentflavor);
    t->SetBranchAddress("gparentid",&gparentid);
    t->SetBranchAddress("gvx",&gvx);
    t->SetBranchAddress("gvy",&gvy);
    t->SetBranchAddress("gvz",&gvz);
    t->SetBranchAddress("gvt",&gvt);
    t->SetBranchAddress("trackID",&trackid);
    t->SetBranchAddress("gpt",&gpt);
    t->SetBranchAddress("gpz",&gpz);

    std::vector<int> parent_ids;
    std::vector<int> parent_flavors;
    std::vector<float> primary_gvt;
    std::vector<float> parent_pt;
    std::vector<float> parent_pz;
    std::vector<float> parent_p;
    std::vector<std::array<float,3>> parent_vxyz;

    std::vector<std::vector<int>> daughter_gtrackids;
    std::vector<std::vector<float>> daughter_trackids;
    std::vector<std::vector<int>> daughter_flavors;
    std::vector<std::vector<float>> daughter_pt;
    std::vector<std::vector<float>> daughter_pz;
    std::vector<std::vector<std::array<float,3>>> secondary_vxyz;
    std::vector<std::vector<float>> secondary_vts;

    int current_file = 0;
    int total_nMothers = 0;
    int total_nDaughters = 0;
    int total_nValid = 0;
    int total_aboveptcut = 0;
    int total_abovemotherptcut = 0;
    int total_abovebothptcut = 0;
    int total_nReconstructible = 0;

    int current_event = 0;
    int current_event_start = 0;
    int nMothers = 0;
    int nDaughters = 0;

    // main event loop
    for(int j=0;j<t->GetEntries();j++)
    {
      t->GetEntry(j);

      // check if event changed
      if(event!=current_event)
      {

        if(event==0)
        {
          std::cout << "file " << current_file << std::endl;
          std::cout << "total mothers " << total_nMothers << std::endl;
          std::cout << "total daughters " << total_nDaughters << std::endl;
          std::cout << "total valid " << total_nValid << " above pt cut " << total_aboveptcut << " above mother pt cut " << total_abovemotherptcut << " above both pt cuts " << total_abovebothptcut << " reconstructible " << total_nReconstructible << std::endl;
          current_file++;
        }
        //std::cout << "event " << event << std::endl;
        //std::cout << "found " << nMothers << " mothers" << std::endl;

        // process full event
        current_event = event;
        int current_event_end = j-1;
        // find daughters
        for(int k=current_event_start; k<=current_event_end; k++)
        {
          t->GetEntry(k);
          if((int)fabs(round(gparentflavor))==3122)
          {
            auto mother_it = std::find(parent_ids.begin(),parent_ids.end(),gparentid);
            if(mother_it != parent_ids.end())
            {
              const int mother_ind = std::distance(parent_ids.begin(),mother_it);
              daughter_gtrackids[mother_ind].push_back(round(gtrackid));
              daughter_trackids[mother_ind].push_back(trackid);
              daughter_flavors[mother_ind].push_back(round(gflavor));
              daughter_pt[mother_ind].push_back(gpt);
              daughter_pz[mother_ind].push_back(gpz);
              secondary_vxyz[mother_ind].push_back({gvx,gvy,gvz});
              secondary_vts[mother_ind].push_back(gvt);
              nDaughters++;
              total_nDaughters++;
            }
          }
        }

        //std::cout << "found " << nDaughters << " daughters" << std::endl;

        int nValid = 0;
        int nAbovePtCut = 0;
        int nAboveMotherPtCut = 0;
        int nAboveBothPtCut = 0;
        int nReconstructible = 0;

        // filter for valid (has two daughters of correct flavor) and reconstructed (daughters both have non-NaN trackIDs) candidates
        // decay time = secondary vt - primary vt
        for(int lambda_ind = 0; lambda_ind<parent_ids.size(); lambda_ind++)
        {
          const int lambda_parity = (parent_flavors[lambda_ind]>0)? 1:-1;

          // find daughter proton, pi, if they exist
          auto daughter_p_it = std::find(daughter_flavors[lambda_ind].begin(),daughter_flavors[lambda_ind].end(),2212*lambda_parity);
          auto daughter_pi_it = std::find(daughter_flavors[lambda_ind].begin(),daughter_flavors[lambda_ind].end(),-211*lambda_parity);

          if(daughter_p_it != daughter_flavors[lambda_ind].end() && daughter_pi_it != daughter_flavors[lambda_ind].end())
          {
            nValid++;
            total_nValid++;

            segment_out = process*100+current_file;
            event_out = current_event;

            const int daughter_p_ind = std::distance(daughter_flavors[lambda_ind].begin(),daughter_p_it);
            const int daughter_pi_ind = std::distance(daughter_flavors[lambda_ind].begin(),daughter_pi_it);

            mother_vx = parent_vxyz[lambda_ind][0];
            mother_vy = parent_vxyz[lambda_ind][1];
            mother_vz = parent_vxyz[lambda_ind][2];
            mother_vt = primary_gvt[lambda_ind];

            track1_vx = secondary_vxyz[lambda_ind][daughter_p_ind][0];
            track1_vy = secondary_vxyz[lambda_ind][daughter_p_ind][1];
            track1_vz = secondary_vxyz[lambda_ind][daughter_p_ind][2];
            track1_vt = secondary_vts[lambda_ind][daughter_p_ind];

            track2_vx = secondary_vxyz[lambda_ind][daughter_pi_ind][0];
            track2_vy = secondary_vxyz[lambda_ind][daughter_pi_ind][1];
            track2_vz = secondary_vxyz[lambda_ind][daughter_pi_ind][2];
            track2_vt = secondary_vts[lambda_ind][daughter_pi_ind];

            const float lab_decay_time = secondary_vts[lambda_ind][daughter_p_ind] - primary_gvt[lambda_ind];
            lifetime_labframe->Fill(lab_decay_time);
            lifetime_labframe_vsmotherpt->Fill(lab_decay_time,parent_pt[lambda_ind]);
            lifetime_labframe_vstrack1pt->Fill(lab_decay_time,daughter_pt[lambda_ind][daughter_p_ind]);
            lifetime_labframe_vstrack2pt->Fill(lab_decay_time,daughter_pt[lambda_ind][daughter_pi_ind]);

            const float betagamma = parent_p[lambda_ind]/1.115; // = sqrt(gamma^2 - 1)
            const float gamma = sqrt(pow(betagamma,2)+1.);
            const float proper_decay_time = lab_decay_time/gamma;
            //std::cout << "proper time: " << proper_decay_time << std::endl;

            //std::cout << "PV: (" << parent_vxyz[lambda_ind][0] << ", " << parent_vxyz[lambda_ind][1] << ", " << parent_vxyz[lambda_ind][2] << "), SV (" << gvx << ", " << gvy << ", " << gvz << ")" << std::endl;

            const float decay_length = sqrt(pow(secondary_vxyz[lambda_ind][daughter_p_ind][0] - parent_vxyz[lambda_ind][0],2) +
                                            pow(secondary_vxyz[lambda_ind][daughter_p_ind][1] - parent_vxyz[lambda_ind][1],2) +
                                            pow(secondary_vxyz[lambda_ind][daughter_p_ind][2] - parent_vxyz[lambda_ind][2],2));
            const float decay_length_err = 1./decay_length*sqrt(6.*pow(vertex_resolution,2));
            //std::cout << "decay length: " << decay_length << " +- " << decay_length_err << std::endl;

            const float p_err = p_resolution_coef*pow(parent_p[lambda_ind],2);

            const float speed = 2.99792458e1; // ctau [cm] -> ctau [ns]

            const float proper_decay_time_fromdecaylength = 1./speed * mass*decay_length/parent_p[lambda_ind];
            const float proper_decay_time_fromdecaylength_err = 1./speed * sqrt(pow(decay_length/parent_p[lambda_ind],2)*pow(mass_resolution,2) +
                                                                                pow(mass/parent_p[lambda_ind],2)*pow(decay_length_err,2) +
                                                                                pow(mass*decay_length/pow(parent_p[lambda_ind],2),2)*pow(p_err,2));

            lifetime_resolution->Fill(proper_decay_time_fromdecaylength_err);

            //std::cout << "proper time from decay length: " << proper_decay_time_fromdecaylength << " +- " << proper_decay_time_fromdecaylength_err << std::endl;

            const float proper_decay_time_resolution_smear = gsl_ran_gaussian(m_rng.get(),proper_decay_time_fromdecaylength_err);
            lifetime_smear_amount->Fill(proper_decay_time_resolution_smear);
            const float proper_decay_time_smeared = fabs(proper_decay_time_fromdecaylength + proper_decay_time_resolution_smear);
            //std::cout << "proper time, smeared: " << proper_decay_time_smeared << std::endl;

            lifetime_out = proper_decay_time;
            lifetime_labframe_out = lab_decay_time;
            decaylength_out = decay_length;
            decaylength_err_out = decay_length_err;
            p_err_out = p_err;
            lifetime_err_out = proper_decay_time_fromdecaylength_err;
            smear_amount_out = proper_decay_time_resolution_smear;

            track1_isReco = !std::isnan(daughter_trackids[lambda_ind][daughter_p_ind]);
            track2_isReco = !std::isnan(daughter_trackids[lambda_ind][daughter_pi_ind]);
            mother_isReco = track1_isReco && track2_isReco;

            lifetime->Fill(proper_decay_time);
            lifetime_vsmotherpt->Fill(proper_decay_time,parent_pt[lambda_ind]);
            lifetime_vstrack1pt->Fill(proper_decay_time,daughter_pt[lambda_ind][daughter_p_ind]);
            lifetime_vstrack2pt->Fill(proper_decay_time,daughter_pt[lambda_ind][daughter_pi_ind]);
            lifetime_fromlength->Fill(proper_decay_time_fromdecaylength);
            lifetime_diffmethods->Fill(proper_decay_time - proper_decay_time_fromdecaylength);
            lifetime_smeared->Fill(proper_decay_time_smeared);
            if(daughter_pt[lambda_ind][daughter_p_ind]>0.16 && daughter_pt[lambda_ind][daughter_pi_ind]>0.16)
            {
              nAbovePtCut++;
              total_aboveptcut++;
              lifetime_ptcut->Fill(proper_decay_time);
              lifetime_labframe_ptcut->Fill(lab_decay_time);
              if(parent_pt[lambda_ind]>0.8)
              {
                nAboveBothPtCut++;
                total_abovebothptcut++;
                lifetime_bothptcut->Fill(proper_decay_time);
                lifetime_labframe_bothptcut->Fill(lab_decay_time);
              }
            }
            if(parent_pt[lambda_ind]>0.8)
            {
              nAboveMotherPtCut++;
              total_abovemotherptcut++;
              lifetime_motherptcut->Fill(proper_decay_time);
              lifetime_labframe_motherptcut->Fill(lab_decay_time);
            }
            if(!std::isnan(daughter_trackids[lambda_ind][daughter_p_ind]) && !std::isnan(daughter_trackids[lambda_ind][daughter_pi_ind]))
            {
              nReconstructible++;
              total_nReconstructible++;
              lifetime_reco->Fill(proper_decay_time);
            }
          }
        }

        //std::cout << "of these, " << nValid << " were valid, " << nAbovePtCut << " were above pt cut, and " << nReconstructible << " were reconstructible" << std::endl;

        // clean up for next event

        for(int i=0; i<parent_ids.size(); i++)
        {
          daughter_gtrackids[i].clear();
          daughter_trackids[i].clear();
          daughter_flavors[i].clear();
          daughter_pt[i].clear();
          daughter_pz[i].clear();
          secondary_vxyz[i].clear();
          secondary_vts[i].clear();
        }
        daughter_gtrackids.clear();
        daughter_trackids.clear();
        daughter_flavors.clear();
        daughter_pt.clear();
        daughter_pz.clear();
        secondary_vxyz.clear();
        secondary_vts.clear();
        parent_ids.clear();
        parent_flavors.clear();
        primary_gvt.clear();
        parent_p.clear();
        parent_pt.clear();
        parent_pz.clear();
        parent_vxyz.clear();

        nMothers = 0;
        nDaughters = 0;
        nValid = 0;
        nAbovePtCut = 0;
        nAboveMotherPtCut = 0;
        nAboveBothPtCut = 0;
        nReconstructible = 0;
        current_event_start = j;
        t->GetEntry(j);
      }

      // find mothers
      if(gflavor==3122)
      {
        parent_ids.push_back(round(gtrackid));
        parent_flavors.push_back(round(gflavor));
        primary_gvt.push_back(gvt);
        parent_p.push_back(sqrt(pow(gpt,2)+pow(gpz,2)));
        parent_pt.push_back(gpt);
        parent_pz.push_back(gpz);
        parent_vxyz.push_back({gvx,gvy,gvz});
        daughter_gtrackids.emplace_back();
        daughter_trackids.emplace_back();
        daughter_flavors.emplace_back();
        daughter_pt.emplace_back();
        secondary_vxyz.emplace_back();
        secondary_vts.emplace_back();
        nMothers++;
        total_nMothers++;
      }
    }
  TCanvas* ctruth = new TCanvas("ctruth","ctruth",600,600);
  lifetime->Scale(1./lifetime->GetEntries());
  lifetime_reco->Scale(1./lifetime_reco->GetEntries());
  lifetime_ptcut->Scale(1./lifetime_ptcut->GetEntries());
  lifetime->Draw();
  lifetime_reco->SetLineColor(kRed);
  lifetime_reco->Draw("SAME");
  lifetime_ptcut->SetLineColor(kBlack);
  lifetime_ptcut->Draw("SAME");
  ctruth->SetLogy();

  TCanvas* cdiff = new TCanvas("cdiff","cdiff",600,600);
  lifetime_diffmethods->Draw();

  TCanvas* csmear = new TCanvas("csmear","csmear",600,600);
  lifetime_fromlength->Scale(1./lifetime_fromlength->GetEntries());
  lifetime_smeared->Scale(1./lifetime_fromlength->GetEntries());
  lifetime_fromlength->SetMaximum(1.1*std::max(lifetime_fromlength->GetMaximum(),lifetime_smeared->GetMaximum()));
  lifetime_fromlength->SetLineColor(kBlack);
  lifetime_fromlength->Draw();
  lifetime_smeared->SetLineColor(kRed);
  lifetime_smeared->Draw("SAME");
  csmear->SetLogy();

  lifetime->Write();
  lifetime_reco->Write();
  lifetime_ptcut->Write();
  lifetime_motherptcut->Write();
  lifetime_bothptcut->Write();
  lifetime_fromlength->Write();
  lifetime_diffmethods->Write();
  lifetime_smeared->Write();
  lifetime_smear_amount->Write();
  lifetime_resolution->Write();

  lifetime_labframe->Write();
  lifetime_labframe_ptcut->Write();
  lifetime_labframe_motherptcut->Write();
  lifetime_labframe_bothptcut->Write();

  lifetime_vsmotherpt->Write();
  lifetime_vstrack1pt->Write();
  lifetime_vstrack2pt->Write();
  lifetime_labframe_vsmotherpt->Write();
  lifetime_labframe_vstrack1pt->Write();
  lifetime_labframe_vstrack2pt->Write();

}
