void fill_histogram(TH1F* h, std::vector<float>* tps)
{
  // even-index turning points are no->yes
  // odd-index turning points are yes->no
  for(int i=1; i<h->GetNbinsX(); i++)
  {
    float bincenter = h->GetBinCenter(i);
    int tp_index = -1;
    // find which turning point index applies to this bin
    for(int j=0; j<tps->size(); j++)
    {
      if(bincenter>tps->at(j))
      {
        tp_index = j;
      }
    }
    if(tp_index != -1 && (tp_index % 2) == 0)
    {
      h->Fill(bincenter);
    }
  }
}

void process_swimming(std::string infile)
{
  gStyle->SetOptStat(0);

  TFile* f = TFile::Open(infile.c_str());
  TTree* t = (TTree*)f->Get("DecayTree");

  std::vector<float>* tp_lifetime = nullptr;
  std::vector<float>* tp_decaylength = nullptr;

  t->SetBranchAddress("turningPoints_TAU",&tp_lifetime);
  t->SetBranchAddress("turningPoints_FD",&tp_decaylength);

  TH1F* decaylength_plot = new TH1F("h_decaylength","Normalized sum of decay-length step functions; 2D decay length [cm]",1000,0.,5.5);
  TH1F* lifetime_plot = new TH1F("h_lifetime","Normalized sum of lifetime step functions; decay time [ps]",1000,0.,500.);

  for(int i=0; i<t->GetEntries(); i++)
  {
    if(i % 1000 == 0) std::cout << i << std::endl;
    t->GetEntry(i);

    std::sort(tp_decaylength->begin(),tp_decaylength->end());
    std::sort(tp_lifetime->begin(),tp_lifetime->end());
/*
    std::cout << "FD size " << tp_decaylength->size() << std::endl;
    for(float fd : *tp_decaylength) std::cout << fd << " ";
    std::cout << std::endl;
    std::cout << "TAU size " << tp_lifetime->size() << std::endl;
    for(float lt : *tp_lifetime) std::cout << lt << " ";
    std::cout << std::endl;
*/
    fill_histogram(lifetime_plot,tp_lifetime);
    fill_histogram(decaylength_plot,tp_decaylength);
  }

  decaylength_plot->Scale(1./t->GetEntries());
  lifetime_plot->Scale(1./t->GetEntries());
  decaylength_plot->GetYaxis()->SetRangeUser(0.,1.);
  lifetime_plot->GetYaxis()->SetRangeUser(0.,1.);

  TCanvas* cd = new TCanvas("fd","fd",600,600);
  decaylength_plot->Draw("L");
  TCanvas* ct = new TCanvas("ft","ft",600,600);
  lifetime_plot->Draw("L");
}
