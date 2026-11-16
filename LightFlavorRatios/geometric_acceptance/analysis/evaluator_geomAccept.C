#include "../../util/binning.h"
#include <Math/Vector4D.h>

TH1F* numerator_reco_mass = new TH1F("n_reco_mass","numerator reco mass",1000,0.,3.);
TH1F* denominator_reco_mass = new TH1F("d_reco_mass","denom reco mass",1000,0.,1.);

struct Hists
{
  TH1F* h_pt;
  TH1F* h_eta;
  TH1F* h_phi;
  TH1F* h_y;
  Hists(std::string basename, std::string basetitle)
  {
    h_pt = makeHistogram(basename,basetitle,BinInfo::final_pt_bins);
    h_eta = makeHistogram(basename,basetitle,BinInfo::final_eta_bins);
    h_phi = makeHistogram(basename,basetitle,BinInfo::final_phi_bins);
    h_y = makeHistogram(basename,basetitle,BinInfo::final_rapidity_bins);

    h_pt->Sumw2();
    h_eta->Sumw2();
    h_phi->Sumw2();
    h_y->Sumw2();
  }
  void divide(Hists& numerator, Hists& denominator, float numerator_scale=1., float denominator_scale=1.)
  {
    h_pt->Divide(numerator.h_pt,denominator.h_pt,numerator_scale,denominator_scale);
    h_eta->Divide(numerator.h_eta,denominator.h_eta,numerator_scale,denominator_scale);
    h_phi->Divide(numerator.h_phi,denominator.h_phi,numerator_scale,denominator_scale);
    h_y->Divide(numerator.h_y,denominator.h_y,numerator_scale,denominator_scale);
  }
  void write()
  {
    h_pt->Write();
    h_eta->Write();
    h_phi->Write();
    h_y->Write();
  }
};

struct MotherInfo
{
  int gtrackID;
  ROOT::Math::PtEtaPhiMVector lorentz_vector;
  std::vector<int> daughter_charge;
  std::vector<int> daughter_gflavor;
  std::vector<float> daughter_px;
  std::vector<float> daughter_py;
  std::vector<float> daughter_pz;
  std::vector<float> daughter_E;

  MotherInfo() {}

  MotherInfo(int id, ROOT::Math::PtEtaPhiMVector lv)
  : gtrackID(id), lorentz_vector(lv)
  {}
  void add_daughter(int charge, int flavor, float px, float py, float pz)
  {
    daughter_charge.push_back(charge);
    daughter_gflavor.push_back(flavor);
    daughter_px.push_back(px);
    daughter_py.push_back(py);
    daughter_pz.push_back(pz);
    double mass = 0.;
    if(TDatabasePDG::Instance()->GetParticle(flavor))
    {
      mass = TDatabasePDG::Instance()->GetParticle(flavor)->Mass();
    }
    double E = sqrt(pow(px,2)+pow(py,2)+pow(pz,2)+pow(mass,2));
    daughter_E.push_back(E);
  }

  MotherInfo(const MotherInfo& m)
  {
    gtrackID = m.gtrackID;
    lorentz_vector = m.lorentz_vector;
    daughter_charge = m.daughter_charge;
    daughter_gflavor = m.daughter_gflavor;
    daughter_px = m.daughter_px;
    daughter_py = m.daughter_py;
    daughter_pz = m.daughter_pz;
    daughter_E = m.daughter_E;
  }
};

bool check_charge_swaps(MotherInfo& sv_info, std::vector<int> required_daughter_flavors, bool parity_inclusive)
{
  // get the set of "effective" PDG IDs after any charge swaps
  std::vector<int> reco_daughter_PDGIDs;
  for(int i=0;i<sv_info.daughter_gflavor.size();i++)
  {
    int gflavor = sv_info.daughter_gflavor[i];
    const int reco_charge = sv_info.daughter_charge[i];
    const int truth_charge = gflavor > 0 ? 1 : -1;
    
    // charges match if their product is 1
    if(reco_charge*truth_charge == 1)
    {
      reco_daughter_PDGIDs.push_back(gflavor);
    }
    else
    {
      reco_daughter_PDGIDs.push_back(-1*gflavor);
    }
  }

  // check to see that we still have the right mix of flavors to form a secondary vertex
  bool has_all_daughters = std::all_of(required_daughter_flavors.begin(),required_daughter_flavors.end(),
    [&](int flavor){ return std::find(reco_daughter_PDGIDs.begin(),reco_daughter_PDGIDs.end(),flavor) != reco_daughter_PDGIDs.end(); });
  bool has_all_opposite_daughters = std::all_of(required_daughter_flavors.begin(),required_daughter_flavors.end(),
    [&](int flavor){ return std::find(reco_daughter_PDGIDs.begin(),reco_daughter_PDGIDs.end(),-1*flavor) != reco_daughter_PDGIDs.end(); });
/*
  std::cout << "daughter PDGIDs: ";
  for(int id : sv_info.daughter_gflavor) std::cout << id << ", ";
  std::cout << std::endl;

  std::cout << "reco daughter PDGIDs: ";
  for(int id : reco_daughter_PDGIDs) std::cout << id << ", ";
  std::cout << std::endl;

  std::cout << "required daughter PDGIDs: ";
  for(int id : required_daughter_flavors) std::cout << id << ", ";
  std::cout << std::endl;

  if(has_all_daughters) std::cout << "has all daughters" << std::endl;
  if(parity_inclusive && has_all_opposite_daughters) std::cout << "has all opposite daughters" << std::endl;
*/
  return (has_all_daughters || (parity_inclusive && has_all_opposite_daughters));
}

std::vector<Hists> process_tree(TTree* t, std::string basename, std::string basetitle, int mother_flavor, std::vector<int> required_daughter_flavors, bool parity_inclusive)
{
  const float mother_mass = TDatabasePDG::Instance()->GetParticle(mother_flavor)->Mass();
  const int mother_charge = TDatabasePDG::Instance()->GetParticle(mother_flavor)->Charge();

  std::vector<Hists> vh;
  vh.emplace_back(basename,basetitle);
  vh.emplace_back(basename+"_wd",basetitle+" with daughters");

  float fevent;
  float gflavor;
  float gtrackID;
  float rtrackID;
  float rnmaps;
  float rcharge;
  float px;
  float py;
  float pz;
  float rvertexID;
  float gparentflavor;
  float gparentID;
  float gvz;
  float gpt;
  float gpz;
  float geta;
  float gphi;
  float gprimary;
  float gprimaryid;

  t->SetBranchAddress("event",&fevent);
  t->SetBranchAddress("gflavor",&gflavor);
  t->SetBranchAddress("gtrackID",&gtrackID);
  t->SetBranchAddress("trackID",&rtrackID);
  t->SetBranchAddress("nmaps",&rnmaps);
  t->SetBranchAddress("charge",&rcharge);
  t->SetBranchAddress("px",&px);
  t->SetBranchAddress("py",&py);
  t->SetBranchAddress("pz",&pz);
  t->SetBranchAddress("vertexID",&rvertexID);
  t->SetBranchAddress("gparentflavor",&gparentflavor);
  t->SetBranchAddress("gparentid",&gparentID);
  t->SetBranchAddress("gvz",&gvz);
  t->SetBranchAddress("gpt",&gpt);
  t->SetBranchAddress("gpz",&gpz);
  t->SetBranchAddress("geta",&geta);
  t->SetBranchAddress("gphi",&gphi);
  t->SetBranchAddress("gprimary",&gprimary);
  t->SetBranchAddress("gprimaryid",&gprimaryid);

  size_t current_event = 0;
  size_t current_event_start_entry = 0;

  // key: mother gtrackID
  std::map<int,MotherInfo> all_daughter_map;
  std::map<int,MotherInfo> valid_daughter_map;

  for(size_t i=0; i<t->GetEntries(); i++)
  {
    t->GetEntry(i);
    int event = round(fevent);

    if(event != current_event)
    {
      if(event % 100 == 0) std::cout << "event " << current_event << std::endl;
      // gather all reconstructible daughters for each mother
      // where "reconstructible" means:
      // - there is a reco track
      // - that reco track has MVTX hits
      // - the event has at least one primary vertex (KFParticle skips the event entirely if it doesn't have a primary vertex)
      std::set<int> vertexIDs;
      for(size_t j=current_event_start_entry; j<i; j++)
      {
        t->GetEntry(j);
        // don't round reco trackID, this gets rid of NaN-ness on conversion to int
        int parentID = round(gparentID);
        //std::cout << "parentID = " << parentID << std::endl;
        int flavor = round(gflavor);
        int nmaps = round(rnmaps);
        int charge = round(rcharge);
        int vertexID = round(rvertexID);
        vertexIDs.insert(vertexID);
        //std::cout << "flavor = " << flavor << std::endl;
        //std::cout << "trackID = " << rtrackID << std::endl;
        //std::cout << "map count = " << current_daughter_map.count(parentID) << std::endl;
        
        // count all daughter tracks for each mother, mainly for cutting on the correct branching ratio
        if(all_daughter_map.count(parentID)>0)
        {
          all_daughter_map.at(parentID).add_daughter(charge,flavor,px,py,pz);
        }

        // valid reco tracks must:
        // - exist
        // - have silicon hits
        // - have a valid mother
        if(!std::isnan(rtrackID) && nmaps>0 && valid_daughter_map.count(parentID)>0)
        {
          valid_daughter_map.at(parentID).add_daughter(charge,flavor,px,py,pz);
        }
      }
      //std::cout << "----------------------------------------------" << std::endl;

      // find all mothers that decayed to the correct daughters
      for(auto [motherID, sv_info] : all_daughter_map)
      {
        ROOT::Math::PtEtaPhiMVector mother_lorentzvector = sv_info.lorentz_vector;
        std::vector<int> daughter_flavors = sv_info.daughter_gflavor;

        bool has_all_daughters = std::all_of(required_daughter_flavors.begin(),required_daughter_flavors.end(),
          [&](int flavor){ return std::find(daughter_flavors.begin(),daughter_flavors.end(),flavor) != daughter_flavors.end(); });
        bool has_all_opposite_daughters = std::all_of(required_daughter_flavors.begin(),required_daughter_flavors.end(),
          [&](int flavor){ return std::find(daughter_flavors.begin(),daughter_flavors.end(),-1*flavor) != daughter_flavors.end(); });

        if(has_all_daughters || (parity_inclusive && has_all_opposite_daughters))
        {
          vh[0].h_pt->Fill(mother_lorentzvector.Pt());
          vh[0].h_eta->Fill(mother_lorentzvector.Eta());
          vh[0].h_phi->Fill(mother_lorentzvector.Phi());
          vh[0].h_y->Fill(mother_lorentzvector.Rapidity());
        }
      }

      // if there's at least one assigned vertex (-1 is default no-vertex-assignment value)
      if(vertexIDs.size()>1 || !(vertexIDs.contains(-1)))
      {
        // check list of mothers for correct set of daughter flavors and check reco charge conservation
        // (KFParticle won't even form a secondary vertex if the charges don't properly add up,
        // but it doesn't matter which particle has which charge as long as the sum matches the charge of the mother)
        for(auto [motherID, sv_info] : valid_daughter_map)
        {
          ROOT::Math::PtEtaPhiMVector mother_lorentzvector = sv_info.lorentz_vector;
          std::vector<int> daughter_flavors = sv_info.daughter_gflavor;

          //std::cout << "mother iD = " << motherID << std::endl;
          //std::cout << "daughters: ";
          //for(int df : daughter_flavors) std::cout << df << " ";
          //std::cout << std::endl;
          bool has_all_daughters = std::all_of(required_daughter_flavors.begin(),required_daughter_flavors.end(),
            [&](int flavor){ return std::find(daughter_flavors.begin(),daughter_flavors.end(),flavor) != daughter_flavors.end(); });
          bool has_all_opposite_daughters = std::all_of(required_daughter_flavors.begin(),required_daughter_flavors.end(),
            [&](int flavor){ return std::find(daughter_flavors.begin(),daughter_flavors.end(),-1*flavor) != daughter_flavors.end(); });
          //if(has_all_daughters) std::cout << "has all daughters" << std::endl;
          //if(has_all_opposite_daughters) std::cout << "has all opposite daughters" << std::endl;

          if((has_all_daughters || (parity_inclusive && has_all_opposite_daughters)) && check_charge_swaps(sv_info,required_daughter_flavors,parity_inclusive))
          {
            vh[1].h_pt->Fill(mother_lorentzvector.Pt());
            vh[1].h_eta->Fill(mother_lorentzvector.Eta());
            vh[1].h_phi->Fill(mother_lorentzvector.Phi());
            vh[1].h_y->Fill(mother_lorentzvector.Rapidity());
            
            double total_px = 0.;
            double total_py = 0.;
            double total_pz = 0.;
            double total_E = 0.;

            for(int ifl=0; ifl<daughter_flavors.size(); ifl++)
            {
              if((abs(mother_flavor)==3122 && (abs(daughter_flavors[ifl])==211 || abs(daughter_flavors[ifl])==2212)) || (abs(mother_flavor)==310 && abs(daughter_flavors[ifl])==211))
              {
                total_px += sv_info.daughter_px[ifl];
                total_py += sv_info.daughter_py[ifl];
                total_pz += sv_info.daughter_pz[ifl];
                total_E += sv_info.daughter_E[ifl];
              }
            }

            double reco_inv_mass = sqrt(pow(total_E,2)-pow(total_px,2)-pow(total_py,2)-pow(total_pz,2));
            if(abs(mother_flavor)==3122) numerator_reco_mass->Fill(reco_inv_mass);
            if(abs(mother_flavor)==310) denominator_reco_mass->Fill(reco_inv_mass);
          }
        }
      }
      // clean up
      all_daughter_map.clear();
      valid_daughter_map.clear();
      vertexIDs.clear();
      current_event = event;
      current_event_start_entry = i;
      t->GetEntry(i);
    }

    // check if this is a valid mother
    int primary = round(gprimary);
    int flavor = round(gflavor);
    int parentflavor = round(gparentflavor);
    int trackID = round(gtrackID);
    int parentID = round(gparentID);
    int primaryID = round(gprimaryid);
    
    //std::cout << "gtrackID = " << gtrackID << " flavor = " << flavor << " parentID = " << parentID << std::endl;
    bool correct_flavor = (flavor == mother_flavor || (parity_inclusive && abs(flavor)==mother_flavor));
    //bool is_duplicate = (parentflavor == flavor && parentID != trackID);
    bool is_primary = (primary==1);
    // handle K0->Ks oscillation
    // sometimes they're primary, other times they get "decayed" from a primary K0
    if(flavor==310) is_primary = (primary==1) || (abs(parentflavor)==311 && parentID == primaryID);
    //if(correct_flavor) std::cout << "correct flavor" << std::endl;
    //if(is_duplicate) std::cout << "is duplicate, parentID = " << parentID << ", parentflavor = " << parentflavor << std::endl;
    if(is_primary && correct_flavor)// && !is_duplicate)
    {
      ROOT::Math::PtEtaPhiMVector mother_lorentzvector(gpt, geta, gphi, mother_mass);
      float rapidity = mother_lorentzvector.Rapidity();

      // add to mother histograms and create new daughter map entry
      if(gpt>=0.6 && gpt<=4. && fabs(geta)<=0.8 && fabs(rapidity)<=0.8 && fabs(gvz)<=10.)
      {
        //vh[0].h_pt->Fill(gpt);
        //vh[0].h_eta->Fill(geta);
        //vh[0].h_phi->Fill(gphi);
        //vh[0].h_y->Fill(rapidity);
        all_daughter_map.insert({trackID,MotherInfo(trackID,mother_lorentzvector)});
        valid_daughter_map.insert({trackID,MotherInfo(trackID,mother_lorentzvector)});
      }
    }
  }

  return vh;
}

void evaluator_geomAccept(int numerator_flavor, std::vector<int> numerator_daughter_flavors,
                           int denominator_flavor, std::vector<int> denominator_daughter_flavors,
                           std::string numerator_particlename, std::string denominator_particlename,
                           std::string numerator_basefile, std::string denominator_basefile,
                           float numerator_scale, float denominator_scale, std::string outfilebase, int process, bool numerator_abs = true, bool denominator_abs = true)
{
  size_t ndigits = 6;
  std::string process_str = std::string(ndigits - std::to_string(process).length(),'0') + std::to_string(process);

  TFile* f_num = TFile::Open((numerator_basefile+"_"+process_str+".root").c_str());
  TFile* f_denom = TFile::Open((denominator_basefile+"_"+process_str+".root").c_str());

  TTree* t_num = (TTree*)f_num->Get("ntp_gtrack");
  TTree* t_denom = (TTree*)f_denom->Get("ntp_gtrack");

  std::string outfname = outfilebase+"_"+process_str+".root";
  TFile* fout = new TFile(outfname.c_str(),"RECREATE");

  std::string numerator_factor = (numerator_scale!=1.)? std::to_string(numerator_scale) : "";
  std::string denominator_factor = (denominator_scale!=1.)? std::to_string(denominator_scale) : "";
  std::string title_r = numerator_factor + numerator_particlename + "/" + denominator_factor + denominator_particlename;

  Hists hists_r("hr",title_r);

  Hists eff_n("eff_n",numerator_particlename + " reconstruction efficiency");
  Hists eff_d("eff_d",denominator_particlename + " reconstruction efficiency");

  std::vector<Hists> vhists_n = process_tree(t_num,"hn",numerator_particlename+" yield",numerator_flavor,numerator_daughter_flavors,numerator_abs);
  std::vector<Hists> vhists_d = process_tree(t_denom,"hd",denominator_particlename+" yield",denominator_flavor,denominator_daughter_flavors,denominator_abs);

  hists_r.divide(vhists_n[0],vhists_d[0],numerator_scale,denominator_scale);
  eff_n.divide(vhists_n[1],vhists_n[0],numerator_scale,denominator_scale);
  eff_d.divide(vhists_d[1],vhists_d[0],numerator_scale,denominator_scale);

  hists_r.h_pt->Divide(vhists_n[0].h_pt,vhists_d[0].h_pt,numerator_scale,denominator_scale);
  hists_r.h_eta->Divide(vhists_n[0].h_eta,vhists_d[0].h_eta,numerator_scale,denominator_scale);
  hists_r.h_phi->Divide(vhists_n[0].h_phi,vhists_d[0].h_phi,numerator_scale,denominator_scale);
  hists_r.h_y->Divide(vhists_n[0].h_y,vhists_d[0].h_y,numerator_scale,denominator_scale);

  std::cout << "numerator: " << vhists_n[1].h_pt->GetEntries() << " / " << vhists_n[0].h_pt->GetEntries() << std::endl;
  std::cout << "denominator: " << vhists_d[1].h_pt->GetEntries() << " / " << vhists_d[0].h_pt->GetEntries() << std::endl;

  for(Hists& h : vhists_n) h.write();
  for(Hists& h : vhists_d) h.write();
  hists_r.write();
  eff_n.write();
  eff_d.write();

  numerator_reco_mass->Write();
  denominator_reco_mass->Write();
}
