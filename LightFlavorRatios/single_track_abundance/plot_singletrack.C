#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "../util/HistogramContainer.h"

void plot_1dspectrum(std::string key,HistogramContainer& htrk, HistogramContainer& hpi, HistogramContainer& hK, HistogramContainer& hp)
{
  std::cout << key << std::endl;

  //TCanvas* c = new TCanvas(("c"+key+htrk.legend_name).c_str(),key.c_str(),600,600);
  htrk.spectra[key]->Draw("E");
  hpi.spectra[key]->Draw("E SAME");
  hK.spectra[key]->Draw("E SAME");
  hp.spectra[key]->Draw("E SAME");

  TLegend* l = new TLegend(0.6,0.8,0.9,0.9);
  l->AddEntry(htrk.spectra[key],htrk.legend_name.c_str(),"LP");
  l->AddEntry(hpi.spectra[key],hpi.legend_name.c_str(),"LP");
  l->AddEntry(hK.spectra[key],hK.legend_name.c_str(),"LP");
  l->AddEntry(hp.spectra[key],hp.legend_name.c_str(),"LP");
  l->Draw();

  //return c;
}

void plot_1dratio(std::string key, HistogramContainer& htrkplus, HistogramContainer& htrkminus,
                  HistogramContainer& hpiplus, HistogramContainer& hpiminus,
                  HistogramContainer& hKplus, HistogramContainer& hKminus,
                  HistogramContainer& hpplus, HistogramContainer& hpminus)
{
  htrkplus.spectra[key]->Sumw2();
  htrkminus.spectra[key]->Sumw2();
  hpiplus.spectra[key]->Sumw2();
  hpiminus.spectra[key]->Sumw2();
  hKplus.spectra[key]->Sumw2();
  hKminus.spectra[key]->Sumw2();
  hpplus.spectra[key]->Sumw2();
  hpminus.spectra[key]->Sumw2();

  TH1F* trk_ratio = (TH1F*)htrkminus.spectra[key]->Clone();
  TH1F* pi_ratio = (TH1F*)hpiminus.spectra[key]->Clone();
  TH1F* K_ratio = (TH1F*)hKminus.spectra[key]->Clone();
  TH1F* p_ratio = (TH1F*)hpminus.spectra[key]->Clone();

  trk_ratio->Divide(htrkplus.spectra[key]);
  pi_ratio->Divide(hpiplus.spectra[key]);
  K_ratio->Divide(hKplus.spectra[key]);
  p_ratio->Divide(hpplus.spectra[key]);

  trk_ratio->SetTitle(("Antiparticle/Particle Ratio vs. "+key).c_str());

  std::vector<double> maxima = {trk_ratio->GetMaximum(),pi_ratio->GetMaximum(),K_ratio->GetMaximum(),p_ratio->GetMaximum()};
  std::vector<double> minima = {trk_ratio->GetMinimum(),pi_ratio->GetMinimum(),K_ratio->GetMinimum(),p_ratio->GetMinimum()};

  double max_max = *(std::max_element(maxima.begin(),maxima.end()));
  if(max_max>3.) max_max = 3.;
  double min_min = *(std::min_element(minima.begin(),minima.end()));

  trk_ratio->GetYaxis()->SetRangeUser(0.9*min_min,1.1*max_max);

  trk_ratio->Draw("E");
  pi_ratio->Draw("E SAME");
  K_ratio->Draw("E SAME");
  p_ratio->Draw("E SAME");

  TLegend* l = new TLegend(0.6,0.8,0.9,0.9);
  l->AddEntry(trk_ratio,"h^{-}/h^{+}","LP");
  l->AddEntry(pi_ratio,"#pi^{-}/#pi^{+}","LP");
  l->AddEntry(K_ratio,"K^{-}/K^{+}","LP");
  l->AddEntry(p_ratio,"p^{-}/p^{+}","LP");
  l->Draw();
}

void plot_normalized_1dspectrum(std::string key, HistogramContainer& htrk, HistogramContainer& hpi, HistogramContainer& hK, HistogramContainer& hp)
{
  TH1F* trk_clone = (TH1F*)htrk.spectra[key]->Clone();
  TH1F* pi_clone = (TH1F*)hpi.spectra[key]->Clone();
  TH1F* K_clone = (TH1F*)hK.spectra[key]->Clone();
  TH1F* p_clone = (TH1F*)hp.spectra[key]->Clone();

  trk_clone->Scale(1./htrk.spectra[key]->Integral("width"));
  pi_clone->Scale(1./hpi.spectra[key]->Integral("width"));
  K_clone->Scale(1./hK.spectra[key]->Integral("width"));
  p_clone->Scale(1./hp.spectra[key]->Integral("width"));

  trk_clone->Draw("E");
  pi_clone->Draw("E SAME");
  K_clone->Draw("E SAME");
  p_clone->Draw("E SAME");

  TLegend* l = new TLegend(0.6,0.8,0.9,0.9);
  l->AddEntry(htrk.spectra[key],htrk.legend_name.c_str(),"LP");
  l->AddEntry(hpi.spectra[key],hpi.legend_name.c_str(),"LP");
  l->AddEntry(hK.spectra[key],hK.legend_name.c_str(),"LP");
  l->AddEntry(hp.spectra[key],hp.legend_name.c_str(),"LP");
  l->Draw();
}

TCanvas* plot_all_correlations(HistogramContainer& h)
{
  TCanvas* c = new TCanvas("corr","corr",1200,600);
  c->Divide(3,2);
  
  c->cd(1);
  h.h_pt_y->Draw("COLZ");
  gPad->SetLogz();
  c->cd(2);
  h.h_pt_phi->Draw("COLZ");
  c->cd(3);
  h.h_pt_ntracks->Draw("COLZ");
  gPad->SetLogz();
  c->cd(4);
  h.h_y_phi->Draw("COLZ");
  c->cd(5);
  h.h_y_ntracks->Draw("COLZ");
  c->cd(6);
  h.h_phi_ntracks->Draw("COLZ");

  return c;
}

void plot_singletrack()
{
  gStyle->SetOptStat(0);
  gStyle->SetImageScaling(4.);

  TFile* f = TFile::Open("output/data_singletrack.root");

  HistogramContainer track(f,"track","All tracks");
  HistogramContainer pi(f,"pi","#pi^{+}+#pi^{-}");
  HistogramContainer K(f,"K","K^{+}+K^{-}");
  HistogramContainer p(f,"p","p^{+}+p^{-}");

  HistogramContainer trackplus(f,"trackplus","Positively-charged tracks");
  HistogramContainer piplus(f,"piplus","#pi^{+}");
  HistogramContainer Kplus(f,"Kplus","K^{+}");
  HistogramContainer pplus(f,"pplus","p^{+}");

  HistogramContainer trackminus(f,"trackminus","Negatively-charged tracks");
  HistogramContainer piminus(f,"piminus","#pi^{-}");
  HistogramContainer Kminus(f,"Kminus","K^{-}");
  HistogramContainer pminus(f,"pminus","p^{-}");

  track.SetColor(kBlack);
  trackplus.SetColor(kBlack);
  trackminus.SetColor(kBlack);

  pi.SetColor(kBlue);
  piplus.SetColor(kBlue);
  piminus.SetColor(kBlue);

  K.SetColor(kGreen);
  Kplus.SetColor(kGreen);
  Kminus.SetColor(kGreen);

  p.SetColor(kRed);
  pplus.SetColor(kRed);
  pminus.SetColor(kRed);

  TCanvas* alltracks_1d = new TCanvas("allchg_1d","Positive + negative, 1D spectra",1600,400);
  alltracks_1d->Divide(4,1);

  alltracks_1d->cd(1);
  plot_1dspectrum("pt",track,pi,K,p);
  gPad->SetLogy();
  //cpt->SaveAs("plots/singletrack_pt.pdf");
  //cpt->Close();

  alltracks_1d->cd(2);
  plot_1dspectrum("y",track,pi,K,p);
  gPad->SetLogy();
  //ceta->SaveAs("plots/singletrack_eta.pdf");
  //ceta->Close();

  alltracks_1d->cd(3);
  plot_normalized_1dspectrum("phi",track,pi,K,p);
  track.h_phi->SetMinimum(0.);
  track.h_phi->SetMaximum(0.4);
  //cphi->SaveAs("plots/singletrack_phi.pdf");
  //cphi->Close();

  alltracks_1d->cd(4);
  plot_1dspectrum("ntracks",track,pi,K,p);
  gPad->SetLogy();
  //cntrk->SaveAs("plots/singletrack_ntracks.pdf");
  //cntrk->Close();

  alltracks_1d->SaveAs("plots/singletrack_allchg.png");
  alltracks_1d->Close();

  TCanvas* postracks_1d = new TCanvas("poschg_1d","Positive-charge 1D spectra",1600,400);
  postracks_1d->Divide(4,1);

  postracks_1d->cd(1);
  plot_1dspectrum("pt",trackplus,piplus,Kplus,pplus);
  gPad->SetLogy();
  //cptplus->SaveAs("plots/singletrack_pt_plus.pdf");
  //cptplus->Close();

  postracks_1d->cd(2);
  plot_1dspectrum("y",trackplus,piplus,Kplus,pplus);
  gPad->SetLogy();
  //cetaplus->SaveAs("plots/singletrack_eta_plus.pdf");
  //cetaplus->Close();

  postracks_1d->cd(3);
  plot_normalized_1dspectrum("phi",trackplus,piplus,Kplus,pplus);
  trackplus.h_phi->SetMinimum(0.);
  trackplus.h_phi->SetMaximum(0.4);
  //cphiplus->SaveAs("plots/singletrack_phi_plus.pdf");
  //cphiplus->Close();

  postracks_1d->cd(4);
  plot_1dspectrum("ntracks",trackplus,piplus,Kplus,pplus);
  gPad->SetLogy();
  //cntrkplus->SaveAs("plots/singletrack_ntrack_plus.pdf");
  //cntrkplus->Close();

  postracks_1d->SaveAs("plots/singletrack_plus.png");
  postracks_1d->Close();

  TCanvas* negtracks_1d = new TCanvas("negchg_1d","Negative-charge 1D spectra",1600,400);
  negtracks_1d->Divide(4,1);

  negtracks_1d->cd(1);
  plot_1dspectrum("pt",trackminus,piminus,Kminus,pminus);
  gPad->SetLogy();
  //cptminus->SaveAs("plots/singletrack_pt_minus.pdf");
  //cptminus->Close();

  negtracks_1d->cd(2);
  plot_1dspectrum("y",trackminus,piminus,Kminus,pminus);
  gPad->SetLogy();
  //cetaminus->SaveAs("plots/singletrack_eta_minus.pdf");
  //cetaminus->Close();

  negtracks_1d->cd(3);
  plot_normalized_1dspectrum("phi",trackminus,piminus,Kminus,pminus);
  trackminus.h_phi->SetMinimum(0.);
  trackminus.h_phi->SetMaximum(0.4);
  //cphiminus->SaveAs("plots/singletrack_phi_minus.pdf");
  //cphiminus->Close();

  negtracks_1d->cd(4);
  plot_1dspectrum("ntracks",trackminus,piminus,Kminus,pminus);
  gPad->SetLogy();
  //cntrkminus->SaveAs("plots/singletrack_ntrack_minus.pdf");
  //cntrkminus->Close();

  negtracks_1d->SaveAs("plots/singletrack_minus.png");
  negtracks_1d->Close();

  TCanvas* ratios = new TCanvas("ratios","ratios",1600,400);
  ratios->Divide(4,1);

  ratios->cd(1);
  plot_1dratio("pt",trackplus,trackminus,piplus,piminus,Kplus,Kminus,pplus,pminus);

  ratios->cd(2);
  plot_1dratio("y",trackplus,trackminus,piplus,piminus,Kplus,Kminus,pplus,pminus);

  ratios->cd(3);
  plot_1dratio("phi",trackplus,trackminus,piplus,piminus,Kplus,Kminus,pplus,pminus);

  ratios->cd(4);
  plot_1dratio("ntracks",trackplus,trackminus,piplus,piminus,Kplus,Kminus,pplus,pminus);

  ratios->SaveAs("plots/charge_ratios.png");
  ratios->Close();

  TCanvas* trkcorr = plot_all_correlations(track);
  trkcorr->SaveAs("plots/alltrack_corr.png");
  trkcorr->Close();

  TCanvas* picorr = plot_all_correlations(pi);
  picorr->SaveAs("plots/allpi_corr.png");
  picorr->Close();

  TCanvas* Kcorr = plot_all_correlations(K);
  Kcorr->SaveAs("plots/allK_corr.png");
  Kcorr->Close();

  TCanvas* pcorr = plot_all_correlations(p);
  pcorr->SaveAs("plots/allp_corr.png");
  pcorr->Close();

  TCanvas* trkpluscorr = plot_all_correlations(trackplus);
  trkpluscorr->SaveAs("plots/trackplus_corr.png");
  trkpluscorr->Close();

  TCanvas* pipluscorr = plot_all_correlations(piplus);
  pipluscorr->SaveAs("plots/piplus_corr.png");
  pipluscorr->Close();

  TCanvas* Kpluscorr = plot_all_correlations(Kplus);
  Kpluscorr->SaveAs("plots/Kplus_corr.png");
  Kpluscorr->Close();

  TCanvas* ppluscorr = plot_all_correlations(pplus);
  ppluscorr->SaveAs("plots/pplus_corr.png");
  ppluscorr->Close();

  TCanvas* trkminuscorr = plot_all_correlations(trackminus);
  trkminuscorr->SaveAs("plots/trackminus_corr.png");
  trkminuscorr->Close();

  TCanvas* piminuscorr = plot_all_correlations(piminus);
  piminuscorr->SaveAs("plots/piminus_corr.png");
  piminuscorr->Close();

  TCanvas* Kminuscorr = plot_all_correlations(Kminus);
  Kminuscorr->SaveAs("plots/Kminus_corr.png");
  Kminuscorr->Close();

  TCanvas* pminuscorr = plot_all_correlations(pminus);
  pminuscorr->SaveAs("plots/pminus_corr.png");
  pminuscorr->Close();
}
