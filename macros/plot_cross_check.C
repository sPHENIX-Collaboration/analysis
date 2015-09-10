
#include "plot_utils.C"

void plot_cross_check(std::string filename = "in/histos_20150910_sum.root",
		      std::string refname = "in/histos_20150909_sum.root",
		      int option = 0,
		      std::string pdfname = "out/crosscheck_20150910.pdf") {

  TCanvas *canvas = new TCanvas("daily","daily",1600,900);
  canvas->Divide(4,2);
  
  // momentum resolution...
  TPad* pad1 = canvas->cd(1);
  apply_markup(pad1);

  TH1D* ref_mom_shift = get_momentum_shift(refname);
  apply_markup(ref_mom_shift,0,kGray+2);
  plot1_markup(ref_mom_shift);
  ref_mom_shift->Draw();
  
  TH1D* ref_mom_reso = get_momentum_reso(refname);
  apply_markup(ref_mom_reso,0,kBlack);
  plot1_markup(ref_mom_reso);
  ref_mom_reso->Draw("same");

  TH1D* daily_mom_shift = get_momentum_shift(filename);
  apply_markup(daily_mom_shift,1,kGray+2);
  plot1_markup(daily_mom_shift);
  daily_mom_shift->Draw("same");
  
  TH1D* daily_mom_reso = get_momentum_reso(filename);
  apply_markup(daily_mom_reso,1,kBlack);
  plot1_markup(daily_mom_reso);
  daily_mom_reso->Draw("same");
  
  if (option == 1) {
    daily_mom_shift->Divide(ref_mom_shift);
    plot1_markup(daily_mom_shift,1);
    daily_mom_shift->Draw();
    
    daily_mom_reso->Divide(ref_mom_reso);
    plot1_markup(daily_mom_reso,1);
    daily_mom_reso->Draw("same");
  }

  TLegend *leg1 = new TLegend(0.20,0.82,0.50,0.92);
  leg1->AddEntry(daily_mom_shift,"offset","p");
  leg1->AddEntry(daily_mom_reso,"resolution","p");
  leg1->SetFillColor(kWhite);
  leg1->SetBorderSize(0);
  leg1->SetTextSize(0.05);
  leg1->Draw();
  
  // dca resolution...
  TPad* pad2 = canvas->cd(2);
  apply_markup(pad2);

  TH1D* ref_dca_shift = get_dca_shift(refname);
  apply_markup(ref_dca_shift,0,kGray+2);
  plot2_markup(ref_dca_shift);
  ref_dca_shift->Draw();
    
  TH1D* ref_dca_reso = get_dca_reso(refname);
  apply_markup(ref_dca_reso,0,kBlack);
  plot2_markup(ref_dca_reso);
  ref_dca_reso->Draw("same");

  TH1D* daily_dca_shift = get_dca_shift(filename);
  apply_markup(daily_dca_shift,1,kGray+2);
  plot2_markup(daily_dca_shift);
  daily_dca_shift->Draw("same");
  
  TH1D* daily_dca_reso = get_dca_reso(filename);
  apply_markup(daily_dca_reso,1,kBlack);
  plot2_markup(daily_dca_shift);
  daily_dca_reso->Draw("same");

  if (option == 1) {
    daily_dca_shift->Divide(ref_dca_shift);
    plot2_markup(daily_dca_shift,1);
    daily_dca_shift->Draw();
    
    daily_dca_reso->Divide(ref_dca_reso);
    plot2_markup(daily_dca_reso,1);
    daily_dca_reso->Draw("same");
  }

  TLegend *leg2 = new TLegend(0.62,0.82,0.92,0.92);
  leg2->AddEntry(daily_dca_shift,"offset","p");
  leg2->AddEntry(daily_dca_reso,"resolution","p");
  leg2->SetFillColor(kWhite);
  leg2->SetBorderSize(0);
  leg2->SetTextSize(0.05);
  leg2->Draw();
  
  // pattern eff by nhits...
  TPad* pad3 = canvas->cd(3);
  apply_markup(pad3);
  
  TH1D* ref_pattern_eff_nhit2 = get_pattern_eff_nhit(refname,2);
  apply_markup(ref_pattern_eff_nhit2,0,kRed);
  plot3_markup(ref_pattern_eff_nhit2);
  ref_pattern_eff_nhit2->Draw("p");

  TH1D* ref_pattern_eff_nhit1 = get_pattern_eff_nhit(refname,1);
  apply_markup(ref_pattern_eff_nhit1,0,kGreen+2);
  plot3_markup(ref_pattern_eff_nhit1);
  ref_pattern_eff_nhit1->Draw("p same");
  
  TH1D* ref_pattern_eff_nhit0 = get_pattern_eff_nhit(refname,0);
  apply_markup(ref_pattern_eff_nhit0,0,kBlue);
  plot3_markup(ref_pattern_eff_nhit0);
  ref_pattern_eff_nhit0->Draw("p same");

  TH1D* daily_pattern_eff_nhit2 = get_pattern_eff_nhit(filename,2);
  apply_markup(daily_pattern_eff_nhit2,1,kRed);
  plot3_markup(daily_pattern_eff_nhit2);
  daily_pattern_eff_nhit2->Draw("p same");

  TH1D* daily_pattern_eff_nhit1 = get_pattern_eff_nhit(filename,1);
  apply_markup(daily_pattern_eff_nhit1,1,kGreen+1);
  plot3_markup(daily_pattern_eff_nhit1);
  daily_pattern_eff_nhit1->Draw("p same");
  
  TH1D* daily_pattern_eff_nhit0 = get_pattern_eff_nhit(filename,0);
  apply_markup(daily_pattern_eff_nhit0,1,kBlue);
  plot3_markup(daily_pattern_eff_nhit0);
  daily_pattern_eff_nhit0->Draw("p same");

  if (option == 1) {
    daily_pattern_eff_nhit2->Divide(ref_pattern_eff_nhit2);
    plot4_markup(daily_pattern_eff_nhit2,1);
    daily_pattern_eff_nhit2->Draw();
    
    daily_pattern_eff_nhit1->Divide(ref_pattern_eff_nhit1);
    plot4_markup(daily_pattern_eff_nhit1,1);
    daily_pattern_eff_nhit1->Draw("p same");

    daily_pattern_eff_nhit0->Divide(ref_pattern_eff_nhit0);
    plot4_markup(daily_pattern_eff_nhit0,1);
    daily_pattern_eff_nhit0->Draw("p same");
  }
  
  TLegend *leg3 = new TLegend(0.32,0.15,0.62,0.30);
  leg3->AddEntry(daily_pattern_eff_nhit0,"exact match","p");
  leg3->AddEntry(daily_pattern_eff_nhit1,"allow 1 noise hit","p");
  leg3->AddEntry(daily_pattern_eff_nhit2,"allow 2 noise hits","p");
  leg3->SetFillColor(kWhite);
  leg3->SetBorderSize(0);
  leg3->SetTextSize(0.05);
  leg3->Draw();
  
  // pattern efficiency by percent
  TPad* pad4 = canvas->cd(4);
  apply_markup(pad4);

  TH1D* ref_pattern_eff_percent2 = get_pattern_eff_percent(refname,2);
  apply_markup(ref_pattern_eff_percent2,0,kRed);
  plot4_markup(ref_pattern_eff_percent2); 
  ref_pattern_eff_percent2->Draw("p");

  TH1D* ref_pattern_eff_percent1 = get_pattern_eff_percent(refname,1);
  apply_markup(ref_pattern_eff_percent1,0,kGreen+2);
  plot4_markup(ref_pattern_eff_percent1); 
  ref_pattern_eff_percent1->Draw("p same");
  
  TH1D* ref_pattern_eff_percent0 = get_pattern_eff_percent(refname,0);
  apply_markup(ref_pattern_eff_percent0,0,kBlue);
  plot4_markup(ref_pattern_eff_percent0); 
  ref_pattern_eff_percent0->Draw("p same");

  TH1D* daily_pattern_eff_percent2 = get_pattern_eff_percent(filename,2);
  apply_markup(daily_pattern_eff_percent2,1,kRed);
  plot4_markup(daily_pattern_eff_percent2); 
  daily_pattern_eff_percent2->Draw("p same");

  TH1D* daily_pattern_eff_percent1 = get_pattern_eff_percent(filename,1);
  apply_markup(daily_pattern_eff_percent1,1,kGreen+2);
  plot4_markup(daily_pattern_eff_percent1); 
  daily_pattern_eff_percent1->Draw("p same");
  
  TH1D* daily_pattern_eff_percent0 = get_pattern_eff_percent(filename,0);
  apply_markup(daily_pattern_eff_percent0,1,kBlue);
  plot4_markup(daily_pattern_eff_percent0); 
  daily_pattern_eff_percent0->Draw("p same");

  if (option == 1) {
    daily_pattern_eff_percent2->Divide(ref_pattern_eff_percent2);
    plot4_markup(daily_pattern_eff_percent2,1);
    daily_pattern_eff_percent2->Draw();
    
    daily_pattern_eff_percent1->Divide(ref_pattern_eff_percent1);
    plot4_markup(daily_pattern_eff_percent1,1);
    daily_pattern_eff_percent1->Draw("p same");

    daily_pattern_eff_percent0->Divide(ref_pattern_eff_percent0);
    plot4_markup(daily_pattern_eff_percent0,1);
    daily_pattern_eff_percent0->Draw("p same");
  }
  
  TLegend *leg4 = new TLegend(0.32,0.15,0.62,0.30);
  leg4->AddEntry(daily_pattern_eff_nhit0,"p_{T} within 3%","p");
  leg4->AddEntry(daily_pattern_eff_nhit1,"p_{T} within 4%","p");
  leg4->AddEntry(daily_pattern_eff_nhit2,"p_{T} within 5%","p");
  leg4->SetFillColor(kWhite);
  leg4->SetBorderSize(0);
  leg4->SetTextSize(0.05);
  leg4->Draw();
    
  // quality...
  TPad* pad5 = canvas->cd(5);
  apply_markup(pad5);
    
  TH1D* ref_quality_mean = get_quality_mean(refname);
  apply_markup(ref_quality_mean,0,kGray+2);
  plot5_markup(ref_quality_mean);
  ref_quality_mean->Draw();

  TH1D* ref_quality_rms = get_quality_rms(refname);
  apply_markup(ref_quality_rms,0,kBlack);
  plot5_markup(ref_quality_rms);
  ref_quality_rms->Draw("same");
    
  TH1D* daily_quality_mean = get_quality_mean(filename);
  apply_markup(daily_quality_mean,1,kGray+2);
  plot5_markup(daily_quality_mean);
  daily_quality_mean->Draw("same");

  TH1D* daily_quality_rms = get_quality_rms(filename);
  apply_markup(daily_quality_rms,1,kBlack);
  plot5_markup(daily_quality_rms);
  daily_quality_rms->Draw("same");

  if (option == 1) {
    daily_quality_mean->Divide(ref_quality_mean);
    plot5_markup(daily_quality_mean,1);
    daily_quality_mean->Draw();
    
    daily_quality_rms->Divide(ref_quality_rms);
    plot5_markup(daily_quality_rms,1);
    daily_quality_rms->Draw("same");
  }

  TLegend *leg2 = new TLegend(0.70,0.82,0.92,0.92);
  leg2->AddEntry(daily_quality_mean,"mean","p");
  leg2->AddEntry(daily_quality_rms,"width","p");
  leg2->SetFillColor(kWhite);
  leg2->SetBorderSize(0);
  leg2->SetTextSize(0.05);
  leg2->Draw();
  
  // vertex...
  TPad* pad6 = canvas->cd(6);
  apply_markup(pad6);
  
  TH1D* ref_vtx_z = get_vertex(refname,2);
  apply_markup(ref_vtx_z,0,kRed);
  plot6_markup(ref_vtx_z);
  ref_vtx_z->Draw("p");
  
  TH1D* ref_vtx_y = get_vertex(refname,1);
  apply_markup(ref_vtx_y,0,kGreen+2);
  plot6_markup(ref_vtx_y);
  ref_vtx_y->Draw("p same");
  
  TH1D* ref_vtx_x = get_vertex(refname,0);
  apply_markup(ref_vtx_x,0,kBlue);
  plot6_markup(ref_vtx_x);
  ref_vtx_x->Draw("p same");

  TH1D* daily_vtx_z = get_vertex(filename,2);
  apply_markup(daily_vtx_z,1,kRed);
  plot6_markup(daily_vtx_z);
  daily_vtx_z->Draw("p same");
  
  TH1D* daily_vtx_y = get_vertex(filename,1);
  apply_markup(daily_vtx_y,1,kGreen+2);
  plot6_markup(daily_vtx_y);
  daily_vtx_y->Draw("p same");
  
  TH1D* daily_vtx_x = get_vertex(filename,0);
  apply_markup(daily_vtx_x,1,kBlue);
  plot6_markup(daily_vtx_x);
  daily_vtx_x->Draw("p same");

  if (option == 0) {
    pad6->SetLogy();
  }

  if (option == 1) {
    daily_vtx_z->Divide(ref_vtx_z);
    plot6_markup(daily_vtx_z,1);
    daily_vtx_z->Draw();
    
    daily_vtx_y->Divide(ref_vtx_y);
    plot6_markup(daily_vtx_y,1);
    daily_vtx_y->Draw("p same");

    daily_vtx_x->Divide(ref_vtx_x);
    plot6_markup(daily_vtx_x,1);
    daily_vtx_x->Draw("p same");
  }

  TLegend *leg6 = new TLegend(0.20,0.77,0.50,0.92);
  leg6->AddEntry(daily_vtx_x,"dx","p");
  leg6->AddEntry(daily_vtx_y,"dy","p");
  leg6->AddEntry(daily_vtx_z,"dz","p");
  leg6->SetFillColor(kWhite);
  leg6->SetBorderSize(0);
  leg6->SetTextSize(0.05);
  leg6->Draw();
  
  // purity nhits...
  TPad* pad7 = canvas->cd(7);
  apply_markup(pad7);
  
  TH1D* ref_purity_nhit2 = get_purity_nhit(refname,2);
  apply_markup(ref_purity_nhit2,0,kRed);
  plot7_markup(ref_purity_nhit2);
  ref_purity_nhit2->Draw("p");

  TH1D* ref_purity_nhit1 = get_purity_nhit(refname,1);
  apply_markup(ref_purity_nhit1,0,kGreen+2);
  plot7_markup(ref_purity_nhit1);
  ref_purity_nhit1->Draw("p same");
  
  TH1D* ref_purity_nhit0 = get_purity_nhit(refname,0);
  apply_markup(ref_purity_nhit0,0,kBlue);
  plot7_markup(ref_purity_nhit0);
  ref_purity_nhit0->Draw("p same");

  TH1D* daily_purity_nhit2 = get_purity_nhit(filename,2);
  apply_markup(daily_purity_nhit2,1,kRed);
  plot7_markup(daily_purity_nhit2);
  daily_purity_nhit2->Draw("p same");

  TH1D* daily_purity_nhit1 = get_purity_nhit(filename,1);
  apply_markup(daily_purity_nhit1,1,kGreen+2);
  plot7_markup(daily_purity_nhit1);
  daily_purity_nhit1->Draw("p same");
  
  TH1D* daily_purity_nhit0 = get_purity_nhit(filename,0);
  apply_markup(daily_purity_nhit0,1,kBlue);
  plot7_markup(daily_purity_nhit0);
  daily_purity_nhit0->Draw("p same");

  if (option == 1) {
    daily_purity_nhit2->Divide(ref_purity_nhit2);
    plot7_markup(daily_purity_nhit2,1);
    daily_purity_nhit2->Draw();
    
    daily_purity_nhit1->Divide(ref_purity_nhit1);
    plot7_markup(daily_purity_nhit1,1);
    daily_purity_nhit1->Draw("p same");

    daily_purity_nhit0->Divide(ref_purity_nhit0);
    plot7_markup(daily_purity_nhit0,1);
    daily_purity_nhit0->Draw("p same");
  }

  TLegend *leg7 = new TLegend(0.32,0.15,0.62,0.30);
  leg7->AddEntry(daily_purity_nhit0,"exact match","p");
  leg7->AddEntry(daily_purity_nhit1,"allow 1 noise hit","p");
  leg7->AddEntry(daily_purity_nhit2,"allow 2 noise hits","p");
  leg7->SetFillColor(kWhite);
  leg7->SetBorderSize(0);
  leg7->SetTextSize(0.05);
  leg7->Draw();
  
  // purity by percent
  TPad* pad8 = canvas->cd(8);
  apply_markup(pad8);

  TH1D* ref_purity_percent2 = get_purity_percent(refname,2);
  apply_markup(ref_purity_percent2,0,kRed);
  plot8_markup(ref_purity_percent2);
  ref_purity_percent2->Draw("p");

  TH1D* ref_purity_percent1 = get_purity_percent(refname,1);
  apply_markup(ref_purity_percent1,0,kGreen+2);
  plot8_markup(ref_purity_percent1);
  ref_purity_percent1->Draw("p same");
  
  TH1D* ref_purity_percent0 = get_purity_percent(refname,0);
  apply_markup(ref_purity_percent0,0,kBlue);
  plot8_markup(ref_purity_percent0);
  ref_purity_percent0->Draw("p same");

  TH1D* daily_purity_percent2 = get_purity_percent(filename,2);
  apply_markup(daily_purity_percent2,1,kRed);
  plot8_markup(daily_purity_percent2);
  daily_purity_percent2->Draw("p same");

  TH1D* daily_purity_percent1 = get_purity_percent(filename,1);
  apply_markup(daily_purity_percent1,1,kGreen+2);
  plot8_markup(daily_purity_percent1);
  daily_purity_percent1->Draw("p same");
  
  TH1D* daily_purity_percent0 = get_purity_percent(filename,0);
  apply_markup(daily_purity_percent0,1,kBlue);
  plot8_markup(daily_purity_percent0);
  daily_purity_percent0->Draw("p same");

  if (option == 1) {
    daily_purity_percent2->Divide(ref_purity_percent2);
    plot7_markup(daily_purity_percent2,1);
    daily_purity_percent2->Draw();
    
    daily_purity_percent1->Divide(ref_purity_percent1);
    plot7_markup(daily_purity_percent1,1);
    daily_purity_percent1->Draw("p same");

    daily_purity_percent0->Divide(ref_purity_percent0);
    plot7_markup(daily_purity_percent0,1);
    daily_purity_percent0->Draw("p same");
  }

  TLegend *leg8 = new TLegend(0.32,0.15,0.62,0.30);
  leg8->AddEntry(daily_purity_percent0,"p_{T} within 3%","p");
  leg8->AddEntry(daily_purity_percent1,"p_{T} within 4%","p");
  leg8->AddEntry(daily_purity_percent2,"p_{T} within 5%","p");
  leg8->SetFillColor(kWhite);
  leg8->SetBorderSize(0);
  leg8->SetTextSize(0.05);
  leg8->Draw();

  canvas->SaveAs(pdfname.c_str());
}
