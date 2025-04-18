#include "TGraph.h"
vector< TString > v_cuts_CEMC; 
vector< TString > v_cuts_CEMC_short;
vector< TString > v_cuts_EEMC;
vector< TString > v_cuts_EEMC_short;
vector< TString > v_cuts_FEMC;
vector< TString > v_cuts_FEMC_short;

void add_CEMC_Cut(double, double);

void add_EEMC_Cut(double);

void add_FEMC_Cut(double);

int plot_true_false_positive()
{

  /*----------------------------------------- */
  /* Add Cuts Here                            */
  /*                                          */
  /* add_CEMC_cut(float_1, float_2)           */
  /*   Cut 1 : E/p  > float_1                 */
  /*   Cut 2 : prob > float_2                 */
  /*   (if float_#=0, Cut # is not applied    */
  /* add_EEMC_cut(float_1)                    */
  /*   Cut 1 : E/True E > float_1             */
  /* add_FEMC_cut(float_1)                    */
  /*   Cut 1 : E/True E > float_1             */
  /* ---------------------------------------- */

  add_CEMC_Cut(0.807,0.02);
  add_CEMC_Cut(0.703,0.02);
  
  add_EEMC_Cut(0.703);
  add_EEMC_Cut(0.807);
  add_EEMC_Cut(0.95);

  add_FEMC_Cut(0.703);
  add_FEMC_Cut(0.807);
  add_FEMC_Cut(0.95);

  /* ---------------------------------------- */

  // Momentums //
  vector< float > v_momenta;
  v_momenta.push_back(1.0);
  v_momenta.push_back(2.0);
  v_momenta.push_back(5.0);
  v_momenta.push_back(10.0);
  v_momenta.push_back(20.0);
  
  // Marker Styles //
  vector< int > v_styles;
  v_styles.push_back(20);
  v_styles.push_back(24);
  v_styles.push_back(21);
  v_styles.push_back(25);
  v_styles.push_back(22);
  v_styles.push_back(26);
  v_styles.push_back(23);
  v_styles.push_back(27);
  v_styles.push_back(34);
  v_styles.push_back(28);
  v_styles.push_back(29);
  v_styles.push_back(30);
 
  /* Loading Macros */

  gROOT->LoadMacro("efficiency_purity_analysisMacro.C");
  gROOT->LoadMacro("/sphenix/user/gregtom3/SBU/research/macros/macros/sPHENIXStyle/sPhenixStyle.C");
  SetsPhenixStyle();
  
  /* Useful local variables */
  float v_rate_tp; //true positive rate value holder
  float v_rate_fp; //false positive rate value holder

  //------------------------------------------------------------//
  //--------                CEMC Plots                  --------//
  //------------------------------------------------------------//

  //CEMC Graph Initialization//
  const int size_C=2*v_cuts_CEMC.size(); //T.P & F.P graph per cut 
  TCanvas *c_CEMC = new TCanvas("cc","cc",1000,600);
  TGraph *graphs_CEMC[size_C]; //Array of TGraph pointers
  auto legend_CEMC_t = new TLegend(0.51,0.2,1.0,0.9); //Legend
  auto legend_CEMC_f = new TLegend(0.51,0.2,1.0,0.9); //Legend

  legend_CEMC_t->SetTextSize(0.06);
  legend_CEMC_f->SetTextSize(0.06);
  // Create the CEMC TGraphs ( 2 per cut )//
  for(unsigned idx_cut=0;idx_cut < v_cuts_CEMC.size(); idx_cut++)
    /* loop over cuts */
    {
      // Index for each graph in TGraph vector //
      int idx_tp=idx_cut*2;
      int idx_fp=idx_cut*2+1;
      
      // Create new TGraphs_CEMC //
      graphs_CEMC[idx_tp]=new TGraph(v_momenta.size());
      graphs_CEMC[idx_fp]=new TGraph(v_momenta.size());
      
      // Clear true positive and false positive //
      v_rate_tp = v_rate_fp = 0.0;

      // Set Points in TGraph for True Positive and False Positive per momenta//
      for ( unsigned idx_p = 0; idx_p < v_momenta.size(); idx_p++ )
	{
	  v_rate_tp=get_true_positive("C",v_cuts_CEMC.at(idx_cut),v_momenta.at(idx_p));
	  v_rate_fp=get_false_positive("C",v_cuts_CEMC.at(idx_cut),v_momenta.at(idx_p));
	  graphs_CEMC[idx_tp]->SetPoint(idx_p,v_momenta[idx_p],v_rate_tp);
	  graphs_CEMC[idx_fp]->SetPoint(idx_p,v_momenta[idx_p],v_rate_fp);	
	}
      
      // Tweak Graph Style //
      graphs_CEMC[idx_tp]->SetMarkerColor(kGreen+2);
      graphs_CEMC[idx_fp]->SetMarkerColor(kRed+2);
      
      graphs_CEMC[idx_tp]->SetLineColor(graphs_CEMC[idx_tp]->GetMarkerColor());
      graphs_CEMC[idx_fp]->SetLineColor(graphs_CEMC[idx_fp]->GetMarkerColor());

      graphs_CEMC[idx_tp]->SetMarkerStyle(v_styles.at(idx_cut));
      graphs_CEMC[idx_fp]->SetMarkerStyle(v_styles.at(idx_cut));
      
      graphs_CEMC[idx_tp]->SetMarkerSize(1); // Possibly too large? //
      graphs_CEMC[idx_fp]->SetMarkerSize(1);

      graphs_CEMC[idx_tp]->SetTitle("");
      graphs_CEMC[idx_fp]->SetTitle("");
      
      

      legend_CEMC_t->AddEntry(graphs_CEMC[idx_tp],v_cuts_CEMC_short.at(idx_cut)+" True Positive","P");
      legend_CEMC_f->AddEntry(graphs_CEMC[idx_fp],v_cuts_CEMC_short.at(idx_cut)+ " False Positive","P");
    }
  
  // Draw all graphs on same canvas (CEMC) //
  c_CEMC->Divide(1,2);
  for(int i = 0; i<size_C; i++)
    {
      if(i==0) 
	{
	  c_CEMC->cd(1);
	  //gPad->SetBottomMargin(0.2);
	  gPad->SetRightMargin(0.5);
	  graphs_CEMC[0]->Draw("PA");
	  graphs_CEMC[0]->GetXaxis()->SetTitle("True Momentum (GeV)");
	  graphs_CEMC[0]->GetYaxis()->SetRangeUser(0.7,1.0);
	  graphs_CEMC[0]->GetYaxis()->SetTitle("Rate");
	}
      else if(i==1)
	{
	  c_CEMC->cd(2);
	  //gPad->SetTopMargin(0.2);
	  gPad->SetRightMargin(0.5);
	  graphs_CEMC[1]->GetXaxis()->SetTitle("True Momentum (GeV)");
	  graphs_CEMC[1]->GetYaxis()->SetRangeUser(-0.01,0.11);
	  graphs_CEMC[1]->GetYaxis()->SetTitle("Rate");
	  graphs_CEMC[1]->Draw("PA");
	}
      else if (i%2==0)
	{
	  c_CEMC->cd(1);
	  graphs_CEMC[i]->Draw("PSame");
	}
      else
	{
	  c_CEMC->cd(2);
	  graphs_CEMC[i]->Draw("P");
	}
      
    }
  c_CEMC->cd(1);
  legend_CEMC_t->Draw();
  c_CEMC->cd(2);
  legend_CEMC_f->Draw();

  //------------------------------------------------------------//
  //--------                EEMC Plots                  --------//
  //------------------------------------------------------------//


  // Create EEMC Graphs (2 per cut) //
   const int size_E=v_cuts_EEMC.size(); //T.P & F.P graph per cut
   TCanvas *c_EEMC = new TCanvas("ce","ce",1000,600);
   TGraph *graphs_EEMC[size_E]; //Array of TGraph pointers
   auto legend_EEMC_t = new TLegend(0.51,0.2,1,0.9); //Legend
   auto legend_EEMC_f = new TLegend(0.51,0.2,1,0.9); //Legend
   
   legend_EEMC_t->SetTextSize(0.06);
   legend_EEMC_f->SetTextSize(0.06);
   for(unsigned idx_cut=0;idx_cut < v_cuts_EEMC.size()/2; idx_cut++)
    // loop over cuts //
    {
      // Index for each graph in TGraph vector //
      int idx_tp=idx_cut*2;
      int idx_fp=idx_cut*2+1;
      
      // Create new TGraphs //
      graphs_EEMC[idx_tp]=new TGraph(v_momenta.size());
      graphs_EEMC[idx_fp]=new TGraph(v_momenta.size());

      // Clear true positive and false positive //
      v_rate_tp = v_rate_fp = 0.0;
      
      // Fill data points for True Positve and False Positive Plot //
      for ( unsigned idx_p = 0; idx_p < v_momenta.size(); idx_p++ )
	{
	  TString cut = v_cuts_EEMC.at(idx_cut*2)+ getCut(v_momenta.at(idx_p)) + v_cuts_EEMC.at(idx_cut*2+1); 
	  v_rate_tp=get_true_positive("E",cut,v_momenta.at(idx_p));
	  v_rate_fp=get_false_positive("E",cut,v_momenta.at(idx_p));	  
	  graphs_EEMC[idx_tp]->SetPoint(idx_p,v_momenta[idx_p],v_rate_tp);
	  graphs_EEMC[idx_fp]->SetPoint(idx_p,v_momenta[idx_p],v_rate_fp);	  
	}
      
      graphs_EEMC[idx_tp]->SetMarkerColor(kGreen+2);
      graphs_EEMC[idx_fp]->SetMarkerColor(kRed+2);

      graphs_EEMC[idx_tp]->SetLineColor(graphs_EEMC[idx_tp]->GetMarkerColor());
      graphs_EEMC[idx_fp]->SetLineColor(graphs_EEMC[idx_fp]->GetMarkerColor());

      graphs_EEMC[idx_tp]->SetMarkerStyle(v_styles.at(idx_cut));
      graphs_EEMC[idx_fp]->SetMarkerStyle(v_styles.at(idx_cut));
      
      graphs_EEMC[idx_tp]->SetMarkerSize(1); // Possibly too large? //
      graphs_EEMC[idx_fp]->SetMarkerSize(1);

      graphs_EEMC[idx_tp]->SetTitle("");
      graphs_EEMC[idx_fp]->SetTitle("");
      
      legend_EEMC_t->AddEntry(graphs_EEMC[idx_tp],v_cuts_EEMC_short.at(idx_cut)+" True Positive","P");
      legend_EEMC_f->AddEntry(graphs_EEMC[idx_tp],v_cuts_EEMC_short.at(idx_cut)+" False Positive","P");
    }
    // Draw all graphs on same canvas (EEMC) //
   c_EEMC->Divide(1,2);
   for(int i = 0; i<size_E; i++)
     {
       if(i==0) 
	 {
	   c_EEMC->cd(1);
	   gPad->SetRightMargin(0.5);
	   graphs_EEMC[0]->Draw("PA");
	   graphs_EEMC[0]->GetXaxis()->SetTitle("True Momentum (GeV)");
	   graphs_EEMC[0]->GetYaxis()->SetRangeUser(0.7,1.0);
	   graphs_EEMC[0]->GetYaxis()->SetTitle("Rate");
	 }
       else if (i==1)
	 {
	   c_EEMC->cd(2);
	   gPad->SetRightMargin(0.5);
	   graphs_EEMC[1]->Draw("PA");
	   graphs_EEMC[1]->GetXaxis()->SetTitle("True Momentum (GeV)");
	   graphs_EEMC[1]->GetYaxis()->SetRangeUser(-0.01,0.11);
	   graphs_EEMC[1]->GetYaxis()->SetTitle("Rate"); 
	 }
       else if (i%2==0)
	 {
	   c_EEMC->cd(1);
	   graphs_EEMC[i]->Draw("PSame");
	 }
       else
	 {
	   c_EEMC->cd(2);
	   graphs_EEMC[i]->Draw("P");
	 }
     }
   c_EEMC->cd(1);
   legend_EEMC_t->Draw();
   c_EEMC->cd(2);
   legend_EEMC_f->Draw();

   //------------------------------------------------------------//
   //--------                FEMC Plots                  --------//
   //------------------------------------------------------------//
  
   
   // Create FEMC Graphs (2 per cut) //
   const int size_F=v_cuts_FEMC.size(); //T.P & F.P graph per cut
   TCanvas *c_FEMC = new TCanvas("cf","cf",1000,600);
   TGraph *graphs_FEMC[size_F]; //Array of TGraph pointers
   auto legend_FEMC_t = new TLegend(0.51,0.2,1.0,0.9); //Legend
   auto legend_FEMC_f = new TLegend(0.51,0.2,1.0,0.9); //Legend
   
   legend_FEMC_t->SetTextSize(0.06);
   legend_FEMC_f->SetTextSize(0.06);
  
   for(unsigned idx_cut=0;idx_cut < v_cuts_FEMC.size()/2; idx_cut++)
    // loop over cuts //
     {
       // Index for each graph in TGraph vector //
      int idx_tp=idx_cut*2;
      int idx_fp=idx_cut*2+1;
      
      // Create new TGraphs //
      graphs_FEMC[idx_tp]=new TGraph(v_momenta.size());
      graphs_FEMC[idx_fp]=new TGraph(v_momenta.size());
     
      // Clear true positive and false positive //
      v_rate_tp = v_rate_fp = 0.0;
      
      // Fill data points for True Positve and False Positive Plot //
      for ( unsigned idx_p = 0; idx_p < v_momenta.size(); idx_p++ )
	{
	  TString cut = v_cuts_FEMC.at(idx_cut*2)+ getCut(v_momenta.at(idx_p)) + v_cuts_FEMC.at(idx_cut*2+1); 
	  v_rate_tp=get_true_positive("F",cut,v_momenta.at(idx_p));
	  v_rate_fp=get_false_positive("F",cut,v_momenta.at(idx_p));
	  graphs_FEMC[idx_tp]->SetPoint(idx_p,v_momenta[idx_p],v_rate_tp);
	  graphs_FEMC[idx_fp]->SetPoint(idx_p,v_momenta[idx_p],v_rate_fp);	  
	}
      
      graphs_FEMC[idx_tp]->SetMarkerColor(kGreen+2);
      graphs_FEMC[idx_fp]->SetMarkerColor(kRed+2);

      graphs_FEMC[idx_tp]->SetLineColor(graphs_FEMC[idx_tp]->GetMarkerColor());
      graphs_FEMC[idx_fp]->SetLineColor(graphs_FEMC[idx_fp]->GetMarkerColor());

      graphs_FEMC[idx_tp]->SetMarkerStyle(v_styles.at(idx_cut));
      graphs_FEMC[idx_fp]->SetMarkerStyle(v_styles.at(idx_cut));
      
      graphs_FEMC[idx_tp]->SetMarkerSize(1); // Possibly too large? //
      graphs_FEMC[idx_fp]->SetMarkerSize(1);

      graphs_FEMC[idx_tp]->SetTitle("");
      graphs_FEMC[idx_fp]->SetTitle("");
      
      legend_FEMC_t->AddEntry(graphs_FEMC[idx_tp],v_cuts_FEMC_short.at(idx_cut)+" True Positive","P");
      legend_FEMC_f->AddEntry(graphs_FEMC[idx_fp],v_cuts_FEMC_short.at(idx_cut)+ " False Positive","P");
    }
   
   // Draw all graphs on same canvas (FEMC) //
   c_FEMC->Divide(1,2);
   for(int i = 0; i<size_F; i++)
     {
        if(i==0) 
	{
	  c_FEMC->cd(1);
	  //gPad->SetBottomMargin(0.2);
	  gPad->SetRightMargin(0.5);
	  graphs_FEMC[0]->Draw("PA");
	  graphs_FEMC[0]->GetXaxis()->SetTitle("True Momentum (GeV)");
	  graphs_FEMC[0]->GetYaxis()->SetRangeUser(0.7,1.0);
	  graphs_FEMC[0]->GetYaxis()->SetTitle("Rate");
	}
      else if(i==1)
	{
	  c_FEMC->cd(2);
	  //gPad->SetTopMargin(0.2);
	  gPad->SetRightMargin(0.5);
	  graphs_FEMC[1]->GetXaxis()->SetTitle("True Momentum (GeV)");
	  graphs_FEMC[1]->GetYaxis()->SetRangeUser(-0.01,0.11);
	  graphs_FEMC[1]->GetYaxis()->SetTitle("Rate");
	  graphs_FEMC[1]->Draw("PA");
	}
      else if (i%2==0)
	{
	  c_FEMC->cd(1);
	  graphs_FEMC[i]->Draw("PSame");
	}
      else
	{
	  c_FEMC->cd(2);
	  graphs_FEMC[i]->Draw("PSame");
	}
     }
   c_FEMC->cd(1);
   legend_FEMC_t->Draw();
   c_FEMC->cd(2);
   legend_FEMC_f->Draw();
   
  return 0;
}

void add_CEMC_Cut(double ep, double prob)
{
  //TStrings necessary to represent cuts//

  TString base_ep = "em_cluster_e/em_track_ptotal > ";
  TString base_prob = "em_cluster_prob >= ";

  //TString necessary to print legible legends//

  TString base_ep_short = "E/P > ";
  TString base_prob_short = "Prob >= ";
  char str_1[40];
  char str_2[40];
  sprintf(str_1,"%.2f", ep);
  sprintf(str_2,"%.3f", prob);

  //Test which cuts to use//

  bool do_ep=true; bool do_prob=true;
  if(ep==0)
    do_ep=false;
  if(prob==0)
    do_prob=false;
  
  //Add cuts to vector//

  if(do_prob&&!do_ep)
    {
      v_cuts_CEMC.push_back(TString(base_prob) += prob);
      v_cuts_CEMC_short.push_back((TString(base_prob_short)+=str_2)+" True Positive");
    }
  else if(do_ep&&!do_prob)
    {
      v_cuts_CEMC.push_back(TString(base_ep) += ep);
      v_cuts_CEMC_short.push_back((TString(base_ep_short)+=str_1)+" True Positive");
    }
  else
    {
      v_cuts_CEMC.push_back((TString(base_ep) += ep) + "&&" + (TString(base_prob) += prob));
      v_cuts_CEMC_short.push_back((TString(base_ep_short) += str_1) + "&&" +(TString(base_prob_short) += str_2));
    }
}

void add_EEMC_Cut(double ep)
{
  TString base_ep="em_cluster_e/";
  TString base_ep_short = "E/P > ";
  char str_1[40];
  sprintf(str_1,"%.2f", ep);

  v_cuts_EEMC.push_back(base_ep); // 1st element : 'em_cluster_e/'
  v_cuts_EEMC.push_back(TString(">")+= str_1); //: 'em_cluster_e/*GeV*>cut'

  v_cuts_EEMC_short.push_back(((TString(base_ep_short)) += str_1));
}

void add_FEMC_Cut(double ep)
{
  TString base_ep="em_cluster_e/";
  TString base_ep_short = "E/P > ";
  char str_1[40];
  sprintf(str_1,"%.2f", ep);

  v_cuts_FEMC.push_back(base_ep); // 1st element : 'em_cluster_e/'
  v_cuts_FEMC.push_back(TString(">")+= str_1); //: 'em_cluster_e/*GeV*>cut'

  v_cuts_FEMC_short.push_back(((TString(base_ep_short)) += str_1));
}
