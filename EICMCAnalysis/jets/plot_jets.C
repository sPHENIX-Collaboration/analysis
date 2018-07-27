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
#include <TString.h>

using namespace std;

int
plot_jets(TString jetfilename)
{
  TFile *fin = new TFile(jetfilename, "OPEN");

  TTree* jets = (TTree*)fin->Get("jets");

  /* basic cut that is applied to every plot */
  TCut cut_base("1");

  /* select different ranges in pseudorapidity */
  vector< TCut > v_cuts_eta;
  v_cuts_eta.push_back( TCut("(jet_truth_eta>-4&&jet_truth_eta<-2)") );
  v_cuts_eta.push_back( TCut("(jet_truth_eta>-1&&jet_truth_eta<1)") );
  v_cuts_eta.push_back( TCut("(jet_truth_eta>2&&jet_truth_eta<4)") );

  /* temporary cancas where objects that need to be drawn end up */
  TCanvas *ctemp = new TCanvas("**Scratch**");


  /* PLOT: True jet energies vs pseudorapidity */
  ctemp->cd();

  TH2F* h_jets_energy_vs_eta = new TH2F("h_jets_energy_vs_eta",";#eta_{jet}^{truth};E_{jet}^{truth}",36,-4.5,4.5,25,0,100);
  jets->Draw("jet_truth_e : jet_truth_eta >> h_jets_energy_vs_eta","","colz");

  TCanvas *c4 = new TCanvas();
  h_jets_energy_vs_eta->Draw("colz");
  c4->SetLogz();
  c4->Print("plots/plot_truth_jet_energy_vs_Eta.eps");

  /* PLOT: Histogram of jet energies in different pseudorapidity ranges */
  ctemp->cd();

  TH1F* h_jets_energy_eta1 = new TH1F("h_jets_energy_eta1",";E_{jet}^{truth} (GeV);counts",100,0,50);
  TH1F* h_jets_energy_eta2 = (TH1F*)h_jets_energy_eta1->Clone("h_jets_energy_eta2");
  TH1F* h_jets_energy_eta3 = (TH1F*)h_jets_energy_eta1->Clone("h_jets_energy_eta3");

  h_jets_energy_eta1->SetLineColor(kBlue);
  h_jets_energy_eta2->SetLineColor(kRed);
  h_jets_energy_eta3->SetLineColor(kGreen+1);

  jets->Draw("jet_truth_e >> h_jets_energy_eta1", v_cuts_eta.at(0), "");
  jets->Draw("jet_truth_e >> h_jets_energy_eta2", v_cuts_eta.at(1), "");
  jets->Draw("jet_truth_e >> h_jets_energy_eta3", v_cuts_eta.at(2), "");

  TCanvas *c2 = new TCanvas();
  h_jets_energy_eta2->Draw("");
  h_jets_energy_eta1->Draw("same");
  h_jets_energy_eta3->Draw("same");
  c2->Print("plots/plot_hist_etruth_etaranges.eps");

  /* PLOT: Jet energy resolution vs energy- all pseudorapidities */
  ctemp->cd();

  TH2F* h_eres_vs_e = new TH2F("h_eres_vs_e",";E_{jet}^{truth};(E_{jet}^{smear}-E_{jet}^{truth}) / E_{jet}^{truth}",2*12,2.5,62.5,2*20,-2,4);
  jets->Draw("(jet_smear_e-jet_truth_e)/jet_truth_e:jet_truth_e >> h_eres_vs_e", cut_base);

  TCanvas *c5 = new TCanvas();
  h_eres_vs_e->Draw("colz");
  c5->Print("plots/plot_eres_vs_e_all.eps");

  /* PLOT: Jet energy resolution vs eta- all energies */
  ctemp->cd();

  TH2F* h_eres_vs_eta = new TH2F("h_eres_vs_eta",";#eta_{jet}^{truth};(E_{jet}^{smear}-E_{jet}^{truth}) / E_{jet}^{truth}",2*18,-4.5,4.5,2*20,-2,4);
  jets->Draw("(jet_smear_e-jet_truth_e)/jet_truth_e:jet_truth_eta >> h_eres_vs_eta", cut_base);

  TCanvas *c6 = new TCanvas();
  h_eres_vs_eta->Draw("colz");
  c6->Print("plots/plot_eres_vs_eta_all.eps");

  //TProfile* hprof_eres_vs_eta = h_eres_vs_eta->ProfileX();
  //hprof_eres_vs_eta->GetYaxis()->SetTitle( h_eres_vs_eta->GetYaxis()->GetTitle() );
  //
  //TCanvas *c7 = new TCanvas();
  //hprof_eres_vs_eta->Draw();


  /* PLOT: Jet energy resolution vs energy in different regions of pseudorapidity */
  ctemp->cd();

  TH1F* h_frame_emean = new TH1F("h_frame_emean", "", 10,2.5,52.5);
  h_frame_emean->GetYaxis()->SetRangeUser(-0.4,0.4);
  h_frame_emean->GetXaxis()->SetTitle("E_{jet}^{truth} [GeV]");
  h_frame_emean->GetYaxis()->SetTitle("(E_{jet}^{smear}-E_{jet}^{truth}) / E_{jet}^{truth}");

  TH1F* h_frame_esigma = new TH1F("h_frame_esigma", "", 10,2.5,52.5);
  h_frame_esigma->GetYaxis()->SetRangeUser(0.1,0.4);
  h_frame_esigma->GetXaxis()->SetTitle("E_{jet}^{truth} [GeV]");
  h_frame_esigma->GetYaxis()->SetTitle("#sigma ( (E_{jet}^{smear}-E_{jet}^{truth}) / E_{jet}^{truth} )");

  for ( unsigned etaidx = 0; etaidx < v_cuts_eta.size(); etaidx++ )
    {
      TH2F* h_eres_etacut = new TH2F(TString::Format("h_eres_etacut%d",etaidx), ";E_{jet}^{truth};(E_{jet}^{smear}-E_{jet}^{truth}) / E_{jet}^{truth}",10,2.5,52.5,20,-1,1);
      Int_t nbins_x_etacut = h_eres_etacut->GetXaxis()->GetNbins();

      TGraphErrors* g_emean_etacut = new TGraphErrors( nbins_x_etacut );
      TGraphErrors* g_esigma_etacut = new TGraphErrors( nbins_x_etacut );

      TCanvas *c_evseta_1 = new TCanvas(TString::Format( "etarange_%d", etaidx ), (TString::Format( "Pseudorapidity range: %s", v_cuts_eta.at(etaidx).GetTitle() )));
      jets->Draw(TString::Format("(jet_smear_e-jet_truth_e)/jet_truth_e:jet_truth_e >> h_eres_etacut%d",etaidx), cut_base && v_cuts_eta.at(etaidx) );
      h_eres_etacut->Draw("COLZ");
      c_evseta_1->Print(TString::Format( "plots/plot_%s.eps", c_evseta_1->GetName() ));

      /* Loop over all x-bins in 2D histogram, create Y projections, and get standard deviation */
      for ( Int_t i = 0; i < nbins_x_etacut; i++ )
        {
          ctemp->cd();

          TH1D* h_proj = h_eres_etacut->ProjectionY("py",i+1,i+1);

          /* skip projections with too few entries */
          if ( h_proj->GetEntries() < 100 )
            continue;

	  /* Fit gaussian to projected histogram */
          h_proj->Fit("gaus","Q");
          TF1* fit = h_proj->GetFunction("gaus");

          TCanvas *cstore = new TCanvas();

          TString proj_name = TString::Format("Projection_etacut%d_bincenter_%.01fGeV", etaidx, h_eres_etacut->GetXaxis()->GetBinCenter(i+1) );

          cstore->SetName(proj_name);
          cstore->SetTitle(proj_name);
          h_proj->DrawClone("clone");
	  cstore->Print(TString::Format( "plots/plot_%s.eps", cstore->GetName() ));

          cout << "RMS: " << h_eres_etacut->GetXaxis()->GetBinCenter(i+1) << " --> " <<  h_proj->GetRMS() << endl;

          if (fit)
            {
              cout << "FIT: " << h_eres_etacut->GetXaxis()->GetBinCenter(i+1) << " --> " <<  fit->GetParameter(2) << endl;
              g_emean_etacut->SetPoint(i, h_eres_etacut->GetXaxis()->GetBinCenter(i+1), fit->GetParameter(1));
              g_emean_etacut->SetPointError(i, 0, fit->GetParError(1));

              g_esigma_etacut->SetPoint(i, h_eres_etacut->GetXaxis()->GetBinCenter(i+1), fit->GetParameter(2));
              g_esigma_etacut->SetPointError(i, 0, fit->GetParError(2));
            }

          //g_esigma_etacut->SetPoint(i, h_eres_etacut->GetXaxis()->GetBinCenter(i+1), h_proj->GetRMS());
        }

      /* Draw resolution graph */
      TCanvas *c_evseta_2 = new TCanvas(TString::Format( "mean_vs_e_etarange%d", etaidx ), (TString::Format( "Pseudorapidity range: %s", v_cuts_eta.at(etaidx).GetTitle() )));
      h_frame_emean->Draw();
      g_emean_etacut->Draw("Psame");
      c_evseta_2->Print(TString::Format( "plots/plot_%s.eps", c_evseta_2->GetName() ));

      TCanvas *c_evseta_3 = new TCanvas(TString::Format( "sigma_vs_e_etarange%d", etaidx), (TString::Format( "Pseudorapidity range: %s", v_cuts_eta.at(etaidx).GetTitle() )));
      h_frame_esigma->Draw();
      g_esigma_etacut->Draw("Psame");
      c_evseta_3->Print(TString::Format( "plots/plot_%s.eps", c_evseta_3->GetName() ));
    }

  /* PLOT: Jet energy resolution vs pseudorapidity per energy range */
  //...

  return 0;
}
