#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"
#endif

TH1D* Draw( TTree* tr,
	   string name, string title,
	   int bin, double xmin, double xmax,
	    string expression_arg, string cut, string option,
	   int color, double alpha )
{
  TH1D* hist = new TH1D( name.c_str(), title.c_str(),
			 bin, xmin, xmax );
  hist->SetLineColor( color );
  hist->SetLineWidth( 3 );
  hist->SetFillColorAlpha( color, alpha );

  cout << name << "\t" << cut << endl;
  string expression = expression_arg + ">>" + name;
  tr->Draw( expression.c_str(), cut.c_str(), "goff" );

  //  hist->Draw( option.c_str() );
  return ((TH1D*)hist->Clone());
}

int macro()
{
  
  string data = "tracking_run41981.root";
  TFile* tf = new TFile( data.c_str(), "READ" );
  TTree* tr = (TTree*)tf->Get( "clus_tree" );

  string output = "output.pdf";
  TCanvas* c = GetCanvas( output, "double" );

  int color = 0;
  string title = "DAC distribution;DAC;Entries";
  string cut = "!(adc_in==210 && size_in==1) && fabs(z_vertex)<23";

  int bin_num = 20;
  double xmin = 0;
  double xmax = 600;
  
  //string cut = "!(adc_in==210 && size_in==1)";
  auto hist_all = Draw( tr, "ADC", title, bin_num, xmin, xmax,
			"adc_in", cut, "", kGray, 0.1 );

  color++;
  cut += " && is_associated_in";
  auto hist_aso = Draw( tr, "Track_asso", title, bin_num, xmin, xmax,
			"adc_in", cut, "", kBlack, 0.1 );

  color++;
  string this_cut = cut + "&& fabs(track_incoming_theta_in)>85";
  auto hist_ang90 = Draw( tr, "Top_pm5", title, bin_num, xmin, xmax,
			"adc_in", this_cut, "", kBlue, 0.1 );

  this_cut = cut + "&& 40 <  fabs(track_incoming_theta_in) &&  fabs(track_incoming_theta_in) <45";
  auto hist_ang45 = Draw( tr, "Ang40_45", title, bin_num, xmin, xmax,
			  "adc_in", this_cut, "", kRed, 0.1 );

  this_cut = cut + "&& 30 <  fabs(track_incoming_theta_in) &&  fabs(track_incoming_theta_in) <35";
  auto hist_ang35 = Draw( tr, "Ang30_35", title, bin_num, xmin, xmax,
			  "adc_in", this_cut, "", kSpring-1, 0.1 );

  vector < TH1D* > hists;
  // hists.push_back( hist_all );
  // hists.push_back( hist_aso );
  hists.push_back( hist_ang90 );
  hists.push_back( hist_ang45 );
  hists.push_back( hist_ang35 );

  MultiHist* mh = new MultiHist( hist_all->GetName(), title, hists);
  gStyle->SetOptFit( true );
  mh->SetStatsFormat( 111111 );
  mh->SetStatsBoxSize( 0.15, 0.25 );
  //mh->SetStatsBoxPoint( 1.0, 1.0 );
  
  vector < TF1* > functions;
  for( auto hist : mh->GetHists() )
    {
      TF1* f = new TF1( "f", "landau", 35, 600 );
      f->SetLineColor( hist->GetLineColor() );
      f->SetLineStyle( 7 );
      hist->Fit( f );
      functions.push_back( f );
    }
  
  c->Print( ((string)c->GetName() + "[").c_str() );
  mh->Draw( "HIST" );
  for( auto& f : functions )
    {
      f->Draw( "same" );  
      
      TLine* line = new TLine();
      line->SetLineColor( f->GetLineColor() );
      line->SetLineStyle( 3 );
      line->DrawLine( f->GetParameter(1), 0, f->GetParameter(1), 1e3 );
    }

  gPad->Update();
  c->Print( c->GetName() );

  mh->SetYmin( 1 );
  mh->Draw( "HIST" );
  for( auto& f : functions )
    f->Draw( "same" );  
  gPad->SetLogy( true );
  c->Print( c->GetName() );  
  
  gPad->SetLogy( false );
  mh->NormalizeHist();
  mh->Draw( "HISTE" );

  for( auto hist : mh->GetHists() )
    {
      TF1* f = new TF1( "f", "landau", 35, 600 );
      f->SetLineColor( hist->GetLineColor() );
      f->SetLineStyle( 7 );
      hist->Fit( f );
      //functions.push_back( f );
      f->Draw( "same" );

      TLine* line = new TLine();
      line->SetLineColor( f->GetLineColor() );
      line->SetLineStyle( 3 );
      line->DrawLine( f->GetParameter(1), 0, f->GetParameter(1), 0.3 );
    }
  c->Print( c->GetName() );  

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


  c->Print( ((string)c->GetName() + "]").c_str() );
  return 0;
}

/*
  root [2] clus_tree->Print()
  ******************************************************************************
  *Tree    :clus_tree : clus_tree                                              *
  *Entries :    13051 : Total =        30533230 bytes  File  Size =   17444818 *
  *        :          : Tree compression factor =   1.75                       *
  ******************************************************************************
  *Br    0 :evt_clus  : evt_clus/I                                             *
  *Entries :    13051 : Total  Size=      52861 bytes  File Size  =      18424 *
  *Baskets :        2 : Basket Size=      32000 bytes  Compression=   2.84     *
  *............................................................................*
  *Br    1 :x_in      : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1749683 bytes  File Size  =     991904 *
  *Baskets :       57 : Basket Size=      32000 bytes  Compression=   1.76     *
  *............................................................................*
  *Br    2 :y_in      : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1749683 bytes  File Size  =     997721 *
  *Baskets :       57 : Basket Size=      32000 bytes  Compression=   1.75     *
  *............................................................................*
  *Br    3 :z_in      : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1749683 bytes  File Size  =     365546 *
  *Baskets :       57 : Basket Size=      32000 bytes  Compression=   4.78     *
  *............................................................................*
  *Br    4 :r_in      : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1749683 bytes  File Size  =    1427984 *
  *Baskets :       57 : Basket Size=      32000 bytes  Compression=   1.22     *
  *............................................................................*
  *Br    5 :size_in   : vector<int>                                            *
  *Entries :    13051 : Total  Size=     966823 bytes  File Size  =     223198 *
  *Baskets :       32 : Basket Size=      32000 bytes  Compression=   4.33     *
  *............................................................................*
  *Br    6 :phi_in    : vector<double>                                         *
  *Entries :    13051 : Total  Size=    2939479 bytes  File Size  =    2385408 *
  *Baskets :       94 : Basket Size=      32000 bytes  Compression=   1.23     *
  *............................................................................*
  *Br    7 :theta_in  : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1749927 bytes  File Size  =    1588554 *
  *Baskets :       57 : Basket Size=      32000 bytes  Compression=   1.10     *
  *............................................................................*
  *Br    8 :adc_in    : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1749805 bytes  File Size  =     366312 *
  *Baskets :       57 : Basket Size=      32000 bytes  Compression=   4.77     *
  *............................................................................*
  *Br    9 :is_associated_in : vector<bool>                                    *
  *Entries :    13051 : Total  Size=     379783 bytes  File Size  =     127553 *
  *Baskets :       14 : Basket Size=      32000 bytes  Compression=   2.97     *
  *............................................................................*
  *Br   10 :track_incoming_theta_in : vector<double>                           *
  *Entries :    13051 : Total  Size=    1750842 bytes  File Size  =    1011283 *
  *Baskets :       57 : Basket Size=      32000 bytes  Compression=   1.73     *
  *............................................................................*
  *Br   11 :x_out     : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1574922 bytes  File Size  =     892933 *
  *Baskets :       51 : Basket Size=      32000 bytes  Compression=   1.76     *
  *............................................................................*
  *Br   12 :y_out     : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1574922 bytes  File Size  =     901352 *
  *Baskets :       51 : Basket Size=      32000 bytes  Compression=   1.75     *
  *............................................................................*
  *Br   13 :z_out     : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1574922 bytes  File Size  =     336037 *
  *Baskets :       51 : Basket Size=      32000 bytes  Compression=   4.68     *
  *............................................................................*
  *Br   14 :r_out     : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1574922 bytes  File Size  =    1294076 *
  *Baskets :       51 : Basket Size=      32000 bytes  Compression=   1.22     *
  *............................................................................*
  *Br   15 :size_out  : vector<int>                                            *
  *Entries :    13051 : Total  Size=     879547 bytes  File Size  =     207403 *
  *Baskets :       30 : Basket Size=      32000 bytes  Compression=   4.24     *
  *............................................................................*
  *Br   16 :phi_out   : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1575032 bytes  File Size  =    1360782 *
  *Baskets :       51 : Basket Size=      32000 bytes  Compression=   1.16     *
  *............................................................................*
  *Br   17 :theta_out : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1575142 bytes  File Size  =    1414321 *
  *Baskets :       51 : Basket Size=      32000 bytes  Compression=   1.11     *
  *............................................................................*
  *Br   18 :adc_out   : vector<double>                                         *
  *Entries :    13051 : Total  Size=    1575032 bytes  File Size  =     334101 *
  *Baskets :       51 : Basket Size=      32000 bytes  Compression=   4.71     *
  *............................................................................*
  *Br   19 :is_associated_out : vector<bool>                                   *
  *Entries :    13051 : Total  Size=     357910 bytes  File Size  =     119582 *
  *Baskets :       13 : Basket Size=      32000 bytes  Compression=   2.99     *
  *............................................................................*
  *Br   20 :track_incoming_theta_out : vector<double>                          *
  *Entries :    13051 : Total  Size=    1575967 bytes  File Size  =     976776 *
  *Baskets :       51 : Basket Size=      32000 bytes  Compression=   1.61     *
  *............................................................................*
  *Br   21 :z_vertex  : z_vertex/D                                             *
  *Entries :    13051 : Total  Size=     105233 bytes  File Size  =      92752 *
  *Baskets :        4 : Basket Size=      32000 bytes  Compression=   1.13     *
  *............................................................................*

  */
