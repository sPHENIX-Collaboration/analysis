// ------------------------------------------------------------------------//
// Code Purpose
// ------------------------------------------------------------------------//
//   To show the difference between the track extrapolation code and truth
//   track information
// ------------------------------------------------------------------------//
//
// ------------------------------------------------------------------------//
//
//
//
//                                 Main Code 
//
//
//
// ------------------------------------------------------------------------//

const int num_files=4;

const int verbosity = 1;

int track2cluster_plot()
{
  gROOT->SetBatch(kTRUE);
  // ------------------------------------------------------------------------//
  //  Uploading Files and Cluster Trees
  // ------------------------------------------------------------------------//
  
  
  TFile *f_1 = new TFile("/sphenix/user/gregtom3/data/Summer2018/track2cluster_studies/e1DIS.root");
  TTree *t_truth_1 = (TTree*)f_1->Get("event_truth");
  TTree *t_cluster_1 = (TTree*)f_1->Get("event_cluster");

  TFile *f_2 = new TFile("/sphenix/user/gregtom3/data/Summer2018/track2cluster_studies/e2DIS.root");
  TTree *t_truth_2 = (TTree*)f_2->Get("event_truth");
  TTree *t_cluster_2 = (TTree*)f_2->Get("event_cluster");

  TFile *f_5 = new TFile("/sphenix/user/gregtom3/data/Summer2018/track2cluster_studies/e5DIS.root");
  TTree *t_truth_5 = (TTree*)f_5->Get("event_truth");
  TTree *t_cluster_5 = (TTree*)f_5->Get("event_cluster");

  TFile *f_10 = new TFile("/sphenix/user/gregtom3/data/Summer2018/track2cluster_studies/e10DIS.root");
  TTree *t_truth_10 = (TTree*)f_10->Get("event_truth");
  TTree *t_cluster_10 = (TTree*)f_10->Get("event_cluster");

  /*TFile *f_20 = new TFile("/sphenix/user/gregtom3/data/Summer2018/track2cluster_studies/e20DIS.root");
  TTree *t_truth_20 = (TTree*)f_20->Get("event_truth");
  TTree *t_cluster_20 = (TTree*)f_20->Get("event_cluster");*/

  // ------------------------------------------------------------------------//
  //  Creating Histograms (Phi, Eta Theta, momentum
  //                      posx, posy, posz, spatial distance,
  // ------------------------------------------------------------------------//
  const int nbins = 200;
  TH2F *h2_phi_base = new TH2F("","",nbins,-3.5,3.5,nbins,-3.5,3.5);
  TH2F *h2_eta_base = new TH2F("","",nbins,-10,10,nbins,-10,10);
  TH2F *h2_theta_base = new TH2F("","",nbins,-3.5,3.5,nbins,-3.5,3.5);
  TH2F *h2_mom_base = new TH2F("","",nbins,0,35,nbins,0,35);
  TH2F *h2_posx_base = new TH2F("","",nbins,-320,320,nbins,-320,320);
  TH2F *h2_posy_base = new TH2F("","",nbins,-320,320,nbins,-320,320);
  TH2F *h2_posz_base = new TH2F("","",nbins,-320,320,nbins,-320,320);
  TH1F *h1_spatial_base = new TH1F("","",nbins,0,30);
  // ------------------------------------------------------------------------//
  //  Creating Histogram Clones
  // ------------------------------------------------------------------------//
  TH2F *h2_phi = (TH2F*)h2_phi_base->Clone();
  TH2F *h2_eta = (TH2F*)h2_eta_base->Clone();
  TH2F *h2_theta = (TH2F*)h2_theta_base->Clone();
  TH2F *h2_mom = (TH2F*)h2_mom_base->Clone();
  TH2F *h2_posx = (TH2F*)h2_posx_base->Clone();
  TH2F *h2_posy = (TH2F*)h2_posy_base->Clone();
  TH2F *h2_posz = (TH2F*)h2_posz_base->Clone();
  TH1F *h1_spatial_cemc = (TH1F*)h1_spatial_base->Clone();
  TH1F *h1_spatial_eemc = (TH1F*)h1_spatial_base->Clone();
  TH1F *h1_spatial_femc = (TH1F*)h1_spatial_base->Clone();
  h1_spatial_cemc->SetLineColor(kRed);
  h1_spatial_eemc->SetLineColor(kBlue);
  h1_spatial_femc->SetLineColor(kGreen);
  // ------------------------------------------------------------------------//
  //  Which momentums would you like to iterate through?
  // ------------------------------------------------------------------------//
  std::vector<int> iter_p;
  iter_p.push_back(1);
  iter_p.push_back(2);
  iter_p.push_back(5);   // Only these momenta are currently available
  iter_p.push_back(10);
  iter_p.push_back(20);    
  // ------------------------------------------------------------------------//
  //  How many events per tree would you like to plot? nEvents < 0 = all evts
  // ------------------------------------------------------------------------//
  const int nEvents = 100000;
  // ------------------------------------------------------------------------//
  //  Analyze each tree
  // ------------------------------------------------------------------------//
  analyzeTree(h2_phi,h2_eta,h2_theta,h2_mom,h2_posx,h2_posy,h2_posz,h1_spatial_cemc,h1_spatial_eemc,h1_spatial_femc,t_truth_1,t_cluster_1,nEvents);
  printHists(h2_phi,h2_eta,h2_theta,h2_mom,h2_posx,h2_posy,h2_posz,h1_spatial_cemc,h1_spatial_eemc,h1_spatial_femc,"1GeV_electron.png");

  analyzeTree(h2_phi,h2_eta,h2_theta,h2_mom,h2_posx,h2_posy,h2_posz,h1_spatial_cemc,h1_spatial_eemc,h1_spatial_femc,t_truth_2,t_cluster_2,nEvents);
  printHists(h2_phi,h2_eta,h2_theta,h2_mom,h2_posx,h2_posy,h2_posz,h1_spatial_cemc,h1_spatial_eemc,h1_spatial_femc,"2GeV_electron.png");

  analyzeTree(h2_phi,h2_eta,h2_theta,h2_mom,h2_posx,h2_posy,h2_posz,h1_spatial_cemc,h1_spatial_eemc,h1_spatial_femc,t_truth_5,t_cluster_5,nEvents);
  printHists(h2_phi,h2_eta,h2_theta,h2_mom,h2_posx,h2_posy,h2_posz,h1_spatial_cemc,h1_spatial_eemc,h1_spatial_femc,"5GeV_electron.png");

  analyzeTree(h2_phi,h2_eta,h2_theta,h2_mom,h2_posx,h2_posy,h2_posz,h1_spatial_cemc,h1_spatial_eemc,h1_spatial_femc,t_truth_10,t_cluster_10,nEvents);
  printHists(h2_phi,h2_eta,h2_theta,h2_mom,h2_posx,h2_posy,h2_posz,h1_spatial_cemc,h1_spatial_eemc,h1_spatial_femc,"10GeV_electron.png");
  return 0;
}
void printHists(TH2F *h2_phi, TH2F* h2_eta, TH2F* h2_theta, TH2F* h2_mom, TH2F* h2_posx, TH2F* h2_posy, TH2F* h2_posz, TH1F* h1_spatial_cemc, TH1F *h1_spatial_eemc, TH1F* h1_spatial_femc,TString save)
{
  hist_to_png(h2_phi,TString(TString("phi_")+save),"phi");
  hist_to_png(h2_eta,TString(TString("eta_")+save),"eta");
  hist_to_png(h2_theta,TString(TString("theta_")+save),"theta");
  hist_to_png(h2_mom,TString(TString("mom_")+save),"mom");
  hist_to_png(h2_posx,TString(TString("posx_")+save),"posx");
  hist_to_png(h2_posy,TString(TString("posy_")+save),"posy");
  hist_to_png(h2_posz,TString(TString("posz_")+save),"posz");
  hist_to_png_h1(h1_spatial_cemc,h1_spatial_eemc,h1_spatial_femc,TString(TString("spatial_")+save),"spatial");
}
void analyzeTree(TH2F *h2_phi, TH2F* h2_eta, TH2F* h2_theta, TH2F* h2_mom, TH2F* h2_posx, TH2F* h2_posy, TH2F* h2_posz, TH1F* h1_spatial_cemc, TH1F* h1_spatial_eemc, TH1F* h1_spatial_femc, TTree* t_truth, TTree* t_cluster, const int nEvents)
{
  h2_phi->Reset();
  h2_eta->Reset();
  h2_theta->Reset();
  h2_mom->Reset();
  h2_posx->Reset();
  h2_posy->Reset();
  h2_posz->Reset();
  h1_spatial_cemc->Reset();
  h1_spatial_eemc->Reset();
  h1_spatial_femc->Reset();
      // -------------------------------//
      // Declare tree variables
      // -------------------------------//
      //   Track & Cluster
      std::vector<float> track_phi; std::vector<float> cluster_phi;
      std::vector<float> track_eta; std::vector<float> cluster_eta;
      std::vector<float> track_theta; std::vector<float> cluster_theta;
      std::vector<float> track_p; std::vector<float> cluster_p; //cluster energy
      std::vector<float> track_posx; std::vector<float> cluster_posx;
      std::vector<float> track_posy; std::vector<float> cluster_posy;
      std::vector<float> track_posz; std::vector<float> cluster_posz;
      //   Truth Particle Info
      std::vector<float> truth_phi; std::vector<float> truth_eta;
      std::vector<float> truth_theta; std::vector<float> truth_p;
      // -------------------------------//
      // Point to tree variables
      // -------------------------------//
      //   Track
      std::vector<float>* track_phi_pointer = &track_phi;
      std::vector<float>* track_eta_pointer = &track_eta;
      std::vector<float>* track_theta_pointer = &track_theta;
      std::vector<float>* track_p_pointer = &track_p;
      std::vector<float>* track_posx_pointer = &track_posx;
      std::vector<float>* track_posy_pointer = &track_posy;
      std::vector<float>* track_posz_pointer = &track_posz;
      //   Cluster
      std::vector<float>* cluster_phi_pointer = &cluster_phi;
      std::vector<float>* cluster_eta_pointer = &cluster_eta;
      std::vector<float>* cluster_theta_pointer = &cluster_theta;
      std::vector<float>* cluster_p_pointer = &cluster_p;
      std::vector<float>* cluster_posx_pointer = &cluster_posx;
      std::vector<float>* cluster_posy_pointer = &cluster_posy;
      std::vector<float>* cluster_posz_pointer = &cluster_posz;
      //   Truth Particle
      std::vector<float>* truth_phi_pointer = &truth_phi;
      std::vector<float>* truth_eta_pointer = &truth_eta;
      std::vector<float>* truth_theta_pointer = &truth_theta;
      std::vector<float>* truth_p_pointer = &truth_p;
      // -------------------------------//
      // Set tree branch addresses
      // -------------------------------//
      //   Track
      t_cluster->SetBranchAddress("em_track_phi2cluster",&track_phi_pointer);
      t_cluster->SetBranchAddress("em_track_eta2cluster",&track_eta_pointer);
      t_cluster->SetBranchAddress("em_track_theta2cluster",&track_theta_pointer);
      t_cluster->SetBranchAddress("em_track_ptotal",&track_p_pointer);
      t_cluster->SetBranchAddress("em_track_x2cluster",&track_posx_pointer);
      t_cluster->SetBranchAddress("em_track_y2cluster",&track_posy_pointer);
      t_cluster->SetBranchAddress("em_track_z2cluster",&track_posz_pointer);
      //   Cluster
      t_cluster->SetBranchAddress("em_cluster_phi",&cluster_phi_pointer);
      t_cluster->SetBranchAddress("em_cluster_eta",&cluster_eta_pointer);
      t_cluster->SetBranchAddress("em_cluster_theta",&cluster_theta_pointer);
      t_cluster->SetBranchAddress("em_cluster_e",&cluster_p_pointer);
      t_cluster->SetBranchAddress("em_cluster_posx",&cluster_posx_pointer);
      t_cluster->SetBranchAddress("em_cluster_posy",&cluster_posy_pointer);
      t_cluster->SetBranchAddress("em_cluster_posz",&cluster_posz_pointer);
      //   Truth Particle
      t_truth->SetBranchAddress("em_evtgen_phi",&truth_phi_pointer);
      t_truth->SetBranchAddress("em_evtgen_eta",&truth_eta_pointer);
      t_truth->SetBranchAddress("em_evtgen_theta",&truth_theta_pointer);
      t_truth->SetBranchAddress("em_evtgen_ptotal",&truth_p_pointer);
      // -------------------------------//
      // Set tree branch addresses
      // -------------------------------//
      Int_t nentries_truth =  Int_t(t_truth->GetEntries());
      Int_t nentries_cluster= Int_t(t_cluster->GetEntries());
      Int_t nentries=0;
      // Check if event_truth and event_cluster are equal
      
      if(nentries_truth!=nentries_cluster)
	{
	  cout << "Warning: Entries in 'event_truth'("<<nentries_truth<<") does not equal Entries in 'event_cluster'("<<nentries_cluster<<"). Using the smaller of the two" << endl;
	  if(nentries_truth>nentries_cluster) nentries = nentries_cluster;
	}
      else
	{
	  nentries = nentries_truth;
	}

      // Check nEvents with nentries 

      if(nEvents>nentries)
	cout << "Warning : Number of Events specified to run is greater than entries in the tree, using all entries" << endl;
      else if(nEvents>0)
	nentries = nEvents;

      // Iterate through tree
      for(Int_t entryInChain=0; entryInChain<nentries; entryInChain++)
	{
	  
	  // Print out progress
	  if(entryInChain%1000==0&&verbosity>0)
	    cout << "Processing event " << entryInChain << " of " << nentries << endl;

	  // Not sure what this line does

	  Int_t entryInTree_truth = t_truth->LoadTree(entryInChain);
	  if (entryInTree_truth < 0) break;
	  Int_t entryInTree_cluster = t_cluster->LoadTree(entryInChain);
	  if (entryInTree_cluster < 0) break;
	  
	  // Get entries (filling vectors)
	  
	  t_truth->GetEntry(entryInChain);
	  t_cluster->GetEntry(entryInChain);

	  // Iterate through all truth particles and see what we were able
	  // to come up with (Fill histograms)
	  for(unsigned k = 0; k<cluster_p.size(); k++)
	    {
	      if(cluster_eta.at(k)<-1.1)
		{
		  h2_phi->Fill(track_phi.at(k),cluster_phi.at(k));
		  h2_eta->Fill(track_eta.at(k),cluster_eta.at(k));
		  h2_theta->Fill(track_theta.at(k),cluster_theta.at(k));
		  h2_mom->Fill(track_p.at(k),cluster_p.at(k));
		  h2_posx->Fill(track_posx.at(k),cluster_posx.at(k));
		  h2_posy->Fill(track_posy.at(k),cluster_posy.at(k));
		  h2_posz->Fill(track_posz.at(k),cluster_posz.at(k));
		}
	      if(cluster_eta.at(k)>1)
		{
		  h1_spatial_femc
		    ->Fill(sqrt( (track_posx.at(k)-cluster_posx.at(k))*
				 (track_posx.at(k)-cluster_posx.at(k))
				                  + 
				 (track_posy.at(k)-cluster_posy.at(k))*
				 (track_posy.at(k)-cluster_posy.at(k))
				              	  + 0*
				 (track_posz.at(k)-cluster_posz.at(k))*
				 (track_posz.at(k)-cluster_posz.at(k))));
		}
	      else if(cluster_eta.at(k)<1&&cluster_eta.at(k)>-1.1)
		{
		  h1_spatial_cemc
		    ->Fill(sqrt( (track_posx.at(k)-cluster_posx.at(k))*
				 (track_posx.at(k)-cluster_posx.at(k))
				                  + 
				 (track_posy.at(k)-cluster_posy.at(k))*
				 (track_posy.at(k)-cluster_posy.at(k))
				              	  + 
				 (track_posz.at(k)-cluster_posz.at(k))*
				 (track_posz.at(k)-cluster_posz.at(k))));
		}
	      else
		{
		  h1_spatial_eemc
		    ->Fill(sqrt( (track_posx.at(k)-cluster_posx.at(k))*
				 (track_posx.at(k)-cluster_posx.at(k))
				                  + 
				 (track_posy.at(k)-cluster_posy.at(k))*
				 (track_posy.at(k)-cluster_posy.at(k))
				              	  + 0*
				 (track_posz.at(k)-cluster_posz.at(k))*
				 (track_posz.at(k)-cluster_posz.at(k))));
		  
		}
	    }
	}
}
void openFiles()
{
  
}

void openTrees()
{
  
}
void BinLog(TH2F *h) 
{

   TAxis *axis = h->GetXaxis(); 
   int bins = axis->GetNbins();

   Axis_t from = axis->GetXmin();
   Axis_t to = axis->GetXmax();
   Axis_t width = (to - from) / bins;
   Axis_t *new_bins = new Axis_t[bins + 1];

   for (int i = 0; i <= bins; i++) {
     new_bins[i] = TMath::Power(10, from + i * width);
   } 
   axis->Set(bins, new_bins); 

   TAxis *axis2 = h->GetYaxis(); 
   int bins2 = axis2->GetNbins();

   Axis_t from2 = axis2->GetXmin();
   Axis_t to2 = axis2->GetXmax();
   Axis_t width2 = (to2 - from2) / bins2;
   Axis_t *new_bins2 = new Axis_t[bins2 + 1];

   for (int i = 0; i <= bins2; i++) {
     new_bins2[i] = TMath::Power(10, from2 + i * width2);
   } 
   axis2->Set(bins2, new_bins2); 

   delete new_bins;
   delete new_bins2;
}

void hist_to_png_h1(TH1F * h_c, TH1F* h_e, TH1F* h_f, TString saveTitle, TString type_string)
{
  TCanvas *cPNG = new TCanvas(saveTitle,"",700,500);
  TImage *img = TImage::Create();
  char * type = type_string.Data();
  gStyle->SetOptStat(0);
  gPad->SetLogy();
  auto legend = new TLegend(0.7,0.7,0.9,0.9);
  legend->SetTextSize(0.06);
  legend->AddEntry(h_c,"CEMC","l");
  legend->AddEntry(h_e,"EEMC","l");
  legend->AddEntry(h_f,"FEMC","l");
  
  if(strcmp(type,"spatial")==0)
    {  
      h_e->GetXaxis()->SetTitle("Extrapolation distance from cluster [cm]");
      h_e->GetYaxis()->SetTitle("Counts");
    }
  h_e->Draw();
  h_c->Draw("SAME");
  h_f->Draw("SAME");
  legend->Draw();
  img->FromPad(cPNG);
  img->WriteImage(saveTitle);
}
void hist_to_png(TH2F * h, TString saveTitle, TString type_string)
{
  TCanvas *cPNG = new TCanvas(saveTitle,"",700,500);
  TImage *img = TImage::Create();
  char * type = type_string.Data();
  gPad->SetLogz();
  gStyle->SetOptStat(0);
  if(strcmp(type,"phi")==0)
    {
      h->GetXaxis()->SetTitle("Track Phi");
      h->GetYaxis()->SetTitle("Cluster Phi");
    }
  else if(strcmp(type,"eta")==0)
    {
      h->GetXaxis()->SetTitle("Track Eta");
      h->GetYaxis()->SetTitle("Cluster Eta"); 
    }
  else if(strcmp(type,"theta")==0)
    {
      h->GetXaxis()->SetTitle("Track Theta");
      h->GetYaxis()->SetTitle("Cluster Theta");
    }
  else if(strcmp(type,"mom")==0)
    {
      h->GetXaxis()->SetTitle("Track Momentum [GeV]");
      h->GetYaxis()->SetTitle("Cluster Momentum [GeV]");
    }
  else if(strcmp(type,"posx")==0)
    {
      h->GetXaxis()->SetTitle("Track Position X [cm]");
      h->GetYaxis()->SetTitle("Cluster Position X [cm]");
    }
  else if(strcmp(type,"posy")==0)
    {
      h->GetXaxis()->SetTitle("Track Position Y [cm]");
      h->GetYaxis()->SetTitle("Cluster Position Y [cm]");
    }
  else if(strcmp(type,"posz")==0)
    {  
      h->GetXaxis()->SetTitle("Track Position Z [cm]");
      h->GetYaxis()->SetTitle("Cluster Position Z [cm]");
    }
  h->Draw("colz9");
  img->FromPad(cPNG);
  img->WriteImage(saveTitle);

  delete img;
}
