#include <limits>
#include <vector>
#include <any>
#include <cmath>

using namespace std;
using namespace ROOT::Math;

struct track_params
{
  float event_ID  = numeric_limits<float>::quiet_NaN();
  float track_ID  = numeric_limits<float>::quiet_NaN();
  float parent_ID = numeric_limits<float>::quiet_NaN();
  float PID       = numeric_limits<float>::quiet_NaN();
  float true_pT   = numeric_limits<float>::quiet_NaN();
  float true_eta  = numeric_limits<float>::quiet_NaN();
  float true_phi  = numeric_limits<float>::quiet_NaN();
  float reco_pT   = numeric_limits<float>::quiet_NaN();
  float reco_eta  = numeric_limits<float>::quiet_NaN();
  float reco_phi  = numeric_limits<float>::quiet_NaN();
  float reco_maps = numeric_limits<float>::quiet_NaN();
  float true_vx   = numeric_limits<float>::quiet_NaN();
  float true_vy   = numeric_limits<float>::quiet_NaN();
  float true_vz   = numeric_limits<float>::quiet_NaN();
};

struct mother_params
{
  track_params mother;
  track_params track_1;
  track_params track_2;
  bool hasTrack1 = false;
  bool hasTrack2 = false;
};


void parseEvaluators(string species = "Kshort", int nFiles = 2, int startNumber = 0)
{
  int endNumber = startNumber + nFiles - 1;
  int mother_PDG = species == "Kshort" ? 310 : 3122;
  int track_1_PDG = species == "Kshort" ? 211 : 2212;
  int track_2_PDG = species == "Kshort" ? -211 : 211;

  string header = "/sphenix/tg/tg01/hf/frawley/sims/pythia_fromhits_detroit_volcut/pythia_reco_" + to_string(mother_PDG) + "_";
  string trailer = "_g4svtx_eval.root";

  string gtrack_string = "ntp_gtrack";

  vector<mother_params> accepted_mothers;

  int tmp = 0;
  int barWidth = 50;

  for (int i = 0; i < nFiles; ++i)
  {
    if (tmp != (int)100*i/nFiles)
    {
      tmp = (int)100*i/nFiles;
      if ((tmp%1)  == 0)
      {
        cout << "[";
        int pos = barWidth * tmp/100;
        for (int k = 0; k < barWidth; ++k)
        {
          if (k < pos) cout << "=";
          else if (k == pos) cout << ">";
          else cout << " ";
        }
        cout << "] " << tmp << " %\r";
        cout.flush();
      }
    } 
    vector<track_params> all_track_2s, all_track_1s;
    vector<mother_params> mothers;  
 
    string inString = header + to_string(i + startNumber) + trailer;
    TFile* inFile = new TFile(inString.c_str());

    TTree* gtrackTree = (TTree*)inFile->Get(gtrack_string.c_str());
    float event;         gtrackTree->SetBranchAddress("event",         &event);
    float gprimary;      gtrackTree->SetBranchAddress("gprimary",      &gprimary);
    float gflavor;       gtrackTree->SetBranchAddress("gflavor",       &gflavor);
    float gtrackID;      gtrackTree->SetBranchAddress("gtrackID",      &gtrackID);
    float gparentflavor; gtrackTree->SetBranchAddress("gparentflavor", &gparentflavor);
    float gparentid;     gtrackTree->SetBranchAddress("gparentid",     &gparentid);
    float gnmaps;        gtrackTree->SetBranchAddress("gnmaps",        &gnmaps);
    float gpt;           gtrackTree->SetBranchAddress("gpt",           &gpt);
    float geta;          gtrackTree->SetBranchAddress("geta",          &geta);
    float gphi;          gtrackTree->SetBranchAddress("gphi",          &gphi);
    float pt;            gtrackTree->SetBranchAddress("pt",            &pt);
    float eta;           gtrackTree->SetBranchAddress("eta",           &eta);
    float phi;           gtrackTree->SetBranchAddress("phi",           &phi);
    float nmaps;         gtrackTree->SetBranchAddress("nmaps",         &nmaps);
    float gvx;           gtrackTree->SetBranchAddress("gvx",           &gvx);
    float gvy;           gtrackTree->SetBranchAddress("gvy",           &gvy);
    float gvz;           gtrackTree->SetBranchAddress("gvz",           &gvz);

    unsigned int gtrack_entries = gtrackTree->GetEntries();

    for (unsigned int j = 0; j < gtrack_entries; ++j)
    {
      gtrackTree->GetEntry(j);

      bool isMother = abs(gflavor) == mother_PDG;
      bool isTrack1 = species == "Kshort" ? gflavor == track_1_PDG : abs(gflavor) == track_1_PDG;
      bool isTrack2 = species == "Kshort" ? gflavor == track_2_PDG : abs(gflavor) == track_2_PDG;

      if (isMother || isTrack1 || isTrack2) //Only care about the mother, track_2s and track_1s
      {
        if (isMother && (gprimary != 1)) continue; //Skip particles that are the mother but are NOT prompt

        if (isMother)
        {
          mother_params particle;
          particle.mother.event_ID = event;
          particle.mother.track_ID = gtrackID;
          particle.mother.PID = gflavor;
          particle.mother.true_pT = gpt;
          particle.mother.true_eta = geta;
          particle.mother.true_phi = gphi;
          particle.mother.true_vx = gvx;
          particle.mother.true_vy = gvy;
          particle.mother.true_vz = gvz;
          mothers.push_back(particle);
          continue;
        }

        track_params particle;
        particle.event_ID = event;
        particle.track_ID = gtrackID;
        particle.parent_ID = gparentid;
        particle.PID = gflavor;
        particle.true_pT = gpt;
        particle.true_eta = geta;
        particle.true_phi = gphi;
        particle.true_vx = gvx;
        particle.true_vy = gvy;
        particle.true_vz = gvz;
        particle.reco_maps = nmaps;
        if (nmaps > 0)
        {
          particle.reco_pT = pt;
          particle.reco_eta = eta;
          particle.reco_phi = phi;
        }

        if (isTrack1) all_track_1s.push_back(particle);
        else all_track_2s.push_back(particle);
      }
    }

    //OK, we have all mothers, track_1s and track_2s. Let's match!
    pair<string, vector<track_params>> track_pairs[2];
    track_pairs[0] = {"track_1", all_track_1s};
    track_pairs[1] = {"track_2", all_track_2s};
    for (auto &data : track_pairs)
    {
      for (auto &track : data.second)
      {
        auto it = std::find_if(mothers.begin(), mothers.end(), [&](const mother_params& candidate)
        {
            return candidate.mother.event_ID == track.event_ID && candidate.mother.track_ID == track.parent_ID;
        });

        if (it != mothers.end())
        {
          if (data.first == "track_1")
          {
            it->hasTrack1 = true;
            it->track_1 = track;
          }
          else
          {
            it->hasTrack2 = true;
            it->track_2 = track;
          }
        }
      }
    }

    inFile->Close();

    for (auto &mother : mothers)
    {
      if (mother.hasTrack1 && mother.hasTrack2) accepted_mothers.push_back(mother);
    }
  }

  cout << "[";
  int pos = barWidth * tmp;
  for (int i = 0; i < barWidth; ++i)
  {
    if (i < pos) cout << "=";
    else if (i == pos) cout << ">";
    else cout << " ";
  }
  cout << "] 100 %\r";
  cout.flush();
  cout<<endl;

  string outString = "/sphenix/user/cdean/private/files/LightFlavor/evaluator_skimmed_files/prompt_" + species + "_" + to_string(startNumber) + "_to_" + to_string(endNumber) + ".root";
  TFile* outFile = new TFile(outString.c_str(), "RECREATE");
  TTree* outTree = new TTree("TruthInfo", "TruthInfo");
  outTree->OptimizeBaskets();
  outTree->SetAutoSave(-5e6);  // Save the output file every 5MB

  float mother_PID;          outTree->Branch("mother_PID",         &mother_PID,         "mother_PID/F");
  float mother_true_vx;      outTree->Branch("mother_true_vx",     &mother_true_vx,     "mother_true_vx/F");
  float mother_true_vy;      outTree->Branch("mother_true_vy",     &mother_true_vy,     "mother_true_vy/F");
  float mother_true_vz;      outTree->Branch("mother_true_vz",     &mother_true_vz,     "mother_true_vz/F");
  float mother_true_mass;    outTree->Branch("mother_true_mass",   &mother_true_mass,   "mother_true_mass/F");
  float mother_reco_mass;    outTree->Branch("mother_reco_mass",   &mother_reco_mass,   "mother_reco_mass/F");
  float mother_true_pT;      outTree->Branch("mother_true_pT",     &mother_true_pT,     "mother_true_pT/F");
  float mother_reco_pT;      outTree->Branch("mother_reco_pT",     &mother_reco_pT,     "mother_reco_pT/F");
  float mother_true_eta;     outTree->Branch("mother_true_eta",    &mother_true_eta,    "mother_true_eta/F");
  float mother_true_phi;     outTree->Branch("mother_true_phi",    &mother_true_phi,    "mother_true_phi/F");
  float mother_true_rap;     outTree->Branch("mother_true_rap",    &mother_true_rap,    "mother_true_rap/F");
  float track_1_PID;         outTree->Branch("track_1_PID",        &track_1_PID,        "track_1_PID/F");
  float track_1_true_vx;     outTree->Branch("track_1_true_vx",    &track_1_true_vx,    "track_1_true_vx/F");
  float track_1_true_vy;     outTree->Branch("track_1_true_vy",    &track_1_true_vy,    "track_1_true_vy/F");
  float track_1_true_vz;     outTree->Branch("track_1_true_vz",    &track_1_true_vz,    "track_1_true_vz/F");
  float track_1_true_pT;     outTree->Branch("track_1_true_pT",    &track_1_true_pT,    "track_1_true_pT/F");
  float track_1_true_eta;    outTree->Branch("track_1_true_eta",   &track_1_true_eta,   "track_1_true_eta/F");
  float track_1_true_phi;    outTree->Branch("track_1_true_phi",   &track_1_true_phi,   "track_1_true_phi/F");
  float track_1_reco_pT;     outTree->Branch("track_1_reco_pT",    &track_1_reco_pT,    "track_1_reco_pT/F");
  float track_1_reco_eta;    outTree->Branch("track_1_reco_eta",   &track_1_reco_eta,   "track_1_reco_eta/F");
  float track_1_reco_phi;    outTree->Branch("track_1_reco_phi",   &track_1_reco_phi,   "track_1_reco_phi/F");
  float track_1_reco_nmaps;  outTree->Branch("track_1_reco_nmaps", &track_1_reco_nmaps, "track_1_reco_nmaps/F");
  float track_2_PID;         outTree->Branch("track_2_PID",        &track_2_PID,        "track_2_PID/F");
  float track_2_true_vx;     outTree->Branch("track_2_true_vx",    &track_2_true_vx,    "track_2_true_vx/F");
  float track_2_true_vy;     outTree->Branch("track_2_true_vy",    &track_2_true_vy,    "track_2_true_vy/F");
  float track_2_true_vz;     outTree->Branch("track_2_true_vz",    &track_2_true_vz,    "track_2_true_vz/F");
  float track_2_true_pT;     outTree->Branch("track_2_true_pT",    &track_2_true_pT,    "track_2_true_pT/F");
  float track_2_true_eta;    outTree->Branch("track_2_true_eta",   &track_2_true_eta,   "track_2_true_eta/F");
  float track_2_true_phi;    outTree->Branch("track_2_true_phi",   &track_2_true_phi,   "track_2_true_phi/F");
  float track_2_reco_pT;     outTree->Branch("track_2_reco_pT",    &track_2_reco_pT,    "track_2_reco_pT/F");
  float track_2_reco_eta;    outTree->Branch("track_2_reco_eta",   &track_2_reco_eta,   "track_2_reco_eta/F");
  float track_2_reco_phi;    outTree->Branch("track_2_reco_phi",   &track_2_reco_phi,   "track_2_reco_phi/F");
  float track_2_reco_nmaps;  outTree->Branch("track_2_reco_nmaps", &track_2_reco_nmaps, "track_2_reco_nmaps/F");

  cout << "Writing accepted mothers to TTree" << endl;

  for (auto &candidate : accepted_mothers)
  {
    mother_PID         = candidate.mother.PID;
    mother_true_pT     = candidate.mother.true_pT;
    mother_true_eta    = candidate.mother.true_eta;
    mother_true_phi    = candidate.mother.true_phi;
    mother_true_vx     = candidate.mother.true_vx;
    mother_true_vy     = candidate.mother.true_vy;
    mother_true_vz     = candidate.mother.true_vz;

    track_1_PID        = candidate.track_1.PID;
    track_1_true_pT    = candidate.track_1.true_pT;
    track_1_true_eta   = candidate.track_1.true_eta;
    track_1_true_phi   = candidate.track_1.true_phi;
    track_1_true_vx    = candidate.track_1.true_vx;
    track_1_true_vy    = candidate.track_1.true_vy;
    track_1_true_vz    = candidate.track_1.true_vz;
    track_1_reco_nmaps = candidate.track_1.reco_maps;
    track_1_reco_pT    = candidate.track_1.reco_pT; 
    track_1_reco_eta   = candidate.track_1.reco_eta; 
    track_1_reco_phi   = candidate.track_1.reco_phi; 

    track_2_PID        = candidate.track_2.PID;
    track_2_true_pT    = candidate.track_2.true_pT;
    track_2_true_eta   = candidate.track_2.true_eta;
    track_2_true_phi   = candidate.track_2.true_phi;
    track_2_true_vx    = candidate.track_2.true_vx;
    track_2_true_vy    = candidate.track_2.true_vy;
    track_2_true_vz    = candidate.track_2.true_vz;
    track_2_reco_nmaps = candidate.track_2.reco_maps;
    track_2_reco_pT    = candidate.track_2.reco_pT; 
    track_2_reco_eta   = candidate.track_2.reco_eta; 
    track_2_reco_phi   = candidate.track_2.reco_phi;

    float track_1_mass = TDatabasePDG::Instance()->GetParticle(track_1_PID)->Mass();
    float track_2_mass = TDatabasePDG::Instance()->GetParticle(track_2_PID)->Mass();
    PtEtaPhiMVector track_1_true_LV = PtEtaPhiMVector(track_1_true_pT, track_1_true_eta, track_1_true_phi, track_1_mass);
    PtEtaPhiMVector track_2_true_LV = PtEtaPhiMVector(track_2_true_pT, track_2_true_eta, track_2_true_phi, track_2_mass);

    PtEtaPhiMVector mother_true_LV = track_1_true_LV + track_2_true_LV;

    mother_true_mass = mother_true_LV.M();
    mother_true_rap  = mother_true_LV.Rapidity();

    if (track_1_reco_nmaps > 0 && track_2_reco_nmaps > 0)
    {
      PtEtaPhiMVector track_1_reco_LV = PtEtaPhiMVector(track_1_reco_pT, track_1_reco_eta, track_1_reco_phi, track_1_mass);
      PtEtaPhiMVector track_2_reco_LV = PtEtaPhiMVector(track_2_reco_pT, track_2_reco_eta, track_2_reco_phi, track_2_mass);

      PtEtaPhiMVector mother_reco_LV = track_1_reco_LV + track_2_reco_LV;
      mother_reco_mass = mother_reco_LV.M();
      mother_reco_pT = mother_reco_LV.Pt();
    }
    else
    {
      mother_reco_mass = numeric_limits<float>::quiet_NaN(); 
      mother_reco_pT = numeric_limits<float>::quiet_NaN();
    }

    outTree->Fill(); 
  }

  outTree->Write();
  outFile->Close();
}
