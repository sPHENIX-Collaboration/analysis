#include "../CommonTools.h"

#include <sPhenixStyle.C>

TCanvas* Draw( TFile* qa_file_new, TFile* qa_file_ref, const TString& hist_name_prefix, const TString& tag );
TCanvas* Draw_eff( TFile* qa_file_new, TFile* qa_file_ref, const TString& hist_name_prefix, const TString& tag );

const char *hist_name_prefix = "QAG4SimulationTpc";
static constexpr int nregions_tpc = 3;
void TpcQA(std::string reffile, std::string newfile, std::string outfile)
{
  SetsPhenixStyle();
  TFile *reff = TFile::Open(reffile.c_str());
  TFile *newf = TFile::Open(newfile.c_str());

  auto c0 = Draw_eff( newf, reff, hist_name_prefix, "efficiency" );
  auto c1 = Draw( newf, reff, hist_name_prefix, "drphi" );
  auto c2 = Draw( newf, reff, hist_name_prefix, "rphi_error" );
  auto c3 = Draw( newf, reff, hist_name_prefix, "phi_pulls" );
  auto c4 = Draw( newf, reff, hist_name_prefix, "dz" );
  auto c5 = Draw( newf, reff, hist_name_prefix, "z_error" );
  auto c6 = Draw( newf, reff, hist_name_prefix, "z_pulls" );
  auto c7 = Draw( newf, reff, hist_name_prefix, "clus_size" );
  auto c8 = Draw( newf, reff, hist_name_prefix, "clus_size_phi" );
  auto c9 = Draw( newf, reff, hist_name_prefix, "clus_size_z" );

  TFile *outfilef = new TFile(outfile.c_str(), "recreate");
  c0->Write();
  c1->Write();
  c2->Write();
  c3->Write();
  c4->Write();
  c5->Write();
  c6->Write();
  c7->Write();
  c8->Write();
  c9->Write();
  outfilef->Close();

}

TCanvas* Draw( TFile* qa_file_new, TFile* qa_file_ref, const TString& hist_name_prefix, const TString& tag )
{

const TString prefix = TString("h_") + hist_name_prefix + TString("_");

auto cv = new TCanvas(
          TString("QA_Draw_Tpc_") + tag + TString("_") + hist_name_prefix,
          TString("QA_Draw_Tpc_") + tag + TString("_") + hist_name_prefix,
  1800, 1000);

cv->Divide( nregions_tpc, 1 );
for( int region = 0; region < nregions_tpc; ++region )
{

  // get histograms
  auto hnew = static_cast<TH1*>( qa_file_new->GetObjectChecked( Form( "%s%s_%i", prefix.Data(), tag.Data(), region ), "TH1" ) );
  hnew->Scale( 1./hnew->GetEntries() );
  hnew->SetMinimum(0);

  // reference
  auto href = qa_file_ref ? static_cast<TH1*>( qa_file_ref->GetObjectChecked( Form( "%s%s_%i", prefix.Data(), tag.Data(), region ), "TH1" ) ) : nullptr;
  if( href )
  {
    href->Scale( 1./href->GetEntries() );
    href->SetMinimum(0);
  }

  // draw
  cv->cd( region+1 );
  DrawReference(hnew, href);

  auto line = VerticalLine( gPad, 0 );
  line->Draw();
}

return cv;

}

TCanvas* Draw_eff( TFile* qa_file_new, TFile* qa_file_ref, const TString& hist_name_prefix, const TString& tag )
{

    const TString prefix = TString("h_") + hist_name_prefix + TString("_");

    auto cv = new TCanvas(
              TString("QA_Draw_Tpc_") + tag + TString("_") + hist_name_prefix,
              TString("QA_Draw_Tpc_") + tag + TString("_") + hist_name_prefix,
              1800, 1000);

    // get histograms
    auto hnew0 = static_cast<TH1*>( qa_file_new->GetObjectChecked( Form( "%s%s_0", prefix.Data(), tag.Data()), "TH1" ) );
    auto hnew1 = static_cast<TH1*>( qa_file_new->GetObjectChecked( Form( "%s%s_1", prefix.Data(), tag.Data()), "TH1" ) );

    hnew1->Divide(hnew1, hnew0, 1, 1, "B");
    hnew1->SetMinimum(0);

    // reference
    auto href0 = qa_file_ref ? static_cast<TH1*>( qa_file_ref->GetObjectChecked( Form( "%s%s_0", prefix.Data(), tag.Data()), "TH1" ) ) : nullptr;
    auto href1 = qa_file_ref ? static_cast<TH1*>( qa_file_ref->GetObjectChecked( Form( "%s%s_1", prefix.Data(), tag.Data()), "TH1" ) ) : nullptr;
    if( href0 )
      {
    href1->Divide(href1, href0, 1, 1, "B");
    href1->SetMinimum(0);
      }

    // draw
    DrawReference(hnew1, href1);

    auto line = HorizontalLine( gPad, 1 );
    line->Draw();


    return cv;

}
