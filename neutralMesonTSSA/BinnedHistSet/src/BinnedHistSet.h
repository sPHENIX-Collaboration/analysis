#pragma once
#include <Rtypes.h>
#include <TH2.h>
#include <TGraphErrors.h>
#include <string>
#include <vector>

class BinnedHistSet {
    public:
	int nbins = 0;
	int hist_Nbins = 0;
	double hist_lower = 0.0;
	double hist_upper = 0.0;
	std::string name_base = "";
	std::string title_base = "";
	std::string binned_quantity = "";
	std::string axislabels = "";
	std::vector<double> bin_edges = std::vector<double>();
	std::vector<TH1*> hist_vec = std::vector<TH1*>();

	BinnedHistSet();
	BinnedHistSet(std::string name, std::string title, int n_hist_bins, double lower, double upper, std::string binquantity, std::vector<double> edges);
	BinnedHistSet(TH1* h_prototype, std::string binquantity, std::vector<double> edges);
	~BinnedHistSet();

	void printEdges(std::vector<double> edges);
	std::vector<double> getUniformBinEdges(int nbins, double lower, double upper);
	std::vector<double> getUniformBinEdges(int nbins, TH1* hist);
	std::vector<double> getEquallyPopulatedBinEdges(int nbins, TH1* hist);
	std::vector<double> getBinCenters(std::vector<double> edges);
	std::vector<double> doubleFitHist(TH1* hist, bool response=false);

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
	void GetAxisLabels();
	std::string GetBinnedQuantityName();
	std::string GetBinnedQuantityUnits();

	void MakeHists();
	void FillHists(double binValue, double fillValue);
	void WriteHists();
	void GetHistsFromFile(std::string name);
	TH1** GetHistArr();

	void PlotAllHistsWithFits(std::string outpdfname, bool response, std::string fitfunc);
	TGraphErrors* GetEnergyResolution();
	void PlotEnergyResolution(std::string outpdfname, double low=0.0, double high=0.3);
	TGraphErrors* GetBinnedMeansWide();
	TGraphErrors* GetBinnedMeansNarrow();
	void PlotBinnedMeans(std::string outpdfname, double low, double high);

    private:
};
