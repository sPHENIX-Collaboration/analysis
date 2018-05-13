#include "tau_commons.h"

/**
 * Plot missing trasnverse energy and other event-wise observables
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


int likelihood_topology()
{
  
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  // labels for geant files //
  string type[2] = {"3pion","DIScharged"};
  string seed[10] = {"1","2","3","4","5","6","7","8","9","10"};
  string R_max = "5";
  string nevents;
  string file;

  /* open inout files and merge trees */
  TChain chain_CC("event");
  TChain chain_tau("event");

  
  for (int b = 0; b<2; b++){
    for (int g = 0; g<10; g++){
      // Set some labels //
      if(b==0) nevents = "1000";
      if(b==1 || b==2) nevents = "100";

      if(b==0) file = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_"+nevents+"events_"+seed[g]+"seed_"+type[b]+"_r0"+R_max+".root";
      if(b==1) file = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_"+nevents+"events_"+seed[g]+"seed_"+type[b]+"_tau_r0"+R_max+".root";

      if(b==0) chain_tau.Add(file.c_str());
      if(b==1) chain_CC.Add(file.c_str());
    }
  }

  // Add file with selected tau event topologies //  
  string inputFile = "./data/Event_topology.csv";

  TTree *t=new TTree("MyTree","MyTree");
  t->ReadFile(inputFile.c_str(),"ptmiss:acoplan:is_lq");

  // Create temporary canvas 
  TCanvas* ctemp = new TCanvas();

  vector< TString > observables;
  vector< TString > observables_name;

  vector< float > plots_xmin;
  vector< float > plots_xmax;

  // Missing energy
  observables.push_back( "Et_miss" );
  observables_name.push_back( "p_{T}^{miss} (GeV)" );
  plots_xmin.push_back(0);
  plots_xmax.push_back(50);

  // Acoplanarity 
  observables.push_back( "Et_miss_phi" );
  observables_name.push_back( "#Delta#phi_{miss - #tau jet}" );
  plots_xmin.push_back(0);
  plots_xmax.push_back(180);

  TString name_CC_base("h_CC_");
  TString name_tau_base("h_tau_");

  const int nvar = observables.size();

  TH1F* h_CC[nvar];
  TH1F* h_tau[nvar];

  TString name_CC_i = name_CC_base;
  TString name_tau_i = name_tau_base;

  // create histograms 
  for ( int l = 0; l < observables.size(); l++ ){
    name_CC_i = name_CC_base;
    name_tau_i = name_tau_base;

    name_CC_i.Append(l);
    name_tau_i.Append(l);

    h_CC[l] = new TH1F( name_CC_i, "", 50, plots_xmin.at(l), plots_xmax.at(l));
    h_tau[l] = new TH1F( name_tau_i, "", 50, plots_xmin.at(l), plots_xmax.at(l));
  }

  // Fill Histograms //
  for(int i = 0;i < observables.size(); i++){

    ctemp->cd();

    name_CC_i = name_CC_base;
    name_tau_i = name_tau_base;

    name_CC_i.Append(i);
    name_tau_i.Append(i);

    // Fill Et_miss straight from tree //
    if(i==0){
      // Fill CC histogram //
      int n_CC = chain_CC.Draw( observables.at(i) + " >> " + name_CC_i, "", "goff");
      h_CC[i]->Scale(1./h_CC[i]->Integral());
      h_CC[i]->GetXaxis()->SetTitle( observables_name.at(i) );
      h_CC[i]->GetYaxis()->SetRangeUser(0, .13 );
      h_CC[i]->SetLineColor(col1);
      h_CC[i]->SetFillColor(col1);
      h_CC[i]->SetFillStyle(0);
      h_CC[i]->GetYaxis()->SetTitle("# entries / #Sigma entries");
      h_CC[i]->GetYaxis()->SetTitleOffset(1.5);
      
      // Fill LQ histogram //
      int n_tau = chain_tau.Draw( observables.at(i) + " >> " + name_tau_i, "", "goff");
      h_tau[i]->Scale(1./h_tau[i]->Integral());
      h_tau[i]->SetLineColor(col2);
      h_tau[i]->SetFillColor(col2);
      h_tau[i]->SetFillStyle(0);
      
      TCanvas *c1 = new TCanvas();
      
      h_CC[i]->DrawClone("");
      h_tau[i]->DrawClone("sames");
      gPad->RedrawAxis();
      
      TLegend *legend = new TLegend(0.4,0.6,0.7,0.89);
      legend->AddEntry(h_CC[i],"Charged Current","l");
      legend->AddEntry(h_tau[i],"Leptoquark","l");
      legend->Draw();
    }

    // Calculate acoplanarity and then fill histogram //
    if(i==1){
      
      float Et_miss_phi_tau, Et_miss_phi_CC;
      float tau_phi_tau, tau_phi_CC;
      
      // Read tau phi and missing momentum phi from tree //
      chain_CC.SetBranchAddress("Et_miss_phi",&Et_miss_phi_CC);
      chain_CC.SetBranchAddress("reco_tau_phi",&tau_phi_CC);
          
      chain_tau.SetBranchAddress("Et_miss_phi",&Et_miss_phi_tau);
      chain_tau.SetBranchAddress("reco_tau_phi",&tau_phi_tau);

      // Phi angle between reco tau and miss mom   //     
      double delta_phi;

      // Calculate delta phi for each event type //
      for(int j = 0; j<n_CC;j++){
	chain_CC.GetEntry(j);
	delta_phi = fabs(Et_miss_phi_CC - tau_phi_CC) * 180 / TMath::Pi();
	if(delta_phi > 180) delta_phi = 360 - delta_phi; 
	h_CC[i]->Fill(delta_phi);
      }

      for(int k = 0; k<n_tau;k++){
        chain_tau.GetEntry(k);
        delta_phi = fabs(Et_miss_phi_tau - tau_phi_tau) * 180 / TMath::Pi();
        if(delta_phi > 180) delta_phi = 360 - delta_phi;
        h_tau[i]->Fill(delta_phi);
      }

      //Fill histograms //
      h_CC[i]->Scale(1./h_CC[i]->Integral());
      h_CC[i]->GetXaxis()->SetTitle( observables_name.at(i) );
      h_CC[i]->GetYaxis()->SetRangeUser(0, 0.27 );
      h_CC[i]->SetLineColor(col1);
      h_CC[i]->SetFillColor(col1);
      h_CC[i]->SetFillStyle(0);
      h_CC[i]->GetYaxis()->SetTitle("# entries / #Sigma entries");
      h_CC[i]->GetYaxis()->SetTitleOffset(1.5);

      h_tau[i]->Scale(1./h_tau[i]->Integral());
      h_tau[i]->SetLineColor(col2);
      h_tau[i]->SetFillColor(col2);
      h_tau[i]->SetFillStyle(0);

      TCanvas *c1 = new TCanvas();

      h_CC[i]->DrawClone("");
      h_tau[i]->DrawClone("sames");
      gPad->RedrawAxis();

      TLegend *legend = new TLegend(0.4,0.6,0.7,0.89);
      legend->AddEntry(h_CC[i],"Charged Current","l");
      legend->AddEntry(h_tau[i],"Leptoquark","l");
      legend->Draw();
            
    }

  }


  float acoplan, ptmiss, is_lq;

  // Get variables from tree //
  t->SetBranchAddress("acoplan",&acoplan);
  t->SetBranchAddress("ptmiss",&ptmiss);
  t->SetBranchAddress("is_lq",&is_lq);
  
  // identification counts  //
  int tau_as_tau = 0;
  int tau_as_DIS = 0;
  int DIS_as_DIS = 0;
  int DIS_as_tau = 0;

  // Define likelihood cut //
  double lik_cut = 0.60;

  int n_tot = t->GetEntries();
  
  // Loop over all events //
  for( int k = 0; k < n_tot; k++){
    t->GetEntry(k);

    vector<double> likelihood;

    int x_bin;

    // Find likelihood for each observable //
    x_bin = h_tau[0]->GetXaxis()->FindBin(ptmiss);
    likelihood.push_back( h_tau[0]->GetBinContent(x_bin) / (h_tau[0]->GetBinContent(x_bin) + h_CC[0]->GetBinContent(x_bin)));

    x_bin = h_tau[1]->GetXaxis()->FindBin(acoplan);
    likelihood.push_back( h_tau[1]->GetBinContent(x_bin) / (h_tau[1]->GetBinContent(x_bin) + h_CC[1]->GetBinContent(x_bin)));
    
    // Average the likelihoods //
    double avg = Average(likelihood);

    // Make sure likelihood is real number //
    if(avg != avg) continue;
    
    // Use likelihood cut for identification //
    if(avg>lik_cut){
      // Call event LQ and check if true //
      if(is_lq == 1) tau_as_tau++;
      else DIS_as_tau++;
    }
    else{
      // Call event SM and check if true //
      if(is_lq == 1) tau_as_DIS++;
      else DIS_as_DIS++;
    }


  }


  // Define efficiency and false positive rate //                                                                                                                                                          
  double efficiency = tau_as_tau*1.0/(n_tot*1.0);
  double FP;

  if(tau_as_tau == 0 && DIS_as_tau == 0) FP = 0;
  else FP = DIS_as_tau*1.0/((tau_as_tau+DIS_as_tau)*1.0);

  // Confusion Matrix //                                                                                                                                                                                   
  cout<<"      | ID SM   ID LQ"<<endl;
  cout<<"---------------------"<<endl;
  cout<<"Act SM| "<<DIS_as_DIS<<"       "<<DIS_as_tau<<endl;
  cout<<"Act LQ| "<<tau_as_DIS<<"       "<<tau_as_tau<<endl;

  cout<<endl;

  cout<<"LQ Recovery Rate: "<<efficiency<<endl;
  cout<<"False Positive Rate: "<<FP<<endl;


  
  return 0;
}
