#include "TFile.h"
#include "TTree.h"

#include "../util/HistogramContainer.h"
#include <dedxfitter/bethe_bloch.h>

void get_singletrack_abundance(std::string infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/data/nTP/nTuplizer_tracks_00053877_00001_00000.root", std::string dedxfile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/data/dedx_00053877_00001_00053877_00001_00000.root", std::string outfile="output/test_out.root")
{
  const float dedx_relative_diff_cutoff = 0.2;

  TFile* f = TFile::Open(infile.c_str());
  TTree* t = (TTree*)f->Get("ntp_track");

  HistogramContainer h_pi("pi","#pi^{+}+#pi^{-}");
  HistogramContainer h_piplus("piplus","#pi^{+}");
  HistogramContainer h_piminus("piminus","#pi^{-}");
  HistogramContainer h_K("K","K^{+} + K^{-}");
  HistogramContainer h_Kplus("Kplus","K^{+}");
  HistogramContainer h_Kminus("Kminus","K^{-}");
  HistogramContainer h_p("p","p^{+} + p^{-}");
  HistogramContainer h_pplus("pplus","p^{+}");
  HistogramContainer h_pminus("pminus","p^{-}");
  HistogramContainer h_track("track","all tracks");
  HistogramContainer h_trackplus("trackplus","positively-charged track");
  HistogramContainer h_trackminus("trackminus","negatively-charged track");

  float px;
  float py;
  float pz;
  float pt;
  float eta;
  float phi;
  float ntracks;
  float charge;
  float measured_dedx;
  float expected_pi_dedx;
  float expected_K_dedx;
  float expected_p_dedx;
  float nmaps;
  float nintt;
  float ntpc;
  float dcaxy;

  t->SetBranchAddress("px",&px);
  t->SetBranchAddress("py",&py);
  t->SetBranchAddress("pz",&pz);
  t->SetBranchAddress("pt",&pt);
  t->SetBranchAddress("eta",&eta);
  t->SetBranchAddress("phi",&phi);
  t->SetBranchAddress("ntrk",&ntracks);
  t->SetBranchAddress("charge",&charge);
  t->SetBranchAddress("dedx",&measured_dedx);
  t->SetBranchAddress("pidedx",&expected_pi_dedx);
  t->SetBranchAddress("kdedx",&expected_K_dedx);
  t->SetBranchAddress("prdedx",&expected_p_dedx);
  t->SetBranchAddress("nmaps",&nmaps);
  t->SetBranchAddress("nintt",&nintt);
  t->SetBranchAddress("ntpc",&ntpc);
  t->SetBranchAddress("dcaxy",&dcaxy);

  TFile* dedx_f = TFile::Open(dedxfile.c_str());
  TF1* piband = (TF1*)dedx_f->Get("pi_band");
  TF1* Kband = (TF1*)dedx_f->Get("K_band");
  TF1* pband = (TF1*)dedx_f->Get("p_band");

  piband->SetRange(0.,100.);
  Kband->SetRange(0.,100.);
  pband->SetRange(0.,100.);

  // find valid PID ranges
  TF1* pi_K_diff = new TF1("pi_K_diff",[&](double* x, double* par){return TMath::Abs((1.+dedx_relative_diff_cutoff)*piband->Eval(x[0]) - (1.-dedx_relative_diff_cutoff)*Kband->Eval(x[0]));},0.1,1.5,0);
  TF1* pi_p_diff = new TF1("pi_p_diff",[&](double* x, double* par){return TMath::Abs((1.+dedx_relative_diff_cutoff)*piband->Eval(x[0]) - (1.-dedx_relative_diff_cutoff)*pband->Eval(x[0]));},0.1,1.5,0);
  TF1* K_p_diff = new TF1("K_p_diff",[&](double* x, double* par){return TMath::Abs((1.+dedx_relative_diff_cutoff)*Kband->Eval(x[0]) - (1.-dedx_relative_diff_cutoff)*pband->Eval(x[0]));},0.1,1.5,0);

  float pi_K_cutoff = pi_K_diff->GetMinimumX();
  float pi_p_cutoff = pi_p_diff->GetMinimumX();
  float K_p_cutoff = K_p_diff->GetMinimumX();

  std::cout << "pi_K_cutoff: " << pi_K_diff->GetMinimumX() << std::endl;
  std::cout << "pi_p_cutoff: " << pi_p_diff->GetMinimumX() << std::endl;
  std::cout << "K_p_cutoff: " << K_p_diff->GetMinimumX() << std::endl; 

  for(size_t entry=0; entry<t->GetEntries(); ++entry)
  {
    if(entry % 10000 == 0) std::cout << entry << std::endl;
    t->GetEntry(entry);

    if(nmaps<3 || nintt==0 || ntpc<30) continue;

    float p = sqrt(px*px+py*py+pz*pz);

    expected_pi_dedx = piband->Eval(p);
    expected_K_dedx = Kband->Eval(p);
    expected_p_dedx = pband->Eval(p);

    h_track.Fill(pt,eta,phi,ntracks);
    if(charge>0.)
    {
      h_trackplus.Fill(pt,eta,phi,ntracks);
    }
    else if(charge<0.)
    {
      h_trackminus.Fill(pt,eta,phi,ntracks);
    }

    float pi_diff = fabs(measured_dedx-expected_pi_dedx)/expected_pi_dedx;
    float K_diff = fabs(measured_dedx-expected_K_dedx)/expected_K_dedx;
    float p_diff = fabs(measured_dedx-expected_p_dedx)/expected_p_dedx;

    if(pi_diff<dedx_relative_diff_cutoff && p<pi_K_cutoff && p<pi_K_cutoff)// && pi_diff<K_diff && pi_diff<p_diff)
    {
      h_pi.Fill(pt,eta,phi,ntracks);
      if(charge>0.)
      {
        h_piplus.Fill(pt,eta,phi,ntracks);
      }
      else if(charge<0.)
      {
        h_piminus.Fill(pt,eta,phi,ntracks);
      }
    }
    else if(K_diff<dedx_relative_diff_cutoff && p<pi_K_cutoff && p<K_p_cutoff)// && K_diff<pi_diff && K_diff<p_diff)
    {
      h_K.Fill(pt,eta,phi,ntracks);
      if(charge>0.)
      {
        h_Kplus.Fill(pt,eta,phi,ntracks);
      }
      else if(charge<0.)
      {
        h_Kminus.Fill(pt,eta,phi,ntracks);
      }
    }
    else if(p_diff<dedx_relative_diff_cutoff && p<pi_p_cutoff && p<K_p_cutoff)// && p_diff<pi_diff && p_diff<K_diff)
    {
      h_p.Fill(pt,eta,phi,ntracks);
      if(charge>0.)
      {
        h_pplus.Fill(pt,eta,phi,ntracks);
      }
      else if(charge<0.)
      {
        h_pminus.Fill(pt,eta,phi,ntracks);
      }
    }
  }

  TFile* outf = new TFile(outfile.c_str(),"RECREATE");
  h_pi.Write();
  h_piplus.Write();
  h_piminus.Write();
  h_K.Write();
  h_Kplus.Write();
  h_Kminus.Write();
  h_p.Write();
  h_pplus.Write();
  h_pminus.Write();
  h_track.Write();
  h_trackplus.Write();
  h_trackminus.Write();
  std::cout << "Finished" << std::endl;
}
