struct TrackData
{
  int gtrackID;
  int gflavor;
  std::array<float,3> gvertex;
  std::array<float,3> gp;
};

void pythia_origin(int process=1)
{
  TChain* f = new TChain("ntp_gtrack");

  std::string file_base = "/sphenix/tg/tg01/hf/frawley/sims/pythia_fromhits_detroit/pythia_reco_3122_";

  if(process==0)
  {
    f->Add((file_base+"[0-9]_g4svtx_eval.root").c_str());
    f->Add((file_base+"[0-9][0-9]_g4svtx_eval.root").c_str());
  }
  else
  {
    f->Add((file_base+std::to_string(process)+"[0-9][0-9]_g4svtx_eval.root").c_str());
  }

  //f->Add("/sphenix/tg/tg01/hf/frawley/sims/pythia_fromhits_detroit/pythia_reco_3122_[0-9]_g4svtx_eval.root");
  //f->Add("/sphenix/tg/tg01/hf/frawley/sims/pythia_fromhits_detroit_volcut/pythia_reco_310_[0-9]_g4svtx_eval.root");
  //f->Add("/sphenix/tg/tg01/hf/cdean/LF_analysis/evaluatorFiles/Lambda2ppi/20260504/evalFile_Lambda2ppi_000[0-9][0-9].root");
  //f->Add("/sphenix/tg/tg01/hf/cdean/LF_analysis/evaluatorFiles/Kshort2pipi/20260504/evalFile_Kshort2pipi_000[0-9][0-9].root");

//  const int mother_pdgid = 310;
  const int mother_pdgid = 3122;

  float f_event;
  float f_gtrackID;
  float f_gflavor;
  float f_gparentflavor;
  float f_gparentid;
  float f_gprimary;
  float gvx;
  float gvy;
  float gvz;
  float gpx;
  float gpy;
  float gpz;

  f->SetBranchAddress("event",&f_event);
  f->SetBranchAddress("gtrackID",&f_gtrackID);
  f->SetBranchAddress("gflavor",&f_gflavor);
  f->SetBranchAddress("gparentflavor",&f_gparentflavor);
  f->SetBranchAddress("gparentid",&f_gparentid);
  f->SetBranchAddress("gprimary",&f_gprimary);
  f->SetBranchAddress("gvx",&gvx);
  f->SetBranchAddress("gvy",&gvy);
  f->SetBranchAddress("gvz",&gvz);
  f->SetBranchAddress("gpx",&gpx);
  f->SetBranchAddress("gpy",&gpy);
  f->SetBranchAddress("gpz",&gpz);

  std::map<int,int> parent_flavor_count;
  std::multimap<int,int> thisevent_nextID_targets;
  std::map<int,std::vector<TrackData>> thisevent_history;

  size_t current_file=0;
  size_t current_event=0;
  size_t current_event_start=0;

  size_t total_parents=0;

  int file_out;
  int event_out;
  std::vector<int> gflavor_out;
  std::vector<int> gtrackID_out;
  std::vector<float> gvx_out;
  std::vector<float> gvy_out;
  std::vector<float> gvz_out;
  std::vector<float> gpx_out;
  std::vector<float> gpy_out;
  std::vector<float> gpz_out;

  TFile* fout = new TFile(("/sphenix/user/mjpeters/analysis/LightFlavorRatios/swimming_correction/output/histout_"+std::to_string(process)+".root").c_str(),"RECREATE");
  TTree* tout = new TTree("history","Candidate history");
  tout->Branch("file",&file_out);
  tout->Branch("event",&event_out);
  tout->Branch("gtrackID_history",&gtrackID_out);
  tout->Branch("gflavor_history",&gflavor_out);
  tout->Branch("gvx_history",&gvx_out);
  tout->Branch("gvy_history",&gvy_out);
  tout->Branch("gvz_history",&gvz_out);
  tout->Branch("gpx_history",&gpx_out);
  tout->Branch("gpy_history",&gpy_out);
  tout->Branch("gpz_history",&gpz_out);

  for(size_t i=0;i<f->GetEntries();i++)
  {
    //if(i % 10000 == 0) std::cout << "entry " << i << std::endl;
    f->GetEntry(i);

    int event = round(f_event);
    int gtrackID = round(f_gtrackID);
    int gflavor = round(f_gflavor);
    int gparentflavor = round(f_gparentflavor);
    int gparentid = round(f_gparentid);
    bool gprimary = ((int)round(f_gprimary) == 1);

    if(event!=current_event)
    {
      //std::cout << "event " << event << std::endl;
      while(thisevent_nextID_targets.size()!=0)
      {
        std::multimap<int,int> new_thisevent_nextID_targets;
        for(size_t j=current_event_start;j<i;j++)
        {
          f->GetEntry(j);

          int event_j = round(f_event);
          int gtrackID_j = round(f_gtrackID);
          int gflavor_j = round(f_gflavor);
          int gparentflavor_j = round(f_gparentflavor);
          int gparentid_j = round(f_gparentid);
          bool gprimary_j = ((int)round(f_gprimary) == 1);

          auto target_range = thisevent_nextID_targets.equal_range(gtrackID_j);

          if(target_range.first!=target_range.second)
          {
            for(auto it = target_range.first; it!=target_range.second; ++it)
            {
              int target_gtrackID = (*it).second;
              thisevent_history[target_gtrackID].push_back({.gtrackID = gtrackID_j, .gflavor = gflavor_j, .gvertex = {gvx,gvy,gvz}, .gp = {gpx,gpy,gpz}});
              if((gprimary_j && (gparentid_j!=gtrackID_j)) || (!gprimary_j && (gparentid_j==gtrackID_j)))
              {
                std::cout << "WARNING: gprimary condition doesn't make sense!!" << std::endl;
                std::cout << "gprimary " << gprimary_j << " gparentid " << gparentid_j << " gtrackID " << gtrackID_j << std::endl;
              }
              if(!gprimary_j)
              {
                new_thisevent_nextID_targets.insert({gparentid_j,target_gtrackID});
              }
            }
          }
        }
        thisevent_nextID_targets = new_thisevent_nextID_targets;
      }
/*
      if(thisevent_history.size()>0)
      {
        std::cout << "event " << current_event << " history:" << std::endl;
        for(auto const& [trackid, history] : thisevent_history)
        {
          std::cout << "track ID " << trackid << ": ";
          for(const TrackData& h : history)
          {
            std::cout << h.gflavor << " (" << h.gtrackID << ") <- ";
          }
          std::cout << std::endl;
        }
      }
*/
      file_out = current_file;
      event_out = current_event;

      for(auto const& [id, history] : thisevent_history)
      {
        for(const TrackData& h : history)
        {
          gflavor_out.push_back(h.gflavor);
          gtrackID_out.push_back(h.gtrackID);
          gvx_out.push_back(h.gvertex[0]);
          gvy_out.push_back(h.gvertex[1]);
          gvz_out.push_back(h.gvertex[2]);
          gpx_out.push_back(h.gp[0]);
          gpy_out.push_back(h.gp[1]);
          gpz_out.push_back(h.gp[2]);
        }

        tout->Fill();

        gflavor_out.clear();
        gtrackID_out.clear();
        gvx_out.clear();
        gvy_out.clear();
        gvz_out.clear();
        gpx_out.clear();
        gpy_out.clear();
        gpz_out.clear();
      }

      current_event = event;
      current_event_start = i;

      thisevent_nextID_targets.clear();
      thisevent_history.clear();

      f->GetEntry(i);
    }

    if(event==0 && i==current_event_start)
    {
      std::cout << "file " << current_file << std::endl;
      current_file++;
    }

    if(fabs(gflavor)==mother_pdgid)
    {
      if(parent_flavor_count.contains(gparentflavor))
      {
        parent_flavor_count[gparentflavor] += 1;
      }
      else
      {
        parent_flavor_count[gparentflavor] = 1;
      }
      thisevent_history[gtrackID].push_back({.gtrackID = gtrackID, .gflavor = gflavor, .gvertex = {gvx,gvy,gvz}, .gp = {gpx,gpy,gpz}});
      //std::cout << "gtrackID " << gtrackID << " gprimary " << gprimary << " gparentid " << gparentid << std::endl;
      if(!gprimary)
      {
        thisevent_nextID_targets.insert({gparentid,gtrackID});
      }
      total_parents++;
    }
  }

  std::cout << "origin counts:" << std::endl;
  for(auto const& [pdgid, ct] : parent_flavor_count)
  {
    std::cout << pdgid << ": " << ct << " / " << total_parents << " (" << (float)ct/total_parents*100. << "%)" << std::endl;
  }

  fout->Write();

}
