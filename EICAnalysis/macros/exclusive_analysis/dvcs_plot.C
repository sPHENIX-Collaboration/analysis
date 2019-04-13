// ------------------------------------------------------------------------//
// Declaring Cut Structs
// ------------------------------------------------------------------------//
struct event_kinematics_cut{
  float min_Q2;
  float max_Q2;
  float min_y;
  float max_y;
  float min_t;
  float max_t;
  float min_angle_separation;
};

struct lepton_cut{
  float min_eta;
  float max_eta;
  float min_mom;
  float max_mom;
  float min_theta;
  float max_theta;
};

struct gamma_cut{
  float min_eta;
  float max_eta;
  float min_mom;
  float max_mom;
  float min_theta;
  float max_theta;
};

struct hadron_cut{
  float min_eta;
  float max_eta;
  float min_mom;
  float max_mom;
  float min_theta;
  float max_theta;
};

// ------------------------------------------------------------------------//
// Standard Cuts
// ------------------------------------------------------------------------//
const event_kinematics_cut ev_standard_cut = {0,1000,0,100,0,100,0};
const lepton_cut l_standard_cut = {-999,999,-999,999,-999,999};
const hadron_cut h_standard_cut = {-999,999,-999,999,-999,999};
const gamma_cut g_standard_cut = {-999,999,-999,999,-999,999};
// ------------------------------------------------------------------------//
// Relevant Arrays and Array Index Storing Variable
// ------------------------------------------------------------------------//
TH2F **h2array;
event_kinematics_cut *ev_cutarray;
lepton_cut *l_cutarray;
gamma_cut *g_cutarray;
hadron_cut *h_cutarray;
TString *pngNames;
int index=0;
int arraySize;
// ------------------------------------------------------------------------//
// Other Global Variables
// ------------------------------------------------------------------------//
int nEvents;
// ------------------------------------------------------------------------//
// Creating Base Histograms for Types of Plots
// ------------------------------------------------------------------------// 

const int nbins=80;
TH2F *h2_x_Q2_base = new TH2F("x_Q2","",nbins,-4,0,nbins,0,2);
TH2F *h2_lepton_base = new TH2F("lepton_mom_eta","",nbins,-4,4,nbins,0,30);
TH2F *h2_gamma_base = new TH2F("gamma_mom_eta","",nbins,-4,4,nbins,0,30);
TH2F *h2_hadron_base = new TH2F("hadron_mom_eta","",nbins,-1,20,nbins,0,300);
TH2F *h2_hadron_scatter_base = new TH2F("scatter","",nbins,0,1.5,nbins,0,0.006);
TH2F *h2_electron_photon_angle_base = new TH2F("angle_separation","",nbins,0,3.2,nbins,0,1);
TH2F *h2_nils_plot = new TH2F("nils_plot","",nbins,0,9,nbins,0,360);

// ------------------------------------------------------------------------//
//
//
//
//                                 Main Code 
//
//
//
// ------------------------------------------------------------------------//

int dvcs_plot()
{
  // ------------------------------------------------------------------------//
  //  Uploading File and Truth Tree
  // ------------------------------------------------------------------------// 

  TFile *f = new TFile("/sphenix/user/gregtom3/data/Summer2018/DVCS_studies/500k_10x250_sartre_dvcs.root","READ");
  TTree *t= (TTree*)f->Get("event_truth");
    
  // ------------------------------------------------------------------------//
  // Creating Plotting Histograms 
  // 1) Edit 'arraySize' to fit how many plots you will like to make
  // 2) Create your own event kinematics, lepton, hadron, or gamma cuts
  //    Or simply select the standard ones already created
  // 3) Add the desired histogram to the array using 'add_to_array'
  // ------------------------------------------------------------------------//

  arraySize = 1;

  h2array=malloc(arraySize * sizeof(TH2F));
  ev_cutarray=malloc(arraySize * sizeof(event_kinematics_cut));
  l_cutarray=malloc(arraySize * sizeof(lepton_cut));
  g_cutarray=malloc(arraySize * sizeof(gamma_cut));
  h_cutarray=malloc(arraySize * sizeof(hadron_cut));
  pngNames=malloc(arraySize * sizeof(TString));

  // event_kinematics_cut *** = {float min_Q2, float max_Q2,
  //             float min_y , float max_y ,
  //             float min_t , float max_t ,
  //             float min_angle_separation}

  // lepton_cut *** = {float min_eta, float max_eta,
  //                   float min_mom, float max_mom,
  //                   float min_theta, float max_theta}

  // gamma_cut *** = {float min_eta, float max_eta,
  //                   float min_mom, float max_mom,
  //                   float min_theta, float max_theta}

  // hadron_cut *** = {float min_eta, float max_eta,
  //                   float min_mom, float max_mom,
  //                   float min_theta, float max_theta}
  
  gamma_cut EEMC = {-4,-1,-999,999,-999,999};
  gamma_cut CEMC = {-1,1,-999,999,-999,999};
  gamma_cut FEMC = {1,4,-999,999,-999,999};
 
  // Adding to Histogram Plot Instructions
  // add_to_array(char *arg_1,  TString arg_2, TString arg_3
  //              event_kinematics_cut arg_4, lepton_cut arg_5, gamma_cut arg_6,                  hadron_cut arg_7)
  //
  // arg_1 : Type of Histogram to draw
  //       a) x_Q2
  //       b) gamma_mom_eta
  //       c) lepton_mom_eta
  //       d) proton_mom_eta
  //       e) scatter
  //       f) angle_separation
  //       g) nils_plot
  // arg_2 : After code is executed, histogram saves as png
  // arg_3 : Histogram Title
  // arg_4 : Cut on event kinematics and other special properties
  // arg_5 : Cut on the scattered electron's kinematics
  // arg_6 : Cut on the scattered photon's kinematics
  // arg_7 : Cut on the scattered proton's kinematics
  // **Note: Leaving any of arg_4 -> arg_7 empty defaults to standard cuts
  //         ev_standard_cut , l_standard_cut , g_standard_cut , h_standard_cut
  
  add_to_array("x_Q2","/sphenix/user/gregtom3/data/Summer2018/DVCS_studies/png/milou_cuts/10x250_x_Q2_EEMC_plot.png","x-Q2 Distribution 10x250 DVCS EEMC (LoI cuts)",ev_standard_cut,l_standard_cut,EEMC,h_standard_cut);
 
  
 
  // ------------------------------------------------------------------------//
  // Events to Run (If nEvents < 0, all events used)
  // ------------------------------------------------------------------------//
  
  nEvents = -1;
  
  // ------------------------------------------------------------------------//
  // Declare all relevant event variables
  // ------------------------------------------------------------------------//

  Float_t Q2;
  Float_t W;
  Float_t x;
  Float_t y;
  Float_t T;
  
  std::vector<float> energy;
  std::vector<float> eta;
  std::vector<float> pid;
  std::vector<float> theta;
  std::vector<float> phi;
  std::vector<float> xvector;
  
  std::vector<float>* energy_pointer=&energy;
  std::vector<float>* eta_pointer=&eta;
  std::vector<float>* pid_pointer=&pid;
  std::vector<float>* theta_pointer=&theta;
  std::vector<float>* phi_pointer=&phi;
  std::vector<float>* xvector_pointer=&xvector;

  t->SetBranchAddress("evtgen_Q2",&Q2);
  t->SetBranchAddress("evtgen_x",&x);
  t->SetBranchAddress("evtgen_y",&y);
  t->SetBranchAddress("evtgen_W",&W);
  t->SetBranchAddress("evtgen_t",&T);
  t->SetBranchAddress("em_evtgen_ptotal",&energy_pointer);
  t->SetBranchAddress("em_evtgen_eta",&eta_pointer);
  t->SetBranchAddress("em_evtgen_pid",&pid_pointer);
  t->SetBranchAddress("em_evtgen_theta",&theta_pointer);
  t->SetBranchAddress("em_evtgen_phi",&phi_pointer);
  t->SetBranchAddress("em_reco_x_e",&xvector_pointer);

  // Momentum, Eta, Theta , Phi of Scattered Photon
  float gamma_eta;
  float gamma_mom;
  float gamma_theta;
  float gamma_phi;
  
  // Momentum, Theta, Phi of Scattered Electron
  float lepton_eta;
  float lepton_mom;
  float lepton_theta;
  float lepton_phi;
  
  // Momentum, Theta, Phi of Scattered Proton
  float hadron_eta;
  float hadron_mom;
  float hadron_theta;
  float hadron_phi;

  // ------------------------------------------------------------------------// 
  //  Loop through Tree
  // ------------------------------------------------------------------------//

  Int_t nentries = Int_t(t->GetEntries());
  if(nEvents>nentries)
    cout << "Warning : Number of Events specified to run is greater than entries in the tree, using all entries" << endl;
  else if(nEvents>0)
    nentries = nEvents;
  for(Int_t entryInChain=0; entryInChain<nentries; entryInChain++)
    {
      if(entryInChain%1000==0)
	cout << "Processing event " << entryInChain << " of " << nentries << endl;
      Int_t entryInTree = t->LoadTree(entryInChain);
      if (entryInTree < 0) break;
      t->GetEntry(entryInChain);
      // Flip through all the particles in the event (only 3)
      for(unsigned i = 0; i<energy.size(); i++)
	{
	  if(pid.at(i)==22) // Scattered photon
	    {
	      gamma_eta = eta.at(i);
	      gamma_mom = energy.at(i);
	      gamma_theta = theta.at(i);
	      gamma_phi = phi.at(i);
	    }
	  if(pid.at(i)==11) // Scattered electron
	    {
	      lepton_eta = eta.at(i);
	      lepton_mom = energy.at(i);
	      lepton_theta = theta.at(i);
	      lepton_phi = phi.at(i);
	      x=xvector.at(i);
	    }
	  if(pid.at(i)==2212) // Scattered proton
	    {
	      hadron_eta = eta.at(i);
	      hadron_mom = energy.at(i);
	      hadron_theta = theta.at(i);
	      hadron_phi = phi.at(i);
	    }
	}  
      
      // Angle between Scattered Electron and Photon
      float angle_separation = get_angle_separation(lepton_phi, lepton_eta, gamma_phi, gamma_eta);

      // Set 't' to abs for plotting
      if(T<0)
	T=-T;
      // Fill the histogram array
      for(int n = 0; n<arraySize; n++)
	{
	  // This is really ugly, but we check each event parameters with the
	  // cuts predefined by the plot.
	  event_kinematics_cut myCut = ev_cutarray[n];
	  lepton_cut lCut = l_cutarray[n];
	  gamma_cut gCut = g_cutarray[n];
	  hadron_cut hCut = h_cutarray[n];
	  char *histTitle = h2array[n]->GetName();
	  float sep = angle_separation;

	  bool passed_Q2 = (Q2 >= myCut.min_Q2 && Q2<= myCut.max_Q2); 
	  bool passed_y = (y >= myCut.min_y && y <= myCut.max_y);
	  bool passed_t = (T >= myCut.min_t && T <= myCut.max_t); 
	  bool passed_separation = ( sep >= myCut.min_angle_separation );
	  bool ev_passed = passed_Q2 && passed_y && passed_t && passed_separation;
	  
	  bool passed_lepton_eta = (lepton_eta >= lCut.min_eta && lepton_eta <= lCut.max_eta);
	  bool passed_lepton_mom = (lepton_mom >= lCut.min_mom && lepton_mom <= lCut.max_mom);
	  bool passed_lepton_theta = (lepton_theta >= lCut.min_theta && lepton_theta <= lCut.max_theta);
	  bool lepton_passed = passed_lepton_eta && passed_lepton_mom && passed_lepton_theta;
	  
	  bool passed_gamma_eta = (gamma_eta >= gCut.min_eta && gamma_eta <= gCut.max_eta);
	  bool passed_gamma_mom = (gamma_mom >= gCut.min_mom && gamma_mom <= gCut.max_mom);
	  bool passed_gamma_theta = (gamma_theta >= gCut.min_theta && gamma_theta <= gCut.max_theta);
	  bool gamma_passed = passed_gamma_eta && passed_gamma_mom && passed_gamma_theta;
	  
	  bool passed_hadron_eta = (hadron_eta >= hCut.min_eta && hadron_eta <= hCut.max_eta);
	  bool passed_hadron_mom = (hadron_mom >= hCut.min_mom && hadron_mom <= hCut.max_mom);
	  bool passed_hadron_theta = (hadron_theta >= hCut.min_theta && hadron_theta <= hCut.max_theta);
	  bool hadron_passed = passed_hadron_eta && passed_hadron_mom && passed_hadron_theta;
	  
	  bool passed = ev_passed && lepton_passed && gamma_passed && hadron_passed;
	  if(passed)
	    {
	      if(strcmp(histTitle,"x_Q2")==0)
		{
		  h2array[n]->Fill(x,Q2);
		}
	      else if(strcmp(histTitle,"lepton_mom_eta")==0)
		{
		  h2array[n]->Fill(lepton_eta,lepton_mom);
		}
	      else if(strcmp(histTitle,"gamma_mom_eta")==0)
		{
		  h2array[n]->Fill(gamma_eta,gamma_mom);
		}
	      else if(strcmp(histTitle,"hadron_mom_eta")==0)
		{
		  h2array[n]->Fill(hadron_eta,hadron_mom);
		}
	      else if(strcmp(histTitle,"scatter")==0)
		{
		  h2array[n]->Fill(T,hadron_theta); 
		}
	      else if(strcmp(histTitle,"angle_separation")==0)
		{
		  h2array[n]->Fill(lepton_theta,angle_separation);
		}
	      else if(strcmp(histTitle,"nils_plot")==0)
		{
		  float eta_diff = lepton_eta-gamma_eta;
		  float phi_diff = lepton_phi-gamma_phi;
		  if(eta_diff<0) eta_diff=-eta_diff;
		  if(phi_diff<0) phi_diff=-phi_diff;
		  h2array[n]->Fill(eta_diff,phi_diff*180/3.14159265);
		}
	      else
		{
		  cout << "Error: Histogram type '"<< histTitle << "' not found. Please use one of the specified types" << endl;
		  return -1;
		}
	    }
	}
    } 
  
  // ------------------------------------------------------------------------// 
  //  Save PNG
  // ------------------------------------------------------------------------//
  for(int n = 0; n<arraySize; n++)
    {
      hist_to_png(h2array[n],pngNames[n]);
    }
return 0;

}

float get_angle_separation(float e_phi, float e_theta, float g_phi, float g_theta)
{
   float photon_x = cos(g_phi)*sin(g_theta);
   float photon_y = sin(g_phi)*sin(g_theta);
   float photon_z = cos(g_theta);
   
   float electron_x = cos(e_phi)*sin(e_theta);
   float electron_y = sin(e_phi)*sin(e_theta);
   float electron_z = cos(e_theta);
   
   float dot_prod = photon_x*electron_x+photon_y*electron_y+photon_z*electron_z;
   
   if(dot_prod>=1)
     return 0;
   else if(dot_prod<=-1)
     return 3.14159265;
   else
     return acos(dot_prod);
}

void hist_to_png(TH2F * h, TString saveTitle)
{
  TCanvas *cPNG = new TCanvas(saveTitle,"",700,500);
  TImage *img = TImage::Create();
  gStyle->SetOptStat(0);
  if(strcmp(h->GetName(),"x_Q2")==0)
    {
      gPad->SetLogx();
      gPad->SetLogy();
      h->GetXaxis()->SetTitle("x");
      h->GetYaxis()->SetTitle("Q2 [GeV^{2}]");
    }
  else if(strcmp(h->GetName(),"gamma_mom_eta")==0)
    {
      h->GetXaxis()->SetTitle("Photon #eta");
      h->GetYaxis()->SetTitle("Photon Momentum [GeV]"); 
      gPad->SetLogz();
    }
  else if(strcmp(h->GetName(),"lepton_mom_eta")==0)
    {
      h->GetXaxis()->SetTitle("Electron #eta");
      h->GetYaxis()->SetTitle("Electron Momentum [GeV]");
      gPad->SetLogz();
    }
  else if(strcmp(h->GetName(),"hadron_mom_eta")==0)
    {
      h->GetXaxis()->SetTitle("Proton #eta");
      h->GetYaxis()->SetTitle("Proton Momentum [GeV]");
      gPad->SetLogz();
    }
  else if(strcmp(h->GetName(),"scatter")==0)
    {
      h->GetXaxis()->SetTitle("|t| [GeV^2]");
      h->GetYaxis()->SetTitle("Proton Scatter Angle [rad]");
    }
  else if(strcmp(h->GetName(),"angle_separation")==0)
    {
      h->GetXaxis()->SetTitle("Electron #theta");
      h->GetYaxis()->SetTitle("Electron Photon Angle Separation [rad]");
    }
  else if(strcmp(h->GetName(),"nils_plot")==0)
    {  
      h->GetXaxis()->SetTitle("Electron Photon Eta Separation");
      h->GetYaxis()->SetTitle("Electron Photon Phi Separation [deg]");
    }
  h->Draw("colz9");
  img->FromPad(cPNG);
  img->WriteImage(saveTitle);

  delete img;
}

void add_to_array(char *type, TString pngName, TString title, 
		  event_kinematics_cut ev_c=ev_standard_cut, 
		  lepton_cut           l_c=l_standard_cut, 
		  gamma_cut            g_c=g_standard_cut, 
		  hadron_cut           h_c=h_standard_cut)
{
  if(index+1>arraySize)
    {
      cout << "Warning: Set 'arraySize' to a value which matches the number of histograms you wish to plot" << endl;
      cout << "Not plotting Plot Index " << index << endl;
      exit();
    }
  else
    {
  TH2F *theH=NULL;
  if(strcmp(type,"x_Q2")==0)
    {
      theH = (TH2F*)h2_x_Q2_base->Clone();
      BinLog(theH);
    }
  else if(strcmp(type,"lepton_mom_eta")==0)
    {
      theH = (TH2F*)h2_lepton_base->Clone();
    }
  else if(strcmp(type,"gamma_mom_eta")==0)
    {
      theH = (TH2F*)h2_gamma_base->Clone();
    }
  else if(strcmp(type,"hadron_mom_eta")==0)
    {
      theH = (TH2F*)h2_hadron_base->Clone();
    }
  else if(strcmp(type,"scatter")==0)
    { 
      theH = (TH2F*)h2_hadron_scatter_base->Clone();
    }
  else if(strcmp(type,"angle_separation")==0)
    {
      theH = (TH2F*)h2_electron_photon_angle_base->Clone();
    }
  else if(strcmp(type,"nils_plot")==0)
    {
      theH = (TH2F*)h2_nils_plot->Clone();
    }
  h2array[index] = theH;
  ev_cutarray[index] = ev_c;
  l_cutarray[index] = l_c;
  g_cutarray[index] = g_c;
  h_cutarray[index] = h_c;
  pngNames[index] = pngName;
  index++;
  theH->SetTitle(title);
    }
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
