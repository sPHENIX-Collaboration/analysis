#include <iostream>
#include <TH2D.h>
#include <TH1D.h>
#include <TChain.h>
#include <TMath.h>
#include <TTree.h>
#include <TFile.h>
#include <sstream> //std::ostringstsream
#include <fstream> //std::ifstream
#include <iostream> //std::cout, std::endl
#include <cmath>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <string>
#include <set>

using namespace std;

void TrackCaloDist()
{	
	TFile *out = new TFile("TrackCaloDist.root","RECREATE");

	TH1D* h_dist[6]; // distance between track (eta,phi) and shower vertex (eta,phi)
	TH1D* h_emcal[6]; // ratio of energy deposition in emcal to track momentum 
	TH1D* h_ihcal[6]; // ratio of energy deposition in ihcal to track momentum 
	TH1D* h_ohcal[6]; // ratio of energy deposition in ohcal to track momentum 
	TH2D* h_totale_dist[6]; // total energy deposition from the shower vs distance between track and shower 

	// only filled in the case of an ohcal shower 
	TH1D* ohcal_top = new TH1D("ohcal_top","",300,0,3); // ratio of energy in leading tower of ohcal to track momentum 
	TH1D* ohcal_towers = new TH1D("ohcal_towers","",300,0,3); // ratio of energy in ohcal tower region to track mom
	TH1D* ohcal_total = new TH1D("ohcal_total","",300,0,3); // ratio of energy in all calo tower region to track mom
	TH1D* ohcal_ihcal_mip = new TH1D("ohcal_ihcal_mip","",300,0,1); // energy in ihcal in case of ohcal shower 
	TH1D* ohcal_emcal_mip = new TH1D("ohcal_emcal_mip","",300,0,1); // energy in emcal in case of ohcal shower 
	TH2D* h_4towers = new TH2D("h_4towers","",5,-2.5,2.5,5,-2.5,2.5); // 5x5 tower energy deposition in ohcal

	// classifications of shower location 
	string detector[6] = { "mip", "ohcal", "magnet", "ihcal", "emcal", "tracker"};
	for (int i = 0; i < 6; i++) {
		h_dist[i] = new TH1D(TString::Format("h_dist_%s",detector[i].c_str()),"",200,0,5);
		h_emcal[i] = new TH1D(TString::Format("h_emcale_%s",detector[i].c_str()),"",100,0,3);
		h_ihcal[i] = new TH1D(TString::Format("h_ihcale_%s",detector[i].c_str()),"",100,0,3);
		h_ohcal[i] = new TH1D(TString::Format("h_ohcale_%s",detector[i].c_str()),"",100,0,3);
		h_totale_dist[i] = new TH2D(TString::Format("h_totale_dist_%s",detector[i].c_str()),"",100,0,3,200,0,5);
	}

	TH2D* h_ohcal_dist = new TH2D("h_ohcale_dist","",100,0,3,200,0,5);
	TH1D* h_class = new TH1D("h_class","",7,-0.5,6.5);

	// distance between the track (eta,phi) and the shower vertex for each classification
	// energy distribution for each calorimeter for each classification

	TFile *file = new TFile("TruthCaloTree.root");
	TTreeReader reader("T",file);

	TTreeReaderValue<int> n(reader, "n_truth");
	TTreeReaderArray<float> E(reader, "truthenergy");
	TTreeReaderArray<int> pid(reader, "truthpid");
	TTreeReaderArray<float> eta(reader, "trutheta");
	TTreeReaderArray<float> phi(reader, "truthphi");
	TTreeReaderArray<float> p(reader, "truthp");
	TTreeReaderArray<float> pt(reader, "truthpt");
	TTreeReaderArray<int> track_id(reader, "truth_track_id");

	TTreeReaderValue<int> n_child(reader, "n_child");
	TTreeReaderArray<int> child_pid(reader, "child_pid");
	TTreeReaderArray<int> child_parent_id(reader, "child_parent_id");
	TTreeReaderArray<int> child_vertex_id(reader, "child_vertex_id");

	TTreeReaderValue<int> BH_n(reader, "BH_n");
	TTreeReaderArray<int> BH_track_id(reader, "BH_track_id");
	TTreeReaderArray<float> BH_e(reader, "BH_e");

	TTreeReaderValue<int> n_vertex(reader, "n_vertex");
	TTreeReaderArray<int> vertex_id(reader, "vertex_id");
	TTreeReaderArray<float> x(reader, "vertex_x");
	TTreeReaderArray<float> y(reader, "vertex_y");
	TTreeReaderArray<float> z(reader, "vertex_z");

	TTreeReaderArray<float> tower_sim_E(reader, "tower_sim_E");
	TTreeReaderArray<int> tower_sim_ieta(reader, "tower_sim_ieta");
	TTreeReaderArray<int> tower_sim_iphi(reader, "tower_sim_iphi");
	TTreeReaderArray<float> tower_sim_eta(reader, "tower_sim_eta");
	TTreeReaderArray<float> tower_sim_phi(reader, "tower_sim_phi");
	TTreeReaderValue<int> tower_sim_n(reader, "tower_sim_n");

	TTreeReaderArray<float> EMcal_sim_E(reader, "EMcal_sim_E");
	TTreeReaderArray<int> EMcal_sim_ieta(reader, "EMcal_sim_ieta");
	TTreeReaderArray<int> EMcal_sim_iphi(reader, "EMcal_sim_iphi");
	TTreeReaderArray<float> EMcal_sim_eta(reader, "EMcal_sim_eta");
	TTreeReaderArray<float> EMcal_sim_phi(reader, "EMcal_sim_phi");
	TTreeReaderValue<int> EMcal_sim_n(reader, "EMcal_sim_n");

	TTreeReaderArray<float> hcalIN_sim_E(reader, "hcalIN_sim_E");
	TTreeReaderArray<int> hcalIN_sim_ieta(reader, "hcalIN_sim_ieta");
	TTreeReaderArray<int> hcalIN_sim_iphi(reader, "hcalIN_sim_iphi");
	TTreeReaderArray<float> hcalIN_sim_eta(reader, "hcalIN_sim_eta");
	TTreeReaderArray<float> hcalIN_sim_phi(reader, "hcalIN_sim_phi");
	TTreeReaderValue<int> hcalIN_sim_n(reader, "hcalIN_sim_n");

	const int nphi_hcal = 64; // number of hcal towers in phi direction 
	const int neta_hcal = 24;
	const int nphi_emcal = 256;
	const int neta_emcal = 96;
	const float max_eta = 0.8; // maximum eta value for isolated track
	const float neigh_max_eta = 1.0;
	const float track_pt_cut = 0.5; // pt cut for neighboring particles 
	const float high_pt_cut = 4.0;
	const float de = 0.045833; // distance from center of one calorimeter tower to edge in eta
	const float dp = 0.049087; // distacne from center of one calorimeter tower to edge in phi
	const float ihcal_sf = 0.162166; // sampling fractions
	const float ohcal_sf = 0.0338021;
	const float emcal_sf = 0.02;
	const float emcal_inner = 92; // inner radius of emcal 
	const float emcal_outer = 116;
	const float ihcal_inner = 117.27;
	const float ihcal_outer = 134.42;
	const float ohcal_inner = 183.3;
	const float ohcal_outer = 274.317;

	double dr, delta_r, delta_eta, delta_phi, dr_charge, dphi, deta, tower_dr;
	float mipEM, mipIH, mipOH;
	double vr, v_phi, v_eta;
	float avgtracks[10] = {0.0};
	int ntracks_centrality[10] = {0};
	int centrality_bins = 10;
	set<int> charged_pid = {-3334,-3312,-3222,-3112,-2212,-431,-411,-321,-211,-13,-11,11,
						 13,211,321,411,431,2212,3112,3222,3312,3334};
	set<int> neutral_pid = {-3322,-3212,-3122,-2112,-421,-14,-12,12,14,22,111,130,310,421,
						 2112,3122,3212,3322};
	int cent[] = {40,50,60,70,80};

	float five_by_five[5][5] = {{0.0}};
	int child;
	set<int> vertex;
	float v_radius, v_rad;
	int classify;

	float eta_map[24],eta_mapIH[24],eta_mapEM[96];
	float phi_map[64],phi_mapIH[64],phi_mapEM[256];


	// first find the mapping of (ieta,iphi) calorimeter towers to the actual (eta,phi) coordinates of the towers 
	while (reader.Next()) {
		for (int i = 0; i < *tower_sim_n; i++) {
			if (tower_sim_ieta[i] >= 0 && tower_sim_ieta[i] < neta_hcal 
				&& tower_sim_iphi[i] >= 0 && tower_sim_iphi[i] < nphi_hcal) {
		    	phi_map[tower_sim_iphi[i]] = tower_sim_phi[i];
		    	eta_map[tower_sim_ieta[i]] = tower_sim_eta[i];
		  	}
	  	}

	 	for (int i = 0; i < *hcalIN_sim_n; i++) {
			if (hcalIN_sim_ieta[i] >= 0 && hcalIN_sim_ieta[i] < neta_hcal 
				&& hcalIN_sim_iphi[i] >= 0 && hcalIN_sim_iphi[i] < nphi_hcal) {
		    	phi_mapIH[hcalIN_sim_iphi[i]] = hcalIN_sim_phi[i];
		    	eta_mapIH[hcalIN_sim_ieta[i]] = hcalIN_sim_eta[i];
		  	}
	  	}

	  	for (int i = 0; i < nphi_hcal; i++) {
			if (phi_map[i] < 0.0) phi_map[i] += 2*M_PI;
			if (phi_mapIH[i] < 0.0) phi_mapIH[i] += 2*M_PI;
		}

		for (int i = 0; i < *EMcal_sim_n; i++) {
			if (EMcal_sim_ieta[i] >= 0 && EMcal_sim_ieta[i] < neta_emcal 
				&& EMcal_sim_iphi[i] >= 0 && EMcal_sim_iphi[i] < nphi_emcal) {
		    	phi_mapEM[EMcal_sim_iphi[i]] = EMcal_sim_phi[i];
		    	eta_mapEM[EMcal_sim_ieta[i]] = EMcal_sim_eta[i];
		  	}
	  	}

	  	for (int i = 0; i < nphi_emcal; i++) {
			if (phi_mapEM[i] < 0.0) phi_mapEM[i] += 2*M_PI;
		}
	}

	reader.Restart();
	int f = 0;
    while (reader.Next()) {

    	if ( f % 1000 == 0) cout << f << endl;
    	if (!vertex.empty()) vertex.clear();
    	classify = -1;

    	float e_sim[24][64] = {{0.0}};
    	float e_simIH[24][64] = {{0.0}};
    	float e_simEM[96][256] = {{0.0}};
    	float total_energy = 0.0;

		// find the energy distribution in the calo towers for the event
    	for (int i = 0; i < *tower_sim_n; i++) {
			if (tower_sim_ieta[i] >= 0 && tower_sim_ieta[i] < neta_hcal 
				&& tower_sim_iphi[i] >= 0 && tower_sim_iphi[i] < nphi_hcal) {
    			e_sim[tower_sim_ieta[i]][tower_sim_iphi[i]] += tower_sim_E[i]/ohcal_sf;
    		}
    	}
    	for (int i = 0; i < *hcalIN_sim_n; i++) {
    		if (hcalIN_sim_ieta[i] >= 0 && hcalIN_sim_ieta[i] < neta_hcal 
				&& hcalIN_sim_iphi[i] >= 0 && hcalIN_sim_iphi[i] < nphi_hcal) {
    			e_simIH[hcalIN_sim_ieta[i]][hcalIN_sim_iphi[i]] += hcalIN_sim_E[i]/ihcal_sf;
    		}
    	}
    	for (int i = 0; i < *EMcal_sim_n; i++) {
    		if (EMcal_sim_ieta[i] >= 0 && EMcal_sim_ieta[i] < neta_emcal
	    		&& EMcal_sim_iphi[i] >= 0 && EMcal_sim_iphi[i] < nphi_emcal) {
    			e_simEM[EMcal_sim_ieta[i]][EMcal_sim_iphi[i]] += EMcal_sim_E[i]/emcal_sf;
    		}
    	}

    	// loop over truth level particles
    	for (int i = 0; i < *n; i++) {

			// find charged isolated high pt particle and check that it is isolated
	    	if (pt[i] >= high_pt_cut && charged_pid.find(pid[i])!=charged_pid.end() && abs(double(eta[i])) < max_eta) {
	    		if (phi[i] < 0.0) phi[i] += 2*M_PI;
	    		dr = 100.0;
		    	for (int j = 0; j < *n; j++) {
		    		if (i != j && pt[j] > track_pt_cut && abs(double(eta[j])) < neigh_max_eta) {
		    			if (phi[j] < 0.0) phi[j] += 2*M_PI;
		    			delta_phi = double(phi[i]) - double(phi[j]);
		    			if (delta_phi > M_PI) delta_phi -= M_PI;
						delta_eta = double(eta[i]) - double(eta[j]);
						delta_r = sqrt(delta_phi*delta_phi + delta_eta*delta_eta);
						if (delta_r < dr) dr = delta_r;
		    		}
		    	}

		    	// check where particle begins showering
		    	if (dr >= 0.2) {
		    		for (int j = 0; j < *n_child; j++) {
						if (child_parent_id[j] == track_id[i] && abs(child_pid[j]) != 11 && child_pid[j] != 22) vertex.insert(child_vertex_id[j]);
					}
					v_radius = ohcal_outer + 1;
					for (set<int>::iterator it = vertex.begin(); it != vertex.end(); it++) {
						child = 0;
						for (int j = 0 ; j < *n_child; j++) {
							if (child_vertex_id[j] == *it && abs(child_pid[j]) != 11 && child_pid[j] != 22) {
								child++;
							}
						}
						if (child > 1) {
							// now find the location of the shower vertex
							for (int v = 0; v < *n_vertex; v++) {
								if (vertex_id[v] == *it) {
									v_rad = sqrt(x[v]*x[v] + y[v]*y[v]);
									if (v_rad < v_radius) {
										v_radius = v_rad;
										v_phi = atan2(y[v],x[v]);
										v_eta = atanh(z[v] / sqrt(x[v]*x[v] + y[v]*y[v] + z[v]*z[v]));
										dphi = abs(double(v_eta - eta[i]));
				    					deta = abs(double(v_phi - phi[i]));
				    					if (dphi >= M_PI) dphi -= M_PI;
				    					vr = sqrt(dphi*dphi + deta*deta);
									}
								}
							}
						}
					}


					// find the energy deposition and number of towers in tower region
					int towers[200][2];
		    		float top_energy = 0.0;
					int ti, tj;
					int numOH = 0;
					int tn = 0;
					mipEM = 0.0;
			    	mipIH = 0.0;
			    	mipOH = 0.0;
		    		for (int j = 0; j < neta_hcal; j++) {
		    			for (int k = 0; k < nphi_hcal; k++) {
		    				dphi = abs(double(eta_map[j] - eta[i]));
		    				deta = abs(double(phi_map[k] - phi[i]));
		    				if (dphi >= M_PI) dphi -= M_PI;
		    				tower_dr = sqrt(dphi*dphi + deta*deta);
		    				// find towers in (eta,phi) range of track 
		    				if (tower_dr <= 0.2) {
		    					towers[tn][0] = j;
		    					towers[tn][1] = k;
		    					mipIH += e_simIH[j][k];
			    				mipOH += e_sim[j][k];
			    				tn++;
		    				}
		    			}
		    		}

		    		for (int j = 0; j < neta_emcal; j++) {
		    			for(int k = 0; k < nphi_emcal; k++) {
		    				dphi = abs(double(eta_mapEM[j] - eta[i]));
		    				deta = abs(double(phi_mapEM[k] - phi[i]));
		    				if (dphi >= M_PI) dphi -= M_PI;
		    				tower_dr = sqrt(dphi*dphi + deta*deta);
		    				if (tower_dr <= 0.2) {
		    					mipEM += e_simEM[j][k];
		    				} 
		    			}
		    		}

		    		int j;
		    		int k;
		    		double mipTotal = mipEM + mipIH + mipOH;

					for (int t = 0; t < tn; t++) {
						j = towers[t][0];
	    				k = towers[t][1];
			    		if (e_sim[j][k] > top_energy) {
			    			top_energy = e_sim[j][k];
			    			ti = j;
			    			tj = k;
			    		}
					}

					// based on shower location classification fill histograms 
					if (v_radius < emcal_inner) {
						h_emcal[5]->Fill(mipEM/p[i]);
						h_ihcal[5]->Fill(mipIH/p[i]);
						h_ohcal[5]->Fill(mipOH/p[i]);
						h_dist[5]->Fill(vr);
						h_class->Fill(5);
						h_totale_dist[5]->Fill(mipTotal/p[i],vr);
					} else if (v_radius >= emcal_inner && v_radius <= emcal_outer) {
						h_emcal[4]->Fill(mipEM/p[i]);
						h_ihcal[4]->Fill(mipIH/p[i]);
						h_ohcal[4]->Fill(mipOH/p[i]);
						h_dist[4]->Fill(vr);
						h_class->Fill(4);
						h_totale_dist[4]->Fill(mipTotal/p[i],vr);
					} else if (v_radius >= ihcal_inner && v_radius <= ihcal_outer) {
						h_emcal[3]->Fill(mipEM/p[i]);
						h_ihcal[3]->Fill(mipIH/p[i]);
						h_ohcal[3]->Fill(mipOH/p[i]);
						h_dist[3]->Fill(vr);
						h_class->Fill(3);
						h_totale_dist[3]->Fill(mipTotal/p[i],vr);
					} else if (v_radius > ihcal_outer && v_radius < ohcal_inner) {
						h_emcal[2]->Fill(mipEM/p[i]);
						h_ihcal[2]->Fill(mipIH/p[i]);
						h_ohcal[2]->Fill(mipOH/p[i]);
						h_dist[2]->Fill(vr);
						h_class->Fill(2);
						h_totale_dist[2]->Fill(mipTotal/p[i],vr);
					} else if (v_radius >= ohcal_inner && v_radius <= ohcal_outer) {
						h_emcal[1]->Fill(mipEM/p[i]);
						h_ihcal[1]->Fill(mipIH/p[i]);
						h_ohcal[1]->Fill(mipOH/p[i]);
						h_dist[1]->Fill(vr);
						h_class->Fill(1);
						h_ohcal_dist->Fill(mipOH/p[i],vr);
						h_totale_dist[1]->Fill(mipTotal/p[i],vr);
						ohcal_top->Fill(top_energy/p[i]);
						ohcal_towers->Fill(mipOH/p[i]);
						ohcal_total->Fill(mipTotal/p[i]);
						ohcal_ihcal_mip->Fill(mipIH);
						ohcal_emcal_mip->Fill(mipEM);

						// fill 5x5 tower energy distribution for ohcal showers 
						for (int fi = -2; fi < 3; fi++) {
							for (int fj = -2; fj < 3; fj++) {
								five_by_five[fi+2][fj+2] += e_sim[ti+fi][tj+fj];
								h_4towers->SetBinContent(h_4towers->FindBin(fi,fj),five_by_five4[fi+2][fj+2]);
							}
						}
					} else if (v_radius > ohcal_outer) {
						// check if the event mips through the entire system
						for (int j = 0; j < *BH_n; j++) {
							if (BH_track_id[j] == track_id[i]) {
								h_emcal[0]->Fill(mipEM/p[i]);
								h_ihcal[0]->Fill(mipIH/p[i]);
								h_ohcal[0]->Fill(mipOH/p[i]);
								h_class->Fill(0);
								h_totale_dist[0]->Fill(mipTotal/p[i],vr);
							}
						}
					}
				}		
		    }	
	    }
	    f++;
	}
  	out->Write();
  	out->Close();
}
