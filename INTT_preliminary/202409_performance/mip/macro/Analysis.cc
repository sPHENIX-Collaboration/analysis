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
      
    }
    
  Init( tree );
}

//////////////////////////////////////////////////////////////////////
// Destructor                                                       //
//////////////////////////////////////////////////////////////////////
Analysis::~Analysis()
{
   if (!fChain)
     return;

   delete fChain->GetCurrentFile();
}

//////////////////////////////////////////////////////////////////////
// Init                                                             //
//////////////////////////////////////////////////////////////////////
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
   if( !tree )
     return;
   
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("evt_clus"			, &evt_clus, &b_evt_clus				      );
   fChain->SetBranchAddress("x_in"			, &x_in, &b_x_in					      );
   fChain->SetBranchAddress("y_in"			, &y_in, &b_y_in					      );
   fChain->SetBranchAddress("z_in"			, &z_in, &b_z_in					      );
   fChain->SetBranchAddress("r_in"			, &r_in, &b_r_in					      );
   fChain->SetBranchAddress("size_in"			, &size_in, &b_size_in					      );
   fChain->SetBranchAddress("phi_in"			, &phi_in, &b_phi_in					      );
   fChain->SetBranchAddress("theta_in"			, &theta_in, &b_theta_in				      );
   fChain->SetBranchAddress("adc_in"			, &adc_in, &b_adc_in					      );
   fChain->SetBranchAddress("is_associated_in"		, &is_associated_in, &b_is_associated_in		      );
   fChain->SetBranchAddress("track_incoming_theta_in"	, &track_incoming_theta_in, &b_track_incoming_theta_in	      );
   fChain->SetBranchAddress("x_out"			, &x_out, &b_x_out					      );
   fChain->SetBranchAddress("y_out"			, &y_out, &b_y_out					      );
   fChain->SetBranchAddress("z_out"			, &z_out, &b_z_out					      );
   fChain->SetBranchAddress("r_out"			, &r_out, &b_r_out					      );
   fChain->SetBranchAddress("size_out"			, &size_out, &b_size_out				      );
   fChain->SetBranchAddress("phi_out"			, &phi_out, &b_phi_out					      );
   fChain->SetBranchAddress("theta_out"			, &theta_out, &b_theta_out				      );
   fChain->SetBranchAddress("adc_out"			, &adc_out, &b_adc_out					      );
   fChain->SetBranchAddress("is_associated_out"		, &is_associated_out, &b_is_associated_out		      );
   fChain->SetBranchAddress("track_incoming_theta_out"	, &track_incoming_theta_out, &b_track_incoming_theta_out      );
   fChain->SetBranchAddress("z_vertex"			, &z_vertex, &b_z_vertex				      );
   Notify();
   
  string title = "ADC distribution;ADC [arb. units];Counts [arb. units]";

  hist_all_ = new MipHist( "Raw", title );  
  hist_all_->SetColorAlpha( kGreen+2, 0.2 );
  hist_all_->SetTag( "Raw ADC" );

  hist_aso_ = new MipHist( "Track_assoiation", title );
  hist_aso_->SetColorAlpha( kRed, 0.1 );
  hist_aso_->SetTag( "Tracklet asso." );

  hist_no_aso_ = new MipHist( "Track_not_assoiated", title );
  hist_no_aso_->SetColorAlpha( kBlack, 0.1 );
  hist_no_aso_->SetTag( "No tracklet asso." );

  ///////////////////////////////////////////////////////////////
  // Misaki's config
  ///////////////////////////////////////////////////////////////
  hist_ang85_ = new MipHist( "Ang85_90", title );
  hist_ang85_->SetColorAlpha( kBlue, 0.1 );
  hist_ang85_->SetTag( "85#circ < |#theta^{INTT}| #leq 90#circ" );

  hist_ang45_ = new MipHist( "Ang40_45", title );
  hist_ang45_->SetColorAlpha( kRed, 0.1 );
  hist_ang45_->SetTag( "40#circ < |#theta^{INTT}| #leq 45#circ" );

  hist_ang35_ = new MipHist( "Ang30_35", title );
  hist_ang35_->SetColorAlpha( kSpring-1, 0.1 );
  hist_ang35_->SetTag( "30#circ < |#theta^{INTT}| #leq 35#circ" );

  hist_ang25_ = new MipHist( "Ang20_25", title );
  hist_ang25_->SetColorAlpha( kOrange+1, 0.1 );
  hist_ang25_->SetTag( "20#circ < |#theta^{INTT}| #leq 25#circ" );

  ///////////////////////////////////////////////////////////////
  // my config                                                 //
  ///////////////////////////////////////////////////////////////
  hist_ang80_ = new MipHist( "Ang80_90", title );
  hist_ang80_->SetColorAlpha( kRed, 0.1 );
  hist_ang80_->SetTag( "80#circ < |#theta^{INTT}| #leq 90#circ" );

  hist_ang70_ = new MipHist( "Ang70_80", title );
  hist_ang70_->SetColorAlpha( kRed, 0.1 );
  hist_ang70_->SetTag( "70#circ < |#theta^{INTT}| #leq 80#circ" );

  hist_ang60_ = new MipHist( "Ang60_70", title );
  hist_ang60_->SetColorAlpha( kRed, 0.1 );
  hist_ang60_->SetTag( "60#circ < |#theta^{INTT}| #leq 70#circ" );

  hist_ang50_ = new MipHist( "Ang50_60", title );
  hist_ang50_->SetColorAlpha( kRed, 0.1 );
  hist_ang50_->SetTag( "50#circ < |#theta^{INTT}| #leq 60#circ" );

  hist_ang40_ = new MipHist( "Ang40_50", title );
  hist_ang40_->SetColorAlpha( kRed, 0.1 );
  hist_ang40_->SetTag( "40#circ < |#theta^{INTT}| #leq 50#circ" );

  hist_ang30_ = new MipHist( "Ang30_40", title );
  hist_ang30_->SetColorAlpha( kRed, 0.1 );
  hist_ang30_->SetTag( "30#circ < |#theta^{INTT}| #leq 40#circ" );

  hist_ang20_ = new MipHist( "Ang20_30", title );
  hist_ang20_->SetColorAlpha( kRed, 0.1 );
  hist_ang20_->SetTag( "20#circ < |#theta^{INTT}| #leq 30#circ" );

  hist_ang10_ = new MipHist( "Ang10_20", title );
  hist_ang10_->SetColorAlpha( kRed, 0.1 );
  hist_ang10_->SetTag( "10#circ < |#theta^{INTT}| #leq 20#circ" );

  hist_ang0_ = new MipHist( "Ang0_10", title );
  hist_ang0_->SetColorAlpha( kRed, 0.1 );
  hist_ang0_->SetTag( "0#circ < |#theta^{INTT}| #leq 10#circ" );

  ///////////////////////////////////////////////////////////////
  // fine selection                                            //
  ///////////////////////////////////////////////////////////////
  hist_ang10_11_ = new MipHist( "Ang10_11", title );
  hist_ang10_11_->SetColorAlpha( kRed, 0.1 );
  hist_ang10_11_->SetTag( "10#circ < |#theta^{INTT}| #leq 11#circ" );

  hist_ang20_21_ = new MipHist( "Ang20_21", title );
  hist_ang20_21_->SetColorAlpha( kRed, 0.1 );
  hist_ang20_21_->SetTag( "20#circ < |#theta^{INTT}| #leq 21#circ" );

  hist_ang30_31_ = new MipHist( "Ang30_31", title );
  hist_ang30_31_->SetColorAlpha( kRed, 0.1 );
  hist_ang30_31_->SetTag( "30#circ < |#theta^{INTT}| #leq 31#circ" );

  ///////////////////////////////////////////////////////////////
  // put all into a vector                                     //
  ///////////////////////////////////////////////////////////////
  hists_.push_back( hist_all_ );
  hists_.push_back( hist_aso_ );
  hists_.push_back( hist_no_aso_ );
  hists_.push_back( hist_ang85_ );
  hists_.push_back( hist_ang45_ );
  hists_.push_back( hist_ang35_ );
  hists_.push_back( hist_ang25_ );

  hists_.push_back( hist_ang80_ );
  hists_.push_back( hist_ang70_ );
  hists_.push_back( hist_ang60_ );
  hists_.push_back( hist_ang50_ );
  hists_.push_back( hist_ang40_ );
  hists_.push_back( hist_ang30_ );
  hists_.push_back( hist_ang20_ );
  hists_.push_back( hist_ang10_ );
  hists_.push_back( hist_ang0_ );
  
  hists_.push_back( hist_ang10_11_ );
  hists_.push_back( hist_ang20_21_ );
  hists_.push_back( hist_ang30_31_ );
  
  hist_correlation = new TH2D( "nhit_correlation_barrel",
			       "Cluster correlation between barrels;Number of clusters at inner barrel;Number of clusters at outer barrel;",
			       100, 0, 100, 
			       100, 0, 100 ); 
  HistSetting( hist_correlation );
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
  double pos_x = 0.45 - right_margin_;
  if( is_preliminary_ == false )
    {
      tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Internal" );
    }
  else
    {
      //pos_x = 0.6;
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
   if (!fChain)
     return 0;
   
   return fChain->GetEntry(entry);
}

Long64_t Analysis::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain)
     return -5;
   
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0)
     return centry;
   
   if (fChain->GetTreeNumber() != fCurrent)
     {
       fCurrent = fChain->GetTreeNumber();
       Notify();
     }
   
   return centry;
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

void Analysis::DrawSingle( MipHist* mip_hist )
{
  int color = mip_hist->GetColor();
  double alpha = mip_hist->GetAlpha();

  mip_hist->SetColorAlpha( kBlack, 0.1, true );
  
  HistSetting1D( mip_hist->GetHist() );
  mip_hist->GetHist()->Draw();
  this->DrawWords();

  string page_title = string("Title: ") + mip_hist->GetHist()->GetName();
  c_->Print( c_->GetName(), page_title.c_str() );

  string output = c_->GetName();
  output = output.substr( 0, output.find_last_of("_") ) + "_" + mip_hist->GetHist()->GetName();
  if( is_preliminary_ == true )
    output += "_preliminary.pdf";
  else
    output += "_internal.pdf";
    
  c_->Print( output.c_str() );

  // restore color and alpha
  mip_hist->SetColorAlpha( color, alpha, true );
}

void Analysis::DrawMultiple( vector < MipHist* >& mip_hists, string output_tag, bool use_color_palette )
{

  TLegend* leg = new TLegend( 0.5, 0.75 - mip_hists.size() * 0.05, 0.8, 0.75);
  int counter = 0;
  for( auto& mip_hist : mip_hists )
    {
      //double top_val = 0.05 * ++counter + 1;
      double top_val = ++counter ;
      
      auto hist = mip_hist->GetNormalizedHist( top_val );
      HistSetting1D( hist );

      hist->GetYaxis()->SetRangeUser( 0, 0.2 );

      if( use_color_palette )
	{
	  hist->SetFillStyle( 0 );
	  hist->SetFillColorAlpha( kWhite, 0 );
	  hist->Draw( (mip_hist == mip_hists[0] ? "HISTE PLC" : "HISTE PLC same" ) );
	}
      else 
	hist->Draw( (mip_hist == mip_hists[0] ? "HISTE" : "HISTE same" ) );

      leg->AddEntry( hist, mip_hist->GetTag().c_str() );
      // auto f = mip_hist->GetNormalizedFunction( top_val );
      // f->Draw( "same" );
      // mip_hist->DrawLine( f );
      
    }

  leg->Draw();
  this->DrawWords();
  c_->Print( c_->GetName() );  

  string output = c_->GetName();
  output = output.substr( 0, output.find_last_of("_") ) + "_" + output_tag;
  if( is_preliminary_ == true )
    output += "_preliminary.pdf";
  else
    output += "_internal.pdf";

  c_->Print( output.c_str() );
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
	{
	  counter_too_large_cluster_++;
	  continue;
	}

      /* if( fabs( z_vertex - (*z)[i] ) > 5 ) */ // no need...
      /*   continue; */
	         
      int adc = (*adcs)[i];
      bool is_single_hit_cluster_adc7  = ( (*size)[i] == 1 && adc == adc7_ );

      // Modification of bins containig 1 hit with ADC7, No need to use it because it's not fair modification.
      // bool is_double_hit_cluster_adc7  = ( (*size)[i] == 2 
      // 					   && (adc == 245 || adc == 255 || adc == 390 )
      // 					   );
      // is_single_hit_cluster_adc7 = is_single_hit_cluster_adc7 || is_double_hit_cluster_adc7;
      
      bool is_double_hit_cluster_adc14 = ( (*size)[i] == 2 && adc == adc7_ * 2 );
      hist_all_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	
      // if this cluster is not associated with a tracklet, store info here and skip the rest parts
      if( (*is_associated)[i] == false )
	{

	  hist_no_aso_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	  continue;
	}
	   
      hist_aso_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
      ///////////////////////////////////////////////////////////////
      // Misaki's config                                           //
      ///////////////////////////////////////////////////////////////
      double angle_abs = fabs( (*track_incoming_theta)[i] );
      
      if( 85 <= angle_abs )
	{
	  hist_ang85_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 40 < angle_abs && angle_abs <= 45 )
	{
	  hist_ang45_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 30 < angle_abs && angle_abs <= 35 )
	{
	  hist_ang35_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 20 < angle_abs && angle_abs <= 25 )
	{
	  hist_ang25_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
  
      ///////////////////////////////////////////////////////////////
      // My config                                                 //
      ///////////////////////////////////////////////////////////////
      if( 80 < angle_abs && angle_abs <= 90 )
	{
	  hist_ang80_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 70 < angle_abs && angle_abs <= 80 )
	{
	  hist_ang70_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 60 < angle_abs && angle_abs <= 70 )
	{
	  hist_ang60_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 50 < angle_abs && angle_abs <= 60 )
	{
	  hist_ang50_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 40 < angle_abs && angle_abs <= 50 )
	{
	  hist_ang40_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 30 < angle_abs && angle_abs <= 40 )
	{
	  hist_ang30_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 20 < angle_abs && angle_abs <= 30 )
	{
	  hist_ang20_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 10 < angle_abs && angle_abs <= 20 )
	{
	  hist_ang10_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 0 < angle_abs && angle_abs <= 10 )
	{
	  hist_ang0_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else
	cout << (*track_incoming_theta)[i] << endl;

      ///////////////////////////////////////////////////////////////
      // Fine selection                                            //
      ///////////////////////////////////////////////////////////////
      if( 10 < angle_abs && angle_abs <= 11 )
	{
	  hist_ang10_11_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 20 < angle_abs && angle_abs <= 21 )
	{
	  hist_ang20_21_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}
      else if( 30 < angle_abs && angle_abs <= 31 )
	{
	  hist_ang30_31_->FillAll( adc, is_single_hit_cluster_adc7, is_double_hit_cluster_adc14 );
	}

    } // end of for( int i=0; i<adc->size(); i++ )

}

///////////////////////////////////////////////////////////////////////
// public functions                                                  //
///////////////////////////////////////////////////////////////////////

void Analysis::Loop()
{
  //     This is the loop skeleton where:
  //    jentry is the global entry number in the chain
  //    ientry is the entry number in the current Tree
  //  Note that the argument to GetEntry must be:
  //    jentry for TChain::GetEntry
  //    ientry for TTree::GetEntry and TBranch::GetEntry
  
  if( fChain == 0 )
    return;

   Long64_t nentries = fChain->GetEntriesFast();

   bool does_z_cut = true;
   
   for (Long64_t jentry=0; jentry<nentries;jentry++)
     {
       Long64_t ientry = LoadTree(jentry);

       int hit_num_inner = std::count( is_associated_in->begin(), is_associated_in->end(), true );
       int hit_num_outer = std::count( is_associated_out->begin(), is_associated_out->end(), true );
       hist_correlation->Fill( hit_num_inner, hit_num_outer );
       
       if (ientry < 0)
	 break;

       fChain->GetEntry(jentry);
       
       double cluster_num_in = adc_in->size();
       double cluster_num_out = adc_out->size();
       double cluster_num = cluster_num_in + cluster_num_out;
       double cluster_num_asymmetry = fabs(cluster_num_in - cluster_num_out) / cluster_num;

       bool is_good_event = true;

       if( cluster_num_in <= 3
	   || cluster_num_out <= 3
	   || cluster_num <= 6
	   || cluster_num_asymmetry > 0.1
	   )
	 is_good_event = false;

       ////////////////////////////////////////////////////////////////////
       // Loop over inner hits
       if( is_good_event == false )
	 continue;
       
       this->FillClusterInfo( 0 );
       this->FillClusterInfo( 1 );

     }

   for( auto& hist : hists_ )
     {
       hist->SetAdc7Correction( does_adc7_correction_ );
       hist->SetAdc14Correction( does_adc14_correction_ );
       hist->ModifyAdc();
       //hist->Print();
     }

}

void Analysis::Draw()
{

  SetsPhenixStyle();
  this->InitParameter();
  
  string output = "results/mip_" + to_string( run_ );
  if( is_preliminary_ == true )
    output += "_preliminary.pdf";
  else
    output += "_internal.pdf";

  TFile* tf_output = new TFile( (output.substr(0, output.size()-4) + ".root").c_str(), "RECREATE" );
  
  c_ = new TCanvas( output.c_str(), "title", 800, 800 );
  c_->Print( ((string)c_->GetName() + "[").c_str() );

  // drawing a single hist
  for( auto& mip_hist : hists_ )
    this->DrawSingle( mip_hist );

  //  if( false )
  { // same as Misaki
    vector < MipHist* > mip_hists;
    mip_hists.push_back( hist_ang85_ );
    mip_hists.push_back( hist_ang45_ );
    mip_hists.push_back( hist_ang35_ );
    this->DrawMultiple( mip_hists, "85-90_40-45_30-35" );
  }

  //  if( false )
  { // show cuts effect
    vector < MipHist* > mip_hists;
    mip_hists.push_back( hist_all_ );
    //mip_hists.push_back( hist_aso_ );
    mip_hists.push_back( hist_ang85_ );
    this->DrawMultiple( mip_hists, "raw_only_top_angle" );
  }

  //  if( false )
  { // tracklet association or not
    vector < MipHist* > mip_hists;
    mip_hists.push_back( hist_aso_ );
    mip_hists.push_back( hist_no_aso_ );
    this->DrawMultiple( mip_hists, "associated_not_associated" );
  }

  gStyle -> SetPalette( kLightTemperature );
  //  if( false )
  { // all my config
    vector < MipHist* > mip_hists;
    mip_hists.push_back( hist_ang80_ );
    mip_hists.push_back( hist_ang70_ );
    mip_hists.push_back( hist_ang60_ );
    mip_hists.push_back( hist_ang50_ );
    mip_hists.push_back( hist_ang40_ );
    mip_hists.push_back( hist_ang30_ );
    mip_hists.push_back( hist_ang20_ );
    mip_hists.push_back( hist_ang10_ );
    mip_hists.push_back( hist_ang0_ );

    this->DrawMultiple( mip_hists, "all", true );
  }

  { // all my config
    vector < MipHist* > mip_hists;
    mip_hists.push_back( hist_ang80_ );
    mip_hists.push_back( hist_ang50_ );
    mip_hists.push_back( hist_ang20_ );
    mip_hists.push_back( hist_ang0_ );
    this->DrawMultiple( mip_hists, "80-90_50-60_20-30_0-10", true );
  }

  { // all my config
    vector < MipHist* > mip_hists;
    mip_hists.push_back( hist_ang10_11_ );
    mip_hists.push_back( hist_ang20_21_ );
    mip_hists.push_back( hist_ang30_31_ );
    this->DrawMultiple( mip_hists, "10-11_20-21_30-31", true );
  }

  //////////////////////////////////////////////////////////////////////
  // #cluster correlation                                             //
  //////////////////////////////////////////////////////////////////////
  gStyle -> SetPalette( kBird );

  top_margin_ = right_margin_ = 0.13;
  c_->SetTopMargin( top_margin_ );
  c_->SetBottomMargin( 0.13 );
  c_->SetRightMargin( right_margin_ );
  c_->SetLeftMargin( 0.13 );

  hist_correlation->GetYaxis()->SetTitleOffset( 1.2 );
  hist_correlation->Draw( "colz" );
  gPad->SetLogz( true );
  //  DrawStats( hist_correlation, 0.7, 0.7, 0.9, 0.9 );
  this->DrawWords();
  c_->Print( c_->GetName() );
  
  string output_correlation = output.substr( 0, output.find_last_of("_") ) + "_cluster_correlation";
  if( is_preliminary_ == true )
    output_correlation += "_preliminary.pdf";
  else
    output_correlation += "_internal.pdf";
    
  c_->Print( output_correlation.c_str() );
  
  c_->Print( ((string)c_->GetName() + "]").c_str() );

  for( auto& mip_hist : hists_ )
    tf_output->WriteTObject( mip_hist->GetHist(), mip_hist->GetHist()->GetName() );

  tf_output->WriteTObject( hist_correlation, hist_correlation->GetName() );
  tf_output->Close();
}

void Analysis::InitParameter()
{

  adc7_ = 210;
  counter_too_large_cluster_ = 0;
  cluster_size_max_ = 5;
  hit_num_inner_ = 0;
  hit_num_outer_ = 0;
  
  top_margin_ = 0.05; // 0.1;
  right_margin_ = 0.05; // 0.15;
  adc7_modification_factor_ = 0.35;
}

void Analysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

void Analysis::Print()
{

  for( auto& hist : hists_ )
    hist->Print();
}
