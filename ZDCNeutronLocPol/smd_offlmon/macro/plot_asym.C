void plot_simple(TPad* pad, TH1* diff, TH1* sum, const char* which) {
    TF1* fit = new TF1("fitsimple", "[0] + [1]*sin(x - [2])", -3.141592, 3.141592);
    fit->SetParLimits(1, 0.001, 0.2);
    fit->SetParameter(1, 0.01);
    /* fit->SetParLimits(1, 3.041592, 3.241592); */
    fit->SetParLimits(2, 0.0, 2*3.141592);
    fit->SetParameter(2, 3.141592);
    fit->SetParName(0, "offset");
    fit->SetParName(1, "#epsilon");
    fit->SetParName(2, "#phi_{0}");

    TH1F* h_asym = (TH1F*)diff->Clone("h_asym");
    h_asym->SetTitle(Form("%s SMD Simple Asymmetry", which));
    h_asym->SetYTitle("Raw Asymmetry");
    h_asym->Divide(diff, sum, 1.0, 1.0, "B"); // "B" option = binomial errors

    pad->cd();
    gStyle->SetOptFit();
    gStyle->SetOptStat(0);
    h_asym->Draw();
    h_asym->Fit("fitsimple");
    float offset = fit->GetParameter(0);
    h_asym->GetYaxis()->SetRangeUser(offset-0.03, offset+0.03);
    TLine* l = new TLine(h_asym->GetXaxis()->GetXmin(), offset, h_asym->GetXaxis()->GetXmax(), offset);
    l->SetLineColor(kRed);
    l->SetLineStyle(kDashed);
    l->Draw();
    pad->Update();
}

void plot_sqrt(TPad* pad, TH1* upleft, TH1* upright, TH1* downleft, TH1* downright, const char* which) {
    TF1* fit = new TF1("fitsqrt", "[0]*sin(x - [1])", -3.141592, 0.0);
    fit->SetParLimits(0, 0.001, 0.2);
    fit->SetParameter(0, 0.01);
    /* fit->SetParLimits(1, 3.041592, 3.241592); */
    fit->SetParLimits(1, 0.0, 2*3.141592);
    fit->SetParameter(1, 3.141592);
    fit->SetParName(0, "#epsilon");
    fit->SetParName(1, "#phi_{0}");

    int nbins = upleft->GetNbinsX();
    TGraphErrors* sqrt_asym = new TGraphErrors(upleft);
    sqrt_asym->SetTitle(Form("%s SMD Square Root Asymmetry", which));
    sqrt_asym->GetXaxis()->SetTitle("#phi");
    sqrt_asym->GetYaxis()->SetTitle("Raw Asymmetry");
    for (int i=0; i<nbins; i++) {
	float n1, n2, n3, n4, e1, e2, e3, e4;
	n1 = upleft->GetBinContent(i+1);
	n2 = upright->GetBinContent(i+1);
	n3 = downleft->GetBinContent(i+1);
	n4 = downright->GetBinContent(i+1);
	e1 = upleft->GetBinError(i+1);
	e2 = upright->GetBinError(i+1);
	e3 = downleft->GetBinError(i+1);
	e4 = downright->GetBinError(i+1);

	float asym = (sqrt(n1*n4)-sqrt(n3*n2))/(sqrt(n1*n4)+sqrt(n3*n2));
	float t1 = sqrt(n2*n3*n4/n1)*e1;
	float t2 = sqrt(n1*n3*n4/n2)*e2;
	float t3 = sqrt(n1*n2*n4/n3)*e3;
	float t4 = sqrt(n1*n2*n3/n4)*e4;
	float asym_err = (1/pow(sqrt(n1*n4)+sqrt(n3*n2),2))*sqrt(pow(t1,2)+pow(t2,2)+pow(t3,2)+pow(t4,2));

	sqrt_asym->SetPointY(i, asym);
	sqrt_asym->SetPointError(i, sqrt_asym->GetErrorX(i), asym_err);
    }
    pad->cd();
    gStyle->SetOptFit();
    gStyle->SetOptStat(0);
    sqrt_asym->Draw("ap");
    sqrt_asym->Fit("fitsqrt");
    sqrt_asym->GetYaxis()->SetRangeUser(-0.03, 0.03);
    pad->Update();
}

void plot_xy(TCanvas* c, TH2* northxy, TH2* southxy, TH1** northx, TH1** southx) {
    c->Clear();
    gStyle->SetOptStat(1);
    c->Divide(2, 2, 0.025, 0.025);
    
    c->cd(1);
    /* gPad->SetLogz(); */
    northxy->Draw("colz");
    gPad->Update();
    TPaveStats* stats = (TPaveStats*)gPad->GetPrimitive("stats");
    stats->SetX1NDC(0.15);
    stats->SetX2NDC(0.295);
    stats->SetOptStat(1110);

    c->cd(2);
    /* gPad->SetLogz(); */
    southxy->Draw("colz");
    gPad->Update();
    stats = (TPaveStats*)gPad->GetPrimitive("stats");
    stats->SetX1NDC(0.15);
    stats->SetX2NDC(0.295);
    stats->SetOptStat(1110);

    c->cd(3);
    northx[0]->Sumw2();
    northx[0]->Scale(1.0/northx[0]->Integral());
    northx[1]->Sumw2();
    northx[1]->Scale(1.0/northx[1]->Integral());
    northx[0]->SetLineColor(kRed);
    northx[1]->SetLineColor(kBlue);
    northx[0]->SetTitle("North SMD Neutron x");
    northx[0]->GetYaxis()->SetTitle("Normalized Counts");
    northx[0]->SetStats(0);
    northx[0]->Draw("hist");
    northx[1]->Draw("same hist");
    TLatex* l = new TLatex;
    l->SetTextColor(kRed);
    l->DrawLatexNDC(0.7, 0.85, "Spin up");
    l->SetTextColor(kBlue);
    l->DrawLatexNDC(0.7, 0.80, "Spin down");

    c->cd(4);
    southx[0]->Sumw2();
    southx[0]->Scale(1.0/southx[0]->Integral());
    southx[1]->Sumw2();
    southx[1]->Scale(1.0/southx[1]->Integral());
    southx[0]->SetLineColor(kRed);
    southx[1]->SetLineColor(kBlue);
    southx[0]->SetTitle("South SMD Neutron x");
    southx[0]->GetYaxis()->SetTitle("Normalized Counts");
    southx[0]->SetStats(0);
    southx[0]->Draw("hist");
    southx[1]->Draw("same hist");
    l->SetTextColor(kRed);
    l->DrawLatexNDC(0.7, 0.85, "Spin up");
    l->SetTextColor(kBlue);
    l->DrawLatexNDC(0.7, 0.80, "Spin down");

    c->Update();
}

void plot_multiplicity(TCanvas* c, TH1** hists) {
    c->Clear();
    gStyle->SetOptStat(0);
    c->Divide(2, 2, 0.025, 0.025);

    c->cd(1);
    hists[0]->Draw();
    c->cd(2);
    hists[1]->Draw();
    c->cd(3);
    hists[2]->Draw();
    c->cd(4);
    hists[3]->Draw();
}

void plot_asym(const char* inname, std::string outname) {
    std::string outname_start = outname + "(";
    std::string outname_end = outname + ")";
    TFile* f = new TFile(inname, "READ");
    TCanvas* c1 = new TCanvas("c1", "c1", 800, 450);

    // hit multiplicities
    TH1F* h_hor_north_multiplicity = (TH1F*)f->Get("smd_hor_north_multiplicity");
    TH1F* h_ver_north_multiplicity = (TH1F*)f->Get("smd_ver_north_multiplicity");
    TH1F* h_hor_south_multiplicity = (TH1F*)f->Get("smd_hor_south_multiplicity");
    TH1F* h_ver_south_multiplicity = (TH1F*)f->Get("smd_ver_south_multiplicity");
    TH1* multiplicities[] = {h_hor_north_multiplicity, h_ver_north_multiplicity, h_hor_south_multiplicity, h_ver_south_multiplicity};
    plot_multiplicity(c1, multiplicities);
    c1->SaveAs(outname_start.c_str());

    // beam centroid xy positions
    TH2F* h_xy_north = (TH2F*)f->Get("smd_xy_neutron_north");
    TH1F* h_ver_north_up = (TH1F*)f->Get("smd_ver_north_up");
    TH1F* h_ver_north_down = (TH1F*)f->Get("smd_ver_north_down");
    TH2F* h_xy_south = (TH2F*)f->Get("smd_xy_neutron_south");
    TH1F* h_ver_south_up = (TH1F*)f->Get("smd_ver_south_up");
    TH1F* h_ver_south_down = (TH1F*)f->Get("smd_ver_south_down");
    TH1* northx[] = {h_ver_north_up, h_ver_north_down};
    TH1* southx[] = {h_ver_south_up, h_ver_south_down};
    plot_xy(c1, h_xy_north, h_xy_south, northx, southx);
    c1->SaveAs(outname.c_str());

    // simple asymmetry
    TH1F* h_phi_north_diff = (TH1F*)f->Get("smd_north_phi_diff");
    TH1F* h_phi_north_sum = (TH1F*)f->Get("smd_north_phi_sum");
    TH1F* h_phi_south_diff = (TH1F*)f->Get("smd_south_phi_diff");
    TH1F* h_phi_south_sum = (TH1F*)f->Get("smd_south_phi_sum");

    // Error testing
    /* TH1F* h_phi_north_up = (TH1F*)f->Get("smd_north_phi_up"); */
    /* TH1F* h_phi_north_down = (TH1F*)f->Get("smd_north_phi_down"); */
    /* std::cout << "North phi up bin 1 = " << h_phi_north_up->GetBinContent(1) << ", error = " << h_phi_north_up->GetBinError(1) << std::endl; */
    /* std::cout << "North phi down bin 1 = " << h_phi_north_down->GetBinContent(1) << ", error = " << h_phi_north_down->GetBinError(1) << std::endl; */
    /* std::cout << "North phi sum bin 1 = " << h_phi_north_sum->GetBinContent(1) << ", error = " << h_phi_north_sum->GetBinError(1) << std::endl; */
    /* std::cout << "North phi diff bin 1 = " << h_phi_north_diff->GetBinContent(1) << ", error = " << h_phi_north_diff->GetBinError(1) << std::endl; */
    
    // sqrt asymmetry
    TH1F* h_phi_north_L_up = (TH1F*)f->Get("smd_north_phi_L_up");
    TH1F* h_phi_north_L_down = (TH1F*)f->Get("smd_north_phi_L_down");
    TH1F* h_phi_north_R_up = (TH1F*)f->Get("smd_north_phi_R_up");
    TH1F* h_phi_north_R_down = (TH1F*)f->Get("smd_north_phi_R_down");
    TH1F* h_phi_south_L_up = (TH1F*)f->Get("smd_south_phi_L_up");
    TH1F* h_phi_south_L_down = (TH1F*)f->Get("smd_south_phi_L_down");
    TH1F* h_phi_south_R_up = (TH1F*)f->Get("smd_south_phi_R_up");
    TH1F* h_phi_south_R_down = (TH1F*)f->Get("smd_south_phi_R_down");

    c1->Clear();
    c1->Divide(2, 2, 0.025, 0.025);
    TPad* p1 = (TPad*)c1->GetPad(1);
    TPad* p2 = (TPad*)c1->GetPad(2);
    TPad* p3 = (TPad*)c1->GetPad(3);
    TPad* p4 = (TPad*)c1->GetPad(4);

    plot_simple(p1, h_phi_north_diff, h_phi_north_sum, "North");
    plot_simple(p3, h_phi_south_diff, h_phi_south_sum, "South");
    plot_sqrt(p2, h_phi_north_L_up, h_phi_north_R_up, h_phi_north_L_down, h_phi_north_R_down, "North");
    plot_sqrt(p4, h_phi_south_L_up, h_phi_south_R_up, h_phi_south_L_down, h_phi_south_R_down, "South");

    c1->Update();
    c1->SaveAs(outname.c_str());

    // Beam center correction
    // beam centroid xy positions
    h_xy_north = (TH2F*)f->Get("smd_xy_neutron_corrected_north");
    h_xy_south = (TH2F*)f->Get("smd_xy_neutron_corrected_south");
    h_ver_north_up = (TH1F*)f->Get("smd_ver_north_corrected_up");
    h_ver_north_down = (TH1F*)f->Get("smd_ver_north_corrected_down");
    h_ver_south_up = (TH1F*)f->Get("smd_ver_south_corrected_up");
    h_ver_south_down = (TH1F*)f->Get("smd_ver_south_corrected_down");
    northx[0] = h_ver_north_up;
    northx[1] = h_ver_north_down;
    southx[0] = h_ver_south_up;
    southx[1] = h_ver_south_down;
    plot_xy(c1, h_xy_north, h_xy_south, northx, southx);
    c1->SaveAs(outname.c_str());

    // simple asymmetry
    h_phi_north_diff = (TH1F*)f->Get("smd_north_phi_diff_corrected");
    h_phi_north_sum = (TH1F*)f->Get("smd_north_phi_sum_corrected");
    h_phi_south_diff = (TH1F*)f->Get("smd_south_phi_diff_corrected");
    h_phi_south_sum = (TH1F*)f->Get("smd_south_phi_sum_corrected");
    
    // sqrt asymmetry
    h_phi_north_L_up = (TH1F*)f->Get("smd_north_phi_L_up_corrected");
    h_phi_north_L_down = (TH1F*)f->Get("smd_north_phi_L_down_corrected");
    h_phi_north_R_up = (TH1F*)f->Get("smd_north_phi_R_up_corrected");
    h_phi_north_R_down = (TH1F*)f->Get("smd_north_phi_R_down_corrected");
    h_phi_south_L_up = (TH1F*)f->Get("smd_south_phi_L_up_corrected");
    h_phi_south_L_down = (TH1F*)f->Get("smd_south_phi_L_down_corrected");
    h_phi_south_R_up = (TH1F*)f->Get("smd_south_phi_R_up_corrected");
    h_phi_south_R_down = (TH1F*)f->Get("smd_south_phi_R_down_corrected");

    c1->Clear();
    c1->Divide(2, 2, 0.025, 0.025);
    p1 = (TPad*)c1->GetPad(1);
    p2 = (TPad*)c1->GetPad(2);
    p3 = (TPad*)c1->GetPad(3);
    p4 = (TPad*)c1->GetPad(4);

    plot_simple(p1, h_phi_north_diff, h_phi_north_sum, "Center-Corrected North");
    plot_simple(p3, h_phi_south_diff, h_phi_south_sum, "Center-Corrected South");
    plot_sqrt(p2, h_phi_north_L_up, h_phi_north_R_up, h_phi_north_L_down, h_phi_north_R_down, "Center-Corrected North");
    plot_sqrt(p4, h_phi_south_L_up, h_phi_south_R_up, h_phi_south_L_down, h_phi_south_R_down, "Center-Corrected South");

    c1->Update();
    c1->SaveAs(outname_end.c_str());

    return 0;
}
