#define cluster_cxx
#include "cluster.h"
#include "truthPhoton.C"
#include "truthConversion.C"
#include <TH2.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TMath.h>
#include <TFile.h>
#include <iostream>
#include <fstream>
#include "BinnedHistSet.C"

bool isCorrectPhoton(double E_truth, double eta_truth, double phi_truth, double E_reco, double eta_reco, double phi_reco) {
    /* double dE_over_E_cut = 0.3; */
    /* double dEta_cut = 0.10; */
    /* double dPhi_cut = 0.10; */
    double dE_over_E_cut = 0.25;
    double dEta_cut = 0.07;
    double dPhi_cut = 0.07;
    bool correct_E = ( abs(E_truth - E_reco)/E_truth < dE_over_E_cut );
    bool correct_eta = ( abs(eta_truth-eta_reco) < dEta_cut );
    bool correct_phi = ( abs(phi_truth-phi_reco) < dPhi_cut );
    return ( correct_E && correct_eta && correct_phi );
}

/* void fillBinnedHists(std::vector<double> edges, TH1** hists, double binValue, double* fillValue, int nFillValues=1) { */
/*     int nbins = edges.size() - 1; */
/*     // Find which bin the event falls into */
/*     int i; // index of the histogram we will fill */
/*     for (i=0; i<nbins; i++) { */
/* 	double ledge = edges.at(i); */
/* 	if ( (i==0) && (binValue<ledge) ) { */
/* 	    i = -1;  // binValue is below the lowest bin edge; stop with i=-1 */
/* 	    break; */
/* 	} */
/* 	double redge = edges.at(i+1); */
/* 	if ( (binValue>ledge) && (binValue<redge) ) break;  // we have the correct value for i; stop here */
/* 	if ( (i==(nbins-1)) && (binValue>redge) ) { */
/* 	    i = nbins;  // binValue is above the highest bin edge; stop with i=nbins */
/* 	    break; */
/* 	} */
/*     } */
/*     if ( (i==-1) || (i==nbins) ) return;  // binValue was outside the expected range; don't fill anything */
/*     // Fill the corresponding histogram */
/*     switch (nFillValues) { */
/* 	case 1: */
/* 	    hists[i]->Fill(fillValue[0]); */
/* 	    break; */
/* 	case 2: */
/* 	    hists[i]->Fill(fillValue[0], fillValue[1]); */
/* 	    break; */
/* 	case 3: */
/* 	    hists[i]->Fill(fillValue[0], fillValue[1], fillValue[2]); */
/* 	    break; */
/* 	default: */
/* 	    std::cout << "fillBinnedHists(): invalid number of fill values\n"; */
/*     } */
/*     return; */
/* } */

void fillBinnedHists(std::vector<double> edges, TH1** hists, double binValue, double fillValue) {
    /* std::cout << "Entering fillBinnedHists\n"; */
    /* printEdges(edges); */
    int nbins = edges.size() - 1;
    // Find which bin the event falls into
    int i; // index of the histogram we will fill
    for (i=0; i<nbins; i++) {
	double ledge = edges.at(i);
	if ( (i==0) && (binValue<ledge) ) {
	    i = -1;  // binValue is below the lowest bin edge; stop with i=-1
	    break;
	}
	double redge = edges.at(i+1);
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
    hists[i]->Fill(fillValue);
    /* hist_arr[i]->Print(); */
    return;
}

void cluster::Loop()
{
//   In a ROOT session, you can do:
//      root> .L cluster.C
//      root> cluster t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();
    // nentries should be the same for the other trees truthPi0 and truthPhoton

    // Load the other trees
    truthPhoton tpho;
    truthConversion tconv;

    /* // Output to csv for python ML studies */
    /* bool write_csv = false; */
    /* std::ofstream outfile; */
    /* if (write_csv) { */
	/* outfile.open("python/clusters.csv"); */
	/* outfile << "PiE,PiEta,PiPhi,LeadE,LeadEta,LeadPhi,SubE,SubEta,SubPhi,TowerE,TowerEta,TowerPhi\n"; */
    /* } */
    /* float tower_threshold = 0.08; */

    // Histogram booking
    TFile* histfile = new TFile("singlePhoton_hists.root","RECREATE");
    histfile->cd();

    // Need:
    // - Energy, eta & phi resolutions
    // - Efficiency and purity vs energy and |eta|
    // - Cluster chi^2 distribution
    // - chi^2 separately for "real" vs "noise" clusters?

    int nbins_E = 100;
    double Ebins_lower = 1.9;
    double Ebins_upper = 20.1;

    // MC Truth distributions
    TH1D* h_truthE = new TH1D("h_truthE", "MC Truth Photon Energy;E_{#gamma} (GeV);Counts", nbins_E, 2.0, 20.0);
    TH1D* h_truthEta = new TH1D("h_truthEta", "MC Truth Photon #eta;#eta;Counts", nbins_E, -1.15, 1.15);
    TH1D* h_truthPhi = new TH1D("h_truthPhi", "MC Truth Photon #phi;#phi;Counts", nbins_E, -3.15, 3.15);
    TH2D* h_truth_eta_phi = new TH2D("h_truth_eta_phi", "MC Truth Photon Angular Distribution;#eta;#phi", 25, -1.15, 1.15, 25, -3.15, 3.15);
    TH1D* h_vtxr = new TH1D("h_vtxr", "MC Truth Conversion Vertex Radius;Vertex r (cm);Counts", 1.5*nbins_E, 0.0, 120.0);
    TH1D* h_vtxr_zoomed = new TH1D("h_vtxr_zoomed", "Conversion Vertex Radius (In EMCal);Vertex r (cm);Counts", 1.5*nbins_E, 90, 115.0);
    TH2D* h_vtx_xy = new TH2D("h_vtx_xy", "MC Truth Conversion Vertex Position;Vertex x (cm);Vertex y (cm)", nbins_E, -125.0, 125.0, nbins_E, 125.0, 125.0);
    TH2D* h_truthE_vtxr = new TH2D("h_truthE_vtxr", "MC Truth Conversion Vertex Radius vs Photon Energy;E_{#gamma} (GeV);Vertex r (cm)", nbins_E, Ebins_lower, Ebins_upper, nbins_E, 0.0, 110.0);
    TH1D* h_truthE_converted = new TH1D("h_truthE_converted", "Conversion Events -- MC Truth Photon Energy;E_{#gamma} (GeV);Counts", nbins_E, 2.0, 20.0);

    // Efficiency and purity
    TH1D* h_nTruth = new TH1D("h_nTruth", "MC Truth Photons by Energy;E_{#gamma} (GeV);Counts", nbins_E, Ebins_lower, Ebins_upper);
    TH1D* h_nReco = new TH1D("h_nReco", "Clusters by Truth Photon Energy;E_{#gamma} (GeV);Counts", nbins_E, Ebins_lower, Ebins_upper);
    TH1D* h_nCorrect = new TH1D("h_nCorrect", "Correct Clusters by Truth Photon Energy;E_{#gamma} (GeV);Counts", nbins_E, Ebins_lower, Ebins_upper);
    TH1D* h_nIncorrect = new TH1D("h_nIncorrect", "Incorrect Clusters by Truth Photon Energy;E_{#gamma} (GeV);Counts", nbins_E, Ebins_lower, Ebins_upper);

    // Cluster merging
    TH2D* h_nClusters_vsE = new TH2D("h_nClusters_vsE", "Number of Clusters (E>300MeV) vs Energy;E_{#gamma} (GeV) (GeV);# Clusters", 50, Ebins_lower, Ebins_upper, 4, -0.5, 3.5);
    TH1D* h_nClusters = new TH1D("h_nClusters", "Number of Clusters (E>300MeV);# Clusters;Counts", 5, -0.5, 4.5);
    TProfile* h_nClusters_vsE_prof = new TProfile("h_nClusters_vsE_prof", "Number of Clusters (E>300MeV) vs Energy;E_{#gamma} (GeV);# Clusters", nbins_E, Ebins_lower, Ebins_upper);

    // Energy and angular resolutions
    int nbins_res = 100;
    double Eratio_lower = 0.0; double Eratio_upper = 2.0;
    double angle_lower = -0.05; double angle_upper = -1*angle_lower;
    double etabins_lower = -1.15; double etabins_upper = -1*etabins_lower;

    TH1D* h_Eratio = new TH1D("h_Eratio", "Energy Response;Ratio E_{reco}/E_{truth}", nbins_res, Eratio_lower, Eratio_upper);
    TH1D* h_deltaEta = new TH1D("h_deltaEta", "#eta Resolution;#Delta #eta", nbins_res, angle_lower, angle_upper);
    TH1D* h_etaRes = new TH1D("h_etaRes", "Relative #eta Resolution;#frac{#Delta #eta}{#eta}", nbins_res, 4*angle_lower, 4*angle_upper);
    TH1D* h_deltaPhi = new TH1D("h_deltaPhi", "#phi Resolution;#Delta #phi", nbins_res, angle_lower, angle_upper);
    TProfile* h_Eratio_vsE = new TProfile("h_Eratio_vsE", "Energy Response vs Energy;E_{#gamma} (GeV);Ratio E_{reco}/E_{truth}", nbins_E, Ebins_lower, Ebins_upper);
    TProfile* h_deltaEta_vsE = new TProfile("h_deltaEta_vsE", "#Delta #eta vs Energy;E_{#gamma} (GeV);#Delta #eta", nbins_E, Ebins_lower, Ebins_upper);
    TProfile* h_etaRes_vsE = new TProfile("h_etaRes_vsE", "Relative #eta Resolution vs Energy;E_{#gamma} (GeV);#frac{#Delta #eta}{#eta}", nbins_E, Ebins_lower, Ebins_upper);
    TProfile* h_deltaPhi_vsE = new TProfile("h_deltaPhi_vsE", "#Delta #phi vs Energy;E_{#gamma} (GeV);#Delta #phi", nbins_E, Ebins_lower, Ebins_upper);
    TProfile* h_Eratio_vsEta = new TProfile("h_Eratio_vsEta", "Energy Response vs #eta;#eta_{#gamma};Ratio E_{reco}/E_{truth}", nbins_E, etabins_lower, etabins_upper);
    TProfile* h_deltaEta_vsEta = new TProfile("h_deltaEta_vsEta", "#Delta #eta vs #eta;#eta_{#gamma};#Delta #eta", nbins_E, etabins_lower, etabins_upper);
    TProfile* h_etaRes_vsEta = new TProfile("h_etaRes_vsEta", "Relative #eta Resolution vs #eta;#eta_{#gamma};#frac{#Delta #eta}{#eta}", nbins_E, etabins_lower, etabins_upper);
    TProfile* h_deltaPhi_vsEta = new TProfile("h_deltaPhi_vsEta", "#Delta #phi vs #eta;#eta_{#gamma};#Delta #phi", nbins_E, etabins_lower, etabins_upper);


    // Cluster chi^2 and nTowers
    TH1D* h_cluster_nTowers = new TH1D("h_cluster_nTowers", "Number of Towers in Clusters w/ E>300MeV;# Towers;Counts", 25, 0.5, 25.5);
    TProfile* h_cluster_nTowers_vsE = new TProfile("h_cluster_nTowers_vsE", "Average Number of Towers in Clusters w/ E>300MeV;E_{#gamma} (GeV);# Towers",nbins_E, Ebins_lower, Ebins_upper );
    TH1D* h_cluster_chi2 = new TH1D("h_cluster_chi2", "#chi^{2} Distribution of Clusters w/ E>300MeV;#chi^{2};Counts", 50, 0.0, 5.0);
    TH1D* h_cluster_chi2_zoomed = new TH1D("h_cluster_chi2_zoomed", "Low-#chi^{2} Clusters w/ E>300MeV;#chi^{2};Counts", 50, 0.0, 0.7);
    TH2D* h_cluster_chi2_vsE = new TH2D("h_cluster_chi2_vsE", "Cluster #chi^{2} vs Energy;Cluster Energy (GeV);#chi^{2}", 100, 0.3, 25.0, 100, 0.0, 2.0);
    TH1D* h_clusterE = new TH1D("h_clusterE", "Cluster Energy;Energy (GeV)", 50, 0.3, 25.0);
    TH1D* h_cluster_chi2_correct = new TH1D("h_cluster_chi2_correct", "#chi^{2} Distribution of Correct Clusters w/ E>300MeV;#chi^{2};Counts", 50, 0.0, 5.0);
    TH1D* h_cluster_chi2_incorrect = new TH1D("h_cluster_chi2_incorrect", "#chi^{2} Distribution of Incorrect Clusters w/ E>300MeV;#chi^{2};Counts", 50, 0.0, 5.0);

    
    // "Missing" photons -- no correctly reconstructed cluster
    TH1D* h_missing_nClusters = new TH1D("h_missing_nClusters", "\"Missing\" Photons -- Number of Clusters (E>300MeV);# Clusters;Counts", 5, -0.5, 4.5);
    TH1D* h_missing_truthE = new TH1D("h_missing_truthE", "\"Missing\" Photons -- MC Truth Photon Energy;E_{#gamma} (GeV);Counts", nbins_E, 2.0, 20.0);
    TH1D* h_missing_truthEta = new TH1D("h_missing_truthEta", "\"Missing\" Photons -- MC Truth Photon #eta;#eta;Counts", nbins_E, -1.15, 1.15);
    TH1D* h_missing_truthPhi = new TH1D("h_missing_truthPhi", "\"Missing\" Photons -- MC Truth Photon #phi;#phi;Counts", nbins_E, -3.15, 3.15);
    TH1D* h_missing_Eratio = new TH1D("h_missing_Eratio", "\"Missing\" Photons -- Energy Response;Ratio E_{reco}/E_{truth}", nbins_res, Eratio_lower, Eratio_upper);
    TH1D* h_missing_deltaEta = new TH1D("h_missing_deltaEta", "\"Missing\" Photons -- #Delta #eta;#Delta #eta", nbins_res, 1.5*angle_lower, 1.5*angle_upper);
    TH1D* h_missing_deltaPhi = new TH1D("h_missing_deltaPhi", "\"Missing\" Photons -- #Delta #phi;#Delta #phi", nbins_res, 1.5*angle_lower, 1.5*angle_upper);
    TH1D* h_missing_vtxr = new TH1D("h_missing_vtxr", "\"Missing\" Photons -- Conversion Vertex Radius;r (cm)", nbins_E, 0.0, 110.0);

    // Energy response binned in E
    const int nEbins = 6;
    std::vector<double> EbinEdges = getUniformBinEdges(nEbins, h_truthE);
    /* std::cout << "Printing current directory and contents\nPath: " << gDirectory->GetPath() << "\n"; */
    /* gDirectory->ls(); */
    BinnedHistSet bhs_Eratio_vsE("Eratio_vsE", "Energy Response;Ratio E_{reco}/E_{truth};Counts", nbins_res, Eratio_lower, Eratio_upper, "E_{#gamma}(GeV)", EbinEdges);
    bhs_Eratio_vsE.MakeHists();
    BinnedHistSet bhs_deltaEta_vsE("deltaEta_vsE", "#Delta #eta;#Delta #eta;Counts", nbins_res, angle_lower, angle_upper, "E_{#gamma}(GeV)", EbinEdges);
    bhs_deltaEta_vsE.MakeHists();
    BinnedHistSet bhs_deltaPhi_vsE("deltaPhi_vsE", "#Delta #phi;#Delta #phi;Counts", nbins_res, angle_lower, angle_upper, "E_{#gamma}(GeV)", EbinEdges);
    bhs_deltaPhi_vsE.MakeHists();

    std::vector<double> etaBinEdges = getUniformBinEdges(nEbins*2, h_truthEta);
    BinnedHistSet bhs_Eratio_vsEta("Eratio_vsEta", "Energy Response;Ratio E_{reco}/E_{truth};Counts", nbins_res, Eratio_lower, Eratio_upper, "#eta_{#gamma}", etaBinEdges);
    bhs_Eratio_vsEta.MakeHists();
    BinnedHistSet bhs_deltaEta_vsEta("deltaEta_vsEta", "#Delta #eta;#Delta #eta;Counts", nbins_res, angle_lower, angle_upper, "#eta_{#gamma}", etaBinEdges);
    bhs_deltaEta_vsEta.MakeHists();
    BinnedHistSet bhs_deltaPhi_vsEta("deltaPhi_vsEta", "#Delta #phi;#Delta #phi;Counts", nbins_res, angle_lower, angle_upper, "#eta_{#gamma}", etaBinEdges);
    bhs_deltaPhi_vsEta.MakeHists();

    std::vector<double> phiBinEdges = getUniformBinEdges(nEbins*2, h_truthPhi);
    BinnedHistSet bhs_Eratio_vsPhi("Eratio_vsPhi", "Energy Response;Ratio E_{reco}/E_{truth};Counts", nbins_res, Eratio_lower, Eratio_upper, "#phi_{#gamma} (rad)", phiBinEdges);
    bhs_Eratio_vsPhi.MakeHists();
    BinnedHistSet bhs_deltaEta_vsPhi("deltaEta_vsPhi", "#Delta #phi;#Delta #phi;Counts", nbins_res, angle_lower, angle_upper, "#phi_{#gamma} (rad)", phiBinEdges);
    bhs_deltaEta_vsPhi.MakeHists();
    BinnedHistSet bhs_deltaPhi_vsPhi("deltaPhi_vsPhi", "#Delta #phi;#Delta #phi;Counts", nbins_res, angle_lower, angle_upper, "#phi_{#gamma} (rad)", phiBinEdges);
    bhs_deltaPhi_vsPhi.MakeHists();
    /* std::cout << "bhs_Eratio_vsE.hist_arr[0] = " << bhs_Eratio_vsE.hist_arr[0] << "\n"; */
    /* bhs_Eratio_vsE.hist_arr[0]->Print(); */

    /* // For checking effect of different chi^2 cuts */
    /* int n_chi2_cuts = 7; */
    /* float chi2_cuts[7] = {2.7, 2.8, 2.9, 3.0, 3.1, 3.2, 3.3}; */
    /* /1* float chi2_cuts[9] = {0.1, 2.7, 2.8, 2.9, 3.0, 3.1, 3.2, 3.3, 100}; *1/ */
    /* TH1D* h_eff_nEvents = new TH1D("h_eff_nEvents", "Total Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_SingleCluster_1 = new TH1D("h_eff_nAccepted_SingleCluster_1", "Accepted Single-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_MultiCluster_1= new TH1D("h_eff_nAccepted_MultiCluster_1", "Accepted Multi-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_Total_1 = new TH1D("h_eff_nAccepted_Total_1", "Accepted Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_SingleCluster_2 = new TH1D("h_eff_nAccepted_SingleCluster_2", "Accepted Single-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_MultiCluster_2= new TH1D("h_eff_nAccepted_MultiCluster_2", "Accepted Multi-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_Total_2 = new TH1D("h_eff_nAccepted_Total_2", "Accepted Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_SingleCluster_3 = new TH1D("h_eff_nAccepted_SingleCluster_3", "Accepted Single-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_MultiCluster_3= new TH1D("h_eff_nAccepted_MultiCluster_3", "Accepted Multi-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_Total_3 = new TH1D("h_eff_nAccepted_Total_3", "Accepted Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_SingleCluster_4 = new TH1D("h_eff_nAccepted_SingleCluster_4", "Accepted Single-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_MultiCluster_4= new TH1D("h_eff_nAccepted_MultiCluster_4", "Accepted Multi-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_Total_4 = new TH1D("h_eff_nAccepted_Total_4", "Accepted Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_SingleCluster_5 = new TH1D("h_eff_nAccepted_SingleCluster_5", "Accepted Single-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_MultiCluster_5= new TH1D("h_eff_nAccepted_MultiCluster_5", "Accepted Multi-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_Total_5 = new TH1D("h_eff_nAccepted_Total_5", "Accepted Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_SingleCluster_6 = new TH1D("h_eff_nAccepted_SingleCluster_6", "Accepted Single-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_MultiCluster_6= new TH1D("h_eff_nAccepted_MultiCluster_6", "Accepted Multi-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_Total_6 = new TH1D("h_eff_nAccepted_Total_6", "Accepted Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_SingleCluster_7 = new TH1D("h_eff_nAccepted_SingleCluster_7", "Accepted Single-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_MultiCluster_7= new TH1D("h_eff_nAccepted_MultiCluster_7", "Accepted Multi-Cluster Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_nAccepted_Total_7 = new TH1D("h_eff_nAccepted_Total_7", "Accepted Events by Pion Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_eff_Single_arr[] = {h_eff_nAccepted_SingleCluster_1, h_eff_nAccepted_SingleCluster_2, h_eff_nAccepted_SingleCluster_3, h_eff_nAccepted_SingleCluster_4, h_eff_nAccepted_SingleCluster_5, h_eff_nAccepted_SingleCluster_6, h_eff_nAccepted_SingleCluster_7}; */
    /* TH1D* h_eff_Multi_arr[] = {h_eff_nAccepted_MultiCluster_1, h_eff_nAccepted_MultiCluster_2, h_eff_nAccepted_MultiCluster_3, h_eff_nAccepted_MultiCluster_4, h_eff_nAccepted_MultiCluster_5, h_eff_nAccepted_MultiCluster_6, h_eff_nAccepted_MultiCluster_7}; */
    /* TH1D* h_eff_Total_arr[] = {h_eff_nAccepted_Total_1, h_eff_nAccepted_Total_2, h_eff_nAccepted_Total_3, h_eff_nAccepted_Total_4, h_eff_nAccepted_Total_5, h_eff_nAccepted_Total_6, h_eff_nAccepted_Total_7}; */
    /* TH1D* h_SingleCluster_dR_1 = new TH1D("h_SingleCluster_dR_1", "Photon #Delta R for Merged Cluster Events", 50, 0.0, 0.2); */
    /* TH1D* h_SingleCluster_dR_2 = new TH1D("h_SingleCluster_dR_2", "Photon #Delta R for Merged Cluster Events", 50, 0.0, 0.2); */
    /* TH1D* h_SingleCluster_dR_3 = new TH1D("h_SingleCluster_dR_3", "Photon #Delta R for Merged Cluster Events", 50, 0.0, 0.2); */
    /* TH1D* h_SingleCluster_dR_4 = new TH1D("h_SingleCluster_dR_4", "Photon #Delta R for Merged Cluster Events", 50, 0.0, 0.2); */
    /* TH1D* h_SingleCluster_dR_5 = new TH1D("h_SingleCluster_dR_5", "Photon #Delta R for Merged Cluster Events", 50, 0.0, 0.2); */
    /* TH1D* h_SingleCluster_dR_6 = new TH1D("h_SingleCluster_dR_6", "Photon #Delta R for Merged Cluster Events", 50, 0.0, 0.2); */
    /* TH1D* h_SingleCluster_dR_7 = new TH1D("h_SingleCluster_dR_7", "Photon #Delta R for Merged Cluster Events", 50, 0.0, 0.2); */
    /* TH1D* h_SingleCluster_dR_arr[] = {h_SingleCluster_dR_1, h_SingleCluster_dR_2, h_SingleCluster_dR_3, h_SingleCluster_dR_4, h_SingleCluster_dR_5, h_SingleCluster_dR_6, h_SingleCluster_dR_7}; */
    /* TH1D* h_purity_1 = new TH1D("h_purity_1", "Correctly Tagged Pions by Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_purity_2 = new TH1D("h_purity_2", "Correctly Tagged Pions by Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_purity_3 = new TH1D("h_purity_3", "Correctly Tagged Pions by Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_purity_4 = new TH1D("h_purity_4", "Correctly Tagged Pions by Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_purity_5 = new TH1D("h_purity_5", "Correctly Tagged Pions by Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_purity_6 = new TH1D("h_purity_6", "Correctly Tagged Pions by Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_purity_7 = new TH1D("h_purity_7", "Correctly Tagged Pions by Energy;E_{#pi^{0}} (GeV);Counts", 40, 0.0, 21.0); */
    /* TH1D* h_purity_arr[] = {h_purity_1, h_purity_2, h_purity_3, h_purity_4, h_purity_5, h_purity_6, h_purity_7}; */


    Long64_t nbytes = 0, nb = 0;
    /* nentries = 5000; */
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
	Long64_t ientry = LoadTree(jentry);
	if (ientry < 0) break;
	nb = fChain->GetEntry(jentry);   nbytes += nb;
	// if (Cut(ientry) < 0) continue;
	// Load the other trees
	tpho.GetEntry(jentry);
	tconv.GetEntry(jentry);

	if (jentry%10000 == 0) std::cout << "Event " << jentry << "\n";

	// Truth kinematics
	float truth_photonE = tpho.photonE->at(0);
	float truth_photonEta = tpho.photonEta->at(0);
	if (abs(truth_photonEta) > 1.1) continue;
	float truth_photonPhi = tpho.photonPhi->at(0);
	float truth_vtx_r = tconv.vtx_r->at(0);
	float truth_vtx_x = tconv.vtx_x->at(0);
	float truth_vtx_y = tconv.vtx_y->at(0);

	h_nTruth->Fill(truth_photonE);
	h_truthE->Fill(truth_photonE);
	h_truthEta->Fill(truth_photonEta);
	h_truthPhi->Fill(truth_photonPhi);
	h_truth_eta_phi->Fill(truth_photonEta, truth_photonPhi);
	h_vtxr->Fill(truth_vtx_r);
	if (truth_vtx_r > 85.0) h_vtxr_zoomed->Fill(truth_vtx_r);
	h_vtx_xy->Fill(truth_vtx_x, truth_vtx_y);
	h_truthE_vtxr->Fill(truth_photonE, truth_vtx_r);
	if (tconv.isConversionEvent->at(0)) h_truthE_converted->Fill(truth_photonE);

	/* if (write_csv) { */
	/*     outfile << Form("%f,%f,%f,%f,%f,%f,%f,%f,%f,", */
	/* 	    truth_piE, truth_piEta, truth_piPhi, */
	/* 	    truth_leadE, truth_leadEta, truth_leadPhi, */
	/* 	    truth_subE, truth_subEta, truth_subPhi); */
	/* } */

	/* // Tower info */
	/* int ntowers = towerEnergy->size(); */
	/* // To write to csv, we need to loop over each tower vector sequentially */
	/* for (int i=0; i<ntowers; i++) { */
	/*     if (write_csv && (towerEnergy->at(i) > tower_threshold)) outfile << towerEnergy->at(i) << " "; // each tower separated by a space */
	/* } */
	/* if (write_csv) outfile << ","; */
	/* for (int i=0; i<ntowers; i++) { */
	/*     if (write_csv && (towerEnergy->at(i) > tower_threshold)) outfile << towerEtaCEnter->at(i) << " "; */
	/* } */
	/* if (write_csv) outfile << ","; */
	/* for (int i=0; i<ntowers; i++) { */
	/*     if (write_csv && (towerEnergy->at(i) > tower_threshold)) outfile << towerPhiCenter->at(i) << " "; */
	/* } */
	/* if (write_csv) outfile << "\n"; // end of the tower data */

	// Cluster info
	int nclusters = clusterE->size();

	float minEcut = 0.3; // eliminate fake clusters from noisy towers
	std::vector<float> real_clusters_idx;
	for (int i=0; i<nclusters; i++) {
	    if (clusterE->at(i) >= minEcut) {
		real_clusters_idx.push_back(i);
		// Fill # towers info
		h_cluster_nTowers->Fill(clusterNTowers->at(i));
		h_cluster_nTowers_vsE->Fill(truth_photonE, clusterNTowers->at(i));
		// Fill chi^2 info
	    	h_cluster_chi2->Fill(clusterChi2->at(i));
	    	h_cluster_chi2_zoomed->Fill(clusterChi2->at(i));
	    	h_cluster_chi2_vsE->Fill(clusterE->at(i), clusterChi2->at(i));
		h_clusterE->Fill(clusterE->at(i));
	    }
	}
	int nRealClusters = real_clusters_idx.size();
	h_nClusters_vsE->Fill(truth_photonE, nRealClusters);
	h_nClusters_vsE_prof->Fill(truth_photonE, nRealClusters);
	h_nClusters->Fill(nRealClusters);

	/* // Check cluster chi^2 for efficiency calculation */
	/* // Need either a) One merged cluster with chi^2 > cutoff, or */
	/* // b) At least two clusters with chi^2 < cutoff */
	/* for (int chi2_cut_idx=0; chi2_cut_idx < n_chi2_cuts; chi2_cut_idx++) { */
	/*     float cutoff = chi2_cuts[chi2_cut_idx]; */
	/*     bool has_merged_cluster = false; */
	/*     bool has_multiple_single_clusters = false; */
	/*     int n_single_clusters = 0; */
	/*     for (int i=0; i<nRealClusters; i++) { */
	/* 	int cluster_idx = real_clusters_idx.at(i); */
	/* 	if (clusterChi2->at(cluster_idx) > cutoff) { */
	/* 	    /1* if (has_merged_cluster == true) std::cout << "Info: multiple merged clusters\n"; *1/ */
	/* 	    has_merged_cluster = true; */
	/* 	    h_eff_Single_arr[chi2_cut_idx]->Fill(truth_piE); */
	/* 	    h_SingleCluster_dR_arr[chi2_cut_idx]->Fill(truth_deltaR); */
	/* 	    // Is it the true pi0 cluster? Check delta R */
	/* 	    float reco_piEta = clusterEta->at(cluster_idx); */
	/* 	    float reco_piPhi = clusterPhi->at(cluster_idx); */
	/* 	    float dR2 = (reco_piEta - truth_piEta)*(reco_piEta - truth_piEta); */
	/* 	    dR2 += (reco_piPhi - truth_piPhi)*(reco_piPhi - truth_piPhi); */
	/* 	    float dR2_cut = 0.05*0.05; */
	/* 	    if (dR2 < dR2_cut) h_purity_arr[chi2_cut_idx]->Fill(truth_piE); */
	/* 	} */
	/* 	else { */
	/* 	    n_single_clusters++; */
	/* 	} */
	/*     } */
	/*     if (n_single_clusters >= 2) { */
	/* 	has_multiple_single_clusters = true; */
	/*     } */

	/*     if (has_multiple_single_clusters) h_eff_Multi_arr[chi2_cut_idx]->Fill(truth_piE); */
	/*     if (has_merged_cluster || has_multiple_single_clusters) h_eff_Total_arr[chi2_cut_idx]->Fill(truth_piE); */

	/* } // loop over chi^2 cuts */

	// Now loop over pairs of real clusters and reconstruct photon kinematics
	if (nRealClusters < 1) { // we need at least one cluster!
	    /* std::cout << "Info: event " << jentry << " has no clusters over threshold. Skipping...\n"; */
	    continue;
	}

	
	float reco_photonE, reco_photonEta, reco_photonPhi;
	bool found_correct_photon = false;

	// Loop over clusters
	for (int i=0; i<nRealClusters; i++) {
	    int cluster_idx = real_clusters_idx.at(i);
	    h_nReco->Fill(truth_photonE);
	    reco_photonE = clusterE->at(cluster_idx);
	    reco_photonEta = clusterEta->at(cluster_idx);
	    reco_photonPhi = clusterPhi->at(cluster_idx);
	    if (isCorrectPhoton(truth_photonE, truth_photonEta, truth_photonPhi, reco_photonE, reco_photonEta, reco_photonPhi)) {
		h_nCorrect->Fill(truth_photonE);
		h_cluster_chi2_correct->Fill(clusterChi2->at(cluster_idx));
		found_correct_photon = true;
	    }
	    else {
		h_nIncorrect->Fill(truth_photonE);
		h_cluster_chi2_incorrect->Fill(clusterChi2->at(cluster_idx));
	    }
	    h_Eratio->Fill(reco_photonE/truth_photonE);
	    h_deltaEta->Fill(reco_photonEta - truth_photonEta);
	    h_etaRes->Fill( (reco_photonEta - truth_photonEta)/truth_photonEta );
	    h_deltaPhi->Fill(reco_photonPhi - truth_photonPhi);
	    h_Eratio_vsE->Fill(truth_photonE, reco_photonE/truth_photonE);
	    h_deltaEta_vsE->Fill(truth_photonE, reco_photonEta - truth_photonEta);
	    h_etaRes_vsE->Fill(truth_photonE, (reco_photonEta - truth_photonEta)/truth_photonEta);
	    h_deltaPhi_vsE->Fill(truth_photonE, reco_photonPhi - truth_photonPhi);
	    h_Eratio_vsEta->Fill(truth_photonEta, reco_photonE/truth_photonE);
	    h_deltaEta_vsEta->Fill(truth_photonEta, reco_photonEta - truth_photonEta);
	    h_etaRes_vsEta->Fill(truth_photonEta, (reco_photonEta - truth_photonEta)/truth_photonEta);
	    h_deltaPhi_vsEta->Fill(truth_photonEta, reco_photonPhi - truth_photonPhi);
	    bhs_Eratio_vsE.FillHists(truth_photonE, (reco_photonE/truth_photonE));
	    bhs_deltaEta_vsE.FillHists(truth_photonE, reco_photonEta - truth_photonEta);
	    bhs_deltaPhi_vsE.FillHists(truth_photonE, reco_photonPhi - truth_photonPhi);
	    bhs_Eratio_vsEta.FillHists(truth_photonEta, (reco_photonE/truth_photonE));
	    bhs_deltaEta_vsEta.FillHists(truth_photonEta, reco_photonEta - truth_photonEta);
	    bhs_deltaPhi_vsEta.FillHists(truth_photonEta, reco_photonPhi - truth_photonPhi);
	    bhs_Eratio_vsPhi.FillHists(truth_photonPhi, (reco_photonE/truth_photonE));
	    bhs_deltaEta_vsPhi.FillHists(truth_photonPhi, reco_photonEta - truth_photonEta);
	    bhs_deltaPhi_vsPhi.FillHists(truth_photonPhi, reco_photonPhi - truth_photonPhi);
	}

	if (! found_correct_photon) {
	    h_missing_nClusters->Fill(nRealClusters);
	    h_missing_truthE->Fill(truth_photonE);
	    h_missing_truthEta->Fill(truth_photonEta);
	    h_missing_truthPhi->Fill(truth_photonPhi);
	    for (int i=0; i<nRealClusters; i++) {
		int cluster_idx = real_clusters_idx.at(i);
		reco_photonE = clusterE->at(cluster_idx);
		reco_photonEta = clusterEta->at(cluster_idx);
		reco_photonPhi = clusterPhi->at(cluster_idx);
		h_missing_Eratio->Fill(reco_photonE/truth_photonE);
		h_missing_deltaEta->Fill(reco_photonEta - truth_photonEta);
		h_missing_deltaPhi->Fill(reco_photonPhi - truth_photonPhi);
	    }
	    h_missing_vtxr->Fill(truth_vtx_r);
	}

    } // end loop over events

    /* TCanvas* c1 = new TCanvas("c1", "c1", 0, 50, 1400, 1000); */
    /* gStyle->SetOptFit(111); */
    /* gStyle->SetOptStat(1110); */
    /* h_nclusters->Draw("e1 x0"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf("); */
    /* h_nRealClusters->Draw("e1 x0"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_missing_photons->Draw("e1 x0"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* /1* c1->SaveAs("plots/missing_energy.pdf"); *1/ */
    /* h_PiE->Draw("e1 x0"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* gStyle->SetOptStat(10); */
    /* h_PiEtaPhi->Draw("colz"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_nclusters_E->Draw("colz"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* gStyle->SetOptStat(1110); */
    /* h_nClusters_vsE->Draw(); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* gStyle->SetOptStat(10); */
    /* h_nTowers_chi2->Draw("colz"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_nTowers_prob->Draw("colz"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* gStyle->SetOptStat(1110); */
    /* h_1Tower_chi2->Draw("e1 x0"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_1Tower_prob->Draw("e1 x0"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_multiTower_chi2->Draw("e1 x0"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_multiTower_prob->Draw("e1 x0"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* gStyle->SetOptStat(10); */
    /* h_dR_E->Draw("colz"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* gStyle->SetOptStat(1110); */
    /* h_PiEratio->Draw("e1 x0"); */
    /* //h_PiEratio->Fit("gaus"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_PideltaEta->Draw("e1 x0"); */
    /* //h_PideltaEta->Fit("gaus"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_PideltaPhi->Draw("e1 x0"); */
    /* //h_PideltaPhi->Fit("gaus"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_PiEratio_vsE->Draw(""); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_PideltaEta_vsE->Draw(""); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_PideltaPhi_vsE->Draw(""); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_PhoEratio->Draw("e1 x0"); */
    /* //h_PhoEratio->Fit("gaus"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_PhodeltaEta->Draw("e1 x0"); */
    /* //h_PhodeltaEta->Fit("gaus"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_PhodeltaPhi->Draw("e1 x0"); */
    /* //h_PhodeltaPhi->Fit("gaus"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_PhoEratio_vsE->Draw(""); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* gStyle->SetOptStat(10); */
    /* h_PhoEratio_E->Draw("colz"); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* gStyle->SetOptStat(1110); */
    /* h_PhodeltaEta_vsE->Draw(); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf"); */
    /* h_PhodeltaPhi_vsE->Draw(); */
    /* c1->Modified(); */
    /* c1->SaveAs("plots/pi0_clustering_benchmark.pdf)"); */

    /* if (write_csv) outfile.close(); */
    histfile->cd();
    bhs_Eratio_vsE.GetBinnedMeans()->Write();
    /* bhs_Eratio_vsE.Write(); */
    histfile->Write();
    histfile->Close();
}
