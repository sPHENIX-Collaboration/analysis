#include "../CommonTools.h"
#include <sPhenixStyle.C>

TCanvas* Draw( TFile* qa_file_new, TFile* qa_file_ref, const TString& hist_name_prefix, const TString& tag );

const char *hist_name_prefix = "QAG4SimulationMicromegas";

// assume Micromegas layers are 55 and 56
static constexpr int first_layer_micromegas = 55;
static constexpr int nlayers_micromegas = 2;

void TpotQA(std::string reffile, std::string newfile, std::string outfile)
{
  SetsPhenixStyle();

  TFile *reff = TFile::Open(reffile.c_str());
  TFile *newf = TFile::Open(newfile.c_str());

  auto c0 = Draw( newf, reff, hist_name_prefix, "adc" );
  auto c1 = Draw( newf, reff, hist_name_prefix, "residual" );
  auto c2 = Draw( newf, reff, hist_name_prefix, "residual_error" );
  auto c3 = Draw( newf, reff, hist_name_prefix, "cluster_pulls" );
  auto c4 = Draw( newf, reff, hist_name_prefix, "clus_size" );

  TFile *outfilef = new TFile(outfile.c_str(), "recreate");
  c1->Write();
  c2->Write();
  c3->Write();
  c4->Write();

  outfilef->Close();
}


TCanvas* Draw( TFile* qa_file_new, TFile* qa_file_ref, const TString& hist_name_prefix, const TString& tag )
{

    const TString prefix = TString("h_") + hist_name_prefix + TString("_");

    auto cv = new TCanvas(
      TString("QA_Draw_Micromegas_") + tag + TString("_") + hist_name_prefix,
      TString("QA_Draw_Micromegas_") + tag + TString("_") + hist_name_prefix,
      1800, 1000);

    cv->Divide( nlayers_micromegas, 1 );
    for( int ilayer = 0; ilayer < nlayers_micromegas; ++ilayer )
    {

      const int layer = ilayer + first_layer_micromegas;

      // get histograms
      auto hnew = static_cast<TH1*>( qa_file_new->GetObjectChecked( Form( "%s%s_%i", prefix.Data(), tag.Data(), layer ), "TH1" ) );
      hnew->Scale( 1./hnew->GetEntries() );
      hnew->SetMinimum(0);

      // reference
      auto href = qa_file_ref ? static_cast<TH1*>( qa_file_ref->GetObjectChecked( Form( "%s%s_%i", prefix.Data(), tag.Data(), layer ), "TH1" ) ) : nullptr;
      if( href )
      {
        href->Scale( 1./href->GetEntries() );
        href->SetMinimum(0);
      }

      // draw
      cv->cd( ilayer+1 );
      DrawReference(hnew, href);

      auto line = VerticalLine( gPad, 0 );
      line->Draw();
    }

    return cv;

}
