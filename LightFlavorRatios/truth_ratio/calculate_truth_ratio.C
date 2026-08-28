#include "../util/binning.h"
#include <Math/Vector4D.h>

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
  void divide(Hists& numerator, Hists& denominator, float scale_factor=1.)
  {
    h_pt->Divide(numerator.h_pt,denominator.h_pt,scale_factor);
    h_eta->Divide(numerator.h_eta,denominator.h_eta,scale_factor);
    h_phi->Divide(numerator.h_phi,denominator.h_phi,scale_factor);
    h_y->Divide(numerator.h_y,denominator.h_y,scale_factor);
  }
  void write()
  {
    h_pt->Write();
    h_eta->Write();
    h_phi->Write();
    h_y->Write();
  }
};

std::vector<Hists> process_tree(TTree* t, std::string basename, std::string basetitle, int mother_flavor, std::vector<int> required_daughter_flavors, bool parity_inclusive)
{
  const float mother_mass = TDatabasePDG::Instance()->GetParticle(mother_flavor)->Mass();

  std::vector<Hists> vh;
  vh.emplace_back(basename,basetitle);
  vh.emplace_back(basename+"_wd",basetitle+" with daughters");

  float fevent;
  float gflavor;
  float gtrackID;
  float rtrackID;
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
  std::map<int,std::pair<ROOT::Math::PtEtaPhiMVector,std::set<int>>> current_daughter_map; 
  // key: parent ID, value: (mother Lorentz vector, set of daughter flavors)

  for(size_t i=0; i<t->GetEntries(); i++)
  {
    t->GetEntry(i);
    int event = round(fevent);

    if(event != current_event)
    {
      if(event % 100 == 0) std::cout << "event " << current_event << std::endl;
      // gather all reconstructible daughters for each mother
      for(size_t j=current_event_start_entry; j<i; j++)
      {
        t->GetEntry(j);
        // don't round reco trackID, this gets rid of NaN-ness on conversion to int
        int parentID = round(gparentID);
        //std::cout << "parentID = " << parentID << std::endl;
        int flavor = round(gflavor);
        //std::cout << "flavor = " << flavor << std::endl;
        //std::cout << "trackID = " << rtrackID << std::endl;
        //std::cout << "map count = " << current_daughter_map.count(parentID) << std::endl;
        if(!std::isnan(rtrackID) && current_daughter_map.count(parentID)>0)
        {
          current_daughter_map[parentID].second.insert(flavor);
        }
      }
      //std::cout << "----------------------------------------------" << std::endl;
      // check list of mothers for correct set of daughter flavors
      for(auto [motherID, sv_info] : current_daughter_map)
      {
        ROOT::Math::PtEtaPhiMVector mother_lorentzvector = sv_info.first;
        std::set<int> daughter_flavors = sv_info.second;

        //std::cout << "mother iD = " << motherID << std::endl;
        //std::cout << "daughters: ";
        //for(int df : daughter_flavors) std::cout << df << " ";
        //std::cout << std::endl;
        bool has_all_daughters = std::all_of(required_daughter_flavors.begin(),required_daughter_flavors.end(),[&](int flavor){ return daughter_flavors.contains(flavor); });
        bool has_all_opposite_daughters = std::all_of(required_daughter_flavors.begin(),required_daughter_flavors.end(),[&](int flavor){ return daughter_flavors.contains(-1*flavor); });
        //if(has_all_daughters) std::cout << "has all daughters" << std::endl;
        //if(has_all_opposite_daughters) std::cout << "has all opposite daughters" << std::endl;
        if(has_all_daughters || (parity_inclusive && has_all_opposite_daughters))
        {
          vh[1].h_pt->Fill(mother_lorentzvector.Pt());
          vh[1].h_eta->Fill(mother_lorentzvector.Eta());
          vh[1].h_phi->Fill(mother_lorentzvector.Phi());
          vh[1].h_y->Fill(mother_lorentzvector.Rapidity());
        }
      }
      // clean up
      current_daughter_map.clear();
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
    bool is_duplicate = (parentflavor == flavor && parentID != trackID);
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
        vh[0].h_pt->Fill(gpt);
        vh[0].h_eta->Fill(geta);
        vh[0].h_phi->Fill(gphi);
        vh[0].h_y->Fill(rapidity);
        current_daughter_map.insert({trackID,std::make_pair(mother_lorentzvector,std::set<int>())});
      }
    }
  }

  return vh;
}

void calculate_truth_ratio(int numerator_flavor, std::vector<int> numerator_daughter_flavors,
                           int denominator_flavor, std::vector<int> denominator_daughter_flavors,
                           std::string numerator_particlename, std::string denominator_particlename,
                           std::string numerator_basefile, std::string denominator_basefile,
                           float scale_factor, std::string outfilebase, int process, bool numerator_abs = true, bool denominator_abs = true)
{
  size_t ndigits = 6;
  std::string process_str = std::string(ndigits - std::to_string(process).length(),'0') + std::to_string(process);

  TFile* f_num = TFile::Open((numerator_basefile+"_"+process_str+".root").c_str());
  TFile* f_denom = TFile::Open((denominator_basefile+"_"+process_str+".root").c_str());

  TTree* t_num = (TTree*)f_num->Get("ntp_gtrack");
  TTree* t_denom = (TTree*)f_denom->Get("ntp_gtrack");

  std::string outfname = outfilebase+"_"+process_str+".root";
  TFile* fout = new TFile(outfname.c_str(),"RECREATE");

  std::string title_r = std::to_string(scale_factor) + numerator_particlename + "/" + denominator_particlename;

  Hists hists_r("hr",title_r);

  Hists eff_n("eff_n",numerator_particlename + " reconstruction efficiency");
  Hists eff_d("eff_d",denominator_particlename + " reconstruction efficiency");

  std::vector<Hists> vhists_n = process_tree(t_num,"hn",numerator_particlename+" yield",numerator_flavor,numerator_daughter_flavors,numerator_abs);
  std::vector<Hists> vhists_d = process_tree(t_denom,"hd",denominator_particlename+" yield",denominator_flavor,denominator_daughter_flavors,denominator_abs);

  hists_r.divide(vhists_n[0],vhists_d[0],scale_factor);
  eff_n.divide(vhists_n[1],vhists_n[0],scale_factor);
  eff_d.divide(vhists_d[1],vhists_d[0],scale_factor);

  hists_r.h_pt->Divide(vhists_n[0].h_pt,vhists_d[0].h_pt,scale_factor);
  hists_r.h_eta->Divide(vhists_n[0].h_eta,vhists_d[0].h_eta,scale_factor);
  hists_r.h_phi->Divide(vhists_n[0].h_phi,vhists_d[0].h_phi,scale_factor);
  hists_r.h_y->Divide(vhists_n[0].h_y,vhists_d[0].h_y,scale_factor);

  for(Hists& h : vhists_n) h.write();
  for(Hists& h : vhists_d) h.write();
  hists_r.write();
  eff_n.write();
  eff_d.write();

}
