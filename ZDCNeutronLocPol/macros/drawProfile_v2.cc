#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TPad.h>
#include "sPhenixStyle.C"

void drawProfile_v2()
{
  SetsPhenixStyle();
  TFile *file = TFile::Open("Profile.root");
  if (!file || file->IsZombie())
  {
    std::cerr << "Error: Cannot open file 'Profile.root'" << std::endl;
    return;
  }

  const char *hist_names[] = {
      "nxy_hits_all_south",
      "nxy_hits_all_south_cut",
      "nxy_hits_all_north",
      "nxy_hits_all_north_cut"};

  const char *hist_names_up[] = {
      "nxy_hits_all_south_up",
      "nxy_hits_all_south_cut_up",
      "nxy_hits_all_north_up",
      "nxy_hits_all_north_cut_up"};
  
  const char *hist_names_down[] = {
      "nxy_hits_all_south_down",
      "nxy_hits_all_south_cut_down",
      "nxy_hits_all_north_down",
      "nxy_hits_all_north_cut_down"};



  const char *cut_name =
      "2.0 cm < |r| < 4.0 cm";
  TLatex *latex = new TLatex();
  latex->SetNDC();
  latex->SetTextSize(0.045);
  latex->SetTextAlign(31);
  
  TCanvas *c2[2];
  TCanvas *c1_south = new TCanvas("SMD_2D_South","SMD 2D Profiles South",2000,800);
  c1_south->Divide(2);
  TCanvas *c1_north = new TCanvas("SMD_2D_North","SMD 2D Profiles North",2000,800);
  c1_north->Divide(2);
  c2[0] = new TCanvas("SMD_South_2", "SMD South", 1600, 1600);
  c2[1] = new TCanvas("SMD_North_2", "SMD North", 1600, 1600);
  c2[0]->Divide(2, 2);
  c2[1]->Divide(2, 2);
  for (int i = 0; i < 4; ++i)
  {
    TH2D *hist = (TH2D *) file->Get(hist_names[i]);
    if (i < 2)
      c1_south->cd(i + 1);
    else
      c1_north->cd(i - 1);
    hist->Draw("COLZ");
    hist->SetXTitle("x [cm]");
    hist->SetYTitle("y [cm]");
    if (i % 2 == 1) latex->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{p+p}, 200 GeV #it{#bf{sPHENIX}} internal"));
    if (i % 2 == 1)
    {
      latex->DrawLatex(0.95 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() - 0.08, Form("%s / Run 42796+42797", c2[i / 2]->GetTitle()));
      latex->DrawLatex(0.95 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() - 0.75, Form("%s", cut_name));
    }
    TH2D *hist_up = (TH2D *) file->Get(hist_names_up[i]);
    TH2D *hist_down = (TH2D *) file->Get(hist_names_down[i]);
    if (!hist)
    {
      std::cerr << "Error: Cannot find histogram '" << hist_names[i] << "'" << std::endl;
      continue;
    }
    if (!hist_up)
    {
      std::cerr << "Error: Cannot find histogram '" << hist_names_up[i] << "'" << std::endl;
      continue;
    }
    if (!hist_down)
    {
      std::cerr << "Error: Cannot find histogram '" << hist_names_down[i] << "'" << std::endl;
      continue;
    }
    TH1D *projX = hist->ProjectionX();
    TH1D *projX_up = hist_up->ProjectionX();
    TH1D *projX_down = hist_down->ProjectionX();
    if (i < 2)
    {
      c2[i / 2]->cd(i * 2 + 1);
    }
    else
      c2[i / 2]->cd((i - 2) * 2 + 1);
    projX->SetTitle(Form("%s ProjectionX", hist_names[i]));
    double maxX = projX->GetMaximum();
    projX->SetMaximum(1.3 * maxX);
    projX->Draw("P");
    projX_up->Draw("SAME P");
    projX_up->SetLineColor(kRed);
    projX_down->SetLineColor(kBlue);
    projX_up->SetMarkerColor(kRed);
    projX_down->SetMarkerColor(kBlue);
    projX_down->Draw("SAME P");
    TLegend *leg_blue = new TLegend(0.2, 0.75, 0.5, 0.85);
    leg_blue->AddEntry(projX_up, "blue spin #uparrow", "P");
    leg_blue->AddEntry(projX_down, "blue spin #downarrow", "P");
    leg_blue->AddEntry(projX, "sum", "P");
    leg_blue->SetTextSize(0.04);
    TLegend *leg_yellow = new TLegend(0.2, 0.75, 0.5, 0.85);
    leg_yellow->AddEntry(projX_up, "yellow spin #uparrow", "P");
    leg_yellow->AddEntry(projX_down, "yellow spin #downarrow", "P");
    leg_yellow->AddEntry(projX, "sum", "P");
    leg_yellow->SetTextSize(0.04);
    if (i<2)
    {
      leg_yellow->Draw();
    }
    else 
    {
      leg_blue->Draw();
    }
    latex->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{p+p}, 200 GeV #it{#bf{sPHENIX}} internal"));
    latex->DrawLatex(0.95 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() - 0.08, Form("%s / Run 42796+42797", c2[i / 2]->GetTitle()));
    if (i % 2 == 1)
      latex->DrawLatex(0.95 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() - 0.13, Form("%s", cut_name));
    projX_up->GetXaxis()->SetTitle("x [cm]");

    TH1D *projY = hist->ProjectionY();
    TH1D *projY_up = hist_up->ProjectionY();
    TH1D *projY_down = hist_down->ProjectionY();
    if (i < 2)
    {
      c2[i / 2]->cd(i * 2 + 2);
    }
    else
      c2[i / 2]->cd((i - 2) * 2 + 2);
    projY->SetTitle(Form("%s ProjectionY", hist_names[i]));
    double maxY = projY->GetMaximum();
    projY->SetMaximum(1.3 * maxY);
    projY->Draw("P");
    projY_up->SetLineColor(kRed);
    projY_down->SetLineColor(kBlue);
    projY_up->SetMarkerColor(kRed);
    projY_down->SetMarkerColor(kBlue);
    projY_up->Draw("SAME P");
    projY_down->Draw("SAME P");
    if (i<2)
    {
      leg_yellow->Draw();
    }
    else 
    {
      leg_blue->Draw();
    }
    latex->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{p+p}, 200 GeV #it{#bf{sPHENIX}} internal"));
    latex->DrawLatex(0.95 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() - 0.08, Form("%s / Run 42796+42797", c2[i / 2]->GetTitle()));
    if (i % 2 == 1)
      latex->DrawLatex(0.95 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() - 0.13, Form("%s", cut_name));
    projY_up->GetXaxis()->SetTitle("x [cm]");
  }
 
  c1_south->SaveAs("h2_profile_xy_south.pdf");
  c1_north->SaveAs("h2_profile_xy_north.pdf");
  c2[0]->SaveAs("h1_xyprofiles_south_all.pdf");
  c2[1]->SaveAs("h1_xyprofiles_north_all.pdf");
  file->Close();
}
