#include <utility>
#include <string>

#include <TH1.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TString.h>

#include "binnedhistset/BinnedHistSet.h"

class TSSAplotter {
    public:
	TSSAplotter();
	~TSSAplotter();

	void GetNMHists();
	void GetTSSAHists();
	void PlotNMHists();
	void PlotTSSAs();

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
	void PlotAsymsBinned(std::string which, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down, std::string outfilename);
	void PlotAsymsBinnedBlueYellow(std::string type, std::string which, BinnedHistSet* bhs_up_blue, BinnedHistSet* bhs_down_blue, BinnedHistSet* bhs_up_yellow, BinnedHistSet* bhs_down_yellow, std::string outfilename);
	std::pair<double, double> FitGraph(std::string type, TGraphErrors* gr);
	TGraphErrors* AmplitudeBinnedGraph(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down);
	void PlotAmplitudes(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down, std::string outfilename);
	void CompareAmplitudes(std::string title, std::string xlabel, BinnedHistSet* bhs_up, BinnedHistSet* bhs_down, std::string outfilename);
	void CompareAmplitudesEtaBinned(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up_lowEta, BinnedHistSet* bhs_down_lowEta, BinnedHistSet* bhs_up_highEta, BinnedHistSet* bhs_down_highEta, std::string outfilename);
	void CompareAmplitudesBlueYellow(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up_blue, BinnedHistSet* bhs_down_blue, BinnedHistSet* bhs_up_yellow, BinnedHistSet* bhs_down_yellow, std::string outfilename);
	std::pair<double, double> GetCorrectedAsymErr(double A_sig, double A_sig_err, double A_bkgr, double A_bkgr_err, double r, double r_err, double pol);
	TGraphErrors* CorrectedAmplitudesGraph(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up_sig, BinnedHistSet* bhs_down_sig, BinnedHistSet* bhs_up_bkgr, BinnedHistSet* bhs_down_bkgr);
	void PlotCorrectedAmplitudes(std::string type, std::string title, std::string xlabel, BinnedHistSet* bhs_up_sig, BinnedHistSet* bhs_down_sig, BinnedHistSet* bhs_up_bkgr, BinnedHistSet* bhs_down_bkgr, std::string outfilename);
	TGraphErrors* AveragedGraphs(TGraphErrors* gr_blue, TGraphErrors* gr_yellow);
	void PlotAveragedAmplitudes(std::string type, std::string title, std::string xlabel, std::string outfilename, BinnedHistSet* bhs_up_sig_blue, BinnedHistSet* bhs_down_sig_blue, BinnedHistSet* bhs_up_bkgr_blue, BinnedHistSet* bhs_down_bkgr_blue, BinnedHistSet* bhs_up_sig_yellow, BinnedHistSet* bhs_down_sig_yellow, BinnedHistSet* bhs_up_bkgr_yellow, BinnedHistSet* bhs_down_bkgr_yellow);
	void PlotGraph(TGraphErrors* gr, std::string title, std::string xlabel, std::string ylabel, std::string outfilename, bool inset = false, bool ylines = false);
	void CompareGraphs(TGraphErrors* gr1, TGraphErrors* gr2, std::string title, std::string xlabel, std::string ylabel, std::string leglabel1, std::string leglabel2, std::string outfilename, bool inset = false);
	void CompareGraphs(TGraphErrors* gr1, TGraphAsymmErrors* gr2, std::string title, std::string xlabel, std::string ylabel, std::string leglabel1, std::string leglabel2, std::string outfilename, bool inset = false);
	TGraphErrors* PolCorrectedGraph(TGraphErrors* gr, double pol);
	void PlotBackgroundFractions(std::string outfilename);
	TGraphErrors* tTestGraph(std::string type, TGraphErrors* gr1, TGraphErrors* gr2);
	TGraphErrors* tTestZeroGraph(TGraphErrors* gr1);
	void PlottTest(TGraphErrors* gr, std::string title, std::string xlabel, std::string outfilename, bool limit_yrange = true);
	void PlotPi0(std::string outfilename);
	void PlotEta(std::string outfilename);
	int main(std::string TSSApdf = "TSSAplots.pdf", std::string NMpdf = "");

	bool do_NMhists = false;
	std::string NMhistfname = "";
	std::string TSSAhistfname = "";
	std::string NMpdfname = "NMplots_Aug26.pdf";
	std::string TSSApdfname = "TSSAplots_Aug26.pdf";

    private:
	const float PI = 3.141592;
	double relLumiBlue = 0.99822;
	double relLumiYellow = 0.99767;
	double polBlue = 0.48644;
	double polYellow = 0.45455;

	// NM hists
	TH1* h_nEvents = nullptr; // from NM
	TH1* h_vtxz = nullptr;
	TH2* h_towerEta_Phi_500MeV = nullptr;
	TH2* h_towerEta_Phi_800MeV = nullptr;
	// cluster distributions
	TH1* h_nAllClusters = nullptr;
	TH1* h_nGoodClusters = nullptr;
	TH1* h_clusterE = nullptr;
	TH1* h_clusterEta = nullptr;
	TH2* h_clusterVtxz_Eta = nullptr;
	TH1* h_clusterPhi = nullptr;
	TH2* h_clusterEta_Phi = nullptr;
	TH1* h_clusterpT = nullptr;
	TH1* h_clusterxF = nullptr;
	TH2* h_clusterpT_xF = nullptr;
	TH1* h_clusterChi2 = nullptr;
	TH1* h_clusterChi2zoomed = nullptr;
	TH1* h_mesonClusterChi2 = nullptr;
	TH1* h_goodClusterE = nullptr;
	TH1* h_goodClusterEta = nullptr;
	TH2* h_goodClusterVtxz_Eta = nullptr;
	TH1* h_goodClusterPhi = nullptr;
	TH2* h_goodClusterEta_Phi = nullptr;
	TH1* h_goodClusterpT = nullptr;
	TH1* h_goodClusterxF = nullptr;
	TH2* h_goodClusterpT_xF = nullptr;
	// diphoton distributions
	TH1* h_nDiphotons = nullptr;
	TH1* h_nRecoPi0s = nullptr;
	TH1* h_nRecoEtas = nullptr;
	TH1* h_diphotonMass = nullptr;
	TH1* h_diphotonE = nullptr;
	TH1* h_diphotonEta = nullptr;
	TH2* h_diphotonVtxz_Eta = nullptr;
	TH1* h_diphotonPhi = nullptr;
	TH2* h_diphotonEta_Phi = nullptr;
	TH1* h_diphotonpT = nullptr;
	TH1* h_diphotonxF = nullptr;
	TH2* h_diphotonpT_xF = nullptr;
	TH1* h_diphotonAsym = nullptr;
	TH1* h_diphotonDeltaR = nullptr;
	TH2* h_diphotonAsym_DeltaR = nullptr;
	BinnedHistSet* bhs_diphotonMass_pT = nullptr;
	BinnedHistSet* bhs_diphotonMass_xF = nullptr;

	// TSSA hists
	TH1* h_nClustersMinbiasTrig = nullptr;
	TH1* h_nClustersPhotonTrig = nullptr;
	TH1* h_Nevents = nullptr;
	TH1* h_Npi0s = nullptr;
	TH1* h_Netas = nullptr;

	BinnedHistSet* bhs_diphotonMass_pT_pi0binning = nullptr;
	BinnedHistSet* bhs_diphotonMass_pT_etabinning = nullptr;
	BinnedHistSet* bhs_pi0_pT_pT = nullptr;
	BinnedHistSet* bhs_eta_pT_pT = nullptr;
	BinnedHistSet* bhs_diphotonxF_xF = nullptr;
	BinnedHistSet* bhs_diphotoneta_eta = nullptr;
	BinnedHistSet* bhs_diphotonvtxz_vtxz = nullptr;
	TH2* h_DiphotonMassAsym = nullptr;
	TH2* h_DiphotonMassdeltaR = nullptr;
	TH2* h_DiphotondeltaRAsym = nullptr;
	TH2* h_DiphotondeltaRAsym_pi0 = nullptr;
	TH2* h_DiphotondeltaRAsym_pi0_smalldR = nullptr;
	TH2* h_DiphotondeltaRAsym_eta = nullptr;
	TH2* h_DiphotondeltaRAsym_etabkgr = nullptr;
	TH2* h_DiphotonMassAsym_highpT = nullptr;
	TH2* h_DiphotonMassdeltaR_highpT = nullptr;
	TH2* h_DiphotondeltaRAsym_highpT = nullptr;
	TH2* h_DiphotondeltaRAsym_highpT_smalldR = nullptr;
	TH2* h_DiphotondeltaRAsym_eta_highpT = nullptr;
	TH2* h_DiphotondeltaRAsym_etabkgr_highpT = nullptr;
	// Armenteros-Podolanski plots
	TH2* h_armen_all = nullptr;
	TH2* h_armen_pi0 = nullptr;
	TH2* h_armen_pi0bkgr = nullptr;
	TH2* h_armen_eta = nullptr;
	TH2* h_armen_etabkgr = nullptr;
	TH2* h_armen_eta_highpT = nullptr;
	TH2* h_armen_etabkgr_highpT = nullptr;
	TH1* h_armen_p_L = nullptr;
	TH1* h_armen_p_T = nullptr;
	TH2* h_armen_alpha_alphaE = nullptr;
	TH2* h_armen_alpha_deltaR = nullptr;
	TH2* h_armen_alpha_deltaR_pi0 = nullptr;
	TH2* h_armen_alpha_deltaR_eta = nullptr;
	TH2* h_armen_p_T_deltaR = nullptr;
	TH2* h_armen_pT_p_L = nullptr;
	TH2* h_armen_pT_p_T = nullptr;

	// Phi hists
	/* std::vector<double> pTbins = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0, 20.0}; */
	std::vector<double> pTbins_pi0 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0};
	std::vector<double> pTbins_eta = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0, 20.0};
	std::vector<double> xFbins = {-0.15, -0.10, -0.06, -0.03, 0.0, 0.03, 0.06, 0.10, 0.15};
	std::vector<double> etabins = {-3.0, -1.75, -1.15, -0.35, 0.0, 0.35, 1.15, 1.75, 3.0};
	std::vector<double> vtxzbins = {200.0, -100.0, -50.0, -30.0, 0.0, 30.0, 50.0, 100.0, 200.0};
	std::vector<double> pTmeans_pi0 = {1.63, 2.483, 3.436, 4.41, 5.395, 6.391, 7.39, 8.588};
	std::vector<double> pTmeans_eta = {2.432, 3.428, 4.422, 5.414, 6.409, 7.415, 8.717, 11.54};
	std::vector<double> xFmeans = {-0.112, -0.0687, -0.0394, -0.0153, 0.0141, 0.0400, 0.0687, 0.112};
	std::vector<double> etameans = {-1.915, -1.368, -0.7822, -0.1776, 0.1731, 0.7643, 1.361, 1.923};
	std::vector<double> vtxzmeans = {-120.6, -68.79, -39.3, -13.03, 13.44, 39.43, 68.59, 121.4};
	std::vector<double> pi0_bkgr_fractions = {0.6831, 0.47138, 0.22116, 0.17628, 0.17482, 0.19737, 0.24736, 0.7249};
	std::vector<double> pi0_bkgr_fraction_errs = {0.00018, 0.00014, 0.00017, 0.00026, 0.00047, 0.00091, 0.00188, 0.00184};
	std::vector<double> eta_bkgr_fractions = {0.96982, 0.89879, 0.83979, 0.79816, 0.76808, 0.73977, 0.74278, 0.76591};
	std::vector<double> eta_bkgr_fraction_errs = {0.00005, 0.00015, 0.00031, 0.00058, 0.00109, 0.00202, 0.00275, 0.00500};
	/* std::vector<double> xFbins = {-0.15, -0.10, -0.06, -0.03, 0.0, 0.03, 0.06, 0.10, 0.15}; */
	/* std::vector<double> etabins = {-2.0, -1.15, -0.35, 0.0, 0.35, 1.15, 2.0}; */
	/* std::vector<double> vtxzbins = {-100.0, -50.0, -30.0, 0.0, 30.0, 50.0, 100.0}; */
	int nHistBins_phi = 12;

	BinnedHistSet* bhs_pi0_blue_up_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_blue_down_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_yellow_up_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_yellow_down_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_blue_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_blue_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_yellow_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_yellow_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_blue_up_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_blue_down_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_yellow_up_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_yellow_down_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_lowEta_blue_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_lowEta_blue_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_lowEta_yellow_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_lowEta_yellow_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_highEta_blue_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_highEta_blue_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_highEta_yellow_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0_highEta_yellow_down_forward_phi_pT = nullptr;
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
	BinnedHistSet* bhs_eta_blue_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_blue_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_yellow_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_yellow_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_lowEta_blue_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_lowEta_blue_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_lowEta_yellow_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_lowEta_yellow_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_highEta_blue_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_highEta_blue_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_highEta_yellow_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_highEta_yellow_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_blue_up_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_blue_down_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_yellow_up_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_eta_yellow_down_backward_phi_pT = nullptr;
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
	BinnedHistSet* bhs_pi0bkgr_blue_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_blue_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_blue_up_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_blue_down_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_up_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_yellow_down_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_lowEta_blue_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_lowEta_blue_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_lowEta_yellow_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_lowEta_yellow_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_highEta_blue_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_highEta_blue_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_highEta_yellow_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_pi0bkgr_highEta_yellow_down_forward_phi_pT = nullptr;
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
	BinnedHistSet* bhs_etabkgr_blue_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_blue_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_up_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_down_forward_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_blue_up_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_blue_down_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_up_backward_phi_pT = nullptr;
	BinnedHistSet* bhs_etabkgr_yellow_down_backward_phi_pT = nullptr;
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
