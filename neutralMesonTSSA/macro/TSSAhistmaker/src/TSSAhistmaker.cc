#include "TSSAhistmaker.h"
#include "binnedhistset/BinnedHistSet.h"

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TH1.h>
#include <TString.h>

#include <iostream>

TSSAhistmaker::TSSAhistmaker(const std::string treepath, const int jobnumber, const std::string histfname):
    treebasepath(treepath),
    jobnum(jobnumber),
    histfilename(histfname)
{
}

TSSAhistmaker::~TSSAhistmaker() {}

bool TSSAhistmaker::InRange(float mass, std::pair<float, float> range)
{
    bool ret = false;
    if ( (mass > range.first) && (mass < range.second) ) ret = true;
    return ret;
}

void TSSAhistmaker::GetTrees()
{
    /* infile_trees = new TFile(treefilename.c_str(), "READ"); */
    /* tree_clusters = (TTree*)infile_trees->Get("Clusters"); */
    /* tree_diphotons = (TTree*)infile_trees->Get("Diphotons"); */
    /* if (!tree_clusters) { */
	/* std::cout << "Unable to read tree Clusters from " << treefilename << "; exiting!"; */
	/* exit(1); */
    /* } */
    /* if (!tree_diphotons) { */
	/* std::cout << "Unable to read tree Diphotons from " << treefilename << "; exiting!"; */
	/* exit(1); */
    /* } */
    tree_event = new TChain("Event");
    tree_clusters = new TChain("Clusters");
    tree_diphotons = new TChain("Diphotons");
    int startingtree = jobnum*nTreesInChain;
    for (int i=startingtree; i<(startingtree+nTreesInChain); i++) {
	std::string treepath = treebasepath + "/" + std::to_string(i) + "/";
	treefilename = treepath + std::string("NMtrees-") + std::to_string(i) + std::string(".root");
	std::cout << "Adding " << treefilename << " to TChains" << std::endl;;
	tree_event->Add(treefilename.c_str());
	tree_clusters->Add(treefilename.c_str());
	tree_diphotons->Add(treefilename.c_str());
	/* std::cout << "Adding " << treefilename << " to TChains... "; */
	/* int cluschain = tree_clusters->AddFile(treefilename.c_str(), TTree::kMaxEntries, "Clusters"); */
	/* int diphchain = tree_diphotons->AddFile(treefilename.c_str(), TTree::kMaxEntries, "Diphotons"); */
	/* std::cout << cluschain << ", " << diphchain << std::endl; */
    }

    /* tree_event->Print(); */
    /* tree_clusters->Print(); */
    /* tree_diphotons->Print(); */
    tree_event->SetBranchAddress("crossingAngle", &crossingAngle);
    tree_event->SetBranchAddress("vtxz", &vtxz);
    tree_event->SetBranchAddress("minbiastrig_live", &minbiastrig_live);
    tree_event->SetBranchAddress("photontrig_live", &photontrig_live);
    tree_event->SetBranchAddress("minbiastrig_scaled", &minbiastrig_scaled);
    tree_event->SetBranchAddress("photontrig_scaled", &photontrig_scaled);
    tree_event->SetBranchAddress("bspin", &bspin);
    tree_event->SetBranchAddress("yspin", &yspin);

    tree_clusters->SetBranchAddress("clusterE", &cluster_E);
    tree_clusters->SetBranchAddress("clusterEcore", &cluster_Ecore);
    tree_clusters->SetBranchAddress("clusterEta", &cluster_Eta);
    tree_clusters->SetBranchAddress("clusterPhi", &cluster_Phi);
    tree_clusters->SetBranchAddress("clusterpT", &cluster_pT);
    tree_clusters->SetBranchAddress("clusterxF", &cluster_xF);
    tree_clusters->SetBranchAddress("clusterChi2", &cluster_Chi2);
    tree_clusters->SetBranchAddress("nAllClusters", &nAllClusters);

    tree_diphotons->SetBranchAddress("diphotonE", &diphoton_E);
    tree_diphotons->SetBranchAddress("diphotonM", &diphoton_M);
    tree_diphotons->SetBranchAddress("diphotonEta", &diphoton_Eta);
    tree_diphotons->SetBranchAddress("diphotonPhi", &diphoton_Phi);
    tree_diphotons->SetBranchAddress("diphotonpT", &diphoton_pT);
    tree_diphotons->SetBranchAddress("diphotonxF", &diphoton_xF);
    tree_diphotons->SetBranchAddress("diphotonClusterIndex1", &diphoton_clus1index);
    tree_diphotons->SetBranchAddress("diphotonClusterIndex2", &diphoton_clus2index);
    tree_diphotons->SetBranchAddress("diphotonDeltaR", &diphoton_deltaR);
    tree_diphotons->SetBranchAddress("diphotonAsym", &diphoton_asym);
}

void TSSAhistmaker::MakePhiHists(std::string which)
{
    PhiHists* hists = new PhiHists();
    std::string nameprefix, titlewhich, titlebeam;
    nameprefix = "h_" + which + "_";
    std::vector<double> pTbins;

    if (which == "pi0") {
	titlewhich = "#pi^{0}";
	pi0Hists = hists;
	pTbins = pTbins_pi0;
    }
    else if (which == "eta") {
	titlewhich = "#eta";
	etaHists = hists;
	pTbins = pTbins_eta;
    }
    else if (which == "pi0bkgr") {
	titlewhich = "#pi^{0} Background";
	pi0BkgrHists = hists;
	pTbins = pTbins_pi0;
    }
    else if (which == "etabkgr") {
	titlewhich = "#eta Background";
	etaBkgrHists = hists;
	pTbins = pTbins_eta;
    }
    else if (which == "pi0_lowEta") {
	titlewhich = "#pi^{0}, |#eta| < 0.35";
	pi0Hists_lowEta = hists;
	pTbins = pTbins_pi0;
    }
    else if (which == "eta_lowEta") {
	titlewhich = "#eta, |#eta| < 0.35";
	etaHists_lowEta = hists;
	pTbins = pTbins_eta;
    }
    else if (which == "pi0bkgr_lowEta") {
	titlewhich = "#pi^{0} Background, |#eta| < 0.35";
	pi0BkgrHists_lowEta = hists;
	pTbins = pTbins_pi0;
    }
    else if (which == "etabkgr_lowEta") {
	titlewhich = "#eta Background, |#eta| < 0.35";
	etaBkgrHists_lowEta = hists;
	pTbins = pTbins_eta;
    }
    else if (which == "pi0_highEta") {
	titlewhich = "#pi^{0}, |#eta| > 0.35";
	pi0Hists_highEta = hists;
	pTbins = pTbins_pi0;
    }
    else if (which == "eta_highEta") {
	titlewhich = "#eta, |#eta| > 0.35";
	etaHists_highEta = hists;
	pTbins = pTbins_eta;
    }
    else if (which == "pi0bkgr_highEta") {
	titlewhich = "#pi^{0} Background, |#eta| > 0.35";
	pi0BkgrHists_highEta = hists;
	pTbins = pTbins_pi0;
    }
    else if (which == "etabkgr_highEta") {
	titlewhich = "#eta Background, |#eta| > 0.35";
	etaBkgrHists_highEta = hists;
	pTbins = pTbins_eta;
    }
    else {
	std::cout << "In MakePhiHists(): Invalid argument " << which << " !" << std::endl;
	return;
    }

    hists->phi_pT = new BinnedHistSet(Form("%sphi_pT", nameprefix.c_str()), Form("%s #phi Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_blue_up = new BinnedHistSet(Form("%sphi_pT_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_blue_down = new BinnedHistSet(Form("%sphi_pT_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_yellow_up = new BinnedHistSet(Form("%sphi_pT_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_yellow_down = new BinnedHistSet(Form("%sphi_pT_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_blue_up_forward = new BinnedHistSet(Form("%sphi_pT_blue_up_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Up #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_blue_down_forward = new BinnedHistSet(Form("%sphi_pT_blue_down_forward", nameprefix.c_str()), Form("%s Blue Beam Forward-Going Spin-Down #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_yellow_up_forward = new BinnedHistSet(Form("%sphi_pT_yellow_up_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Up #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_yellow_down_forward = new BinnedHistSet(Form("%sphi_pT_yellow_down_forward", nameprefix.c_str()), Form("%s Yellow Beam Forward-Going Spin-Down #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_blue_up_backward = new BinnedHistSet(Form("%sphi_pT_blue_up_backward", nameprefix.c_str()), Form("%s Blue Beam Backward-Going Spin-Up #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_blue_down_backward = new BinnedHistSet(Form("%sphi_pT_blue_down_backward", nameprefix.c_str()), Form("%s Blue Beam Backward-Going Spin-Down #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_yellow_up_backward = new BinnedHistSet(Form("%sphi_pT_yellow_up_backward", nameprefix.c_str()), Form("%s Yellow Beam Backward-Going Spin-Up #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_yellow_down_backward = new BinnedHistSet(Form("%sphi_pT_yellow_down_backward", nameprefix.c_str()), Form("%s Yellow Beam Backward-Going Spin-Down #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);

    hists->phi_xF = new BinnedHistSet(Form("%sphi_xF", nameprefix.c_str()), Form("%s #phi Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    hists->phi_xF_blue_up = new BinnedHistSet(Form("%sphi_xF_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    hists->phi_xF_blue_down = new BinnedHistSet(Form("%sphi_xF_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    hists->phi_xF_yellow_up = new BinnedHistSet(Form("%sphi_xF_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    hists->phi_xF_yellow_down = new BinnedHistSet(Form("%sphi_xF_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);

    hists->phi_eta = new BinnedHistSet(Form("%sphi_eta", nameprefix.c_str()), Form("%s #phi Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    hists->phi_eta_blue_up = new BinnedHistSet(Form("%sphi_eta_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    hists->phi_eta_blue_down = new BinnedHistSet(Form("%sphi_eta_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    hists->phi_eta_yellow_up = new BinnedHistSet(Form("%sphi_eta_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);
    hists->phi_eta_yellow_down = new BinnedHistSet(Form("%sphi_eta_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "#eta", etabins);

    hists->phi_vtxz = new BinnedHistSet(Form("%sphi_vtxz", nameprefix.c_str()), Form("%s #phi Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx} (cm)", vtxzbins);
    hists->phi_vtxz_blue_up = new BinnedHistSet(Form("%sphi_vtxz_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx} (cm)", vtxzbins);
    hists->phi_vtxz_blue_down = new BinnedHistSet(Form("%sphi_vtxz_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx} (cm)", vtxzbins);
    hists->phi_vtxz_yellow_up = new BinnedHistSet(Form("%sphi_vtxz_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx} (cm)", vtxzbins);
    hists->phi_vtxz_yellow_down = new BinnedHistSet(Form("%sphi_vtxz_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "z_{vtx} (cm)", vtxzbins);

    hists->phi_pT->MakeHists();
    hists->phi_pT_blue_up->MakeHists();
    hists->phi_pT_blue_down->MakeHists();
    hists->phi_pT_yellow_up->MakeHists();
    hists->phi_pT_yellow_down->MakeHists();
    hists->phi_pT_blue_up_forward->MakeHists();
    hists->phi_pT_blue_down_forward->MakeHists();
    hists->phi_pT_yellow_up_forward->MakeHists();
    hists->phi_pT_yellow_down_forward->MakeHists();
    hists->phi_pT_blue_up_backward->MakeHists();
    hists->phi_pT_blue_down_backward->MakeHists();
    hists->phi_pT_yellow_up_backward->MakeHists();
    hists->phi_pT_yellow_down_backward->MakeHists();

    hists->phi_xF->MakeHists();
    hists->phi_xF_blue_up->MakeHists();
    hists->phi_xF_blue_down->MakeHists();
    hists->phi_xF_yellow_up->MakeHists();
    hists->phi_xF_yellow_down->MakeHists();

    hists->phi_eta->MakeHists();
    hists->phi_eta_blue_up->MakeHists();
    hists->phi_eta_blue_down->MakeHists();
    hists->phi_eta_yellow_up->MakeHists();
    hists->phi_eta_yellow_down->MakeHists();

    hists->phi_vtxz->MakeHists();
    hists->phi_vtxz_blue_up->MakeHists();
    hists->phi_vtxz_blue_down->MakeHists();
    hists->phi_vtxz_yellow_up->MakeHists();
    hists->phi_vtxz_yellow_down->MakeHists();
}

void TSSAhistmaker::MakeAllHists()
{
    // create file
    outfile_hists = new TFile(histfilename.c_str(), "RECREATE");
    outfile_hists->cd();

    // event-level
    int nbins_vtxz = 100;
    double vtxz_upper = 200.0;
    h_vtxz = new TH1F("h_vtxz", "Vertex z Distribution;z_{vtx} (cm);Counts", nbins_vtxz, -vtxz_upper, vtxz_upper);
    h_Nevents = new TH1F("h_Nevents", "Number of Events (Minbias trig, GlobalVertex, Positive E_{EMCal}, N_{clusters} > 1);;Counts", 1, -0.5, 0.5);
    h_Npi0s = new TH1F("h_Npi0s", "Number of Diphotons in #pi^{0} Mass Range;;Counts", 1, -0.5, 0.5);
    h_Netas = new TH1F("h_Netas", "Number of Diphotons in #eta Mass Range;;Counts", 1, -0.5, 0.5);

    // clusters
    /* h_nAllClusters = new TH1F("h_nAllClusters", "Total Number of Clusters per Event;# Clusters;Counts", 3500, 0.0, 3500.0); */
    /* h_nGoodClusters = new TH1F("h_nGoodClusters", "Number of \"Good\" Clusters per Event;# Clusters;Counts", 13, -0.5, 12.5); */
    h_nClustersMinbiasTrig = new TH1F("h_nClustersMinbiasTrig", "Number of \"Good\" Clusters per Event, Minbias Trigger;# Clusters;Counts", 13, -0.5, 12.5);
    h_nClustersPhotonTrig = new TH1F("h_nClustersPhotonTrig", "Number of \"Good\" Clusters per Event, Photon Trigger;# Clusters;Counts", 13, -0.5, 12.5);

    // diphotons
    int nbins_mass = 100;
    int nbins_asym = 100;
    int nbins_dR = 100;
    double dR_upper_small = 0.5;
    h_DiphotonMassAsym = new TH2F("h_DiphotonMassAsym", "Diphoton Pair Asymmetry vs Mass;Mass (GeV);#alpha", nbins_mass, 0.0, 1.0, nbins_asym, 0.0, 1.0);
    h_DiphotonMassdeltaR = new TH2F("h_DiphotonMassdeltaR", "Diphoton #Delta R vs Mass; Mass (GeV);#Delta R", nbins_mass, 0.0, 1.0, nbins_dR, 0.0, PI);
    h_DiphotondeltaRAsym = new TH2F("h_DiphotondeltaRAsym", "Diphoton Pair Asymmetry vs #Delta R;#Delta R;#alpha", nbins_dR, 0.0, PI, nbins_dR, 0.0, 1.0);
    h_DiphotondeltaRAsym_pi0 = new TH2F("h_DiphotondeltaRAsym_pi0", "Diphoton Pair Asymmetry vs #Delta R, Mass in #pi^{0} Range;#Delta R;#alpha", nbins_dR, 0.0, PI, nbins_dR, 0.0, 1.0);
    h_DiphotondeltaRAsym_pi0_smalldR = new TH2F("h_DiphotondeltaRAsym_pi0_smalldR", "Diphoton Pair Asymmetry vs #Delta R, Mass in #pi^{0} Range;#Delta R;#alpha", nbins_dR, 0.0, dR_upper_small, nbins_dR, 0.0, 1.0);
    h_DiphotondeltaRAsym_eta = new TH2F("h_DiphotondeltaRAsym_eta", "Diphoton Pair Asymmetry vs #Delta R, Mass in #eta Range;#Delta R;#alpha", nbins_dR, 0.0, PI, nbins_dR, 0.0, 1.0);
    h_DiphotondeltaRAsym_etabkgr = new TH2F("h_DiphotondeltaRAsym_etabkgr", "Diphoton Pair Asymmetry vs #Delta R, Mass in #eta Background Range;#Delta R;#alpha", nbins_dR, 0.0, PI, nbins_dR, 0.0, 1.0);
    h_DiphotonMassAsym_highpT = new TH2F("h_DiphotonMassAsym_highpT", "Diphoton Pair Asymmetry vs Mass, p_{T} > 7 GeV;Mass (GeV);#alpha", nbins_mass, 0.0, 1.0, nbins_asym, 0.0, 1.0);
    h_DiphotonMassdeltaR_highpT = new TH2F("h_DiphotonMassdeltaR_highpT", "Diphoton #Delta R vs Mass, p_{T} > 7 GeV;Mass (GeV);#Delta R", nbins_mass, 0.0, 1.0, nbins_dR, 0.0, dR_upper_small);
    h_DiphotondeltaRAsym_highpT = new TH2F("h_DiphotondeltaRAsym_highpT", "Diphoton Pair Asymmetry vs #Delta R, p_{T} > 7 GeV;#Delta R;#alpha", nbins_dR, 0.0, PI, nbins_dR, 0.0, 1.0);
    h_DiphotondeltaRAsym_highpT_smalldR = new TH2F("h_DiphotondeltaRAsym_highpT_smalldR", "Diphoton Pair Asymmetry vs #Delta R, p_{T} > 7 GeV;#Delta R;#alpha", nbins_dR, 0.0, dR_upper_small, nbins_dR, 0.0, 1.0);
    h_DiphotondeltaRAsym_eta_highpT = new TH2F("h_DiphotondeltaRAsym_eta_highpT", "Diphoton Pair Asymmetry vs #Delta R, Mass in #eta Range, p_{T} > 7 GeV;#Delta R;#alpha", nbins_dR, 0.0, dR_upper_small, nbins_dR, 0.0, 1.0);
    h_DiphotondeltaRAsym_etabkgr_highpT = new TH2F("h_DiphotondeltaRAsym_etabkgr_highpT", "Diphoton Pair Asymmetry vs #Delta R, Mass in #eta Background Range, p_{T} > 7 GeV;#Delta R;#alpha", nbins_dR, 0.0, dR_upper_small, nbins_dR, 0.0, 1.0);
    // Armenteros-Podolanski plots
    h_armen_all = new TH2F("h_armen_all", "Armenteros-Podolanski Plot, All Diphotons;#alpha = #frac{p_{L1} - p_{L2}}{p_{L1} + p_{L2}};p_{T_{#gamma#gamma}}", 100, -1.0, 1.0, 100, 0.0, 5.0);
    h_armen_pi0 = new TH2F("h_armen_pi0", "Armenteros-Podolanski Plot, #pi^{0} Mass Range;#alpha = #frac{p_{L1} - p_{L2}}{p_{L1} + p_{L2}};p_{T_{#gamma#gamma}}", 100, -1.0, 1.0, 100, 0.0, 0.5);
    h_armen_pi0bkgr = new TH2F("h_armen_pi0bkgr", "Armenteros-Podolanski Plot, #pi^{0} Background Mass Range;#alpha = #frac{p_{L1} - p_{L2}}{p_{L1} + p_{L2}};p_{T_{#gamma#gamma}}", 100, -1.0, 1.0, 100, 0.0, 0.5);
    h_armen_eta = new TH2F("h_armen_eta", "Armenteros-Podolanski Plot, #eta Mass Range;#alpha = #frac{p_{L1} - p_{L2}}{p_{L1} + p_{L2}};p_{T_{#gamma#gamma}}", 100, -1.0, 1.0, 100, 0.0, 0.5);
    h_armen_etabkgr = new TH2F("h_armen_etabkgr", "Armenteros-Podolanski Plot, #eta Background Mass Range;#alpha = #frac{p_{L1} - p_{L2}}{p_{L1} + p_{L2}};p_{T_{#gamma#gamma}}", 100, -1.0, 1.0, 100, 0.0, 0.5);
    h_armen_eta_highpT = new TH2F("h_armen_eta_highpT", "Armenteros-Podolanski Plot, #eta Mass Range, p_{T} > 7 GeV;#alpha = #frac{p_{L1} - p_{L2}}{p_{L1} + p_{L2}};p_{T_{#gamma#gamma}}", 100, -1.0, 1.0, 100, 0.0, 0.5);
    h_armen_etabkgr_highpT = new TH2F("h_armen_etabkgr_highpT", "Armenteros-Podolanski Plot, #eta Background Mass Range, p_{T} > 7 GeV;#alpha = #frac{p_{L1} - p_{L2}}{p_{L1} + p_{L2}};p_{T_{#gamma#gamma}}", 100, -1.0, 1.0, 100, 0.0, 0.5);
    h_armen_p_L = new TH1F("h_armen_p_L", "Cluster p_{L} w.r.t. Diphoton;p_{L, #gamma#gamma} (GeV);Counts", 200, 0.0, 10.0);
    h_armen_p_T = new TH1F("h_armen_p_T", "Cluster p_{T} w.r.t. Diphoton;p_{T, #gamma#gamma} (GeV);Counts", 200, 0.0, 10.0);
    h_armen_alpha_alphaE = new TH2F("h_armen_alpha_alphaE", "Energy Asymmetry vs Armenteros #alpha;(p_{L1} - p_{L2})/(p_{L1} + p_{L2});(E1 - E2)/(E1 + E2)", 100, -1.0, 1.0, 100, -1.0, 1.0);
    h_armen_alpha_deltaR = new TH2F("h_armen_alpha_deltaR", "Cluster #Delta R vs Armenteros #alpha;(p_{L1} - p_{L2})/(p_{L1} + p_{L2});#Delta R (rad)", 100, -1.0, 1.0, 100, 0.0, PI);
    h_armen_alpha_deltaR_pi0 = new TH2F("h_armen_alpha_deltaR_pi0", "Cluster #Delta R vs Armenteros #alpha, #pi^{0} Mass Range;(p_{L1} - p_{L2})/(p_{L1} + p_{L2});#Delta R (rad)", 100, -1.0, 1.0, 100, 0.0, PI);
    h_armen_alpha_deltaR_eta = new TH2F("h_armen_alpha_deltaR_eta", "Cluster #Delta R vs Armenteros #alpha, #eta Mass Range;(p_{L1} - p_{L2})/(p_{L1} + p_{L2});#Delta R (rad)", 100, -1.0, 1.0, 100, 0.0, PI);
    h_armen_p_T_deltaR = new TH2F("h_armen_p_T_deltaR", "Cluster #Delta R vs p_{T} w.r.t. Diphoton;p_{T, #gamma#gamma};#Delta R (rad)", 200, 0.0, 10.0, 100, 0.0, PI);
    h_armen_pT_p_L = new TH2F("h_armen_pT_p_L", "Diphoton p_{T} vs Cluster p_{L} w.r.t. Diphoton;p_{T} (GeV);p_{L, #gamma#gamma}", 200, 0.0, 20.0, 100, 0.0, 10.0);
    h_armen_pT_p_T = new TH2F("h_armen_pT_p_T", "Diphoton p_{T} vs Cluster p_{T} w.r.t. Diphoton;p_{T} (GeV);p_{T, #gamma#gamma}", 200, 0.0, 20.0, 100, 0.0, 10.0);

    // diphoton mass
    float pT_upper = 20.0;
    h_diphotonMass = new TH1F("h_diphotonMass", "Diphoton Mass Distribution (Cluster p_{T} > 1.5GeV);Diphoton Mass (GeV);Counts", nbins_mass, 0.0, 1.0);
    std::vector<double> pTbins_mass;
    /* std::vector<double> xFbins; */
    int nbins_bhs = 20;
    for (int i=0; i<nbins_bhs; i++) {
	pTbins_mass.push_back(i*(pT_upper/nbins_bhs));
	/* xFbins.push_back(2*xF_upper*i/nbins_bhs - xF_upper); */
    }
    pTbins_mass.push_back(pT_upper);
    /* xFbins.push_back(xF_upper); */
    /* bhs_diphotonMass_pT = new BinnedHistSet("h_diphotonMass_pT", "Diphoton Mass;Mass (GeV);Counts", nbins_mass, 0.0, 1.0, "p_{T} (GeV)", pTbins_mass); */
    bhs_diphotonMass_pT_pi0binning = new BinnedHistSet("h_diphotonMass_pT_pi0binning", "Diphoton Mass;Mass (GeV);Counts", nbins_mass, 0.0, 1.0, "p_{T} (GeV)", pTbins_pi0);
    bhs_diphotonMass_pT_etabinning = new BinnedHistSet("h_diphotonMass_pT_etabinning", "Diphoton Mass;Mass (GeV);Counts", nbins_mass, 0.0, 1.0, "p_{T} (GeV)", pTbins_eta);
    bhs_pi0_pT_pT = new BinnedHistSet("h_pi0_pT_pT", "#pi^{0} p_{T};p_{T} (GeV);Counts", 100, 1.0, 10.0, "p_{T} (GeV)", pTbins_pi0);
    bhs_eta_pT_pT = new BinnedHistSet("h_eta_pT_pT", "#eta p_{T};p_{T} (GeV);Counts", 100, 2.0, 20.0, "p_{T} (GeV)", pTbins_eta);
    bhs_diphotonxF_xF = new BinnedHistSet("h_diphotonxF_xF", "Diphoton x_{F};x_{F};Counts", 100, -0.15, 0.15, "x_{F}", xFbins);
    bhs_diphotoneta_eta = new BinnedHistSet("h_diphotoneta_eta", "Diphoton #eta;#eta;Counts", 100, -3.0, 3.0, "#eta", etabins);
    bhs_diphotonvtxz_vtxz = new BinnedHistSet("h_diphotonvtxz_vtxz", "Diphoton vtxz;vtxz (cm);Counts", 100, -200.0, 200.0, "vtxz (cm)", vtxzbins);
    bhs_diphotonMass_pT_pi0binning->MakeHists();
    bhs_diphotonMass_pT_etabinning->MakeHists();
    bhs_pi0_pT_pT->MakeHists();
    bhs_eta_pT_pT->MakeHists();
    bhs_diphotonxF_xF->MakeHists();
    bhs_diphotoneta_eta->MakeHists();
    bhs_diphotonvtxz_vtxz->MakeHists();

    // phi hists
    MakePhiHists("pi0");
    MakePhiHists("eta");
    MakePhiHists("pi0bkgr");
    MakePhiHists("etabkgr");
    MakePhiHists("pi0_lowEta");
    MakePhiHists("eta_lowEta");
    MakePhiHists("pi0bkgr_lowEta");
    MakePhiHists("etabkgr_lowEta");
    MakePhiHists("pi0_highEta");
    MakePhiHists("eta_highEta");
    MakePhiHists("pi0bkgr_highEta");
    MakePhiHists("etabkgr_highEta");
}

void TSSAhistmaker::FillPhiHists(std::string which, int index)
{
    PhiHists* hists = nullptr;
    PhiHists* hists_lowEta = nullptr;
    PhiHists* hists_highEta = nullptr;

    if (which == "pi0") {
	hists = pi0Hists;
	hists_lowEta = pi0Hists_lowEta;
	hists_highEta = pi0Hists_highEta;
    }
    if (which == "eta") {
	hists = etaHists;
	hists_lowEta = etaHists_lowEta;
	hists_highEta = etaHists_highEta;
    }
    if (which == "pi0bkgr") {
	hists = pi0BkgrHists;
	hists_lowEta = pi0BkgrHists_lowEta;
	hists_highEta = pi0BkgrHists_highEta;
    }
    if (which == "etabkgr") {
	hists = etaBkgrHists;
	hists_lowEta = etaBkgrHists_lowEta;
	hists_highEta = etaBkgrHists_highEta;
    }

    if (!hists || !hists_lowEta || !hists_highEta) {
	std::cout << "In FillPhiHists: Invalid argument" << which << " !" << std::endl;
	return;
    }

    float eta = diphoton_Eta->at(index);
    float phi = diphoton_Phi->at(index);
    float pT = diphoton_pT->at(index);
    float xF = diphoton_xF->at(index);
    // phi and xF need beam-dependent considerations
    // rotate phi away from global coordinates, into PHENIX coordinate system
    float phiblue = 999.9;
    float phiyellow = 999.9;
    phiblue = phi - PI/2.0;
    if (phiblue < -PI) phiblue += 2.0*PI;
    phiyellow = phi + PI/2.0;
    if (phiyellow > PI) phiyellow -= 2.0*PI;
    /* // xF is defined such that forward is to the north. For yellow beam, */
    /* // invert this such that xF > 0 <--> forward direction */
    float xFblue = xF;
    float xFyellow = -1.0*xF;
    /* float xFyellow = xF; */

    hists->phi_pT->FillHists(pT, phi);
    hists->phi_xF->FillHists(xF, phi);
    hists->phi_eta->FillHists(eta, phi);
    hists->phi_vtxz->FillHists(vtxz, phi);
    if (bspin == 1) {
	hists->phi_pT_blue_up->FillHists(pT, phiblue);
	if (eta > 0) hists->phi_pT_blue_up_forward->FillHists(pT, phiblue);
	if (eta < 0) hists->phi_pT_blue_up_backward->FillHists(pT, phiblue);
	hists->phi_xF_blue_up->FillHists(xFblue, phiblue);
	hists->phi_eta_blue_up->FillHists(eta, phiblue);
	hists->phi_vtxz_blue_up->FillHists(vtxz, phiblue);
	if (abs(eta) < 0.35)
	{
	    hists_lowEta->phi_pT_blue_up->FillHists(pT, phiblue);
	    if (eta > 0) hists_lowEta->phi_pT_blue_up_forward->FillHists(pT, phiblue);
	    if (eta < 0) hists_lowEta->phi_pT_blue_up_backward->FillHists(pT, phiblue);
	    hists_lowEta->phi_xF_blue_up->FillHists(xFblue, phiblue);
	    hists_lowEta->phi_eta_blue_up->FillHists(eta, phiblue);
	    hists_lowEta->phi_vtxz_blue_up->FillHists(vtxz, phiblue);
	}
	if (abs(eta) > 0.35)
	{
	    hists_highEta->phi_pT_blue_up->FillHists(pT, phiblue);
	    if (eta > 0) hists_highEta->phi_pT_blue_up_forward->FillHists(pT, phiblue);
	    if (eta < 0) hists_highEta->phi_pT_blue_up_backward->FillHists(pT, phiblue);
	    hists_highEta->phi_xF_blue_up->FillHists(xFblue, phiblue);
	    hists_highEta->phi_eta_blue_up->FillHists(eta, phiblue);
	    hists_highEta->phi_vtxz_blue_up->FillHists(vtxz, phiblue);
	}
    }
    if (bspin == -1) {
	hists->phi_pT_blue_down->FillHists(pT, phiblue);
	if (eta > 0) hists->phi_pT_blue_down_forward->FillHists(pT, phiblue);
	if (eta < 0) hists->phi_pT_blue_down_backward->FillHists(pT, phiblue);
	hists->phi_xF_blue_down->FillHists(xFblue, phiblue);
	hists->phi_eta_blue_down->FillHists(eta, phiblue);
	hists->phi_vtxz_blue_down->FillHists(vtxz, phiblue);
	if (abs(eta) < 0.35)
	{
	    hists_lowEta->phi_pT_blue_down->FillHists(pT, phiblue);
	    if (eta > 0) hists_lowEta->phi_pT_blue_down_forward->FillHists(pT, phiblue);
	    if (eta < 0) hists_lowEta->phi_pT_blue_down_backward->FillHists(pT, phiblue);
	    hists_lowEta->phi_xF_blue_down->FillHists(xFblue, phiblue);
	    hists_lowEta->phi_eta_blue_down->FillHists(eta, phiblue);
	    hists_lowEta->phi_vtxz_blue_down->FillHists(vtxz, phiblue);
	}
	if (abs(eta) > 0.35)
	{
	    hists_highEta->phi_pT_blue_down->FillHists(pT, phiblue);
	    if (eta > 0) hists_highEta->phi_pT_blue_down_forward->FillHists(pT, phiblue);
	    if (eta < 0) hists_highEta->phi_pT_blue_down_backward->FillHists(pT, phiblue);
	    hists_highEta->phi_xF_blue_down->FillHists(xFblue, phiblue);
	    hists_highEta->phi_eta_blue_down->FillHists(eta, phiblue);
	    hists_highEta->phi_vtxz_blue_down->FillHists(vtxz, phiblue);
	}
    }
    if (yspin == 1) {
	hists->phi_pT_yellow_up->FillHists(pT, phiyellow);
	if (eta < 0) hists->phi_pT_yellow_up_forward->FillHists(pT, phiyellow);
	if (eta > 0) hists->phi_pT_yellow_up_backward->FillHists(pT, phiyellow);
	hists->phi_xF_yellow_up->FillHists(xFyellow, phiyellow);
	hists->phi_eta_yellow_up->FillHists(eta, phiyellow);
	hists->phi_vtxz_yellow_up->FillHists(vtxz, phiyellow);
	if (abs(eta) < 0.35)
	{
	    hists_lowEta->phi_pT_yellow_up->FillHists(pT, phiyellow);
	    if (eta < 0) hists_lowEta->phi_pT_yellow_up_forward->FillHists(pT, phiyellow);
	    if (eta > 0) hists_lowEta->phi_pT_yellow_up_backward->FillHists(pT, phiyellow);
	    hists_lowEta->phi_xF_yellow_up->FillHists(xFyellow, phiyellow);
	    hists_lowEta->phi_eta_yellow_up->FillHists(eta, phiyellow);
	    hists_lowEta->phi_vtxz_yellow_up->FillHists(vtxz, phiyellow);
	}
	if (abs(eta) > 0.35)
	{
	    hists_highEta->phi_pT_yellow_up->FillHists(pT, phiyellow);
	    if (eta < 0) hists_highEta->phi_pT_yellow_up_forward->FillHists(pT, phiyellow);
	    if (eta > 0) hists_highEta->phi_pT_yellow_up_backward->FillHists(pT, phiyellow);
	    hists_highEta->phi_xF_yellow_up->FillHists(xFyellow, phiyellow);
	    hists_highEta->phi_eta_yellow_up->FillHists(eta, phiyellow);
	    hists_highEta->phi_vtxz_yellow_up->FillHists(vtxz, phiyellow);
	}
    }
    if (yspin == -1) {
	hists->phi_pT_yellow_down->FillHists(pT, phiyellow);
	if (eta < 0) hists->phi_pT_yellow_down_forward->FillHists(pT, phiyellow);
	if (eta > 0) hists->phi_pT_yellow_down_backward->FillHists(pT, phiyellow);
	hists->phi_xF_yellow_down->FillHists(xFyellow, phiyellow);
	hists->phi_eta_yellow_down->FillHists(eta, phiyellow);
	hists->phi_vtxz_yellow_down->FillHists(vtxz, phiyellow);
	if (abs(eta) < 0.35)
	{
	    hists_lowEta->phi_pT_yellow_down->FillHists(pT, phiyellow);
	    if (eta < 0) hists_lowEta->phi_pT_yellow_down_forward->FillHists(pT, phiyellow);
	    if (eta > 0) hists_lowEta->phi_pT_yellow_down_backward->FillHists(pT, phiyellow);
	    hists_lowEta->phi_xF_yellow_down->FillHists(xFyellow, phiyellow);
	    hists_lowEta->phi_eta_yellow_down->FillHists(eta, phiyellow);
	    hists_lowEta->phi_vtxz_yellow_down->FillHists(vtxz, phiyellow);
	}
	if (abs(eta) > 0.35)
	{
	    hists_highEta->phi_pT_yellow_down->FillHists(pT, phiyellow);
	    if (eta < 0) hists_highEta->phi_pT_yellow_down_forward->FillHists(pT, phiyellow);
	    if (eta > 0) hists_highEta->phi_pT_yellow_down_backward->FillHists(pT, phiyellow);
	    hists_highEta->phi_xF_yellow_down->FillHists(xFyellow, phiyellow);
	    hists_highEta->phi_eta_yellow_down->FillHists(eta, phiyellow);
	    hists_highEta->phi_vtxz_yellow_down->FillHists(vtxz, phiyellow);
	}
    }
}

/* void TSSAhistmaker::FillAllPhiHists(int index) */
/* { */
/*     FillPhiHists("pi0", i); */
/*     FillPhiHists("eta", i); */
/*     FillPhiHists("pi0bkgr", i); */
/*     FillPhiHists("etabkgr", i); */
/* } */

/* void TSSAhistmaker::LoopClusters() */
/* { */
/* } */

/* void TSSAhistmaker::LoopDiphotons() */
/* { */
/* } */

void TSSAhistmaker::LoopTrees()
{
    int nevents_event = tree_event->GetEntries();
    int nevents_clusters = tree_clusters->GetEntries();
    int nevents_diphotons = tree_diphotons->GetEntries();
    if (nevents_clusters != nevents_diphotons) {
	std::cout << "Found different number of events in Cluster and Diphoton! Exiting!" << std::endl;
	exit(1);
    }
    if (nevents_event != nevents_diphotons) {
	std::cout << "Found different number of events in Event and Diphoton! Exiting!" << std::endl;
	exit(1);
    }
    int nevents = nevents_clusters;
    std::cout << "Found " << nevents << " total events" << std::endl;
    for (int i=0; i<nevents; i++) {
	if (i%10000 == 0) std::cout << "Event " << i << std::endl;
	h_Nevents->Fill(0);
	tree_event->GetEntry(i);
	tree_clusters->GetEntry(i);

	int nclusters = cluster_E->size();
	h_vtxz->Fill(vtxz);
	/* h_nAllClusters->Fill(nAllClusters); */
	/* h_nGoodClusters->Fill(nclusters); */
	if (minbiastrig_scaled) h_nClustersMinbiasTrig->Fill(nclusters);
	if (photontrig_scaled) h_nClustersPhotonTrig->Fill(nclusters);

	// Armenteros-Podolanski plot
	if (nclusters < 2) continue;
	for (int j=0; j<(nclusters-1); j++) {
	    for (int k=(j+1); k<nclusters; k++) {
		double E1 = cluster_E->at(j);
		double E2 = cluster_E->at(k);
		double asymE = (E1 - E2)/(E1 + E2);
		double eta1 = cluster_Eta->at(j);
		double eta2 = cluster_Eta->at(k);
		double phi1 = cluster_Phi->at(j);
		double phi2 = cluster_Phi->at(k);
		double pT1 = cluster_pT->at(j);
		double pT2 = cluster_pT->at(k);
		TLorentzVector v1, v2;
		v1.SetPtEtaPhiE(pT1, eta1, phi1, E1);
		v2.SetPtEtaPhiE(pT2, eta2, phi2, E2);
		TLorentzVector diphoton = v1 + v2;
		double mass = diphoton.M();
		double pT = diphoton.Pt();
		if (pT < 1.0) continue;
		double deltaR = v1.DeltaR(v2);
		TVector3 v1_3d = v1.Vect();
		TVector3 v2_3d = v2.Vect();
		TVector3 d_3d = diphoton.Vect();
		double p_T1 = v1_3d.Pt(d_3d);
		double p_T2 = v2_3d.Pt(d_3d);
		double p_L1 = TMath::Sqrt(v1_3d.Mag2() - p_T1*p_T1);
		double p_L2 = TMath::Sqrt(v2_3d.Mag2() - p_T2*p_T2);
		double alpha = (p_L1 - p_L2)/(p_L1 + p_L2);
		h_armen_all->Fill(alpha, p_T1);
		if (InRange(mass, pi0MassRange)) h_armen_pi0->Fill(alpha, p_T1);
		if (InRange(mass, pi0BkgrLowRange) || InRange(mass, pi0BkgrHighRange)) h_armen_pi0bkgr->Fill(alpha, p_T1);
		if (InRange(mass, etaMassRange)) {
		    h_armen_eta->Fill(alpha, p_T1);
		    if (pT > 7.0) h_armen_eta_highpT->Fill(alpha, p_T1);
		}
		if (InRange(mass, etaBkgrLowRange) || InRange(mass, etaBkgrHighRange)) {
		    h_armen_etabkgr->Fill(alpha, p_T1);
		    if (pT > 7.0) h_armen_etabkgr_highpT->Fill(alpha, p_T1);
		}
		h_armen_p_L->Fill(p_L1);
		h_armen_p_L->Fill(p_L2);
		h_armen_p_T->Fill(p_T1);
		h_armen_alpha_alphaE->Fill(alpha, asymE);
		h_armen_alpha_deltaR->Fill(alpha, deltaR);
		if (InRange(mass, pi0MassRange)) h_armen_alpha_deltaR_pi0->Fill(alpha, deltaR);
		if (InRange(mass, etaMassRange)) h_armen_alpha_deltaR_eta->Fill(alpha, deltaR);
		h_armen_p_T_deltaR->Fill(p_T1, deltaR);
		h_armen_pT_p_L->Fill(pT, p_L1);
		h_armen_pT_p_L->Fill(pT, p_L2);
		h_armen_pT_p_T->Fill(pT, p_T1);
	    }
	}


	tree_diphotons->GetEntry(i);
	int ndiphotons = diphoton_E->size();
	if (ndiphotons > 0) {
	    for (int j=0; j<ndiphotons; j++) {
		// additional diphoton cuts here
		float mass = diphoton_M->at(j);
		float pT = diphoton_pT->at(j);
		float asym = diphoton_asym->at(j);
		float deltaR = diphoton_deltaR->at(j);
		if (pT < 1.0) continue;
		if (asym > 0.7) continue;
		float xF = diphoton_xF->at(j);
		float eta = diphoton_Eta->at(j);
		/* if (pT > 4.0 && deltaR > 1.0) continue; */

		/* float pT1 = cluster_pT->at(diphoton_clus1index->at(j)); */
		/* float pT2 = cluster_pT->at(diphoton_clus2index->at(j)); */
		/* if (pT1 > 1.5 && pT2 > 1.5) { */
		    /* h_diphotonMass->Fill(mass); */
		    /* bhs_diphotonMass_pT->FillHists(pT, mass); */
		/* } */
		h_diphotonMass->Fill(mass);
		bhs_diphotonMass_pT_pi0binning->FillHists(pT, mass);
		bhs_diphotonMass_pT_etabinning->FillHists(pT, mass);
		if (InRange(mass, pi0MassRange)) bhs_pi0_pT_pT->FillHists(pT, pT);
		if (InRange(mass, etaMassRange)) bhs_eta_pT_pT->FillHists(pT, pT);
		bhs_diphotonxF_xF->FillHists(xF, xF);
		bhs_diphotoneta_eta->FillHists(eta, eta);
		bhs_diphotonvtxz_vtxz->FillHists(vtxz, vtxz);

		h_DiphotonMassAsym->Fill(mass, asym);
		h_DiphotonMassdeltaR->Fill(mass, deltaR);
		h_DiphotondeltaRAsym->Fill(deltaR, asym);
		if (pT > 7.0) {
		    h_DiphotonMassAsym_highpT->Fill(mass, asym);
		    h_DiphotonMassdeltaR_highpT->Fill(mass, deltaR);
		    h_DiphotondeltaRAsym_highpT->Fill(deltaR, asym);
		    h_DiphotondeltaRAsym_highpT_smalldR->Fill(deltaR, asym);
		}

		if (InRange(mass, pi0MassRange)) {
		    h_Npi0s->Fill(0);
		    FillPhiHists("pi0", j);
		    h_DiphotondeltaRAsym_pi0->Fill(deltaR, asym);
		    h_DiphotondeltaRAsym_pi0_smalldR->Fill(deltaR, asym);
		}
		if (InRange(mass, pi0BkgrLowRange) || InRange(mass, pi0BkgrHighRange)) {
		    FillPhiHists("pi0bkgr", j);
		}
		if (InRange(mass, etaMassRange)) {
		    h_Netas->Fill(0);
		    FillPhiHists("eta", j);
		    h_DiphotondeltaRAsym_eta->Fill(deltaR, asym);
		    if (pT > 7.0) h_DiphotondeltaRAsym_eta_highpT->Fill(deltaR, asym);
		}
		if (InRange(mass, etaBkgrLowRange) || InRange(mass, etaBkgrHighRange)) {
		    FillPhiHists("etabkgr", j);
		    h_DiphotondeltaRAsym_etabkgr->Fill(deltaR, asym);
		    if (pT > 7.0) h_DiphotondeltaRAsym_etabkgr_highpT->Fill(deltaR, asym);
		}
	    }
	}
    }
}

void TSSAhistmaker::Cleanup()
{
    outfile_hists->Write();
    outfile_hists->Close();
    delete outfile_hists;

    /* infile_trees->Close(); */
    /* delete infile_trees; */

    delete pi0Hists; delete etaHists; delete pi0BkgrHists; delete etaBkgrHists;
    delete pi0Hists_lowEta; delete etaHists_lowEta; delete pi0BkgrHists_lowEta; delete etaBkgrHists_lowEta;
    delete pi0Hists_highEta; delete etaHists_highEta; delete pi0BkgrHists_highEta; delete etaBkgrHists_highEta;
}

void TSSAhistmaker::main()
{
    GetTrees();
    MakeAllHists();
    LoopTrees();
    Cleanup();
    std::cout << "All done!" << std::endl;
}
