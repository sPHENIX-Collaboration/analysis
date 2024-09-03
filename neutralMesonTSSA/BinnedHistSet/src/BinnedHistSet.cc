#include <TNamed.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TProfile.h>
#include <TString.h>
#include <TList.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <iostream>
#include <vector>
#include "BinnedHistSet.h"

void BinnedHistSet::printEdges(std::vector<double> edges) {
    int nbins = edges.size() - 1;
    std::cout << "Bin edges: (";
    for (int i=0; i<nbins; i++) std::cout << edges.at(i) << ", ";
    std::cout << edges.at(nbins) << ")\n";
}

std::vector<double> BinnedHistSet::getUniformBinEdges(int nbins, double lower, double upper) {
    double range = upper - lower;
    double delta = range/nbins;
    std::vector<double> edges;
    for (int i=0; i<nbins; i++) {
	double ledge = lower + i*delta;
	edges.push_back(ledge);
    }
    edges.push_back(upper);
    return edges;
}

std::vector<double> BinnedHistSet::getUniformBinEdges(int nbins, TH1* hist) {
    double lower = hist->GetXaxis()->GetXmin();
    double upper = hist->GetXaxis()->GetXmax();
    return getUniformBinEdges(nbins, lower, upper);
}

std::vector<double> BinnedHistSet::getEquallyPopulatedBinEdges(int nbins, TH1* hist) {
    std::vector<double> edges;
    edges.push_back(hist->GetXaxis()->GetXmin());
    double* quants = new double[nbins]; // positions in [0,1] at which to compute the quantiles
    for (int i=0; i<nbins; i++) {
	quants[i] = (double)(i+1)/nbins;
    }
    double* x_q = new double[nbins]; // array to hold the computed quantiles
    hist->GetQuantiles(nbins, x_q, quants);
    for (int i=0; i<nbins; i++) {
	edges.push_back(x_q[i]);
    }
    return edges;
}

std::vector<double> BinnedHistSet::getBinCenters(std::vector<double> edges) {
    std::vector<double> centers;
    int nbins = edges.size() - 1;
    for (int i=0; i<nbins; i++) {
	double center = (edges.at(i) + edges.at(i+1))/2.0;
	centers.push_back(center);
    }
    return centers;
}

double fitfunc(double* x, double* par) {
    double arg = (x[0] - par[1])/par[2];
    return (par[0]*TMath::Exp(-0.5*arg*arg) + par[3]);
}

std::vector<double> BinnedHistSet::doubleFitHist(TH1* hist, bool response) {
    std::vector<double> out;
    double mean;
    double stddev;
    /* TF1* func = new TF1("fit", fitfunc, -2.0, 2.0, 4); */
    /* func->SetParameters(100, hist->GetMean(), hist->GetRMS(), 10); */
    /* func->SetParLimits(2, 0, 999); */
    /* hist->Fit(func, "SQ"); */
    /* TF1* fit1 = hist->GetFunction("fit"); */
    if (response) hist->Fit("gaus", "SQ", "", 0.8, 1.3);
    else hist->Fit("gaus", "SQ");
    TF1* fit1 = hist->GetFunction("gaus");
    if (!fit1) {
	std::cout << "Greg info: bad fit #1 for hist ";
	hist->Print();
	out.push_back(9999);
	out.push_back(0);
	out.push_back(1);
	return out;
    }
    mean = fit1->GetParameter(1);
    stddev = fit1->GetParameter(2);
    hist->GetListOfFunctions()->Clear();
    /* hist->Fit(fit1, "SQ", "", (mean-1.5*stddev), (mean+1.5*stddev)); */
    /* TF1* fit2 = hist->GetFunction("fit"); */
    hist->Fit("gaus", "SQ", "", (mean-1.0*stddev), (mean+1.0*stddev));
    TF1* fit2 = hist->GetFunction("gaus");
    if (!fit2) {
	std::cout << "Greg info: bad fit #2 for hist ";
	hist->Print();
	out.push_back(9999);
	out.push_back(0);
	out.push_back(1);
	return out;
    }
    mean = fit2->GetParameter(1);
    stddev = fit2->GetParameter(2);
    hist->GetListOfFunctions()->Clear();
    hist->Fit("gaus", "SQ", "", (mean-1.0*stddev), (mean+1.0*stddev));
    TF1* fit3 = hist->GetFunction("gaus");
    if (!fit3) {
	std::cout << "Greg info: bad fit #3 for hist ";
	hist->Print();
	out.push_back(9999);
	out.push_back(0);
	out.push_back(1);
	return out;
    }
    mean = fit3->GetParameter(1);
    stddev = fit3->GetParameter(2);
    out.push_back(mean);
    out.push_back(stddev);
    out.push_back(hist->GetEntries());
    // next bit is only relevant for calculating error on the energy resolution
    double resolution_error = (fit3->GetParError(1))*(fit3->GetParError(1));
    resolution_error += (fit3->GetParError(2))*(fit3->GetParError(2));
    resolution_error = sqrt(resolution_error);
    out.push_back(resolution_error);
    return out;
}

BinnedHistSet::BinnedHistSet() {
    name_base = "EMPTY";
    title_base = "EMPTY";
    binned_quantity = "EMPTY";
}

/* BinnedHistSet::BinnedHistSet(std::string name, std::string title, int n_hist_bins, double lower, double upper, std::string binquantity, std::vector<double> edges): */ 
/*     TNamed(name, title) { */
BinnedHistSet::BinnedHistSet(std::string name, std::string title, int n_hist_bins, double lower, double upper, std::string binquantity, std::vector<double> edges) {
    name_base = name;
    title_base = title;
    hist_Nbins = n_hist_bins;
    hist_lower = lower;
    hist_upper = upper;
    binned_quantity = binquantity;
    bin_edges = edges;
    nbins = edges.size() - 1;
    /* std::cout << "In initializer: name_base=" << name_base << ", nbins=" << nbins << "\n"; */
}

/* BinnedHistSet::BinnedHistSet(TH1* h_prototype, std::string binquantity, std::vector<double> edges): */ 
    /* TNamed(Form("bhs_%s", h_prototype->GetName()), h_prototype->GetTitle()) { */
BinnedHistSet::BinnedHistSet(TH1* h_prototype, std::string binquantity, std::vector<double> edges) {
    /* std::cout << "In initializer: h_prototype->GetName() = " << h_prototype->GetName() << "\n"; */
    name_base = std::string(h_prototype->GetName()) + "__" + binquantity;
    /* name_base.replace(name_base.find("h_"), std::string("h_").size(), "bhs_"); */
    /* std::cout << "In initializer: h_prototype->GetTitle() = " << h_prototype->GetTitle() << "\n"; */
    title_base = h_prototype->GetTitle();
    std::string xtitle = h_prototype->GetXaxis()->GetTitle();
    std::string ytitle = h_prototype->GetYaxis()->GetTitle();
    axislabels = ";" + xtitle + ";" + ytitle;
    hist_Nbins = h_prototype->GetNbinsX();
    hist_lower = h_prototype->GetXaxis()->GetXmin();
    hist_upper = h_prototype->GetXaxis()->GetXmax();;
    binned_quantity = binquantity;
    bin_edges = edges;
    nbins = edges.size() - 1;
}

BinnedHistSet::~BinnedHistSet() {
    std::cout << "Entering destructor for BinnedHitSet " << name_base << "\n";
}

void BinnedHistSet::GetAxisLabels() {
    // if we used a prototype histogram, axislabel should already be filled in
    if (axislabels != "") return;
    size_t pos = title_base.find(";");
    /* std::cout << "Getting axis labels\n"; */
    /* std::cout << "title_base = " << title_base << "\n"; */
    /* std::cout << "pos = " << pos << "\n"; */
    std::string axis_title = title_base.substr(pos);
    /* std::cout << "Got axis labels ( = " << axis_title << " )\n"; */
    axislabels = axis_title;
}

std::string BinnedHistSet::GetTitlePrefix() {
    size_t pos = title_base.find(";");
    /* std::cout << "Getting title prefix\n"; */
    std::string prefix = title_base.substr(0, pos);
    /* std::cout << "Got title prefix ( = " << prefix << " )\n"; */
    return prefix;
}

std::string BinnedHistSet::GetBinnedQuantityName() {
    size_t pos = binned_quantity.find_first_of("([");
    /* std::cout << "Getting binned quantity name\n"; */
    std::string name = binned_quantity.substr(0, pos);
    /* std::cout << "Got binned quantity name ( = " << name << " )\n"; */
    return name;
}

std::string BinnedHistSet::GetBinnedQuantityUnits() {
    size_t start_pos = binned_quantity.find_first_of("([");
    size_t end_pos = binned_quantity.find_first_of(")]");
    int length = end_pos - start_pos - 1;
    std::string units;
    /* std::cout << "Getting binned quantity units\n"; */
    if (start_pos == end_pos) units = "";
    else units = binned_quantity.substr(start_pos+1, length);
    /* std::cout << "Got binned quantity units ( = " << units << " )\n"; */
    return units;
}

void BinnedHistSet::MakeHists() {
    /* std::cout << "Entering MakeHists for " << name_base << "; nbins=" << nbins << "\n"; */
    std::string title_prefix = this->GetTitlePrefix(); // Hold the first part of the title string
    /* std::cout << "Got title_prefix = " << title_prefix << std::endl; */
    this->GetAxisLabels(); // Hold the axis label part of the title string
    /* std::cout << "Got axislabels = " << axislabels << std::endl; */
    char* title_range; // Hold the range of bin_quantity for the current bin */
    std::string binquant = this->GetBinnedQuantityName(); // Hold the name for bin_quantity
    /* std::cout << "Got binquant = " << binquant << std::endl; */
    std::string units = this->GetBinnedQuantityUnits(); // Hold the units for bin_quantity
    /* std::cout << "Got units = " << units << std::endl; */

    TH1* h;
    /* std::cout << "Making hists; hist0 has name " << Form("%s_0", name_base.c_str()) << " and title " << title_base.c_str() << "\n"; */
    h = new TH1D(Form("%s_0", name_base.c_str()), title_base.c_str(), hist_Nbins, hist_lower, hist_upper);
    hist_vec.push_back(h);
    for (int i=0; i<nbins; i++) {
	char* namestring = Form("%s_%d", name_base.c_str(), i+1);
	double lower = bin_edges.at(i);
	double upper = bin_edges.at(i+1);
	title_range = Form("%.2f%s #leq %s < %.2f%s", lower, units.c_str(), binquant.c_str(), upper, units.c_str());
	char* titlestring = Form("%s [%s]%s", title_prefix.c_str(), title_range, axislabels.c_str());
	/* std::cout << "Making hists; hist" << (i+1) << " has name " << namestring << " and title " << titlestring << "\n"; */
	h = new TH1D(namestring, titlestring, hist_Nbins, hist_lower, hist_upper);
	hist_vec.push_back(h);
    }
    /* hist_vec.at(0)->Print(); */
}

void BinnedHistSet::FillHists(double binValue, double fillValue) {
    if (hist_vec.size() < 1) { // don't try to fill anything if hist_arr isn't set
	std::cout << "Greg warning: trying to FillHists without setting hist_vec. Skipping this!\n";
	return;
    }
    // First fill the unbinned distribution
    hist_vec.at(0)->Fill(fillValue);
    /* std::cout << "Entering BinnedHistSet::fillHists\n"; */
    /* printEdges(edges); */
    /* int nbins = edges.size() - 1; */
    // Find which bin the event falls into
    int i; // index of the histogram we will fill
    for (i=0; i<nbins; i++) {
	double ledge = bin_edges.at(i);
	if ( (i==0) && (binValue<ledge) ) {
	    i = -1;  // binValue is below the lowest bin edge; stop with i=-1
	    break;
	}
	double redge = bin_edges.at(i+1);
	if ( (binValue>=ledge) && (binValue<redge) ) break;  // we have the correct value for i; stop here
	if ( (i==(nbins-1)) && (binValue>redge) ) {
	    i = nbins;  // binValue is above the highest bin edge; stop with i=nbins
	    break;
	}
    }
    /* std::cout << "Ended up with i=" << i << "\n"; */
    if ( (i==-1) || (i==nbins) ) return;  // binValue was outside the expected range; don't fill anything
    // Fill the corresponding histogram
    hist_vec.at(i+1)->Fill(fillValue);
    /* hist_vec.at(i+1)->Print(); */
    return;
}

void BinnedHistSet::WriteHists() {
    for (int i=0; i<nbins+1; i++) {
	/* std::cout << "hist_vec.at(" << i << ") = " << hist_vec.at(i) << "\nPrinting: "; */
	/* hist_vec.at(i)->Print(); */
	hist_vec.at(i)->Write();
    }
    std::cout << "Wrote hists for " << name_base << "\n";
}

void BinnedHistSet::GetHistsFromFile(std::string name) {
    /* std::cout << "In GetHistsFromFile for " << name_base << ": nbins=" << nbins << "\n"; */
    for (int i=0; i<nbins+1; i++) {
	char* namestring = Form("%s_%d", name.c_str(), i);
	/* std::cout << "In GetHistsFromFile for " << name_base << ": Looking for hist named " << namestring << "\n"; */
	TH1D* hist = (TH1D*)(gDirectory->Get(namestring));
	hist_vec.push_back(hist);
	/* std::cout << "Address is " << hist << "; title is " << hist->GetTitle() << "\n"; */
    }
}

TH1** BinnedHistSet::GetHistArr() {
    TH1** hist_arr = new TH1*[nbins+1];
    for (int i=0; i<nbins+1; i++) {
	hist_arr[i] = hist_vec.at(i);
    }
    return hist_arr;
}

void BinnedHistSet::PlotAllHistsWithFits(std::string outpdfname, bool response=false, std::string fitfunc="gaus") {
    /* std::cout << "In PlotAllHistsWithFits for " << name_base << "; nbins=" << nbins << "\n"; */
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1600,900);
    TH1* hist;
    for (int i=0; i<=nbins; i++) {
	hist = hist_vec.at(i);
	/* std::cout << "Plotting hist " << hist->GetName() << "\n"; */
	TLatex* fit_latex = new TLatex(0.75, 0.5, "blaahhh");
	fit_latex->SetTextSize(fit_latex->GetTextSize()/1.25);
	fit_latex->SetTextColor(kRed);
	fit_latex->SetTextAlign(21);
	double mean;
	double stddev;
	/* TF1* func = new TF1("fit", fitfunc, -2.0, 2.0, 4); */
	/* func->SetParameters(100, hist->GetMean(), hist->GetRMS(), 10); */
	/* func->SetParLimits(2, 0, 999); */
	c1->cd();
	hist->Draw("e1 x0");
	/* hist->Fit(func, "SQ"); */
	/* TF1* fit1 = hist->GetFunction("fit"); */
	if (fitfunc == "gaus") {
	    if (response) hist->Fit("gaus", "SQ", "", 0.8, 1.3);
	    else hist->Fit("gaus", "SQ");
	    TF1* fit1 = hist->GetFunction("gaus");
	    if (!fit1) {
		std::cout << "\n\nGreg info: bad fit #1 for hist ";
		hist->Print();
		fit_latex->DrawLatexNDC(0.50, 0.80, "First fit failed!");
		c1->Modified();
		c1->SaveAs(outpdfname.c_str());
		continue;
	    }
	    mean = fit1->GetParameter(1);
	    stddev = fit1->GetParameter(2);
	    hist->GetListOfFunctions()->Clear();
	    hist->Fit("gaus", "SQ", "", (mean-stddev), (mean+stddev));
	    /* hist->Fit(fit1, "SQ", "", (mean-stddev), (mean+stddev)); */
	    /* TF1* fit2 = hist->GetFunction("fit"); */
	    TF1* fit2 = hist->GetFunction("gaus");
	    if (!fit2) {
		std::cout << "\n\nGreg info: bad fit #2 for hist ";
		hist->Print();
		fit_latex->DrawLatexNDC(0.50, 0.80, "Second fit failed! Showing first fit.");
		fit1->Draw("same");
		c1->Modified();
		c1->SaveAs(outpdfname.c_str());
		continue;
	    }
	    mean = fit2->GetParameter(1);
	    stddev = fit2->GetParameter(2);
	    hist->GetListOfFunctions()->Clear();
	    hist->Fit("gaus", "SQ", "", (mean-stddev), (mean+stddev));
	    TF1* fit3 = hist->GetFunction("gaus");
	    if (!fit3) {
		std::cout << "\n\nGreg info: bad fit #3 for hist ";
		hist->Print();
		fit_latex->DrawLatexNDC(0.50, 0.80, "Third fit failed! Showing second fit.");
		fit2->Draw("same");
		c1->Modified();
		c1->SaveAs(outpdfname.c_str());
		continue;
	    }
	    c1->Modified();
	    c1->SaveAs(outpdfname.c_str());
	    delete fit_latex;
	}
	if (fitfunc == "mass") {
	    TF1* fit = new TF1("fit", "[0] + [1]*sqrt(x) + [2]*x + [3]*x^2 + [4]*x^3 + [5]*exp(-0.5*((x-[6])/[7])^2)", 0.0, 1.0);
	    fit->SetParLimits(5, hist->GetMaximum()/20.0, hist->GetMaximum()*1.2);
	    fit->SetParLimits(6, 0.05, 0.35);
	    fit->SetParLimits(7, 0.01, 0.10);
	    fit->SetParameter(5, hist->GetMaximum()/2.0);
	    fit->SetParameter(6, 0.145);
	    fit->SetParameter(7, 0.025);

	    hist->Fit(fit, "SQ", "E1", 0.05, 0.5);
	    mean = fit->GetParameter(6);
	    stddev = fit->GetParameter(7);
	    fit->SetLineColor(kRed);
	    TF1* signal = new TF1("signal", "[0]*exp(-0.5*((x-[1])/[2])^2)", 0.05, 0.5);
	    TF1* background = new TF1("background", "[0] + [1]*sqrt(x) + [2]*x + [3]*x^2 + [4]*x^3", 0.05, 0.5);
	    for (int i=0; i<5; i++) {
		background->SetParameter(i, fit->GetParameter(i));
		if (i<3) signal->SetParameter(i, fit->GetParameter(i+5));
	    }
	    signal->SetLineColor(kBlue);
	    background->SetLineColor(kGreen);
	    signal->Draw("same");
	    background->SetLineStyle(kDashed);
	    background->Draw("same");
	    /* fit->Draw("same"); */

	    TLatex* fit_latex = new TLatex(0.25, 0.8, "blaahhh");
	    /* fit_latex->SetTextSize(fit_latex->GetTextSize()/1.25); */
	    fit_latex->SetTextColor(kBlue);
	    fit_latex->SetTextAlign(21);
	    fit_latex->DrawLatexNDC(0.45, 0.70, Form("#splitline{#mu=%.5f#pm%.5f}{#sigma=%.5f#pm%.5f}", fit->GetParameter(6), fit->GetParError(6), fit->GetParameter(7), fit->GetParError(7)));
	    c1->Modified();
	    c1->SaveAs(outpdfname.c_str());
	    hist->GetListOfFunctions()->Clear();
	    delete fit_latex;
	    delete fit;
	}
    }
    delete c1;
}

TGraphErrors* BinnedHistSet::GetEnergyResolution() {
    TGraphErrors* gr_res = new TGraphErrors(nbins);
    gr_res->SetName(Form("gr_res_%s", name_base.c_str()));
    gr_res->SetTitle(Form("Energy Resolution vs %s;%s;Resolution", binned_quantity.c_str(), binned_quantity.c_str()));
    std::vector<double> bc = getBinCenters(bin_edges);
    for (int i=0; i<nbins; i++) {
	std::vector<double> mean_std_n = doubleFitHist(hist_vec.at(i+1), true);
	double x = bc.at(i);
	double y = mean_std_n.at(1)/mean_std_n.at(0);
	gr_res->SetPoint(i, x, y);
	double ex = 0;
	double ey = 0;
        if (mean_std_n.size() == 4) ey = mean_std_n.at(3);
	gr_res->SetPointError(i, ex, ey);
	/* std::cout << Form("%f\t%f\t%f\n", x, y, ey); */
    }
    /* std::cout << "Greg info: done creating graph " << gr_res->GetName() << "\n"; */
    return gr_res;
}

void BinnedHistSet::PlotEnergyResolution(std::string outpdfname, double low, double high) {
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1600,900);
    TGraphErrors* gr = this->GetEnergyResolution();
    gr->SetMarkerColor(1);
    gr->SetMarkerStyle(20);
    gr->Draw("AP");
    gr->GetYaxis()->SetRangeUser(low, high);
    gr->GetXaxis()->CenterTitle(true);
    gr->GetYaxis()->CenterTitle(true);
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete c1; delete gr;
}

TGraphErrors* BinnedHistSet::GetBinnedMeansWide() {
    TGraphErrors* gr_wide = new TGraphErrors(nbins);
    gr_wide->SetName(Form("gr_wide_%s", name_base.c_str()));
    /* std::string xlabel = binned_quantity + GetBinnedQuantityUnits(); */
    /* gr_wide->SetTitle(Form("%s vs %s;%s;%s", this->GetTitlePrefix().c_str(), binned_quantity.c_str(), xlabel.c_str(), this->GetTitlePrefix().c_str())); */
    gr_wide->SetTitle(Form("%s vs %s;%s;%s", this->GetTitlePrefix().c_str(), binned_quantity.c_str(), binned_quantity.c_str(), this->GetTitlePrefix().c_str()));
    std::vector<double> bc = getBinCenters(bin_edges);
    /* // to print bin centers and corresponding y, yerr */
    /* std::cout << "Greg info: in GetBinnedMeansWide for " << name_base << ", nbins=" << nbins << ", bin centers are\n"; */
    /* printEdges(bc); */
    /* std::cout << "x\ty\tey\n"; */
    for (int i=0; i<nbins; i++) {
	std::vector<double> mean_std_n = doubleFitHist(hist_vec.at(i+1));
	double x = bc.at(i);
	double y = mean_std_n.at(0);
	gr_wide->SetPoint(i, x, y);
	double ex = 0;
	double ey = mean_std_n.at(1);
	gr_wide->SetPointError(i, ex, ey);
	/* std::cout << Form("%f\t%f\t%f\n", x, y, ey); */
    }
    /* std::cout << "Greg info: done creating graph " << gr_wide->GetName() << "\n"; */
    return gr_wide;
}

TGraphErrors* BinnedHistSet::GetBinnedMeansNarrow() {
    TGraphErrors* gr_narrow = new TGraphErrors(nbins);
    gr_narrow->SetName(Form("gr_narrow_%s", name_base.c_str()));
    /* std::string xlabel = binned_quantity;// + GetBinnedQuantityUnits(); */
    /* gr_narrow->SetTitle(Form("%s vs %s;%s;%s", this->GetTitlePrefix().c_str(), binned_quantity.c_str(), xlabel.c_str(), this->GetTitlePrefix().c_str())); */
    gr_narrow->SetTitle(Form("%s vs %s;%s;%s", this->GetTitlePrefix().c_str(), binned_quantity.c_str(), binned_quantity.c_str(), this->GetTitlePrefix().c_str()));
    std::vector<double> bc = getBinCenters(bin_edges);
    for (int i=0; i<nbins; i++) {
	std::vector<double> mean_std_n = doubleFitHist(hist_vec.at(i+1));
	double x = bc.at(i);
	double y = mean_std_n.at(0);
	gr_narrow->SetPoint(i, x, y);
	double ex = 0;
	double ey;
        if (mean_std_n.at(2) == 0) ey = 0;
	else ey	= mean_std_n.at(1)/sqrt(mean_std_n.at(2));
	gr_narrow->SetPointError(i, ex, ey);
    }
    /* std::cout << "Greg info: done creating graph " << gr_narrow->GetName() << "\n"; */
    return gr_narrow;
}

void BinnedHistSet::PlotBinnedMeans(std::string outpdfname, double low=0.85, double high=1.2) {
    /* std::cout << "Greg info: entering PlotBinnedMeans for bhs " << name_base << "\n"; */
    /* std::cout << "nbins = " << nbins << ", hist_vec size = " << hist_vec.size() << "\n"; */
    TCanvas *c1 = new TCanvas("c1", "c1",0,50,1600,900);
    TGraphErrors* gr_wide = this->GetBinnedMeansWide();
    TGraphErrors* gr_narrow = this->GetBinnedMeansNarrow();
    gr_wide->SetMarkerColor(1); gr_narrow->SetMarkerColor(1);
    gr_wide->SetMarkerStyle(20); gr_narrow->SetMarkerStyle(20);
    gr_wide->SetLineWidth(2);
    gr_wide->GetXaxis()->SetTitleSize(0.05);  gr_wide->GetYaxis()->SetTitleSize(0.05);
    gr_wide->GetXaxis()->SetTitleOffset(0.8); gr_wide->GetYaxis()->SetTitleOffset(0.8);
    /* gr_narrow->SetLineColor(kRed); */
    gr_wide->Draw("AP[]");
    gr_narrow->Draw("P");
    gr_wide->GetYaxis()->SetRangeUser(low, high);
    gr_wide->GetXaxis()->CenterTitle(true);
    gr_wide->GetYaxis()->CenterTitle(true);
    c1->Modified();
    c1->SaveAs(outpdfname.c_str());
    delete c1; delete gr_wide; delete gr_narrow;
}
