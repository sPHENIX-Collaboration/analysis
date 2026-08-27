// TurningPoint::val is function value to the right of turning point
struct TurningPoint
{
  float x;
  float val;
};

void fill_histogram(TH1F* h, std::vector<TurningPoint>& tps)
{
  for(int i=1; i<h->GetNbinsX(); i++)
  {
    //std::cout << "bin " << i << std::endl;
    float bincenter = h->GetBinCenter(i);
    //std::cout << "bincenter " << bincenter << std::endl;
    int tp_index = -1;
    // find the greatest turning point xval to the left of this bin (current value of function)
    for(int j=0; j<tps.size(); j++)
    {
      //std::cout << "compare with " << tps[j].x << std::endl;
      if(bincenter>tps[j].x)
      {
        //std::cout << "yes" << std::endl;
        tp_index = j;
      }
    }
    //std::cout << "tp index " << tp_index << std::endl;
    if(tp_index != -1)
    {
      //if(i==999 && tps[tp_index].val==0) 
      {
        //std::cout << "fill with " << tps[tp_index].val << std::endl;
        //for(TurningPoint& tp : tps) std::cout << tp.x << std::endl;
      }
      if(tps[tp_index].val == 1)
      {
        h->Fill(bincenter);
      }
    }
  }
}

void process_swimming(std::string infile,std::string mother_name)
{
  gStyle->SetOptStat(0);

  TFile* f = TFile::Open(infile.c_str());
  TTree* t = (TTree*)f->Get("DecayTree");

  string outputName = infile.substr(0,infile.size() - 10);
  outputName += "_swum_weighted.root";
  TFile* outFile = new TFile(outputName.c_str(),"RECREATE");

  TCut cut = "";

  TTree* midTree = t->CopyTree(cut);
  TTree* outTree = midTree->CloneTree(-1); //copying and cloning the input tree

  std::vector<float>* tp_lifetime = nullptr;
  std::vector<float>* tp_accept = nullptr;
  std::vector<float>* tp_decaylength = nullptr;

  t->SetBranchAddress("turningPoints_TAU",&tp_lifetime);
  t->SetBranchAddress("turningPoints_ACCEPT",&tp_accept);
  t->SetBranchAddress("turningPoints_FD",&tp_decaylength);

  float out_lifetime;
  float swim_weight;

  outTree->SetBranchAddress((mother_name+"_decayTime").c_str(),&out_lifetime);
  TBranch* swim_weight_branch = outTree->Branch("swim_weight",&swim_weight);

  float max_decaylength = 50.;
  float mass = 1.115;
//  float mass = 0.4976;
  float min_p = 0.5;
  float max_decaytime = max_decaylength*mass/(min_p*2.99792458e-2);

  TH1F* decaylength_plot = new TH1F("h_decaylength","Normalized sum of decay-length step functions; 2D decay length [cm]",1000,0.01,max_decaylength);
  TH1F* lifetime_plot = new TH1F("h_lifetime","Normalized sum of lifetime step functions; decay time [ps]",1000,0.1,max_decaytime);

  int nGoodCandidates = 0;

  // forming sum of step functions
  for(int i=0; i<t->GetEntries(); i++)
  {
    if(i % 1000 == 0) std::cout << i << std::endl;
    t->GetEntry(i);

    std::vector<TurningPoint> tps_fd;
    std::vector<TurningPoint> tps_t;
    //std::cout << "tp size " << tp_accept->size() << std::endl;
    if(tp_accept->size()==0)
    {
      std::cout << "no turning points, continuing" << std::endl;
      continue;
    }
    nGoodCandidates++;
    for(int j=0; j<tp_accept->size(); j++)
    {
      //std::cout << "tp " << j << std::endl;
      //std::cout << "fd " << tp_decaylength->at(j) << " lifetime " << tp_lifetime->at(j) << " accept " << tp_accept->at(j) << std::endl;
      tps_fd.emplace_back(tp_decaylength->at(j),tp_accept->at(j));
      tps_t.emplace_back(tp_lifetime->at(j),tp_accept->at(j));
    }

/*
    std::cout << "FD size " << tp_decaylength->size() << std::endl;
    for(float fd : *tp_decaylength) std::cout << fd << " ";
    std::cout << std::endl;
    std::cout << "TAU size " << tp_lifetime->size() << std::endl;
    for(float lt : *tp_lifetime) std::cout << lt << " ";
    std::cout << std::endl;
*/
    fill_histogram(lifetime_plot,tps_t);
    fill_histogram(decaylength_plot,tps_fd);
  }

  decaylength_plot->Scale(1./nGoodCandidates);
  lifetime_plot->Scale(1./nGoodCandidates);

  std::cout << "of " << t->GetEntries() << " candidates, " << nGoodCandidates << " were good" << std::endl;

  TH1F* orig_lifetime = new TH1F("orig_lifetime","lifetime, before correction",100,0.,500.);
  TH1F* corrected_lifetime = new TH1F("corrected_lifetime","lifetime, after correction",100,0.,500.);

  std::cout << "nentries: " << outTree->GetEntries() << std::endl;

  // storing weights
  for(int i=0; i<outTree->GetEntries(); i++)
  {
    if(i % 1000 == 0) std::cout << i << std::endl;
    outTree->GetEntry(i);

    //std::cout << "entry " << i << std::endl;

    float acceptance = lifetime_plot->Interpolate(out_lifetime);
    //std::cout << "lifetime " << out_lifetime << std::endl;
    //std::cout << "acceptance " << acceptance << std::endl;
    swim_weight = 1./acceptance;
    swim_weight_branch->Fill();
    //std::cout << "swim_weight " << swim_weight << std::endl;
    orig_lifetime->Fill(out_lifetime);
    corrected_lifetime->Fill(out_lifetime,swim_weight);
  }

  decaylength_plot->GetYaxis()->SetRangeUser(0.,1.);
  lifetime_plot->GetYaxis()->SetRangeUser(0.,1.);
/*
  TCanvas* cd = new TCanvas("fd","fd",600,600);
  decaylength_plot->Draw("L");
  cd->SetLogx();
  TCanvas* ct = new TCanvas("ft","ft",600,600);
  lifetime_plot->Draw("L");
  ct->SetLogx();

  TCanvas* cl = new TCanvas("fl","fl",600,600);
  orig_lifetime->SetLineColor(kBlack);
  corrected_lifetime->SetLineColor(kRed);
  corrected_lifetime->Draw();
  orig_lifetime->Draw("SAME");
*/
  outFile->cd();
  decaylength_plot->Write();
  lifetime_plot->Write();
  orig_lifetime->Write();
  corrected_lifetime->Write();

  outFile->Write();
}
