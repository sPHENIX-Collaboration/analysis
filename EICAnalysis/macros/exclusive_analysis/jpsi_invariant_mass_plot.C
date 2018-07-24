#include "jpsi_sartre_analysis.C"

  
std::vector<TString> file_names; 
std::vector<TString> titles; // Legend titles
std::vector<TString> save_names;

// ------------------------------------------------------------------------- //

int jpsi_invariant_mass_plot()
{
  // Set sPHENIX style
  //gROOT->LoadMacro("/sphenix/user/gregtom3/SBU/research/macros/macros/sPHENIXStyle/sPhenixStyle.C");
  //SetsPhenixStyle();

  // ------------------- What to Plot -------------------- //

  // Plotting PHG4ParticleGeneratorVectorMeson in Fun4All //
  // Pathed from /sphenix/user/gregtom3/data/Summer2018 //

  //addPHG4ParticleGeneratorVectorMeson();


  // Plotting Sartre DVMP generator Sample (*Slideshow*) //
  // Pathed from /sphenix/user/gregtom3/data/Summer2018 //

  addSartre();


  // ------------------- Plotting -------------------------//
  // Test if vectors are same size //
  if(!((save_names.size()==titles.size())&&(save_names.size()==file_names.size())))
    {
      cout << "Vector arrays are not same size. \n Each file must have a titled legend and PNG save name" << endl;
      return -1;
    }

  // Begin sifting through analysis files
  for(unsigned file_idx=0 ; file_idx<file_names.size() ; file_idx++ )
    {
      // Analyze 'file_name' using jpsi_sartre_analysis.C
      file_name=file_names.at(file_idx);
      jpsi_sartre_analysis(); // creates vector 'invariant_mass'
      invariant_mass_track=get_invariant_mass_vector();
      // Histograms //
      TH1F *h1_track = new TH1F("h1track","h1track",120,0,15);
      TH1F *h1_truth = new TH1F("h1truth","h1truth",120,0,15);
      TH2F *h2_eta = new TH2F("h2eta","Eta Comparison",120,-4,4,120,-4,4);
      TH2F *h2_phi = new TH2F("h2phi","Phi Comparison",120,-4,4,120,-4,4);
      h1_track->SetLineColor(kRed);
      h1_truth->SetLineColor(kBlue);
      h1_track->GetXaxis()->SetTitle("Invariant Mass (GeV)");
      h1_track->GetYaxis()->SetTitle("Counts");

      h2_eta->GetXaxis()->SetTitle("Reco eta");
      h2_phi->GetXaxis()->SetTitle("Reco phi");
      h2_eta->GetYaxis()->SetTitle("True eta");
      h2_phi->GetYaxis()->SetTitle("True phi");

      // Fill Histograms //
      for(int i = 0; i<invariant_mass_track.size(); i++)
	{
	  h1_track->Fill(invariant_mass_track.at(i));
	  h1_truth->Fill(invariant_mass_truth.at(i));
	}
      for(int i = 0; i<holdeta.size(); i++)
	{
	  h2_eta->Fill(holdeta.at(i),true_holdeta.at(i));
	  h2_phi->Fill(holdphi.at(i),true_holdphi.at(i));
	}

      // Save histogram as a .png //
      histToPNG(h1_track,h1_truth,titles.at(file_idx),save_names.at(file_idx));
      histToPNG(h2_eta,"Eta vs. True Eta","reco2true_eta_20x250.png");
      histToPNG(h2_phi,"Phi vs. True Phi","reco2true_phi_20x250.png");
      // Cout out histogram characteristics //
      cout << "Track Histogram Characteristics" << endl;
      cout << "-------------------------------" << endl;
      cout << " Mean = " << h1_track->GetMean() << endl;
      cout << " STD  = " << h1_track->GetRMS() << endl;
      
      cout << "Truth Histogram Characteristics" << endl;
      cout << "-------------------------------" << endl;
      cout << " Mean = " << h1_truth->GetMean() << endl;
      cout << " STD  = " << h1_truth->GetRMS() << endl;
      
      // Cleaning //
      invariant_mass_track.clear(); invariant_mass_truth.clear();
    }
      return 0;
}

void histToPNG(TH1F* h_track, TH1F* h_truth, TString title, TString saveFileName)
{
  TCanvas *cPNG = new TCanvas("cPNG",title,600,400);
  TImage *img = TImage::Create();
  //cPNG->SetLogy();
  h_track->Draw();
  h_truth->Draw("SAME");
  auto legend = new TLegend(0.7,0.7,0.9,0.9,title);
  legend->AddEntry(h_track,"Track","l");
  legend->AddEntry(h_truth,"Truth","l");
  legend->SetTextSize(0.06);
  legend->Draw();
  gPad->RedrawAxis();
  
  img->FromPad(cPNG);
  img->WriteImage(saveFileName);
  
  delete img;
  delete cPNG;

}

void histToPNG(TH2F *h2, TString title, TString saveFileName)
{
  
  TCanvas *cPNG = new TCanvas("cPNG",title,600,400);
  TImage *img = TImage::Create();
  h2->Draw("colz");
  cPNG->SetLogz();
  gPad->RedrawAxis();
  gStyle->SetOptStat(0);
  
  img->FromPad(cPNG);
  img->WriteImage(saveFileName);
  
  delete img;
  delete cPNG;
}
void addPHG4ParticleGeneratorVectorMeson()
{
  // Add DVMP output generated from PHG4ParticleGeneratorVectorMeson to plotting
  file_names.push_back("JPsi_reco_studies/JP_C_1.root");
  file_names.push_back("JPsi_reco_studies/JP_C_2.root");
  file_names.push_back("JPsi_reco_studies/JP_C_3.root");
  file_names.push_back("JPsi_reco_studies/JP_C_5.root");
  file_names.push_back("JPsi_reco_studies/JP_C_8.root");
  file_names.push_back("JPsi_reco_studies/JP_C_10.root");
  file_names.push_back("JPsi_reco_studies/JP_C_15.root");
  file_names.push_back("JPsi_reco_studies/JP_C_20.root");
  file_names.push_back("JPsi_reco_studies/JP_F_1.root");
  file_names.push_back("JPsi_reco_studies/JP_F_2.root");
  file_names.push_back("JPsi_reco_studies/JP_F_3.root");
  file_names.push_back("JPsi_reco_studies/JP_F_5.root");
  file_names.push_back("JPsi_reco_studies/JP_F_8.root");
  file_names.push_back("JPsi_reco_studies/JP_F_10.root");
  file_names.push_back("JPsi_reco_studies/JP_F_15.root");
  file_names.push_back("JPsi_reco_studies/JP_F_20.root");

  
  
  titles.push_back("1GeV CEMC ");
  titles.push_back("2GeV CEMC ");
  titles.push_back("3GeV CEMC ");
  titles.push_back("5GeV CEMC ");
  titles.push_back("8GeV CEMC ");
  titles.push_back("10GeV CEMC ");
  titles.push_back("15GeV CEMC ");
  titles.push_back("20GeV CEMC ");
  titles.push_back("1GeV FEMC ");
  titles.push_back("2GeV FEMC ");
  titles.push_back("3GeV FEMC ");
  titles.push_back("5GeV FEMC ");
  titles.push_back("8GeV FEMC ");
  titles.push_back("10GeV FEMC ");
  titles.push_back("15GeV FEMC ");
  titles.push_back("20GeV FEMC ");


  save_names.push_back("recoMass_JPsi_1GeV_pt_CEMC.png");
  save_names.push_back("recoMass_JPsi_2GeV_pt_CEMC.png");
  save_names.push_back("recoMass_JPsi_3GeV_pt_CEMC.png");
  save_names.push_back("recoMass_JPsi_5GeV_pt_CEMC.png");
  save_names.push_back("recoMass_JPsi_8GeV_pt_CEMC.png");
  save_names.push_back("recoMass_JPsi_10GeV_pt_CEMC.png");
  save_names.push_back("recoMass_JPsi_15GeV_pt_CEMC.png");
  save_names.push_back("recoMass_JPsi_20GeV_pt_CEMC.png");
  save_names.push_back("recoMass_JPsi_1GeV_pt_FEMC.png");
  save_names.push_back("recoMass_JPsi_2GeV_pt_FEMC.png");
  save_names.push_back("recoMass_JPsi_3GeV_pt_FEMC.png");
  save_names.push_back("recoMass_JPsi_5GeV_pt_FEMC.png");
  save_names.push_back("recoMass_JPsi_8GeV_pt_FEMC.png");
  save_names.push_back("recoMass_JPsi_10GeV_pt_FEMC.png");
  save_names.push_back("recoMass_JPsi_15GeV_pt_FEMC.png");
  save_names.push_back("recoMass_JPsi_20GeV_pt_FEMC.png");
}

void addSampleSartre()
{
  // Plot Sartre Sample from slideshow 
  file_names.push_back("JPsi_sartre_studies/sample/20x250_100Events.root");
  titles.push_back("20x250");
  save_names.push_back("recoMass_JPsi_20x250.png");
}

void addSartre()
{
  file_names.push_back("/sphenix/user/gregtom3/data/Summer2018/track2cluster_studies/100k_DVMP_sartre.root");
  titles.push_back("20x250");
  save_names.push_back("recoMass_JPsi_20x250.png");
}
