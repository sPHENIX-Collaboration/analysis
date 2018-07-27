/* ROOT includes */
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TCut.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphErrors.h>
#include <TProfile.h>
#include <TCanvas.h>

using namespace std;

int
plot_jets()
{
  //  TFile *fin = new TFile("jets_20x250_Q2=10.0-100.0.root", "OPEN");
  TFile *fin = new TFile("jets_20x250_Q2=10.0-100.0.root", "OPEN");
  //  TFile *fin = new TFile("jets_20x250_Q2=100.0-1000.0.root", "OPEN");

  TTree* jets = (TTree*)fin->Get("jets");

  TCut cut_base("1");
  //  TCut cut_eemc("jet_truth_eta>-4&&jet_truth_eta<-1.55");
  //  TCut cut_cemc("jet_truth_eta>-1.55&&jet_truth_eta<1.24");
  //  TCut cut_femc("jet_truth_eta>1.24&&jet_truth_eta<4");

  //TCut cut_eta1("jet_truth_eta>-3.3&&jet_truth_eta<-2.3");
  TCut cut_eta1("jet_truth_eta>-4&&jet_truth_eta<-2");
  TCut cut_eta2("jet_truth_eta>-1&&jet_truth_eta<1");
  //TCut cut_eta3("jet_truth_eta>2.3&&jet_truth_eta<3.3");
  TCut cut_eta3("jet_truth_eta>2&&jet_truth_eta<4");

  TCanvas *ctemp = new TCanvas("**Scratch**");

  /* jet energies */
  ctemp->cd();

  TH2F* h_jets_energy_vs_eta = new TH2F("h_jets_energy_vs_eta",";#eta_{jet}^{truth};E_{jet}^{truth}",36,-4.5,4.5,25,0,100);
  jets->Draw("jet_truth_e : jet_truth_eta >> h_jets_energy_vs_eta","","colz");

  TCanvas *c4 = new TCanvas();
  h_jets_energy_vs_eta->Draw("colz");
  c4->SetLogz();

  /* jet energies in bins of pseudorapidity */
  ctemp->cd();

  TH1F* h_jets_energy_eta1 = new TH1F("h_jets_energy_eta1",";E_{jet}^{truth} (GeV);counts",100,0,50);
  TH1F* h_jets_energy_eta2 = (TH1F*)h_jets_energy_eta1->Clone("h_jets_energy_eta2");
  TH1F* h_jets_energy_eta3 = (TH1F*)h_jets_energy_eta1->Clone("h_jets_energy_eta3");

  h_jets_energy_eta1->SetLineColor(kBlue);
  h_jets_energy_eta2->SetLineColor(kRed);
  h_jets_energy_eta3->SetLineColor(kGreen+1);

  jets->Draw("jet_truth_e >> h_jets_energy_eta1",cut_eta1,"");
  jets->Draw("jet_truth_e >> h_jets_energy_eta2",cut_eta2,"");
  jets->Draw("jet_truth_e >> h_jets_energy_eta3",cut_eta3,"");

  TCanvas *c2 = new TCanvas();
  h_jets_energy_eta2->Draw("");
  h_jets_energy_eta1->Draw("same");
  h_jets_energy_eta3->Draw("same");


  /* jet energy resolution vs energy- all eta */
  ctemp->cd();

  TH2F* h_eres_vs_e = new TH2F("h_eres_vs_e",";E_{jet}^{truth};(E_{jet}^{smear}-E_{jet}^{truth}) / E_{jet}^{truth}",2*12,2.5,62.5,2*20,-2,4);
  jets->Draw("(jet_smear_e-jet_truth_e)/jet_truth_e:jet_truth_e >> h_eres_vs_e", cut_base);

  TCanvas *c5 = new TCanvas();
  h_eres_vs_e->Draw("colz");


  /* jet energy resolution vs eta */
  ctemp->cd();

  TH2F* h_eres_vs_eta = new TH2F("h_eres_vs_eta",";#eta_{jet}^{truth};(E_{jet}^{smear}-E_{jet}^{truth}) / E_{jet}^{truth}",2*18,-4.5,4.5,2*20,-2,4);
  jets->Draw("(jet_smear_e-jet_truth_e)/jet_truth_e:jet_truth_eta >> h_eres_vs_eta", cut_base);

  TProfile* hprof_eres_vs_eta = h_eres_vs_eta->ProfileX();
  hprof_eres_vs_eta->GetYaxis()->SetTitle( h_eres_vs_eta->GetYaxis()->GetTitle() );
  TCanvas *c6 = new TCanvas();
  h_eres_vs_eta->Draw("colz");

  TCanvas *c7 = new TCanvas();
  hprof_eres_vs_eta->Draw();


  /* jet energy resolution per etaregion */
  ctemp->cd();

  TH2F* h_eres_eta3 = new TH2F("h_eres_eta3",";E_{jet}^{truth};(E_{jet}^{smear}-E_{jet}^{truth}) / E_{jet}^{truth}",10,2.5,52.5,20,-1,1);
  Int_t nbins_x_eta3 = h_eres_eta3->GetXaxis()->GetNbins();

  TGraphErrors* g_emean_eta3 = new TGraphErrors( nbins_x_eta3 );
  TGraphErrors* g_esigma_eta3 = new TGraphErrors( nbins_x_eta3 );

  TH1F* h_frame_emean = new TH1F("h_frame_emean", "", 10,2.5,52.5);
  h_frame_emean->GetYaxis()->SetRangeUser(-0.15,0.15);
  h_frame_emean->GetXaxis()->SetTitle("E_{jet}^{truth} [GeV]");
  h_frame_emean->GetYaxis()->SetTitle("(E_{jet}^{smear}-E_{jet}^{truth}) / E_{jet}^{truth}");

  TH1F* h_frame_esigma = new TH1F("h_frame_esigma", "", 10,2.5,52.5);
  h_frame_esigma->GetYaxis()->SetRangeUser(0.1,0.25);
  h_frame_esigma->GetXaxis()->SetTitle("E_{jet}^{truth} [GeV]");
  h_frame_esigma->GetYaxis()->SetTitle("#sigma ( (E_{jet}^{smear}-E_{jet}^{truth}) / E_{jet}^{truth} )");

  TCanvas *c1 = new TCanvas("eta3");
  jets->Draw("(jet_smear_e-jet_truth_e)/jet_truth_e:jet_truth_e >> h_eres_eta3",cut_eta3);
  h_eres_eta3->Draw("COLZ");

  /* Loop over all x-bins in 2D histogram, create Y projections, and get standard deviation */
  for ( Int_t i = 0; i < nbins_x_eta3; i++ )
    {
      ctemp->cd();

      TH1D* h_proj = h_eres_eta3->ProjectionY("py",i+1,i+1);

      /* skip projections with too few entries */
      if ( h_proj->GetEntries() < 100 )
        continue;

      h_proj->Fit("gaus","Q");
      TF1* fit = h_proj->GetFunction("gaus");

      TCanvas *cstore = new TCanvas();

      TString proj_name = TString::Format("Projection_bin_center_%.01f", h_eres_eta3->GetXaxis()->GetBinCenter(i+1) );

      cstore->SetName(proj_name);
      cstore->SetTitle(proj_name);
      h_proj->DrawClone("clone");

      cout << "RMS: " << h_eres_eta3->GetXaxis()->GetBinCenter(i+1) << " --> " <<  h_proj->GetRMS() << endl;

      if (fit)
        {
          cout << "FIT: " << h_eres_eta3->GetXaxis()->GetBinCenter(i+1) << " --> " <<  fit->GetParameter(2) << endl;
          g_emean_eta3->SetPoint(i, h_eres_eta3->GetXaxis()->GetBinCenter(i+1), fit->GetParameter(1));
          g_emean_eta3->SetPointError(i, 0, fit->GetParError(1));

          g_esigma_eta3->SetPoint(i, h_eres_eta3->GetXaxis()->GetBinCenter(i+1), fit->GetParameter(2));
          g_esigma_eta3->SetPointError(i, 0, fit->GetParError(2));
        }

      //g_esigma_eta3->SetPoint(i, h_eres_eta3->GetXaxis()->GetBinCenter(i+1), h_proj->GetRMS());
    }

  /* Draw resolution graph */
  TCanvas *c3m = new TCanvas();
  h_frame_emean->Draw();
  g_emean_eta3->Draw("Psame");

  TCanvas *c3 = new TCanvas();
  h_frame_esigma->Draw();
  g_esigma_eta3->Draw("Psame");


  /* jet energy resolution as function of pseudorapidity per energy range */
  //...

  return 0;
}
