#include <iostream>
#include <vector>
#include <TNamed.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TProfile.h>
#include <TString.h>
#include <TList.h>
#include <TDirectory.h>


void printEdges(std::vector<double> edges) {
    int nbins = edges.size() - 1;
    std::cout << "Bin edges: (";
    for (int i=0; i<nbins; i++) std::cout << edges.at(i) << ", ";
    std::cout << edges.at(nbins) << ")\n";
}

std::vector<double> getUniformBinEdges(int nbins, double lower, double upper) {
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

std::vector<double> getUniformBinEdges(int nbins, TH1* hist) {
    double lower = hist->GetXaxis()->GetXmin();
    double upper = hist->GetXaxis()->GetXmax();
    return getUniformBinEdges(nbins, lower, upper);
}

std::vector<double> getEquallyPopulatedBinEdges(int nbins, TH1* hist) {
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

std::vector<double> getBinCenters(std::vector<double> edges) {
    std::vector<double> centers;
    int nbins = edges.size() - 1;
    for (int i=0; i<nbins; i++) {
	double center = (edges.at(i) + edges.at(i+1))/2.0;
	centers.push_back(center);
    }
    return centers;
}

std::vector<double> doubleFitHist(TH1* hist) {
    std::vector<double> out;
    double mean;
    double stddev;
    hist->Fit("gaus", "S");
    TF1* fit = hist->GetFunction("gaus");
    if (!fit) {
	std::cout << "\n\nGreg info: bad fit #1 for hist ";
	hist->Print();
    }
    mean = fit->GetParameter(1);
    stddev = fit->GetParameter(2);
    hist->GetListOfFunctions()->Clear();
    hist->Fit("gaus", "S", "", (mean-stddev), (mean+stddev));
    fit = hist->GetFunction("gaus");
    if (!fit) {
	std::cout << "\n\nGreg info: bad fit #2 for hist ";
	hist->Print();
    }
    mean = fit->GetParameter(1);
    stddev = fit->GetParameter(2);
    out.push_back(mean);
    out.push_back(stddev);
    return out;
}

/* class BinnedHistSet: public TNamed { */
class BinnedHistSet {
    public:
	int nbins;
	int hist_Nbins;
	double hist_lower;
	double hist_upper;
	std::string name_base;
	std::string title_base;
	std::string binned_quantity;
	std::vector<double> bin_edges;
	TH1** hist_arr;

	BinnedHistSet();
	BinnedHistSet(std::string name, std::string title, int n_hist_bins, double lower, double upper, std::string binquantity, std::vector<double> edges);
	BinnedHistSet(TH1D* h_prototype, std::string binquantity, std::vector<double> edges);
	std::string GetName() {return name_base;};
	void SetName(std::string namestr) {name_base = namestr;};
	std::string GetTitle() {return title_base;};
	void SetTitle(std::string titlestr) {title_base = titlestr;};
	std::string GetBinQuantity() {return binned_quantity;};
	void SetBinQuantity(std::string binquantstr) {binned_quantity = binquantstr;};
	std::vector<double> GetBinEdges() {return bin_edges;};
	void SetBinEdges(std::vector<double> edges) {bin_edges = edges; nbins = edges.size() - 1;};
	int GetHistBins() {return hist_Nbins;};
	void SetHistBins(int N) {hist_Nbins = N;};
	std::string GetTitlePrefix();
	std::string GetAxisLabels();
	std::string GetBinnedQuantityName();
	std::string GetBinnedQuantityUnits();
	void MakeHists();
	void FillHists(double binValue, double fillValue);
	void WriteHists();
	void GetHistsFromFile(std::string name);
	TGraphErrors* GetBinnedMeans();
	~BinnedHistSet();
};

/* BinnedHistSet::BinnedHistSet(): TNamed() { */
BinnedHistSet::BinnedHistSet() {
    name_base = "EMPTY";
    title_base = "EMPTY";
    binned_quantity = "EMPTY";
    nbins = 0;
    hist_Nbins = 0;
    hist_lower = 0;
    hist_upper = 0;
    hist_arr = nullptr;
    bin_edges = std::vector<double>();
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
    hist_arr = nullptr;
}

/* BinnedHistSet::BinnedHistSet(TH1D* h_prototype, std::string binquantity, std::vector<double> edges): */ 
    /* TNamed(Form("bhs_%s", h_prototype->GetName()), h_prototype->GetTitle()) { */
BinnedHistSet::BinnedHistSet(TH1D* h_prototype, std::string binquantity, std::vector<double> edges) {
    name_base = h_prototype->GetName();
    title_base = h_prototype->GetTitle();
    hist_Nbins = h_prototype->GetNbinsX();
    hist_lower = h_prototype->GetXaxis()->GetXmin();
    hist_upper = h_prototype->GetXaxis()->GetXmax();;
    binned_quantity = binquantity;
    bin_edges = edges;
    nbins = edges.size() - 1;
    hist_arr = nullptr;
}

BinnedHistSet::~BinnedHistSet() {
    std::cout << "Entering destructor for BinnedHitSet " << name_base << "\n";
    delete[] hist_arr;
}

std::string BinnedHistSet::GetAxisLabels() {
    size_t pos = title_base.find(";");
    std::string axis_title = title_base.substr(pos);
    return axis_title;
}

std::string BinnedHistSet::GetTitlePrefix() {
    size_t pos = title_base.find(";");
    std::string prefix = title_base.substr(0, pos);
    return prefix;
}

std::string BinnedHistSet::GetBinnedQuantityName() {
    size_t pos = binned_quantity.find_first_of("([");
    std::string name = binned_quantity.substr(0, pos);
    return name;
}

std::string BinnedHistSet::GetBinnedQuantityUnits() {
    size_t start_pos = binned_quantity.find_first_of("([");
    size_t end_pos = binned_quantity.find_first_of(")]");
    int length = end_pos - start_pos - 1;
    std::string units = binned_quantity.substr(start_pos+1, length);
    return units;
}

void BinnedHistSet::MakeHists() {
    hist_arr = new TH1*[nbins];
    std::string title_prefix = this->GetTitlePrefix(); // Hold the first part of the title string
    std::string title_axislabels = this->GetAxisLabels(); // Hold the axis label part of the title string
    char* title_range; // Hold the range of bin_quantity for the current bin */
    std::string binquant = this->GetBinnedQuantityName(); // Hold the name for bin_quantity
    std::string units = this->GetBinnedQuantityUnits(); // Hold the units for bin_quantity

    for (int i=0; i<nbins; i++) {
	char* namestring = Form("%s_%d", name_base.c_str(), i);
	double lower = bin_edges.at(i);
	double upper = bin_edges.at(i+1);
	title_range = Form("%.1f %s #leq %s < %.1f %s", lower, units.c_str(), binquant.c_str(), upper, units.c_str());
	char* titlestring = Form("%s, %s%s", title_prefix.c_str(), title_range, title_axislabels.c_str());
	hist_arr[i] = new TH1D(namestring, titlestring, hist_Nbins, hist_lower, hist_upper);
    }
    /* std::cout << "Just finished making h_Eratios_binned\nh_Eratios_binned[0]->Print():\n"; */
    hist_arr[0]->Print();
}

void BinnedHistSet::FillHists(double binValue, double fillValue) {
    if (! hist_arr) { // don't try to fill anything if hist_arr isn't set
	std::cout << "Greg warning: trying to FillHists without setting hist_arr. Skipping this!\n";
	return;
    }
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
    /* std::cout << "Filling histogram " << i << "with value " << fillValue << "\nhist_arr[i]->Print():\n"; */
    /* hist_arr[i]->Print(); */
    hist_arr[i]->Fill(fillValue);
    /* hist_arr[i]->Print(); */
    return;
}

void BinnedHistSet::WriteHists() {
    for (int i=0; i<nbins; i++) {
	std::cout << "hist_arr[" << i << "] = " << hist_arr[i] << "\nPrinting: ";
	hist_arr[i]->Print();
	hist_arr[i]->Write();
    }
    std::cout << "Wrote hists\n";
}

void BinnedHistSet::GetHistsFromFile(std::string name) {
    hist_arr = new TH1*[nbins];
    for (int i=0; i<nbins; i++) {
	char* namestring = Form("%s_%d", name.c_str(), i);
	std::cout << "Looking for hist named " << namestring << "\n";
	hist_arr[i] = (TH1D*)gDirectory->Get(namestring);
	std::cout << "Address is " << (TH1D*)gDirectory->Get(namestring) << "\n";
    }
}

TGraphErrors* BinnedHistSet::GetBinnedMeans() {
    TGraphErrors* gr = new TGraphErrors(nbins);
    gr->SetName(Form("gr_%s", name_base.c_str()));
    gr->SetTitle(Form("%s vs %s;%s;%s", this->GetTitlePrefix().c_str(), binned_quantity.c_str(), binned_quantity.c_str(), this->GetTitlePrefix().c_str()));
    /* gr->SetTitle(title_base.c_str()); */
    std::vector<double> bc = getBinCenters(bin_edges);
    for (int i=0; i<nbins; i++) {
	std::vector<double> mean_std = doubleFitHist(hist_arr[i]);
	double x = bc.at(i);
	double y = mean_std.at(0);
	gr->SetPoint(i, x, y);
	double ex = 0;
	double ey = mean_std.at(1)/sqrt(hist_arr[i]->GetEntries());
	gr->SetPointError(i, ex, ey);
    }
    std::cout << "Greg info: done creating graph " << gr->GetName() << "\n";
    return gr;
}

