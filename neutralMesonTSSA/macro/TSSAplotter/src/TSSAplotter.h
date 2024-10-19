#include <utility>
#include <string>

#include <TH1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TString.h>

#include "binnedhistset/BinnedHistSet.h"

class TSSAplotter {
    public:
	TSSAplotter();
	~TSSAplotter();

	void GetHists(std::string infilename);

	double RelLumiAsym(double Nup, double Ndown, double relLumi);
	double RelLumiError(double Nup, double Ndown, double relLumi);
	double SqrtAsym(double NLup, double NLdown, double NRup, double NRdown);
	double SqrtError(double NLup, double NLdown, double NRup, double NRdown,
		double NLupErr, double NLdownErr, double NRupErr, double NRdownErr);
	TGraphErrors* RelLumiGraph(TH1* phi_up, TH1* phi_down, double relLumi);
	TGraphErrors* SqrtGraph(TH1* phi_up, TH1* phi_down);
	std::pair<float, float> RawAsym(std::string type, TGraphErrors* gr);
	void PlotOneAsym(std::string type, std::string title, TPad* pad, TGraphErrors* gr);
	void CompareOneAsym(std::string title, std::string outfilename, TGraphErrors* rl, TGraphErrors* sqrt);
	void PlotAsymsBinned(std::string type, std::string which, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down, std::string outfilename);
	TGraphErrors* AsymBinnedGraph(std::string type, BinnedHistSet* bhs);
	int main(std::string infilename, std::string outfilename = "NMhists.pdf");

    private:
	const float PI = 3.141592;
	TH1* nClusters = nullptr;
	TH1* nGoodClusters = nullptr;
	TH1* vtxz = nullptr;
	TH1* clusterE = nullptr;
	TH1* clusterEta = nullptr;
	TH2* clusterEta_vtxz = nullptr;
	TH1* clusterPhi = nullptr;
	TH2* clusterEta_Phi = nullptr;
	TH1* clusterpT = nullptr;
	TH1* clusterxF = nullptr;
	TH2* clusterpT_xF = nullptr;
	TH1* clusterChi2 = nullptr;
	TH1* clusterChi2zoomed = nullptr;
	TH1* mesonClusterChi2 = nullptr;
	TH2* goodClusterEta_Phi = nullptr;

	TH1* nDiphotons = nullptr;
	TH1* nRecoPi0s = nullptr;
	TH1* nRecoEtas = nullptr;
	TH1* diphoton_mass = nullptr;
	TH1* diphoton_pT = nullptr;
	TH1* diphoton_xF = nullptr;
	BinnedHistSet* bhs_diphotonMass_pT = nullptr;
	BinnedHistSet* bhs_diphotonMass_xF = nullptr;

	double relLumiBlue = 0.99771;
	double relLumiYellow = 1.00360;

	BinnedHistSet* bhs_pi0_blue_up_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_blue_down_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_yellow_up_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_yellow_down_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_blue_up_phi_xF = nullptr;
	BinnedHistSet* bhs_pi0_blue_down_phi_xF = nullptr;
	BinnedHistSet* bhs_pi0_yellow_up_phi_xF = nullptr;
	BinnedHistSet* bhs_pi0_yellow_down_phi_xF = nullptr;
	BinnedHistSet* bhs_pi0_blue_up_phi_eta = nullptr;
	BinnedHistSet* bhs_pi0_blue_down_phi_eta = nullptr;
	BinnedHistSet* bhs_pi0_yellow_up_phi_eta = nullptr;
	BinnedHistSet* bhs_pi0_yellow_down_phi_eta = nullptr;
	BinnedHistSet* bhs_pi0_blue_up_phi_vtxz = nullptr;
	BinnedHistSet* bhs_pi0_blue_down_phi_vtxz = nullptr;
	BinnedHistSet* bhs_pi0_yellow_up_phi_vtxz = nullptr;
	BinnedHistSet* bhs_pi0_yellow_down_phi_vtxz = nullptr;

	BinnedHistSet* bhs_eta_blue_up_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_blue_down_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_yellow_up_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_yellow_down_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_blue_up_phi_xF = nullptr;
	BinnedHistSet* bhs_eta_blue_down_phi_xF = nullptr;
	BinnedHistSet* bhs_eta_yellow_up_phi_xF = nullptr;
	BinnedHistSet* bhs_eta_yellow_down_phi_xF = nullptr;
	BinnedHistSet* bhs_eta_blue_up_phi_eta = nullptr;
	BinnedHistSet* bhs_eta_blue_down_phi_eta = nullptr;
	BinnedHistSet* bhs_eta_yellow_up_phi_eta = nullptr;
	BinnedHistSet* bhs_eta_yellow_down_phi_eta = nullptr;
	BinnedHistSet* bhs_eta_blue_up_phi_vtxz = nullptr;
	BinnedHistSet* bhs_eta_blue_down_phi_vtxz = nullptr;
	BinnedHistSet* bhs_eta_yellow_up_phi_vtxz = nullptr;
	BinnedHistSet* bhs_eta_yellow_down_phi_vtxz = nullptr;

	BinnedHistSet* bhs_pi0bkgr_blue_up_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_blue_down_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_up_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_down_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_blue_up_phi_xF = nullptr;
	BinnedHistSet* bhs_pi0bkgr_blue_down_phi_xF = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_up_phi_xF = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_down_phi_xF = nullptr;
	BinnedHistSet* bhs_pi0bkgr_blue_up_phi_eta = nullptr;
	BinnedHistSet* bhs_pi0bkgr_blue_down_phi_eta = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_up_phi_eta = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_down_phi_eta = nullptr;
	BinnedHistSet* bhs_pi0bkgr_blue_up_phi_vtxz = nullptr;
	BinnedHistSet* bhs_pi0bkgr_blue_down_phi_vtxz = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_up_phi_vtxz = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_down_phi_vtxz = nullptr;

	BinnedHistSet* bhs_etabkgr_blue_up_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_blue_down_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_up_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_down_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_blue_up_phi_xF = nullptr;
	BinnedHistSet* bhs_etabkgr_blue_down_phi_xF = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_up_phi_xF = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_down_phi_xF = nullptr;
	BinnedHistSet* bhs_etabkgr_blue_up_phi_eta = nullptr;
	BinnedHistSet* bhs_etabkgr_blue_down_phi_eta = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_up_phi_eta = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_down_phi_eta = nullptr;
	BinnedHistSet* bhs_etabkgr_blue_up_phi_vtxz = nullptr;
	BinnedHistSet* bhs_etabkgr_blue_down_phi_vtxz = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_up_phi_vtxz = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_down_phi_vtxz = nullptr;
};
