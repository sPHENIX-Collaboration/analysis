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
  string type[3] = {"3pion","DIScharged","DISneutral"};
  string seed[10] = {"1","2","3","4","5","6","7","8","9","10"};
  string R_max = "5";

  /* open inout files and merge trees */
  TChain chain("event");
  TChain chain_tau("event");
  
  // Loop over geant files and split LQ and SM events into two TChains used for jet characterization //
  for (int b = 0; b<3; b++){
    for (int g = 0; g<10; g++){
      // Skip file that is used to test //
      if(b == 0 && g==0) continue;
      string file = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_1000events_"+seed[g]+"seed_"+type[b]+"_r0"+R_max+".root";      

      if(b==0) chain_tau.Add(file.c_str());
      else chain.Add(file.c_str());
    }
  }

  // File to test identification //
  string inputFile = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_1000events_2seed_3pion_r05.root";  
  
  TFile *f = TFile::Open(inputFile.c_str());
  TTree *t = (TTree*)f->Get("event");


  
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
      chain.Draw( observables.at(i) + " >> " + name_uds_i, tau_commons::select_true_uds, "goff");
      h_uds[i]->Scale(1./h_uds[i]->Integral());
      h_uds[i]->GetXaxis()->SetTitle( observables_name.at(i) );
      h_uds[i]->GetYaxis()->SetRangeUser(0, plots_ymax.at(i) );
      h_uds[i]->SetLineColor(col1);
      h_uds[i]->SetFillColor(col1);
      h_uds[i]->SetFillStyle(1001);
      h_uds[i]->GetYaxis()->SetTitle("# entries / #Sigma entries");
      h_uds[i]->GetYaxis()->SetTitleOffset(1.5);

      // LQ tau jet observable histograms //
      chain_tau.Draw( observables.at(i) + " >> " + name_tau_i, tau_commons::select_true_tau, "goff");
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
  
  // Define jet and event variables //  
  vector<float> * tracks_rmax;
  vector<float> * tracks_count;
  vector<float> * tracks_chargesum;
  vector<float> * tracks_vertex;
  vector<float> * jetshape_radius;
  vector<float> * jetshape_econe_1;
  vector<float> * jetshape_econe_2;
  vector<float> * jetshape_econe_5;
  vector<float> * jet_eta;
  vector<float> * jet_minv;
  vector<float> * jet_etotal;
  vector<float> * jet_ptrans;
  vector<int> * evtgen_pid;

  // See variables from tree //
  t->SetBranchAddress("tracks_rmax_R",&tracks_rmax);
  t->SetBranchAddress("tracks_count_R",&tracks_count);
  t->SetBranchAddress("tracks_chargesum_R",&tracks_chargesum);
  t->SetBranchAddress("tracks_vertex",&tracks_vertex);
  t->SetBranchAddress("jetshape_radius",&jetshape_radius);
  t->SetBranchAddress("jetshape_econe_r01",&jetshape_econe_1);
  t->SetBranchAddress("jetshape_econe_r02",&jetshape_econe_2);
  t->SetBranchAddress("jetshape_econe_r05",&jetshape_econe_5);
  t->SetBranchAddress("jet_eta",&jet_eta);
  t->SetBranchAddress("jet_minv",&jet_minv);
  t->SetBranchAddress("jet_etotal",&jet_etotal);
  t->SetBranchAddress("evtgen_pid",&evtgen_pid);
  t->SetBranchAddress("jet_ptrans",&jet_ptrans);

  /* identification counts */
  int tau_as_tau = 0;
  int tau_as_DIS = 0;
  int DIS_as_DIS = 0;
  int DIS_as_tau = 0;

  // Define likelihood cut for tau jet identification //
  double lik_cut = 0.786;
  
  int n_tot = t->GetEntries();
  
  // Loop over all events //
  for( int k = 0; k < n_tot; k++){
    t->GetEntry(k);
  
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
      double minv = (*jet_minv)[l];
      double etotal = (*jet_etotal)[l];
      double ptrans = (*jet_ptrans)[l];
      int pid = (*evtgen_pid)[l];

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
      if(ptrans < 5) avg = 0;
      
      // This block uses hard cuts on variables for identification //
      /*
      if(count == 3 && chargesum == -1 && ptrans > 5 && vertex > 0.028 && eta < 0.82) {
	if(pid == 15) tau_as_tau++;
	else DIS_as_tau++;          
      }
      else {
	if(pid == 15) tau_as_DIS++;
	else DIS_as_DIS++;
      }
    
      avg_v.push_back(avg);
      pid_v.push_back(pid); 
    }
      */

      // This block uses overall likelihood for identification //
      if(avg>lik_cut){
	// call tau particle and check if is true tau //
	if(pid == 15) tau_as_tau++;
	else{	  
	  DIS_as_tau++; }
      }
      else{
	// call parton and check if is true parton //
	if(pid == 15) tau_as_DIS++;
	else{ DIS_as_DIS++; }
      } 
    }
      
  }
 
  // Define efficiency and false positive rate //
  double efficiency = tau_as_tau*1.0/(n_tot*1.0);
  double FP;

  if(tau_as_tau == 0 && DIS_as_tau == 0) FP = 0;
  else FP = DIS_as_tau*1.0/((tau_as_tau+DIS_as_tau)*1.0);  


  // Confusion Matrix //                                                                                                                                                                                   
  cout<<"          | ID parton   ID Tau"<<endl;
  cout<<"------------------------------"<<endl;
  cout<<"Act parton| "<<DIS_as_DIS<<"          "<<DIS_as_tau<<endl;
  cout<<"Act Tau   | "<<tau_as_DIS<<"          "<<tau_as_tau<<endl;

  cout<<endl;

  cout<<"Tau Recovery Rate: "<<efficiency<<endl;
  cout<<"False Positive Rate: "<<FP<<endl;

   return 0;
  }


