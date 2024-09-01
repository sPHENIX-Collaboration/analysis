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

  int run_ = 0;
  int adc7_ = 210;
  int cluster_size_max_ = 9999;
  
  bool is_preliminary_ = false;
  
  double top_margin_ = 0.05; // 0.1;
  double right_margin_ = 0.05; // 0.15;
  double adc7_modification_factor_ = 0.35;
  
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
  MipHist* hist_ang90_;
  MipHist* hist_ang45_;
  MipHist* hist_ang35_;
  MipHist* hist_ang25_;

  TH1D* hist_all;
  TH1D* hist_aso;
  TH1D* hist_no_aso;
  TH1D* hist_ang90;
  TH1D* hist_ang45;
  TH1D* hist_ang35;
  TH1D* hist_ang25;

  TH2D* hist_correlation;
  
  virtual ~Analysis();
  virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree);
  virtual Bool_t   Notify();

  string GetDate();
  void DrawWords();
  void FillClusterInfo( int mode = 0 ); // 0: inner, 1: outer
  void ModifyAdcs();
  void ModifyAdc( TH1D* hist );
  void ModifyAdc( TH1D* hist, int mode ); // mode: 0: ADC7, 1: ADC14
  
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
  void SetPreliminary( bool flag ){ is_preliminary_ = flag;};
  void Draw();
};

#endif

#ifndef Analysis_cxx
#include "Analysis.cc"
#endif // #ifdef Analysis_cxx
