#ifndef Analysis_h
#define Analysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

#include <sPhenixStyle.C>
#include "HistSetting.hh"
#include "MipHist.hh"

class Analysis
{
private:
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain
  TCanvas* c_;
  
  int run_ = 0;
  int adc7_ = 210;
  int counter_too_large_cluster_ = 0;
  int cluster_size_max_ = 5;
  int hit_num_inner_ = 0;
  int hit_num_outer_ = 0;
  
  double top_margin_ = 0.05; // 0.1;
  double right_margin_ = 0.05; // 0.15;
  double adc7_modification_factor_ = 0.35;

  bool is_preliminary_ = false;
  bool does_adc7_correction_ = true;
  bool does_adc14_correction_ = true;
 
  // Fixed size dimensions of array or collections stored in the TTree if any.

  // Declaration of leaf types
  Int_t           evt_clus;
  vector<double>  *x_in;
  vector<double>  *y_in;
  vector<double>  *z_in;
  vector<double>  *r_in;
  vector<int>     *size_in;
  vector<double>  *phi_in;
  vector<double>  *theta_in;
  vector<double>  *adc_in;
  vector<bool>    *is_associated_in;
  vector<double>  *track_incoming_theta_in;
  vector<double>  *x_out;
  vector<double>  *y_out;
  vector<double>  *z_out;
  vector<double>  *r_out;
  vector<int>     *size_out;
  vector<double>  *phi_out;
  vector<double>  *theta_out;
  vector<double>  *adc_out;
  vector<bool>    *is_associated_out;
  vector<double>  *track_incoming_theta_out;
  Double_t        z_vertex;

  // List of branches
  TBranch        *b_evt_clus;   //!
  TBranch        *b_x_in;   //!
  TBranch        *b_y_in;   //!
  TBranch        *b_z_in;   //!
  TBranch        *b_r_in;   //!
  TBranch        *b_size_in;   //!
  TBranch        *b_phi_in;   //!
  TBranch        *b_theta_in;   //!
  TBranch        *b_adc_in;   //!
  TBranch        *b_is_associated_in;   //!
  TBranch        *b_track_incoming_theta_in;   //!
  TBranch        *b_x_out;   //!
  TBranch        *b_y_out;   //!
  TBranch        *b_z_out;   //!
  TBranch        *b_r_out;   //!
  TBranch        *b_size_out;   //!
  TBranch        *b_phi_out;   //!
  TBranch        *b_theta_out;   //!
  TBranch        *b_adc_out;   //!
  TBranch        *b_is_associated_out;   //!
  TBranch        *b_track_incoming_theta_out;   //!
  TBranch        *b_z_vertex;   //!

  //TH1D* hist;
  vector < MipHist* > hists_;
  MipHist* hist_all_;
  MipHist* hist_aso_;
  MipHist* hist_no_aso_;
  MipHist* hist_ang85_; // 85 - 90 deg
  MipHist* hist_ang45_; // 45 - 55 deg
  MipHist* hist_ang35_; // 35 - 45 deg
  MipHist* hist_ang25_; // 25 - 35 deg

  MipHist* hist_ang80_; // 80 - 90 deg
  MipHist* hist_ang70_; // 70 - 80 deg
  MipHist* hist_ang60_; // 60 - 70 deg
  MipHist* hist_ang50_; // 50 - 60 deg
  MipHist* hist_ang40_; // 40 - 50 deg
  MipHist* hist_ang30_; // 30 - 40 deg
  MipHist* hist_ang20_; // 20 - 30 deg
  MipHist* hist_ang10_; // 10 - 20 deg
  MipHist* hist_ang0_ ; //  0 - 10 deg

  MipHist* hist_ang10_11_ ; //  10 - 11 deg
  MipHist* hist_ang20_21_ ; //  20 - 21 deg
  MipHist* hist_ang30_31_ ; //  30 - 31 deg

  TH1D* hist_association_;
  TH1D* hist_association_in_;
  TH1D* hist_association_out_;
  TH2D* hist_association_in_out_;
  TH2D* hist_correlation_;
  
  virtual ~Analysis();
  virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree);
  virtual Bool_t   Notify();

  string GetDate();
  double GetExpectedPeakPosition( double theta );
  void DrawSingle( MipHist* mip_hist );
  void DrawMultiple( vector < MipHist* >& mip_hists, string output_tag, bool use_color_palette = false );
  void DrawMultiPanel( vector < MipHist* >& mip_hists, string output_tag );

  TBox* DrawExpectedPeakRegion( double theta_min, double theta_max );
  void DrawWords();
  void FillClusterInfo( int mode = 0 ); // 0: inner, 1: outer
  
  template < typename TH >
  void DrawStats( TH* hist, double xmin, double ymin, double xmax, double ymax, int font = 4)
  {

    gPad->Update();

    TPaveStats *st = (TPaveStats*)hist->FindObject("stats");
    if( st == nullptr )
      {
	string color_red = "\033[31m";
	string color_cancel = "\033[m";

	cerr << color_red << endl;
	cerr << " " << string( 50, '=' ) << endl;
	cerr << " void DrawStats( TH* hist, double xmin, double ymin, double xmax, double ymax, int font = 4)" << endl;
	cerr << " TPaveStats *st = (TPaveStats*)hist->FindObject(\"stats\")  ---> nullptr" << endl;
	cerr << " Someting wrong!!!" << endl;
	cerr << " " << string( 50, '=' ) << endl;
	cerr << color_cancel << endl;
	return;

      }

    st->SetTextColorAlpha( hist->GetLineColor(), 1.0 );
    st->SetLineColorAlpha( hist->GetLineColor(), 1.0 );
    st->SetFillStyle( 1001 );
    st->SetFillColor( 0 );
    //  st->SetTextFont( font );

    st->SetX1NDC( xmin );
    st->SetX2NDC( xmax );
    st->SetY1NDC( ymin );
    st->SetY2NDC( ymax );

    //  st->SetOptStat( 111111 ) ; // overflow and underflos are ON
    st->Draw("same");
  }

  
public:

  Analysis( int run = 41981 );
  
  virtual void     Loop();
  virtual void     Show(Long64_t entry = -1);

  void Draw();
  void InitParameter();

  void Print();
  
  void SetAdc7Correction( bool flag=true ){ does_adc7_correction_ = flag; };
  void SetAdc14Correction( bool flag=true ){ does_adc14_correction_ = flag; };
  void SetPreliminary( bool flag ){ is_preliminary_ = flag;};
};

#endif

#ifndef Analysis_cxx
#include "Analysis.cc"
#endif // #ifdef Analysis_cxx
