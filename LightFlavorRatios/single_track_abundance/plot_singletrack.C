#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "../util/HistogramContainer.h"

TCanvas* plot_1dspectrum(std::string key,HistogramContainer& htrk, HistogramContainer& hpi, HistogramContainer& hK, HistogramContainer& hp)
{
  std::cout << key << std::endl;

  TCanvas* c = new TCanvas(("c"+key+htrk.legend_name).c_str(),key.c_str(),600,600);
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

  return c;
}

TCanvas* plot_normalized_1dspectrum(std::string key, HistogramContainer& htrk, HistogramContainer& hpi, HistogramContainer& hK, HistogramContainer& hp)
{
  htrk.spectra[key]->Scale(1./htrk.spectra[key]->Integral("width"));
  hpi.spectra[key]->Scale(1./hpi.spectra[key]->Integral("width"));
  hK.spectra[key]->Scale(1./hK.spectra[key]->Integral("width"));
  hp.spectra[key]->Scale(1./hp.spectra[key]->Integral("width"));

  return plot_1dspectrum(key,htrk,hpi,hK,hp);
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

  TCanvas* cpt = plot_1dspectrum("pt",track,pi,K,p);
  cpt->SetLogy();
  cpt->SaveAs("plots/singletrack_pt.pdf");
  cpt->Close();

  TCanvas* ceta = plot_1dspectrum("y",track,pi,K,p);
  ceta->SaveAs("plots/singletrack_eta.pdf");
  ceta->Close();

  TCanvas* cphi = plot_normalized_1dspectrum("phi",track,pi,K,p);
  track.h_phi->SetMinimum(0.);
  track.h_phi->SetMaximum(0.4);
  cphi->SaveAs("plots/singletrack_phi.pdf");
  cphi->Close();

  TCanvas* cntrk = plot_1dspectrum("ntracks",track,pi,K,p);
  cntrk->SetLogy();
  cntrk->SaveAs("plots/singletrack_ntracks.pdf");
  cntrk->Close();

  TCanvas* cptplus = plot_1dspectrum("pt",trackplus,piplus,Kplus,pplus);
  cptplus->SetLogy();
  cptplus->SaveAs("plots/singletrack_pt_plus.pdf");
  cptplus->Close();

  TCanvas* cetaplus = plot_1dspectrum("y",trackplus,piplus,Kplus,pplus);
  cetaplus->SaveAs("plots/singletrack_eta_plus.pdf");
  cetaplus->Close();

  TCanvas* cphiplus = plot_normalized_1dspectrum("phi",trackplus,piplus,Kplus,pplus);
  trackplus.h_phi->SetMinimum(0.);
  trackplus.h_phi->SetMaximum(0.4);
  cphiplus->SaveAs("plots/singletrack_phi_plus.pdf");
  cphiplus->Close();

  TCanvas* cntrkplus = plot_1dspectrum("ntracks",trackplus,piplus,Kplus,pplus);
  cntrkplus->SetLogy();
  cntrkplus->SaveAs("plots/singletrack_ntrack_plus.pdf");
  cntrkplus->Close();

  TCanvas* cptminus = plot_1dspectrum("pt",trackminus,piminus,Kminus,pminus);
  cptminus->SetLogy();
  cptminus->SaveAs("plots/singletrack_pt_minus.pdf");
  cptminus->Close();

  TCanvas* cetaminus = plot_1dspectrum("y",trackminus,piminus,Kminus,pminus);
  cetaminus->SaveAs("plots/singletrack_eta_minus.pdf");
  cetaminus->Close();

  TCanvas* cphiminus = plot_normalized_1dspectrum("phi",trackminus,piminus,Kminus,pminus);
  trackminus.h_phi->SetMinimum(0.);
  trackminus.h_phi->SetMaximum(0.4);
  cphiminus->SaveAs("plots/singletrack_phi_minus.pdf");
  cphiminus->Close();

  TCanvas* cntrkminus = plot_1dspectrum("ntracks",trackminus,piminus,Kminus,pminus);
  cntrkminus->SetLogy();
  cntrkminus->SaveAs("plots/singletrack_ntrack_minus.pdf");
  cntrkminus->Close();

  TCanvas* trkcorr = plot_all_correlations(track);
  trkcorr->SaveAs("plots/alltrack_corr.pdf");
  trkcorr->Close();

  TCanvas* picorr = plot_all_correlations(pi);
  picorr->SaveAs("plots/allpi_corr.pdf");
  picorr->Close();

  TCanvas* Kcorr = plot_all_correlations(K);
  Kcorr->SaveAs("plots/allK_corr.pdf");
  Kcorr->Close();

  TCanvas* pcorr = plot_all_correlations(p);
  pcorr->SaveAs("plots/allp_corr.pdf");
  pcorr->Close();

  TCanvas* trkpluscorr = plot_all_correlations(trackplus);
  trkpluscorr->SaveAs("plots/trackplus_corr.pdf");
  trkpluscorr->Close();

  TCanvas* pipluscorr = plot_all_correlations(piplus);
  pipluscorr->SaveAs("plots/piplus_corr.pdf");
  pipluscorr->Close();

  TCanvas* Kpluscorr = plot_all_correlations(Kplus);
  Kpluscorr->SaveAs("plots/Kplus_corr.pdf");
  Kpluscorr->Close();

  TCanvas* ppluscorr = plot_all_correlations(pplus);
  ppluscorr->SaveAs("plots/pplus_corr.pdf");
  ppluscorr->Close();

  TCanvas* trkminuscorr = plot_all_correlations(trackminus);
  trkminuscorr->SaveAs("plots/trackminus_corr.pdf");
  trkminuscorr->Close();

  TCanvas* piminuscorr = plot_all_correlations(piminus);
  piminuscorr->SaveAs("plots/piminus_corr.pdf");
  piminuscorr->Close();

  TCanvas* Kminuscorr = plot_all_correlations(Kminus);
  Kminuscorr->SaveAs("plots/Kminus_corr.pdf");
  Kminuscorr->Close();

  TCanvas* pminuscorr = plot_all_correlations(pminus);
  pminuscorr->SaveAs("plots/pminus_corr.pdf");
  pminuscorr->Close();
}
