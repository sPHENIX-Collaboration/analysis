#define Analysis_cxx
#include "Analysis.hh"

//////////////////////////////////////////////////////////////////////
// Constructor                                                      //
//////////////////////////////////////////////////////////////////////
Analysis::Analysis( int run ) :
  fChain(0),
  run_( run )
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
  TTree* tree = 0;
  //if (tree == 0)
    {
      string file_name = "results/tracking_run" + to_string( run_ ) + ".root";
      cout << file_name << endl;
      // TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject( file_name.c_str() );
      TFile *f = new TFile( file_name.c_str() );
      f->GetObject("clus_tree",tree);

      /*
      if (!f || !f->IsOpen()) {
	f = new TFile("tracking_run41981.root");
      }
      f->GetObject("clus_tree",tree);
      */
    }
  Init(tree);
}

//////////////////////////////////////////////////////////////////////
// Destructor                                                       //
//////////////////////////////////////////////////////////////////////
Analysis::~Analysis()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

//////////////////////////////////////////////////////////////////////
// private functions                                                //
//////////////////////////////////////////////////////////////////////
string Analysis::GetDate()
{
  
  TDatime dt;
  int year	= dt.GetYear();
  int month	= dt.GetMonth();
  int day	= dt.GetDay();

  // format: mm/dd/yyyy
  std::stringstream ss;
  ss << month << "/" << day << "/" << year;

  return ss.str();
}

void Analysis::DrawWords()
{

  ///////////////////////////////////////////////////////////////////////////////
  // Writting words in the canvas                                              //
  ///////////////////////////////////////////////////////////////////////////////
  TLatex* tex = new TLatex();

  double line_height = 0.05;
  double first_margin = 0.005;  
  double pos_y = 1.0 - top_margin_ + first_margin;// - line_height;

  // Date
  tex->DrawLatexNDC( 0.7, pos_y,
		     string("#it{" + GetDate() + "}").c_str() );

  // sPHENIX Internal or sPHENIX Prelimnary
  pos_y -= line_height - first_margin + 0.025;
  double pos_x = 0.2;
  if( is_preliminary_ == false )
    {
      tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Internal" );
    }
  else
    {

      pos_x = 0.4;
      tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary" );
    }

  // p+p 200 GeV
  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, "#it{p+p} 200 GeV, Run 50889 " );

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, "INTT Streaming Readout " );
}


Int_t Analysis::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

Long64_t Analysis::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Analysis::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   x_in = 0;
   y_in = 0;
   z_in = 0;
   r_in = 0;
   size_in = 0;
   phi_in = 0;
   theta_in = 0;
   adc_in = 0;
   is_associated_in = 0;
   track_incoming_theta_in = 0;
   x_out = 0;
   y_out = 0;
   z_out = 0;
   r_out = 0;
   size_out = 0;
   phi_out = 0;
   theta_out = 0;
   adc_out = 0;
   is_associated_out = 0;
   track_incoming_theta_out = 0;

   // Set branch addresses and branch pointers
   if (!tree)
     return;
   
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("evt_clus", &evt_clus, &b_evt_clus);
   fChain->SetBranchAddress("x_in", &x_in, &b_x_in);
   fChain->SetBranchAddress("y_in", &y_in, &b_y_in);
   fChain->SetBranchAddress("z_in", &z_in, &b_z_in);
   fChain->SetBranchAddress("r_in", &r_in, &b_r_in);
   fChain->SetBranchAddress("size_in", &size_in, &b_size_in);
   fChain->SetBranchAddress("phi_in", &phi_in, &b_phi_in);
   fChain->SetBranchAddress("theta_in", &theta_in, &b_theta_in);
   fChain->SetBranchAddress("adc_in", &adc_in, &b_adc_in);
   fChain->SetBranchAddress("is_associated_in", &is_associated_in, &b_is_associated_in);
   fChain->SetBranchAddress("track_incoming_theta_in", &track_incoming_theta_in, &b_track_incoming_theta_in);
   fChain->SetBranchAddress("x_out", &x_out, &b_x_out);
   fChain->SetBranchAddress("y_out", &y_out, &b_y_out);
   fChain->SetBranchAddress("z_out", &z_out, &b_z_out);
   fChain->SetBranchAddress("r_out", &r_out, &b_r_out);
   fChain->SetBranchAddress("size_out", &size_out, &b_size_out);
   fChain->SetBranchAddress("phi_out", &phi_out, &b_phi_out);
   fChain->SetBranchAddress("theta_out", &theta_out, &b_theta_out);
   fChain->SetBranchAddress("adc_out", &adc_out, &b_adc_out);
   fChain->SetBranchAddress("is_associated_out", &is_associated_out, &b_is_associated_out);
   fChain->SetBranchAddress("track_incoming_theta_out", &track_incoming_theta_out, &b_track_incoming_theta_out);
   fChain->SetBranchAddress("z_vertex", &z_vertex, &b_z_vertex);
   Notify();
   
  int bin_num = 20;
  double xmin = 0;
  double xmax = 600;
  
  string title = "DAC distribution;DAC [arb. units];Counts";
  hist_all = new TH1D( "ADC", title.c_str(), bin_num, xmin, xmax) ; // , "adc_in", cut, "",
  // hist_all->SetFillColorAlpha( kGray, 0.2 );
  // hist_all->SetLineColorAlpha( kBlack, 1 );
  hist_all->SetFillColorAlpha( kGreen+2, 0.2 );
  hist_all->SetLineColorAlpha( hist_all->GetFillColor(), 1 );
  hist_all->SetLineWidth( 2 );
  HistSetting( hist_all );

  hist_all_ = new MipHist( "name", "title" );  
  hist_all_->SetColorAlpha( kGreen+2, 0.2 );

  hist_aso_ = new MipHist( "Track_assoiation", title.c_str() );
  hist_aso_->SetColorAlpha( kBlue, 0.1 );

  hist_no_aso_ = new MipHist( "Track_not_assoiated", title.c_str() );
  hist_no_aso_->SetColorAlpha( kGray, 0.1 );

  hist_ang90_ = new MipHist( "Top_pm5", title.c_str() );
  hist_ang90_->SetColorAlpha( kBlue, 0.1 );

  hist_ang45_ = new MipHist( "Ang40_45", title.c_str() );
  hist_ang45_->SetColorAlpha( kRed, 0.1 );

  hist_ang35_ = new MipHist( "Ang30_35", title.c_str() );
  hist_ang35_->SetColorAlpha( kSpring-1, 0.1 );

  hist_ang25_ = new MipHist( "Ang20_25", title.c_str() );
  hist_ang25_->SetColorAlpha( kOrange+1, 0.1 );

  hists_.push_back( hist_all_ );
  hists_.push_back( hist_aso_ );
  hists_.push_back( hist_no_aso_ );
  hists_.push_back( hist_ang90_ );
  hists_.push_back( hist_ang45_ );
  hists_.push_back( hist_ang35_ );
  hists_.push_back( hist_ang25_ );

  hist_aso = new TH1D( "Track_assoiation", title.c_str(), bin_num, xmin, xmax) ; // , "adc_in", cut, "",
  //hist_aso->SetFillColorAlpha( kRed + 2, 0.1 );
  //hist_aso->SetFillColorAlpha( kViolet + 1, 0.1 );
  //  hist_aso->SetFillColorAlpha( kGreen + 2, 0.1 );
  hist_aso->SetFillColorAlpha( kBlue, 0.1 );
  hist_aso->SetLineColorAlpha( hist_aso->GetFillColor(), 1 );
  hist_aso->SetLineWidth( 2 );
  HistSetting( hist_aso );
  
  hist_no_aso = new TH1D( "Track_not_assoiated", title.c_str(), bin_num, xmin, xmax) ; // , "adc_in", cut, "",
  hist_no_aso->SetFillColorAlpha( kGray, 0.1 );
  hist_no_aso->SetLineColorAlpha( hist_no_aso->GetFillColor(), 1 );
  hist_no_aso->SetLineWidth( 1 );
  HistSetting( hist_no_aso );
  
  hist_ang90 = new TH1D( "Top_pm5", title.c_str(), bin_num, xmin, xmax) ; // , "adc_in", this_cut, "",
  hist_ang90->SetFillColorAlpha( kBlue, 0.1 );
  //hist_ang90->SetFillColorAlpha( kRed - 4 , 0.2 );
  hist_ang90->SetLineColorAlpha( hist_ang90->GetFillColor(), 1 );
  hist_ang90->SetLineWidth( 3 );
  HistSetting( hist_ang90 );
    
  hist_ang45 = new TH1D( "Ang40_45", title.c_str(), bin_num, xmin, xmax) ; // ,   "adc_in", this_cut, "",
  hist_ang45->SetFillColorAlpha( kRed, 0.1 );
  hist_ang45->SetLineColorAlpha( hist_ang45->GetFillColor(), 1 );
  hist_ang45->SetLineWidth( 3 );
  HistSetting( hist_ang45 );
  
  hist_ang35 = new TH1D( "Ang30_35", title.c_str(), bin_num, xmin, xmax) ; // ,   "adc_in", this_cut, "",
  hist_ang35->SetFillColorAlpha( kSpring-1, 0.1 );
  hist_ang35->SetLineColorAlpha( hist_ang35->GetFillColor(), 1 );
  hist_ang35->SetLineWidth( 3 );
  HistSetting( hist_ang35 );
  
  hist_ang25 = new TH1D( "Ang20_25", title.c_str(), bin_num, xmin, xmax) ; // ,   "adc_in", this_cut, "",
  hist_ang25->SetFillColorAlpha( kOrange+1, 0.1 );
  hist_ang25->SetLineColorAlpha( hist_ang25->GetFillColor(), 1 );
  hist_ang25->SetLineWidth( 3 );
  HistSetting( hist_ang25 );
  
  hist_correlation = new TH2D( "nhit_correlation_barrel",
			       "Cluster correlation b/w barrels;#cluster_{Inner};#cluster_{Outer};Counts",
			       80, 0, 80,
			       80, 0, 80 );
			       /* 100, 0, 100, */
			       /* 100, 0, 100 ); */
  HistSetting( hist_correlation );
}

Bool_t Analysis::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

Int_t Analysis::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

void Analysis::ModifyAdcs()
{
  
  this->ModifyAdc( hist_all );
  // this->ModifyAdc( hist_aso );
  // this->ModifyAdc( hist_no_aso );
  //this->ModifyAdc( hist_ang90 );
  // this->ModifyAdc( hist_ang45 );
  // this->ModifyAdc( hist_ang35 );
  // this->ModifyAdc( hist_ang25 );
  
}

void Analysis::ModifyAdc( TH1D* hist )
{
  this->ModifyAdc( hist, 0 );
  this->ModifyAdc( hist, 1 );
}

void Analysis::ModifyAdc( TH1D* hist, int mode )
{
  //  auto hist = hist_all;
  int index_over_under_flow = 0;
  int adc = adc7_;

  if( mode == 1 ) // for 2-hit cluster with ADC14 = 2 * DAC 210
    {
      index_over_under_flow = hist->GetNbinsX() + 1;
      adc = adc7_ * 2;
    }

  int index_adc = 0;
  for( int i=1; i<hist->GetNbinsX() + 1; i++ )
    {
      double bin_low = hist->GetBinLowEdge( i );
      double bin_high = bin_low + hist->GetBinWidth( i );

      if( bin_low <= adc && adc < bin_high )
	{
	  index_adc = i;
	  break;
	}
    }
  
  int num_cluster_adc                     = hist->GetBinContent( index_adc );
  int num_single_hit_cluster_adc          = hist->GetBinContent( index_over_under_flow );
  int num_multiple_hit_cluster_adc        = num_cluster_adc - num_single_hit_cluster_adc;
  int num_modified_single_hit_cluster_adc = num_single_hit_cluster_adc * adc7_modification_factor_;
  if( mode == 1 )
    num_modified_single_hit_cluster_adc *= adc7_modification_factor_;
  
  int num_modified_cluster_adc            = num_modified_single_hit_cluster_adc + num_multiple_hit_cluster_adc;
  
  cout << "ModifyAdc" << endl;
  cout << "Index of ADC: " << index_adc << endl;
  cout << "#ADC: " << num_cluster_adc << endl;
  cout << "#ADC (single-hit cluster): " << num_single_hit_cluster_adc << endl;
  cout << "#ADC (multiple-hit cluster): " << num_multiple_hit_cluster_adc << endl;
  cout << "Modified #ADC (single-hit cluster): " << num_modified_single_hit_cluster_adc << endl;
  cout << "Moddified #ADC: " << num_modified_cluster_adc << endl;

  hist->SetBinContent( index_adc, num_modified_cluster_adc );
}

void Analysis::FillClusterInfo( int mode )
{

  vector<double>  *x;
  vector<double>  *y;
  vector<double>  *z;
  vector<double>  *r;
  vector<int>     *size;
  vector<double>  *phi;
  vector<double>  *theta;
  vector<double>  *adcs;
  vector<bool>    *is_associated;
  vector<double>  *track_incoming_theta;

  if( mode == 0 )
    {
      x = x_in;
      y = y_in;
      z = z_in;
      r = r_in;
      size = size_in;
      phi = phi_in;
      theta = theta_in;
      adcs = adc_in;
      is_associated = is_associated_in;
      track_incoming_theta = track_incoming_theta_in;
    }
  else if( mode == 1 )
    {
      x = x_out;
      y = y_out;
      z = z_out;
      r = r_out;
      size = size_out;
      phi = phi_out;
      theta = theta_out;
      adcs = adc_out;
      is_associated = is_associated_out;
      track_incoming_theta = track_incoming_theta_out;
    }

  for( int i=0; i<adcs->size(); i++ )
    {
      
      if( (*size)[i] > cluster_size_max_ )
	continue;
      /* if( fabs( z_vertex - (*z)[i] ) > 5 ) */
      /*   continue; */
	   
      int adc = (*adcs)[i];
      bool is_single_hit_cluster_adc7  = ( (*size)[i] == 1 && adc == adc7_ );
      bool is_double_hit_cluster_adc14 = ( (*size)[i] == 2 && adc == adc7_ * 2);

      hist_all_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	
      // if this cluster is not associated with a tracklet, store info here and skip the rest parts
      if( (*is_associated)[i] == false )
	{

	  hist_no_aso_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	  continue;
	}
	   
      hist_aso_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );

      if( (*track_incoming_theta)[i] > 85 )
	hist_ang90_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
      else if( 40 < (*track_incoming_theta)[i] && (*track_incoming_theta)[i] < 45 )
	hist_ang45_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
      else if( 30 < (*track_incoming_theta)[i] && (*track_incoming_theta)[i] < 35 )
	hist_ang35_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
      else if( 20 < (*track_incoming_theta)[i] && (*track_incoming_theta)[i] < 25 )
	hist_ang25_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	
    } // end of for( int i=0; i<adc->size(); i++ )


}

///////////////////////////////////////////////////////////////////////
// public functions                                                  //
///////////////////////////////////////////////////////////////////////

void Analysis::Loop()
{
//   In a ROOT session, you can do:
//      root> .L Analysis.C
//      root> Analysis t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   bool does_z_cut = true;
   bool does_adc7_cut = true;
   //does_adc7_cut = false;
   
   bool does_double_adc7_cut = true;
   //does_double_adc7_cut = false;
   
   for (Long64_t jentry=0; jentry<nentries;jentry++)
     {
       Long64_t ientry = LoadTree(jentry);
       if (ientry < 0)
	 break;

       double cluster_num_in = adc_in->size();
       double cluster_num_out = adc_out->size();
       double cluster_num = cluster_num_in + cluster_num_out;
       double cluster_num_asymmetry = fabs(cluster_num_in - cluster_num_out) / cluster_num;
       fChain->GetEntry(jentry);

       bool is_good_event = true;
       if( cluster_num_in <= 3
	   || cluster_num_out <= 3
	   || cluster_num <= 6
	   || cluster_num_asymmetry > 0.1
	   )
	 is_good_event = false;

       // if( is_good_event )
       // 	 cout << cluster_num_in << "\t"
       // 	      << cluster_num_out << "\t"
       // 	      << cluster_num << "\t"
       // 	      << cluster_num_asymmetry
       // 	      << endl;
       
       
       /* if( is_good_event && does_z_cut && fabs( z_vertex ) > 25 ) */
       /* 	 is_good_event = false; */

       // if( is_good_event )
       // 	 cout << setw(4) << cluster_num_in << "\t"
       // 	      << setw(4) << cluster_num_out << "\t"
       // 	      << setw(7) << setprecision(4) << left << cluster_num_in / cluster_num_out << "\t"
       // 	      << endl;
       
       //is_good_event = true;
       int hit_num_inner = 0, hit_num_outer = 0;
       ////////////////////////////////////////////////////////////////////
       // Loop over inner hits
       if( is_good_event == false )
	 continue;
       
       this->FillClusterInfo( 0 );
       this->FillClusterInfo( 1 );

       /*
       for( int i=0; i<adc_in->size(); i++ )
	 {

	   if( is_good_event == false )
	     continue;
	   
	   if( (*size_in)[i] > cluster_size_max_ )
	     continue;
	   / * if( fabs( z_vertex - (*z_in)[i] ) > 5 ) * /
       / *   continue; * /
	   
	   int adc = (*adc_in)[i];
	   hist_all->Fill( adc );

	   bool is_single_hit_cluster_adc7 = ( does_adc7_cut && (*size_in)[i] == 1 && adc == adc7_ );
	   bool is_double_hit_cluster_adc14 = does_double_adc7_cut && (*size_in)[i] == 2 && adc == adc7_ * 2;
	   if( is_single_hit_cluster_adc7 )
	       hist_all->Fill( -9999 );
	   else if( is_double_hit_cluster_adc14 )	     
	     hist_all->Fill( hist_all->GetNbinsX()+1 );

	   // if this cluster is not associated with a tracklet, store info here and skip the rest parts
	   if( (*is_associated_in)[i] == false )
	     {

	       hist_no_aso->Fill( adc );
	       if( is_single_hit_cluster_adc7 )
		 hist_no_aso->Fill( -9999 );
	       else if( is_double_hit_cluster_adc14 )
		 hist_no_aso->Fill( hist_no_aso->GetNbinsX()+1 );

	       continue;
	     }
	   
	   hist_aso->Fill( adc );
	   if( is_single_hit_cluster_adc7 )
	     hist_aso->Fill( -9999 );
	   else if( is_double_hit_cluster_adc14 )
	     hist_aso->Fill( hist_aso->GetNbinsX()+1 );

	   if( (*track_incoming_theta_in)[i] > 85 )
	     hist_ang90->Fill( adc );
	   else if( 40 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 45 )
	     hist_ang45->Fill( adc );
	   else if( 30 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 35 )
	     hist_ang35->Fill( adc );
	   else if( 20 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 25 )
	     hist_ang25->Fill( adc );

	   if( is_single_hit_cluster_adc7 )
	     {
	       if( (*track_incoming_theta_in)[i] > 85 )
		 hist_ang90->Fill( -9999 );
	       else if( 40 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 45 )
		 hist_ang45->Fill( -9999 );
	       else if( 30 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 35 )
		 hist_ang35->Fill( -9999 );
	       else if( 20 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 25 )
		 hist_ang25->Fill( -9999 );
	     }
	   else if( is_double_hit_cluster_adc14 )
	     {
	       if( (*track_incoming_theta_in)[i] > 85 )
		 hist_ang90->Fill( hist_ang90->GetNbinsX()+1 );
	       else if( 40 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 45 )
		 hist_ang45->Fill( hist_ang45->GetNbinsX()+1 );
	       else if( 30 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 35 )
		 hist_ang35->Fill( hist_ang35->GetNbinsX()+1 );
	       else if( 20 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 25 )
		 hist_ang25->Fill( hist_ang25->GetNbinsX()+1 );
	     }

	   hit_num_inner++;
	 }

       ////////////////////////////////////////////////////////////////////
       // Loop over outer hits
       for( int i=0; i<adc_out->size(); i++ )
	 {

	   if( (*size_out)[i] > cluster_size_max_ )
	     continue;
	   
	   if( is_good_event == false )
	     continue;

	   int adc = (*adc_out)[i];
	   hist_all->Fill( adc );
	   
	   bool is_single_hit_cluster_adc7 = ( does_adc7_cut && (*size_out)[i] == 1 && adc == adc7_ );
	   bool is_double_hit_cluster_adc14 = does_double_adc7_cut && (*size_out)[i] == 2 && adc == adc7_ * 2;
	   if( is_single_hit_cluster_adc7 )
	       hist_all->Fill( -9999 ) ;
	   else if( is_double_hit_cluster_adc14 )
	     hist_all->Fill( hist_all->GetNbinsX()+1 );

	   if( (*is_associated_out)[i] == false )
	     {
	       hist_no_aso->Fill( adc );

	       if( is_single_hit_cluster_adc7 )
		 hist_no_aso->Fill( -9999 );
	       else if( is_double_hit_cluster_adc14 )
		 hist_no_aso->Fill( hist_no_aso->GetNbinsX()+1 );
	       
	       continue;
	     }

	   hist_aso->Fill( adc );
	   if( is_single_hit_cluster_adc7 )
	     hist_aso->Fill( -9999 );
	   else if( is_double_hit_cluster_adc14 )
	     hist_aso->Fill( hist_aso->GetNbinsX()+1 );

	   if( (*track_incoming_theta_out)[i] > 85 )
	     hist_ang90->Fill( adc );
	   else if( 40 < (*track_incoming_theta_out)[i] && (*track_incoming_theta_out)[i] < 45 )
	     hist_ang45->Fill( adc );
	   else if( 30 < (*track_incoming_theta_out)[i] && (*track_incoming_theta_out)[i] < 35 )
	     hist_ang35->Fill( adc );
	   else if( 20 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 25 )
	     hist_ang25->Fill( adc );

	   if( is_single_hit_cluster_adc7 )
	     {
	       if( (*track_incoming_theta_out)[i] > 85 )
		 hist_ang90->Fill( -9999 );
	       else if( 40 < (*track_incoming_theta_out)[i] && (*track_incoming_theta_out)[i] < 45 )
		 hist_ang45->Fill( -9999 );
	       else if( 30 < (*track_incoming_theta_out)[i] && (*track_incoming_theta_out)[i] < 35 )
		 hist_ang35->Fill( -9999 );
	       else if( 20 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 25 )
		 hist_ang25->Fill( -9999 );
	     }
	   else if( is_double_hit_cluster_adc14 )
	     {
	       if( (*track_incoming_theta_out)[i] > 85 )
		 hist_ang90->Fill( hist_ang90->GetNbinsX()+1 );
	       else if( 40 < (*track_incoming_theta_out)[i] && (*track_incoming_theta_out)[i] < 45 )
		 hist_ang45->Fill( hist_ang45->GetNbinsX()+1 );
	       else if( 30 < (*track_incoming_theta_out)[i] && (*track_incoming_theta_out)[i] < 35 )
		 hist_ang35->Fill( hist_ang35->GetNbinsX()+1 );
	       else if( 20 < (*track_incoming_theta_in)[i] && (*track_incoming_theta_in)[i] < 25 )
		 hist_ang25->Fill( hist_ang25->GetNbinsX()+1 );
	     }

	   hit_num_outer++;
	 }

       //       if(  hit_num_inner != 0  && hit_num_outer != 0 )
       hist_correlation->Fill( hit_num_inner, hit_num_outer );
       
       //break;
       */
     }

   this->ModifyAdcs();

   for( auto& hist : hists_ )
     {
       hist->ModifyAdc();
       //hist->Print();
     }

}

void Analysis::Draw()
{

  SetsPhenixStyle();
  string output = "results/mip_" + to_string( run_ ) + ".pdf";
  TCanvas* c = new TCanvas( output.c_str(), "title", 800, 800 );
  c->Print( ((string)c->GetName() + "[").c_str() );

  hist_all_->GetHist()->Draw( "same" );
  c->Print( c->GetName() );
  
  auto hist_temp = (TH1D*)hist_ang90_->GetHist()->Clone();
  hist_temp->SetLineColor( kBlack );
  hist_temp->SetFillColorAlpha( kBlack, 0.1 );
  hist_temp->Draw();
  this->DrawWords();
  c->Print( c->GetName() );

  vector < MipHist* > mip_hists;
  mip_hists.push_back( hist_ang90_ );
  mip_hists.push_back( hist_ang45_ );
  mip_hists.push_back( hist_ang35_ );
  
  vector < TH1D* > hists;
  // hists.push_back( hist_all_->GetHist() );
  // hists.push_back( hist_aso_->GetHist() );
  //hists.push_back( hist_no_aso_->GetHist() );
  hists.push_back( hist_ang90_->GetHist() );
  hists.push_back( hist_ang45_->GetHist() );
  hists.push_back( hist_ang35_->GetHist() );
  // hists.push_back( hist_ang25_->GetHist() );

  //  gStyle->SetOptFit( true );
  //  mh->SetStatsFormat( 111111 );
  
  for( auto& mip_hist : mip_hists )
    {
      mip_hist->GetHist()->Draw( (mip_hist == mip_hists[0] ? "" : "same" ) );
    }

  gPad->Update();
  this->DrawWords();
  c->Print( c->GetName() );

  for( auto& mip_hist : mip_hists )
    {     
      mip_hist->GetNormalizedHist()->Draw( (mip_hist == mip_hists[0] ? "HISTE" : "HISTE same" ) );
    }
  
  gPad->SetLogy( false );
  for( auto& mip_hist : mip_hists )
    {
      mip_hist->GetNormalizedHist()->Draw( (mip_hist == mip_hists[0] ? "HISTE" : "HISTE same" ) );
    }

  this->DrawWords();
  c->Print( c->GetName() );  

  /*
  this_cut = "!(adc_in==210 && size_in==1) && fabs(z_vertex)<23 && is_associated_in";
  //string cut = "!(adc_in==210 && size_in==1)";
  auto hist_theta = Draw( tr, "theta", "#theta distribution;#theta;Entries", 180, -90, 90,
			"track_incoming_theta_in", this_cut, "", kBlack, 0.1 );
  hist_theta->Draw();
  c->Print( c->GetName() );  

  this_cut = "!(adc_in==210 && size_in==1) && is_associated_in";
  auto hist_z = Draw( tr, "z_vertex", "z_{vtx} distribution;z_{vtx} (cm);Entries", 100, -50, 50,
		      "z_vertex", this_cut, "", kBlack, 0.1 );
  hist_z->Draw();
  c->Print( c->GetName() );  
  */

  hist_correlation->Draw( "colz" );
  gPad->SetLogz( true );
  //  DrawStats( hist_correlation, 0.7, 0.7, 0.9, 0.9 );
  this->DrawWords();
  c->Print( c->GetName() );  
  c->Print( ((string)c->GetName() + "]").c_str() );

  
}

void Analysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}


