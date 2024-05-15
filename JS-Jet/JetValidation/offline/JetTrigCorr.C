#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include "utilities.h"

void JetTrigCorr()
{
  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();
  // TH2D histogram for correlation plot
  TH2D *jet_trig_correlation = new TH2D("jet_trig_correlation", "Jet Calo Correlation;Calo Energy;Jet Energy", 100, 0, 20, 100, 0, 40);
  TH1F* dR_hist = new TH1F("dR_hist", "Distribution of dR", 50, 0, 35.0);
  // TChain for the jet data
  TChain *ct = new TChain("T");
  ct->Add("../macro/outputJETVAL.root");

  vector<float> *m_event = nullptr;
  vector<float> *phi = nullptr;
  vector<float> *eta = nullptr;
  vector<float> *pt = nullptr;
  ct->SetBranchAddress("pt", &pt);
  ct->SetBranchAddress("eta", &eta);
  ct->SetBranchAddress("phi", &phi);
  ct->SetBranchAddress("m_event", &m_event);

  //Int_t m_event;

  // TChain for the calorimeter energy data
  TChain *cp = new TChain("ttree");
  cp->Add("/sphenix/user/jamesj3j3/macros/30GeVJetTest.root");
  vector<float> *hcalout_energy = nullptr;
  vector<float> *hcalout_etabin = nullptr;
  vector<float> *hcalout_phibin = nullptr;
  cp->SetBranchAddress("hcalout_energy", &hcalout_energy);
  cp->SetBranchAddress("hcalout_etabin", &hcalout_etabin);
  cp->SetBranchAddress("hcalout_phibin", &hcalout_phibin);
  // Initialize leading jet variables
  float leading_jet_energy = 0.0;
  float leading_jet_pt = 0.0;

  Long64_t nEntries_ct = ct->GetEntries();
  // Loop over entries in ct TChain
  for (Long64_t iEvent = 0; iEvent < nEntries_ct; ++iEvent) {
    ct->GetEntry(iEvent);

    // Look for the corresponding event in cp TChain based on event numbers from the vector
    bool foundMatch = false;
    for (size_t i = 0; i < m_event->size(); ++i) {
      Long64_t cp_event_entry = cp->GetEntryNumberWithIndex(static_cast<Long64_t>(m_event[i]));
      if (cp_event_entry >= 0) {
	cp->GetEntry(cp_event_entry);
	foundMatch = true;
	break;
      }
    }
    if (!foundMatch) {
      continue;  // Skip this event if not found in cp TChain
    }

    // Loop over jets in the correlation file
    for (size_t i = 0; i < eta->size(); ++i) {
      // Loop over jets in the energy file
      for (size_t j = 0; j < hcalout_energy->size(); ++j) {
	float dPhi = (*hcalout_phibin)[j] - (*phi)[i];
	float dEta = (*hcalout_etabin)[j] - (*eta)[i];

	// Adjust dPhi to ensure it's within (-pi, pi) range
	while (dPhi > M_PI) {
	  dPhi -= 2 * M_PI;
	}
	while (dPhi <= -M_PI) {
	  dPhi += 2 * M_PI;
	}

	float R = 0.4; // Jet radius
	float dR = sqrt(dPhi * dPhi + dEta * dEta); // Calculation of dR
	dR_hist->Fill(dR);

	// Check if the current jet is leading
	if ((*hcalout_energy)[j] > leading_jet_energy) {
	  leading_jet_energy = (*hcalout_energy)[j];
	  leading_jet_pt = (*pt)[i];
	}
      }
    }
  }

  Int_t ci;      // for color index setting                                                                                                                       
  TColor *color; // for color definition with alpha                                                                                                                   
  TCanvas * canvas = new TCanvas("canvas", " ", 550, 500);
  gStyle->SetErrorX(0);
  canvas->SetRightMargin(0.13);
  //  gPad->SetTickx();
  //  gPad->SetTicky();
  //  gPad->SetLogy();
  //  gPad->SetLogx();
  jet_trig_correlation->SetMarkerStyle(20);
  jet_trig_correlation->SetMarkerColor(2);
  jet_trig_correlation->SetMarkerSize(1.0);
  jet_trig_correlation->GetXaxis()->SetTitleSize(0.05);
  jet_trig_correlation->GetXaxis()->CenterTitle(true);
  jet_trig_correlation->GetXaxis()->SetLabelSize(0.03);
  jet_trig_correlation->GetYaxis()->SetLabelSize(0.03);
  jet_trig_correlation->GetZaxis()->SetLabelSize(0.03);
  jet_trig_correlation->GetXaxis()->SetTitleSize(0.05);
  jet_trig_correlation->SetTitle("Correlation Between z_{sj} & z_{g} ");
  jet_trig_correlation->Draw("colz"); // Use "colz" option to draw the color map                                                                                      
  //  drawText("p+p #sqrt{s} = 200 GeV", 0.2, 0.34, 14);
  //  drawText("anti-k_{T} R_{jet}=0.4, |#eta_{jet}| < 0.6", 0.2, 0.31, 14);
  //  drawText("10 < p_{T} < 20 GeV/c", 0.2, 0.25, 14);
  //  drawText("#it{#bf{sPHENIX}} Internal", 0.2, 0.215, 14);
  //  drawText("Pythia, p_{T}= 30 GeV", 0.2, 0.18, 14);

  canvas->SaveAs("jet_trig_correlation.pdf","RECREATE");       

  canvas->Modified();
  canvas->cd();
  canvas->SetSelected(canvas);
  canvas->Draw();

  TCanvas* c1 = new TCanvas("c1", "dR distribution", 800, 600);
  dR_hist->Draw();
  c1->SaveAs("dR_distribution.png"); // Save the plot as an image file
}

