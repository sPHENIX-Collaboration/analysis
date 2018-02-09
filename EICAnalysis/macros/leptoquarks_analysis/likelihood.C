#include "tau_commons.h"

/**
 * Compare reconstructed observables between tau jets and quark jets
 *
 * Written by nils.feege@stonybrook.edu
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

  string type = "3pion";
  string seed[7] = {"1","2","3","6","7","8","9"};
  string R_max = "8";
  /* open inout files and merge trees */
  TChain chain("event");

  for (int g = 0; g<7;g++){
    if(g==4 || g==5) continue;
    string file = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_1000events_"+seed[g]+"seed_"+type+"_r0"+R_max+".root";
    chain.Add(file.c_str());
  }
  

  string inputFile = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_1000events_10seed_"+type+"_r0"+R_max+".root";  
  

  TFile *f = TFile::Open(inputFile.c_str());
  TTree *t = (TTree*)f->Get("event");

  /* particle selection */
  //TCut select_prong("tracks_count_r02 == 3 && tracks_chargesum_r02 == -1");
  //TCut select_prong("tracks_count_r04 == 3");
  TCut select_prong("1");

  /* Create temporary canvas */
  TCanvas* ctemp = new TCanvas();

  vector< TString > observables;
  vector< TString > observables_name;

  vector< float > plots_ymax;
  vector< float > plots_xmin;
  vector< float > plots_xmax;

  /* R_max */
  observables.push_back( "tracks_rmax_R" );
  observables_name.push_back( "R_{track}^{max}" );
  plots_ymax.push_back(0.06);
  plots_xmin.push_back(0);
  plots_xmax.push_back(0.5);

  /* Number of tracks */
  observables.push_back( "tracks_count_R" );
  observables_name.push_back( "N_{tracks}" );
  plots_ymax.push_back(0.9);
  plots_xmin.push_back(0);
  plots_xmax.push_back(10);

  /* Charge sum from tracks */
  observables.push_back( "tracks_chargesum_R" );
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
  plots_xmin.push_back(-2);
  plots_xmax.push_back(2);

  /* Jet mass */
  observables.push_back( "jet_minv" );
  observables_name.push_back( "Mass_{jet}" );
  plots_ymax.push_back(0.1);
  plots_xmin.push_back(0);
  plots_xmax.push_back(10);

  /* Jet energy */
  observables.push_back( "jet_etotal" );
  observables_name.push_back( "E_{jet}" );
  plots_ymax.push_back(0.08);
  plots_xmin.push_back(0);
  plots_xmax.push_back(70);

  /* Plot distributions */
  TString name_uds_base("h_uds_");
  TString name_tau_base("h_tau_");

  const int nvar = observables.size();

  TH1F* h_uds[nvar];
  TH1F* h_tau[nvar];

  TString name_uds_i = name_uds_base;
  TString name_tau_i = name_tau_base;

  for ( unsigned j = 0; j < observables.size(); j++ ){
    name_uds_i = name_uds_base;
    name_tau_i = name_tau_base;

    name_uds_i.Append(j);
    name_tau_i.Append(j);
    
    h_uds[j] = new TH1F( name_uds_i, "", 100, plots_xmin.at(j), plots_xmax.at(j));
    h_tau[j] = new TH1F( name_tau_i, "", 100, plots_xmin.at(j), plots_xmax.at(j));
  }

  for ( unsigned i = 0; i < observables.size(); i++ )
    {
      //cout << "Plotting " << observables.at(i) << endl;

      ctemp->cd();

      name_uds_i = name_uds_base;
      name_tau_i = name_tau_base;

      name_uds_i.Append(i);
      name_tau_i.Append(i);

      chain.Draw( observables.at(i) + " >> " + name_uds_i, tau_commons::select_true_uds && tau_commons::select_accept_jet, "goff");
      h_uds[i]->Scale(1./h_uds[i]->Integral());
      h_uds[i]->GetXaxis()->SetTitle( observables_name.at(i) );
      h_uds[i]->GetYaxis()->SetRangeUser(0, plots_ymax.at(i) );
      h_uds[i]->SetLineColor(col1);
      h_uds[i]->SetFillColor(col1);
      h_uds[i]->SetFillStyle(1001);
     
      chain.Draw( observables.at(i) + " >> " + name_tau_i, tau_commons::select_true_tau && tau_commons::select_accept_jet, "goff");
      h_tau[i]->Scale(1./h_tau[i]->Integral());
      h_tau[i]->SetLineColor(col2);
      h_tau[i]->SetFillColor(col2);
      h_tau[i]->SetFillStyle(0);
      
      
      /* create Canvas and draw histograms */
      /*TCanvas *c1 = new TCanvas();

      h_uds[i]->DrawClone("");
      h_tau[i]->DrawClone("sames");
      gPad->RedrawAxis();
      */
      //      c1->Print( Form( "plots/compare_observables_%d.eps", i ) );
      //   c1->Print( Form( "plots/compare_observables_%d.png", i ) );
    }

  vector<float> * tracks_rmax;
  vector<float> * tracks_count;
  vector<float> * tracks_chargesum;
  vector<float> * jetshape_radius;
  vector<float> * jetshape_econe_1;
  vector<float> * jetshape_econe_2;
  vector<float> * jetshape_econe_5;
  vector<float> * jet_eta;
  vector<float> * jet_minv;
  vector<float> * jet_etotal;
  vector<int> * evtgen_pid;

  t->SetBranchAddress("tracks_rmax_R",&tracks_rmax);
  t->SetBranchAddress("tracks_count_R",&tracks_count);
  t->SetBranchAddress("tracks_chargesum_R",&tracks_chargesum);
  t->SetBranchAddress("jetshape_radius",&jetshape_radius);
  t->SetBranchAddress("jetshape_econe_r01",&jetshape_econe_1);
  t->SetBranchAddress("jetshape_econe_r02",&jetshape_econe_2);
  t->SetBranchAddress("jetshape_econe_r05",&jetshape_econe_5);
  t->SetBranchAddress("jet_eta",&jet_eta);
  t->SetBranchAddress("jet_minv",&jet_minv);
  t->SetBranchAddress("jet_etotal",&jet_etotal);
  t->SetBranchAddress("evtgen_pid",&evtgen_pid);

  int tau_as_tau = 0;
  int tau_as_DIS = 0;
  int DIS_as_DIS = 0;
  int DIS_as_tau = 0;
  
  int n_tot = t->GetEntries();
  
  TH1F *h = new TH1F("h","",100,0,1);

  for( int k = 0; k < n_tot; k++){
    t->GetEntry(k);
    //cout<<k+1<<endl;

    //vector<double> avg_v;
    //vector<int> pid_v;

    for(int l = 0; l < tracks_rmax->size(); l++){

      vector<double> likelihood;

      double rmax = (*tracks_rmax)[l];
      double count = (*tracks_count)[l];
      double chargesum = (*tracks_chargesum)[l];
      double radius = (*jetshape_radius)[l];
      double econe_1 = (*jetshape_econe_1)[l];
      double econe_2 = (*jetshape_econe_2)[l];
      double econe_5 = (*jetshape_econe_5)[l];
      double eta = (*jet_eta)[l];
      double minv = (*jet_minv)[l];
      double etotal = (*jet_etotal)[l];
      int pid = (*evtgen_pid)[l];
      
      
      //if(radius < 0 || fabs(eta) > 1.0) continue;

      int x_bin;
      
      x_bin = h_tau[0]->GetXaxis()->FindBin(rmax);
      likelihood.push_back( h_tau[0]->GetBinContent(x_bin) / (h_tau[0]->GetBinContent(x_bin) + h_uds[0]->GetBinContent(x_bin)));
      
      x_bin = h_tau[1]->GetXaxis()->FindBin(count);
      likelihood.push_back( h_tau[1]->GetBinContent(x_bin) / (h_tau[1]->GetBinContent(x_bin) + h_uds[1]->GetBinContent(x_bin)));
      
      x_bin = h_tau[2]->GetXaxis()->FindBin(chargesum);
      likelihood.push_back( h_tau[2]->GetBinContent(x_bin) / (h_tau[2]->GetBinContent(x_bin) + h_uds[2]->GetBinContent(x_bin)));
      
      x_bin = h_tau[3]->GetXaxis()->FindBin(radius);
      likelihood.push_back( h_tau[3]->GetBinContent(x_bin) / (h_tau[3]->GetBinContent(x_bin) + h_uds[3]->GetBinContent(x_bin)));
      
      x_bin = h_tau[4]->GetXaxis()->FindBin(econe_1/econe_5);
      likelihood.push_back( h_tau[4]->GetBinContent(x_bin) / (h_tau[4]->GetBinContent(x_bin) + h_uds[4]->GetBinContent(x_bin)));
      
      x_bin = h_tau[5]->GetXaxis()->FindBin(econe_2/econe_5);
      likelihood.push_back( h_tau[5]->GetBinContent(x_bin) / (h_tau[5]->GetBinContent(x_bin) + h_uds[5]->GetBinContent(x_bin)));

      x_bin = h_tau[6]->GetXaxis()->FindBin(eta);
      likelihood.push_back( h_tau[6]->GetBinContent(x_bin) / (h_tau[6]->GetBinContent(x_bin) + h_uds[6]->GetBinContent(x_bin)));
      
      x_bin = h_tau[7]->GetXaxis()->FindBin(minv);
      likelihood.push_back( h_tau[7]->GetBinContent(x_bin) / (h_tau[7]->GetBinContent(x_bin) + h_uds[7]->GetBinContent(x_bin)));
      
      x_bin = h_tau[8]->GetXaxis()->FindBin(etotal);
      likelihood.push_back( h_tau[8]->GetBinContent(x_bin) / (h_tau[8]->GetBinContent(x_bin) + h_uds[8]->GetBinContent(x_bin)));
      
      double avg = Average(likelihood);           
      if(avg != avg) continue;

      h->Fill(avg);

      //    cout<<avg<<" "<<pid<<endl;
      //avg_v.push_back(avg);
      //pid_v.push_back(pid);
      
      if(avg>0.6){
	if(pid == 15) tau_as_tau++;
	else{ DIS_as_tau++; }
      }
      else{
	if(pid == 15) tau_as_DIS++;
	else{ DIS_as_DIS++; }
      }
      
    }
    /*
    if(avg_v.size() != 2) continue;

    int index;
    if(avg_v[1] > avg_v[0]) index = 1;
    else{ index = 0; }
    
    if(fabs(avg_v[1]-avg_v[0]) > 0.15) { 
      if(pid_v[index] == 15) {
	tau_as_tau++;
	DIS_as_DIS++;
      }                                                                                                                                                                
      else{ 
	tau_as_DIS++;
	DIS_as_tau++; 
      }     
    }
    else{                                                                                                                                                                                                 
      tau_as_DIS++;                                                                                                                                                                 
      DIS_as_DIS++;                                                                                                                                                                                
    }                  
    */

  }
  
  cout<<tau_as_tau*1.0/(n_tot*1.0)<<endl;
  cout<<DIS_as_tau*1.0/((tau_as_tau+DIS_as_tau)*1.0)<<endl;
  h->Draw();

  return 0;
}


