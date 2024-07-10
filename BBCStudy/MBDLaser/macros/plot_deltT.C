
// I need to KNOW how to  do somthing like TH1D* h1_mbdtt = hist->ProjectionY();
// nevents_tt += h1_mbdtt->Integral();



#include <fstream>
#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH2F.h>

// varaibles needed 
const int MAXRUNS = 20000;
TFile *savefile;
TFile *tfile[MAXRUNS];
TTree *t;
Double_t runindex[MAXRUNS];
TCanvas *ac[100];
TString dir;
TFile *refFile_all;

//TGraph
TGraph *graph;
TGraph *gt_chBych;
TGraph *gq_chBych;

//Time in all runs
TH2 *h2_mbdtt[MAXRUNS];
Double_t T_current;
TFile *refFile;
TH2 *h2_ref;
Double_t T_ref;
Double_t deltaT;

//charge in each ch 
TH1 *h_mbdq[MAXRUNS][128];
Double_t Q_current[MAXRUNS][128];
TH1 *h_ref_Q[128];
Double_t Q_ref[128];
Double_t Q_ratio[MAXRUNS][128];


//Time in each ch 
TH1 *h_mbdt[MAXRUNS][128];
//Double_t btmean[128][MAXRUNS];		// bt mean in each ch 
//Double_t btmeanerr[128][MAXRUNS];
Double_t T_current_all[MAXRUNS][128];
TH1 *h_ref_all[128];
Double_t T_ref_all[128];
Double_t deltaT_all[MAXRUNS][128];


//Canvas
TCanvas *g_Canvas;
TCanvas *gt_Canvas;
TCanvas *gq_Canvas;




//int Colors_maker[MAXRUNS]={kRed, kBlue, kGreen,kYellow, kMagenta, kCyan, kOrange, kSpring, kTeal, kAzure, kViolet, kPink, kGray, kBlack, kRed-1, kBlue-1, kGreen-1, kYellow-1, kMagenta-1, kCyan-1, kOrange-1, kSpring-1, kTeal-1, kAzure-1, kViolet-1};


void plot_deltT(const char *flistname = "analyzed_Laser_data2023_To_current.list")
{
    ifstream flist(flistname);
    TString dstfname;
    int nruns = 0;
    TString name;
    TString title;
    int icv = 0;
    savefile = new TFile(dir + "Tlaser.root", "RECREATE");

    //initialize TGraph 
    graph = new TGraph();
    gt_chBych = new TGraph();
    gq_chBych = new TGraph();

          // get the time of the laser in the reference run
          refFile = new TFile("LASER_UNCALMBD-00040759-0000.root", "READ");
          h2_ref = (TH2 *)refFile->Get("h2_mbdtt");
          T_ref = h2_ref->GetMean(1);

    while (flist >> dstfname)
    {
        tfile[nruns] = new TFile(dstfname, "READ");
        title = dstfname;
        cout << "Title = " << dstfname << endl;

        h2_mbdtt[nruns] = (TH2 *)tfile[nruns]->Get("h2_mbdtt");

        if (h2_mbdtt[nruns] != nullptr)
        {
          // get the time of the laser in the current run
          Double_t T_current = h2_mbdtt[nruns]->GetMean(1);


          // calculate delta-T
          deltaT = T_current - T_ref; // cout<< "deltaT = " << deltaT<< endl;

          // fill 
          graph->SetPoint(nruns, nruns, deltaT);


          //here you should write h_mbdt[128] and calculate the deltaT channels by channels 
          // to get the meean and the mean error from  North MBD/MBD time && South MBD/MBD time 
          for (int ipmt=0; ipmt<128; ipmt++)
          {
            // appending the value of ipmt to the string h_mbdq and then assigns it to the varaible name 
            name = "h_mbdt"; name += ipmt; // h_mbdt0, h_mbdt1, ... so on 

            h_mbdt[nruns][ipmt] = (TH1*)tfile[nruns]->Get(name);

            // btmean[ipmt][nruns] = h_mbdt[nruns][ipmt]->GetMean();
            // btmeanerr[ipmt][nruns] = h_mbdt[nruns][ipmt]->GetMeanError();

            name = "h_mbdq"; name += ipmt; 
            h_mbdq[nruns][ipmt] = (TH1*)tfile[nruns]->Get(name);


            //cout<< name<<" \t"<< btmean[ipmt][nruns]<< "\t"<< btmeanerr[ipmt][nruns]<<endl;

            if (h_mbdt[nruns][ipmt] != nullptr)
            {

              // get the time of the laser in the current run for each channls (we should have 128 values )
              T_current_all[nruns][ipmt] = h_mbdt[nruns][ipmt]->GetMean();  //cout<< T_current_all << "\t"<< name<< " \t" << ipmt<<endl;

              Q_current[nruns][ipmt] = h_mbdq[nruns][ipmt]->GetMean();

              // get the time of the laser in the reference run for each ch 
              //if ( refFile_all==nullptr ) refFile_all = new TFile("LASER_UNCALMBD-00041145-0000.root", "READ");
              if ( refFile_all==nullptr ) refFile_all = new TFile("LASER_UNCALMBD-00044961-0000.root", "READ");

              name = "h_mbdt"; name += ipmt;
              h_ref_all[ipmt] = (TH1*)refFile_all->Get(name);
              T_ref_all[ipmt] = h_ref_all[ipmt]->GetMean();


              name = "h_mbdq"; name += ipmt;
              h_ref_Q[ipmt] = (TH1*)refFile_all->Get(name);
              Q_ref[ipmt] = h_ref_Q[ipmt]->GetMean();



              //calculation 
              deltaT_all[nruns][ipmt] = T_current_all[nruns][ipmt] - T_ref_all[ipmt];

              Q_ratio[nruns][ipmt] = Q_current[nruns][ipmt] / Q_ref[ipmt];


              //cout<< "deltaT = " << deltaT_all[nruns][ipmt]<< " \t" <<"T_current in each ch = " << T_current_all[nruns][ipmt] << "\t" << "T_ref in each ch = "<<T_ref_all[ipmt] << " \t" << name <<"\t"<< nruns<< endl;

              if ( ipmt==0 || ipmt==127 ) cout<< "Q_ratio = " << Q_ratio[nruns][ipmt]<< " \t" <<"Q_current in each ch = " << Q_current[nruns][ipmt] << "\t" << "Q_ref in each ch = "<<Q_ref[ipmt] << " \t" << name <<"\t"<< nruns<< endl;

              //Fill 
              int n = gt_chBych->GetN();
              gt_chBych->SetPoint(n,nruns, deltaT_all[nruns][ipmt]);

              n = gq_chBych->GetN();
              gq_chBych->SetPoint(n,nruns, Q_ratio[nruns][ipmt] );

              // set the color maker
              //int makerColor = kRed + nruns % 24 ; // to use different color for each run ; 24 different colors 
              //int colors_maker_index = nruns % MAXRUNS;
              //gt_chBych->SetMarkerColor(Colors_maker[(ipmt) % 25]); // the remainder is used as index to slect the colors from color maker[] so the rang is 0 to 24 (to find the avalible colors)

              // maker color directly for each ch 
              /*   for ( int ipmt_col = 0; ipmt_col<128; ipmt_col++)
                   {
                   int channelColor = kRed +(nruns *128 + ipmt_col) % 24;
              //gt_chBych->SetMarkerColor(channelColor);

              }
              */
              //refFile_all->Close(); // I had to add it avoiding error like too many files open 
              //delete refFile_all;


            }

          }

        }
        else
        {
          cout << "Histogram 'h2_mbdtt' not found in file: " << dstfname << endl;
        }

        ac[icv] = new TCanvas(Form("h2_mbdtt_%d", nruns), Form("btt_vs_ch_%d", nruns), 1000, 500);
        h2_mbdtt[nruns]->SetTitle(title + nruns);
        h2_mbdtt[nruns]->GetXaxis()->SetTitle("ch");
        h2_mbdtt[nruns]->GetYaxis()->SetTitle("Time[ADC]");
        //h2_mbdtt[nruns]->Draw("colz");
        gPad->SetLogz(1);
        ac[icv]->SaveAs(Form("h2_mbdtt_%d.png", nruns));

        nruns++;
        icv++;



    }
    if(tfile[nruns] != nullptr) // to avoid such error like opening too many files 
    { 
      tfile[nruns]->Close();
      delete tfile[nruns];
    }


    g_Canvas = new TCanvas("deltaT_graph", "Delta-T vs Run Index ", 1000, 500);
    graph->SetTitle("Delta-T vs Run Index");
    graph->GetXaxis()->SetTitle("Run Index");
    graph->GetYaxis()->SetTitle("Delta-T[ns]");
    graph->SetMarkerSize(1);
    graph->SetMarkerStyle(20);
    graph->SetMarkerColor(kRed);
    graph->Draw("AP");
    g_Canvas->SaveAs("Delta-T_vs_Run_Index.png");


    gt_Canvas= new TCanvas("deltaT_graph_128ch", "Delta-T vs Run Index128 ch ", 1000, 500);
    gt_chBych->SetTitle("Delta-T vs Run Index");
    gt_chBych->GetXaxis()->SetTitle("Run Index");
    gt_chBych->GetYaxis()->SetTitle("Delta-T[ns]");
    gt_chBych->SetMarkerSize(0.2);
    gt_chBych->SetMarkerStyle(5);
    gt_chBych->SetMarkerColor(2.);
    gt_chBych->Draw("AP");
    gt_Canvas->SaveAs("Delta_T_vs_Run_Index128ch.png");


    gq_Canvas= new TCanvas("Q_ratio_graph_128ch", "Q_ratio vs Run Index128 ", 1000, 500);
    gq_chBych->SetTitle("Q_ratio vs Run Index");
    gq_chBych->GetXaxis()->SetTitle("Run Index");
    gq_chBych->GetYaxis()->SetTitle("Q_ratio[ADC]");
    gq_chBych->SetMarkerSize(0.2);
    gq_chBych->SetMarkerStyle(5);
    gq_chBych->SetMarkerColor(kBlue);
    gq_chBych->Draw("AP");
    gq_Canvas->SaveAs("Q_ratio_vs_Run_Index128ch.png");





    savefile->Write();
    savefile->Close();

    cout << "Processed " << nruns << " runs." << endl;
}
