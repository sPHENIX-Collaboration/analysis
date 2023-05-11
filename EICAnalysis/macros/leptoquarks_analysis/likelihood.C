#include "tau_commons.h"

/**
 * Compare reconstructed observables between tau jets and quark jets
 *
 * Written by nils.feege@stonybrook.edu & sean.jeffas@stonybrook.edu
 */



double Average(vector<double> v)
{
  double sum=0;
  for(int i=0;(unsigned)i<v.size();i++)
    sum+=v[i];
  return sum/v.size();
}


int likelihood()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  // Labels for geant files //
  string type[4] = {"3pion","SM","DIScharged","DISneutral"};
  string seed[10] = {"1","2","3","4","5","6","7","8","9","10"};
  string R_max = "5";

  /* open inout files and merge trees */
  TChain chain("event");
  TChain chain_tau("event");
  TChain t("event");

  // File for event topology of ID tau events
  ofstream myfile;
  string filename = "./data/Event_topology.csv";
  myfile.open(filename.c_str());
  
  // Loop over geant files and split LQ and SM events into two TChains used for jet characterization //
  for (int b = 0; b<2; b++){
    for (int g = 0; g<10; g++){
      // Skip file that is used to test //
      if(b == 1 && g==0) continue;
      string file = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_1000events_"+seed[g]+"seed_"+type[b]+"_r0"+R_max+".root";      

      if(b==0) chain_tau.Add(file.c_str());
      else chain.Add(file.c_str());
    }
  }
  
  for(int b = 1; b<2; b++){
    for(int g=0; g<1; g++){
      // File to test identification //
      string inputFile = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_1000events_"+seed[g]+"seed_"+type[b]+"_r05.root";  
      
      t.Add(inputFile.c_str());
    }
  }
  

  
  /* Create temporary canvas */
  TCanvas* ctemp = new TCanvas();

  vector< TString > observables;
  vector< TString > observables_name;

  vector< float > plots_ymax;
  vector< float > plots_xmin;
  vector< float > plots_xmax;

  /* R_max */
  observables.push_back( "tracks_rmax_r04" );
  observables_name.push_back( "R_{track}^{max}" );
  plots_ymax.push_back(0.06);
  plots_xmin.push_back(0);
  plots_xmax.push_back(0.4);

  /* Number of tracks */
  observables.push_back( "tracks_count_r04" );
  observables_name.push_back( "N_{tracks}" );
  plots_ymax.push_back(0.9);
  plots_xmin.push_back(0);
  plots_xmax.push_back(10);

  /* Charge sum from tracks */
  observables.push_back( "tracks_chargesum_r04" );
  observables_name.push_back( "#Sigma q_{tracks}" );
  plots_ymax.push_back(0.9);
  plots_xmin.push_back(-5);
  plots_xmax.push_back(5);

  /* Jet radius */
  observables.push_back( "jetshape_radius" );
  observables_name.push_back( "R_{jet}" );
  plots_ymax.push_back(0.08);
  plots_xmin.push_back(0);
  plots_xmax.push_back(0.5);

  /* Jetshape */
  observables.push_back( "jetshape_econe_r01 / jetshape_econe_r05" );
  observables_name.push_back( "E_{cone}^{R<0.1} / E_{cone}^{R<0.5}" );
  plots_ymax.push_back(0.08);
  plots_xmin.push_back(0);
  plots_xmax.push_back(1);

  /* Jetshape */
  observables.push_back( "jetshape_econe_r02 / jetshape_econe_r05" );
  observables_name.push_back( "E_{cone}^{R<0.2} / E_{cone}^{R<0.5}" );
  plots_ymax.push_back(0.08);
  plots_xmin.push_back(0);
  plots_xmax.push_back(1);

  /* Jet eta */
  observables.push_back( "jet_eta" );
  observables_name.push_back( "#eta_{jet}" );
  plots_ymax.push_back(0.1);
  plots_xmin.push_back(-1);
  plots_xmax.push_back(3);

  /* Jet mass */
  observables.push_back( "jet_minv" );
  observables_name.push_back( "Mass_{jet} (GeV)" );
  plots_ymax.push_back(0.1);
  plots_xmin.push_back(0);
  plots_xmax.push_back(10);

  /* Jet energy */
  observables.push_back( "jet_etotal" );
  observables_name.push_back( "E_{jet} (GeV)" );
  plots_ymax.push_back(0.08);
  plots_xmin.push_back(0);
  plots_xmax.push_back(70);

  /* Jet vertex */
  observables.push_back( "tracks_vertex" );
  observables_name.push_back( "vertex distance (cm)" );
  plots_ymax.push_back(0.6);
  plots_xmin.push_back(0);
  plots_xmax.push_back(1);


  /* Plot distributions */
  TString name_uds_base("h_uds_");
  TString name_tau_base("h_tau_");

  const int nvar = observables.size();

  TH1F* h_uds[nvar];
  TH1F* h_tau[nvar];

  TString name_uds_i = name_uds_base;
  TString name_tau_i = name_tau_base;

  /* create histograms */
  for ( unsigned j = 0; j < observables.size(); j++ ){
    name_uds_i = name_uds_base;
    name_tau_i = name_tau_base;

    name_uds_i.Append(j);
    name_tau_i.Append(j);
    
    h_uds[j] = new TH1F( name_uds_i, "", 100, plots_xmin.at(j), plots_xmax.at(j));
    h_tau[j] = new TH1F( name_tau_i, "", 100, plots_xmin.at(j), plots_xmax.at(j));
  }

  /* Fill histograms*/
  for ( unsigned i = 0; i < observables.size(); i++ )
    {
      //cout << "Plotting " << observables.at(i) << endl;

      ctemp->cd();

      name_uds_i = name_uds_base;
      name_tau_i = name_tau_base;

      name_uds_i.Append(i);
      name_tau_i.Append(i);

      // SM parton jet observable histograms //
      chain.Draw( observables.at(i) + " >> " + name_uds_i, tau_commons::select_true_uds && tau_commons::select_accept_jet, "goff");
      h_uds[i]->Scale(1./h_uds[i]->Integral());
      h_uds[i]->GetXaxis()->SetTitle( observables_name.at(i) );
      h_uds[i]->GetYaxis()->SetRangeUser(0, plots_ymax.at(i) );
      h_uds[i]->SetLineColor(col1);
      h_uds[i]->SetFillColor(col1);
      h_uds[i]->SetFillStyle(1001);
      h_uds[i]->GetYaxis()->SetTitle("# entries / #Sigma entries");
      h_uds[i]->GetYaxis()->SetTitleOffset(1.5);

      // LQ tau jet observable histograms //
      chain_tau.Draw( observables.at(i) + " >> " + name_tau_i, tau_commons::select_true_tau && tau_commons::select_accept_jet, "goff");
      h_tau[i]->Scale(1./h_tau[i]->Integral());
      h_tau[i]->SetLineColor(col2);
      h_tau[i]->SetFillColor(col2);
      h_tau[i]->SetFillStyle(0);
      
      
      /* create Canvas and draw histograms */
      TCanvas *c1 = new TCanvas();

      h_uds[i]->DrawClone("");
      h_tau[i]->DrawClone("sames");
      gPad->RedrawAxis();

      TLegend *legend = new TLegend(0.4,0.6,0.7,0.89);
      legend->AddEntry(h_uds[i],"SM parton jet","l");
      legend->AddEntry(h_tau[i],"LQ #tau jet","l");
      legend->Draw();

      //      c1->Print( Form( "plots/compare_observables_%d.eps", i ) );
      //   c1->Print( Form( "plots/compare_observables_%d.png", i ) );
    }
  
  // Define jet and //  
  vector<float> * tracks_rmax;
  vector<float> * tracks_count;
  vector<float> * tracks_chargesum;
  vector<float> * tracks_vertex;
  vector<float> * jetshape_radius;
  vector<float> * jetshape_econe_1;
  vector<float> * jetshape_econe_2;
  vector<float> * jetshape_econe_5;
  vector<float> * jet_eta;
  vector<float> * jet_phi;
  vector<float> * jet_minv;
  vector<float> * jet_etotal;
  vector<float> * jet_ptrans;
  vector<float> * evtgen_pid;

  // Define event variables //
  float ptmiss;
  float miss_phi;
  float is_lq;

  // See variables from tree //
  t.SetBranchAddress("tracks_rmax_R",&tracks_rmax);
  t.SetBranchAddress("tracks_count_R",&tracks_count);
  t.SetBranchAddress("tracks_chargesum_R",&tracks_chargesum);
  t.SetBranchAddress("tracks_vertex",&tracks_vertex);
  t.SetBranchAddress("jetshape_radius",&jetshape_radius);
  t.SetBranchAddress("jetshape_econe_r01",&jetshape_econe_1);
  t.SetBranchAddress("jetshape_econe_r02",&jetshape_econe_2);
  t.SetBranchAddress("jetshape_econe_r05",&jetshape_econe_5);
  t.SetBranchAddress("jet_eta",&jet_eta);
  t.SetBranchAddress("jet_phi",&jet_phi);
  t.SetBranchAddress("jet_minv",&jet_minv);
  t.SetBranchAddress("jet_etotal",&jet_etotal);
  t.SetBranchAddress("evtgen_pid",&evtgen_pid);
  t.SetBranchAddress("jet_ptrans",&jet_ptrans);
  t.SetBranchAddress("Et_miss",&ptmiss);
  t.SetBranchAddress("Et_miss_phi",&miss_phi);
  t.SetBranchAddress("is_lq_event",&is_lq); 

  /* identification counts */
  int tau_as_tau = 0;
  int tau_as_DIS = 0;
  int DIS_as_DIS = 0;
  int DIS_as_tau = 0;
  int id_e = 0;
  
  int tot_jets = 0;

  // Define likelihood cut for tau jet identification //
  double lik_cut = 0.71;
  
  int n_tot = t.GetEntries();
  
  // Loop over all events //
  for( int k = 0; k < n_tot; k++){
    t.GetEntry(k);

    // Define some event variables //
    double delta_phi;
    bool is_e = false;
    bool is_miss = false;
    vector<double> jet_lik;
    vector<double> jet_id;
    vector<double> miss_jet_phi;
    
    // Loop over jets in event to look for electron //
    for(int l = 0; l < tracks_rmax->size(); l++){
      int count = (*tracks_count)[l];
      double rmax = (*tracks_rmax)[l];
      int chargesum = (*tracks_chargesum)[l];
      double radius = (*jetshape_radius)[l];
      double eta = (*jet_eta)[l];
      double phi = (*jet_phi)[l];
      double etotal = (*jet_etotal)[l];
      double vertex = (*tracks_vertex)[l];
      double ptrans = (*jet_ptrans)[l];
      int pid = (*evtgen_pid)[l];
    
      // If any jets pass these cuts then this is considered an electron event //
      if(count == 1 && chargesum == -1 && ptrans > 5 && vertex < 0.03 && radius < 0.05) is_e = true;
      
    }
    
    // Loop over all jets in events //
    for(int l = 0; l < tracks_rmax->size(); l++){
      
      // Likelihood variable //      
      vector<double> likelihood;

      // Get jet observables //
      double rmax = (*tracks_rmax)[l];
      int count = (*tracks_count)[l];
      int chargesum = (*tracks_chargesum)[l];
      double vertex = (*tracks_vertex)[l];
      double radius = (*jetshape_radius)[l];
      double econe_1 = (*jetshape_econe_1)[l];
      double econe_2 = (*jetshape_econe_2)[l];
      double econe_5 = (*jetshape_econe_5)[l];
      double eta = (*jet_eta)[l];
      double phi = (*jet_phi)[l];
      double minv = (*jet_minv)[l];
      double etotal = (*jet_etotal)[l];
      double ptrans = (*jet_ptrans)[l];
      int pid = (*evtgen_pid)[l];
      tot_jets++;
      
      // If electron event then ID as parton and continue //
      if(is_e && pid == 15) tau_as_DIS++;
      if(is_e && pid != 15) DIS_as_DIS++;
      if(is_e) continue;
      
      
      // x bin number //
      int x_bin;
      
      // Find where jet observable lies on characteristic histograms and use histogram heights to find likelihood //
      // Currently count, chargesum, eta, and vertex give best results. Rest are left commented out //

      //x_bin = h_tau[0]->GetXaxis()->FindBin(rmax);
      //likelihood.push_back( h_tau[0]->GetBinContent(x_bin) / (h_tau[0]->GetBinContent(x_bin) + h_uds[0]->GetBinContent(x_bin)));   

      x_bin = h_tau[1]->GetXaxis()->FindBin(count);
      likelihood.push_back( h_tau[1]->GetBinContent(x_bin) / (h_tau[1]->GetBinContent(x_bin) + h_uds[1]->GetBinContent(x_bin)));
      
      x_bin = h_tau[2]->GetXaxis()->FindBin(chargesum);
      likelihood.push_back( h_tau[2]->GetBinContent(x_bin) / (h_tau[2]->GetBinContent(x_bin) + h_uds[2]->GetBinContent(x_bin)));
      
      //x_bin = h_tau[3]->GetXaxis()->FindBin(radius);
      //likelihood.push_back( h_tau[3]->GetBinContent(x_bin) / (h_tau[3]->GetBinContent(x_bin) + h_uds[3]->GetBinContent(x_bin)));
      /*
      x_bin = h_tau[4]->GetXaxis()->FindBin(econe_1/econe_5);
      likelihood.push_back( h_tau[4]->GetBinContent(x_bin) / (h_tau[4]->GetBinContent(x_bin) + h_uds[4]->GetBinContent(x_bin)));
      
      x_bin = h_tau[5]->GetXaxis()->FindBin(econe_2/econe_5);
      likelihood.push_back( h_tau[5]->GetBinContent(x_bin) / (h_tau[5]->GetBinContent(x_bin) + h_uds[5]->GetBinContent(x_bin)));
      */
      
      x_bin = h_tau[6]->GetXaxis()->FindBin(eta);
      likelihood.push_back( h_tau[6]->GetBinContent(x_bin) / (h_tau[6]->GetBinContent(x_bin) + h_uds[6]->GetBinContent(x_bin)));
      
      //x_bin = h_tau[7]->GetXaxis()->FindBin(minv);
      //likelihood.push_back( h_tau[7]->GetBinContent(x_bin) / (h_tau[7]->GetBinContent(x_bin) + h_uds[7]->GetBinContent(x_bin)));
      
      //x_bin = h_tau[8]->GetXaxis()->FindBin(etotal);
      //likelihood.push_back( h_tau[8]->GetBinContent(x_bin) / (h_tau[8]->GetBinContent(x_bin) + h_uds[8]->GetBinContent(x_bin)));

      // Exclude jets where vertex = inf which causes problems //      
      x_bin = h_tau[9]->GetXaxis()->FindBin(vertex);
      if(vertex == vertex) likelihood.push_back( h_tau[9]->GetBinContent(x_bin) / (h_tau[9]->GetBinContent(x_bin) + h_uds[9]->GetBinContent(x_bin)));


      // Define total likelihood as average of likelihoods of observables //
      double avg = Average(likelihood);           

      // Make sure final likelihood is real finite number //
      if(avg != avg) continue;
      
      // Only use jets with transverse momentum > 5 GeV //
      // Write jet likelihoods to array for event //
      if(ptrans < 5) avg = 0;
      if(ptrans > 5) {
	jet_lik.push_back(avg);
	jet_id.push_back(pid);
	miss_jet_phi.push_back(phi);
      }
      
      // If pt < 5 call parton jet //
      if(ptrans < 5 && pid == 15) tau_as_DIS++;
      if(ptrans < 5 && pid != 15) DIS_as_DIS++;
      
      // This block uses hard cuts on variables for identification //
      /*
      if(count == 3 && chargesum == -1 && ptrans > 5 && vertex > 0.028 && eta < 0.82)) {
	if(pid == 15) tau_as_tau++;
	else DIS_as_tau++;          
      }
      else {
	if(pid == 15) tau_as_DIS++;
	else DIS_as_DIS++;
      }
    */  
            
    }

    // Increment if electron is in event //
    if(is_e) id_e++;
    
    // This block is used for likelihood identification //
    // Define some variables for jet selection //
    int a = -1;
    int b = -1;
    double max_lik = 0;
    int max_lik_i;
    bool is_tau = false;

    // If no jets with pt > 5 then skip //
    if(jet_lik.size() == 0) continue;

    // Loop over all jets in event with pt > 5 //
    for(int m = 0; m < jet_lik.size(); m++){
      // Find if there is a tau in this event //
      if(jet_id[m] == 15) is_tau = true;
      // Find jet with maximum tau likelihood //
      if(jet_lik[m] > max_lik) {
	max_lik = jet_lik[m];
	max_lik_i = m;
      }
      // There is a problem with same jets being tagged with two different true values 
      // Here we check if one of the jets is repeated //
      // Loop over all jets to check if two have exact same likelihood //
      for(int n = 0; n < jet_lik.size(); n++){
	// If different jets have same likelihood then set a and b to the jet indices //
	if(m!=n && jet_lik[m]==jet_lik[n]) {
	  a = m;
	  b = n;
	  // If either is tau jet then set them both to a tau jet id //
	  if(jet_id[m] == 15 || jet_id[n] 15) {
	    jet_id[m] == 15;
	    jet_id[n] ==15;
	  }
	  // Otherwise set them both to 0 id //
	  else {
	    jet_id[m] = 0;
	    jet_id[n] = 0;
	  }
	}
      }
    }
    // End loop over all jets with pt > 5 //
    
    // Check if maximum likelihood jet passes cut then ID as tau //
    if(jet_lik[max_lik_i] > lik_cut){
      // Find ID tau jet as true tau //
      if(jet_id[max_lik_i] == 15) tau_as_tau++;
      // Otherwise find ID tau jet as true parton //
      else DIS_as_tau++;
      // All other jets are ID as parton, minus one that was ID as tau //
      DIS_as_DIS = DIS_as_DIS + jet_lik.size() - 1;
      // Set event topology to be recorded for later study //
      is_miss = true;
    }
    // If fails likelihood cut then ID as parton
    else{
      // If a tau was in the event then it is ID as parton with all other jets //
      if(is_tau) tau_as_DIS++;
      if(is_tau) DIS_as_DIS = DIS_as_DIS + jet_lik.size() - 1;
      // If no tau then every jet is parton and ID correctly //
      if(!is_tau) DIS_as_DIS = DIS_as_DIS + jet_lik.size();
    }

    // Check if duplicate jets were found and subtract one from ID jets if it was //    
    if(a > -1 && b > -1){
      if(jet_id[a] == 15 && jet_lik[max_lik_i] > lik_cut) DIS_as_tau--;
      if(jet_id[a] != 15 && jet_lik[max_lik_i] < lik_cut) DIS_as_DIS--;
    }
    
    // If jet is ID as tau then calculate ptmiss and acoplanarity for this event //
    if(is_miss){
      delta_phi = fabs(miss_phi - miss_jet_phi[max_lik_i]) * 180 / TMath::Pi();
      if(delta_phi > 180) delta_phi = 360 - delta_phi;
      // Write variables with true LQ tagging to file for later study //
      myfile<<ptmiss<<","<<delta_phi<<","<<is_lq<<endl;
	}
  }
 
  // Define efficiency and false positive rate //
  double efficiency = tau_as_tau*1.0/(n_tot*1.0);
  double FP;

  if(tau_as_tau == 0 && DIS_as_tau == 0) FP = 0;
  else FP = DIS_as_tau*1.0/((tau_as_tau+DIS_as_tau)*1.0);  


  // Confusion Matrix //                                                                                                                                                                                   
  cout<<"         | Act Tau   Act Parton"<<endl;
  cout<<"------------------------------"<<endl;
  cout<<"ID Tau   | "<<tau_as_tau<<"         "<<DIS_as_tau<<endl;
  cout<<"ID Parton| "<<tau_as_DIS<<"         "<<DIS_as_DIS<<endl;

  cout<<endl;

  // Write out some important variables //
  cout<<"Tau Recovery Rate: "<<efficiency<<endl;
  cout<<"False Positive Rate: "<<FP<<endl;
  cout<<"Identified Electron Events : "<<id_e<<endl;
  cout<<"Total Jets: "<<tot_jets<<endl;

  return 0;
  }


