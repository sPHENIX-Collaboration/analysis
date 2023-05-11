//need to run make_ggg_contribution_graphs.C first

#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TF1.h"
#include "TLatex.h"
#include "TLegend.h"

const int NUM_PT_BINS = 4;
const float BIN_CENTERS[ NUM_PT_BINS ] = { 5.39, 6.69, 8.77, 11.9 };
const float ASYMMETRY[ NUM_PT_BINS ] = 
  { 0, 0, 0, 0 };
const float STAT_ERR[ NUM_PT_BINS ] = 
  { 0.0010, 0.0013, 0.0025, 0.0035 };

//https://arxiv.org/abs/2002.08384 
//https://arxiv.org/abs/1410.3448 
const int NUM_QGQ_PT_BINS  = 12;
const float PT[ NUM_QGQ_PT_BINS ]= 
  { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
const float QGQ[ NUM_QGQ_PT_BINS ] = 
  { -0.000500, -0.000434, -0.000339, -0.000213, -0.000054, 0.000137, 0.000357, 
     0.000603,  0.000874,  0.001166,  0.001477, 0.001804 };
const float QGQ_ERR[ NUM_QGQ_PT_BINS ] = 
  { 0.000077, 0.000070, 0.000072, 0.000079, 0.000091, 0.000113, 0.000146, 
    0.000191, 0.000250, 0.000321, 0.000405, 0.000501 };

void plot_sPHENIX_A_N_dp()
{
  TH1 *frame = new TH2F( "frame", " ", 1, 4.69, 12.6, 1, -0.018, 0.015 );
  frame->SetTitle( "; p_{T} [GeV/c]; A_{N}" );
  frame->SetStats( false ); 
  TAxis *xAxis = frame->GetXaxis();
  xAxis->SetTitleSize( 0.05 );
  xAxis->SetLabelSize( 0.055 );
  xAxis->SetTitleOffset( 1.2 );
  TAxis *yAxis = frame->GetYaxis();
  yAxis->SetTitleSize( 0.07 );
  yAxis->SetLabelSize( 0.055 );
  yAxis->CenterTitle( true );
  yAxis->SetTitleOffset( 0.8 );
  yAxis->SetNdivisions( 504 );//310 );

  TGraphErrors *projectedMeasurement = 
    new TGraphErrors( NUM_PT_BINS, BIN_CENTERS, ASYMMETRY, 0, STAT_ERR );
  projectedMeasurement->SetMarkerStyle( kFullCircle );

  TF1 *zeroLine = new TF1( "zeroLine", "0", 0, 20 );
  zeroLine->SetLineColor( kBlack );
  zeroLine->SetLineStyle( 2 );

  TFile *trigluonFile = TFile::Open( "gggContribution.root" );
  //https://arxiv.org/abs/1112.1161
  TGraphErrors* ggg1 = (TGraphErrors*)trigluonFile->Get( "model1" );
  ggg1->SetFillStyle( 3005 );
  ggg1->SetFillColor( kRed );
  ggg1->SetLineColor( kWhite );
  TGraphErrors* ggg2 = (TGraphErrors*)trigluonFile->Get( "model2" );
  ggg2->SetFillStyle( 3004 );
  ggg2->SetFillColor( kBlue );
  ggg2->SetLineColor( kWhite );

  TGraphErrors *qgq = new TGraphErrors( NUM_QGQ_PT_BINS, PT, QGQ, 
					     0, QGQ_ERR );
  qgq->SetFillColor( kGreen + 3 );
  qgq->SetFillStyle( 3003 );
  qgq->SetLineColor( kGreen + 1 );
  qgq->SetLineWidth( 2 );

  TCanvas *c1 = new TCanvas( "c11", "with QGQ", 200, 200, 800, 500 );  
  gPad->SetBottomMargin( 0.15 );
  gPad->SetLeftMargin( 0.15 );
  frame->Draw( "" );
  ggg1->Draw( "3" );
  ggg2->Draw( "3" );
  qgq->Draw( "C3" );
  projectedMeasurement->Draw( "P" );
  zeroLine->Draw( "same" );

  TLatex text1;
  text1.SetTextSize( 0.05 );
  //  text1.SetTextAlign( 13 );
  text1.DrawLatex( 5,  0.012, "p^{#uparrow} + p #rightarrow #gamma + X" );
  text1.DrawLatex( 5.25, 0.0095, "#sqrt{s} = 200 GeV" );
  text1.DrawLatex( 10, 0.012, "sPHENIX:" );
  text1.DrawLatex( 10, 0.0095, "L=100/pb P=0.57" );
 
  TLegend *leg = new TLegend( 0.18, 0.18, 0.5, 0.41 );
  leg->AddEntry( qgq,  "qgq Contribution (calc. by D.Pitonyak)" );
  leg->AddEntry( ggg1, "Trigluon Contribution Model 1 (calc. by S.Yoshida)" );
  leg->AddEntry( ggg2, "Trigluon Contribution Model 2 (calc. by S.Yoshida)" );
  leg->SetBorderSize( 0 );
  leg->SetTextSize( 0.04 );
  leg->Draw();

  c1->SaveAs( "AN_dp_sPHENIX.pdf" );
}
