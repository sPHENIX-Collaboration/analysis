#include <string>
#include <vector>
#include <utility>

class TFile;
/* class TTree; */
class TChain;
class TH1;
class TH2;
class BinnedHistSet;

class PhiHists
{
    public:
	BinnedHistSet *phi_pT, *phi_pT_blue_up, *phi_pT_blue_down, *phi_pT_yellow_up, *phi_pT_yellow_down;
	BinnedHistSet *phi_pT_blue_up_forward, *phi_pT_blue_down_forward, *phi_pT_yellow_up_forward, *phi_pT_yellow_down_forward;
	BinnedHistSet *phi_pT_blue_up_backward, *phi_pT_blue_down_backward, *phi_pT_yellow_up_backward, *phi_pT_yellow_down_backward;
	BinnedHistSet *phi_xF, *phi_xF_blue_up, *phi_xF_blue_down, *phi_xF_yellow_up, *phi_xF_yellow_down;
	BinnedHistSet *phi_eta, *phi_eta_blue_up, *phi_eta_blue_down, *phi_eta_yellow_up, *phi_eta_yellow_down;
	BinnedHistSet *phi_vtxz, *phi_vtxz_blue_up, *phi_vtxz_blue_down, *phi_vtxz_yellow_up, *phi_vtxz_yellow_down;
	/* PhiHists() {} */
	/* ~PhiHists() {} */
};

class TSSAhistmaker {
    public:
	TSSAhistmaker(const std::string treebasename, const int jobnumber, const std::string histfname);
	~TSSAhistmaker();

	bool InRange(float mass, std::pair<float, float> range);
	void GetTrees();
	void MakePhiHists(std::string which); // which = pi0, eta, pi0bkgr, etabkgr
	void MakeAllHists();
	void FillPhiHists(std::string which, int index); // which = pi0, eta, pi0bkgr, etabkgr
	/* void FillAllPhiHists(int index); */
	/* void LoopClusters(); */
	/* void LoopDiphotons(); */
	void LoopTrees();
	void Cleanup();
	void main();

    private:
	// files and trees
	std::string treebasepath = "";
	int jobnum = 0;
	int nTreesInChain = 100;
	std::string treefilename = "";
	std::string histfilename = "";
	TFile* infile_trees = nullptr;
	TFile* outfile_hists = nullptr;
	/* TTree* tree_clusters = nullptr; */
	/* TTree* tree_diphotons = nullptr; */
	TChain* tree_event = nullptr;
	TChain* tree_clusters = nullptr;
	TChain* tree_diphotons = nullptr;

	// branches
	float crossingAngle = -9999999.9;
	float vtxz = -9999999.9;
	bool minbiastrig_live = false;
	bool photontrig_live = false;
	bool minbiastrig_scaled = false;
	bool photontrig_scaled = false;
	int bspin = 0;
	int yspin = 0;
	int nAllClusters = 0;
	std::vector<float>* cluster_E = nullptr;
	std::vector<float>* cluster_Ecore = nullptr;
	std::vector<float>* cluster_Eta = nullptr;
	std::vector<float>* cluster_Phi = nullptr;
	std::vector<float>* cluster_pT = nullptr;
	std::vector<float>* cluster_xF = nullptr;
	std::vector<float>* cluster_Chi2 = nullptr;
	std::vector<float>* diphoton_E = nullptr;
	std::vector<float>* diphoton_M = nullptr;
	std::vector<float>* diphoton_Eta = nullptr;
	std::vector<float>* diphoton_Phi = nullptr;
	std::vector<float>* diphoton_pT = nullptr;
	std::vector<float>* diphoton_xF = nullptr;
	std::vector<int>* diphoton_clus1index = nullptr;
	std::vector<int>* diphoton_clus2index = nullptr;
	std::vector<float>* diphoton_deltaR = nullptr;
	std::vector<float>* diphoton_asym = nullptr;

	// event-level distributions
	TH1* h_vtxz = nullptr;
	TH1* h_Nevents = nullptr;
	TH1* h_Npi0s = nullptr;
	TH1* h_Netas = nullptr;
	TH1* h_crossingAngle = nullptr;
	
	// cluster distributions
	/* TH1* h_nAllClusters = nullptr; */
	/* TH1* h_nGoodClusters = nullptr; */
	TH1* h_nClustersMinbiasTrig = nullptr;
	TH1* h_nClustersPhotonTrig = nullptr;

	// meson reco details
	/* std::pair<float, float> pi0MassRange{0.117, 0.167}; */
	std::pair<float, float> pi0MassRange{0.08, 0.22};
	/* std::pair<float, float> pi0BkgrLowRange{0.047, 0.097}; */
	/* std::pair<float, float> pi0BkgrHighRange{0.177, 0.227}; */
	std::pair<float, float> pi0BkgrLowRange{0.02, 0.07};
	std::pair<float, float> pi0BkgrHighRange{0.23, 0.28};
	/* std::pair<float, float> etaMassRange{0.494, 0.634}; */
	std::pair<float, float> etaMassRange{0.45, 0.75};
	std::pair<float, float> etaBkgrLowRange{0.330, 0.430};
	/* std::pair<float, float> etaBkgrHighRange{0.700, 0.800}; */
	std::pair<float, float> etaBkgrHighRange{0.770, 0.870};

	// diphoton distributions
	TH1* h_diphotonMass = nullptr;
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
	// Armeteros-Podalanski plots
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
	
	// phi histograms for asymmetries
	const float PI = 3.141592;
	int nHistBins_phi = 12;
	std::vector<double> pTbins_pi0 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0};
	std::vector<double> pTbins_eta = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0, 20.0};
	std::vector<double> xFbins = {-0.15, -0.10, -0.06, -0.03, 0.0, 0.03, 0.06, 0.10, 0.15};
	std::vector<double> etabins = {-3.0, -1.75, -1.15, -0.35, 0.0, 0.35, 1.15, 1.75, 3.0};
	std::vector<double> vtxzbins = {-200.0, -100.0, -50.0, -30.0, 0.0, 30.0, 50.0, 100.0, 200.0};
	PhiHists* pi0Hists = nullptr;
	PhiHists* etaHists = nullptr;
	PhiHists* pi0BkgrHists = nullptr;
	PhiHists* etaBkgrHists = nullptr;
	PhiHists* pi0Hists_lowEta = nullptr;
	PhiHists* etaHists_lowEta = nullptr;
	PhiHists* pi0BkgrHists_lowEta = nullptr;
	PhiHists* etaBkgrHists_lowEta = nullptr;
	PhiHists* pi0Hists_highEta = nullptr;
	PhiHists* etaHists_highEta = nullptr;
	PhiHists* pi0BkgrHists_highEta = nullptr;
	PhiHists* etaBkgrHists_highEta = nullptr;
};
