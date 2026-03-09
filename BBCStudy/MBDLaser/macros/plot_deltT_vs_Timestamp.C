#include <iostream>
#include <fstream>
#include <string>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSystem.h>
#include <TDatime.h>
#include <fstream>
#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH2F.h>
#include <TDatime.h>

#include <fun4all/Fun4AllUtils.h>
R__LOAD_LIBRARY(libfun4all.so)

  // varaibles needed 
  const int MAXRUNS = 20000;
  TFile *savefile;
  TFile *tfile[MAXRUNS];
  TTree *t;
  Double_t runindex[MAXRUNS];
  TCanvas *ac[100];
  TFile *refFile_all;
  TString dir;

  //declare TGraph
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


  //set of variable to get the time from database 
  UInt_t runSeconds; //Double_t runSeconds; this wrong insted I should use UInt_t 
  Int_t startSeconds;

  //Canvas
  TCanvas *g_Canvas;
  TCanvas *gt_Canvas;
  TCanvas *gq_Canvas;

unsigned int get_time( Double_t run_number)
{
  // connect to the database
  TString servername = "pgsql://sphnxdaqdbreplica.sdcc.bnl.gov:5432/daq";
  TString username = "phnxrc";
  TString password = "";
  TSQLServer* dbserver = TSQLServer::Connect(servername, username, password);

  if (!dbserver)
  {
    cout << "Failed to connect " << endl;
    return -1;
  }

  // check the connection status
  if (dbserver->IsConnected())
  {
    cout << "Connected to the db" <<  endl;
  }
  else
  {
    cout << "Failed to connect to db" <<  endl;
    return -1;
  }

  TDatime dt;

  // query the database for the timestamp using the run_number number; constructs a SQL query to retriveve the ctime column from the filelist table in db, basically the query searches for rows where the filename column contains the extracted sRUN_number then stored the query result in res variable
  TString query = "SELECT ctime FROM filelist WHERE filename LIKE '%";
  query += run_number;
  query += "%';";


  TSQLResult* res = dbserver->Query(query);

  //checking that the query is not null (res !=nullptr), then if there is a result it retieves the next row using res->Next() function and then if a row exisit it retrieves the value of the first filed in the row which is index 0 and stores it in the timestampe variable
  if (res==nullptr)
  {
    cout << "Failed to query the database for runnumber " << run_number <<  endl;
    return 0;
  }

  TSQLRow* row = res->Next();
  if (row==nullptr)
  {
    cout << "No entry found in the database for runnumber " << run_number <<  endl;
    delete res;
    return 0;
  }

  dt.Set( row->GetField(0) );

  // checking
  //cout << "Timestamp for run " << run_number << ": " << dt.AsString() << "\t"<< " field = " <<  row->GetField(0) << endl;

  delete row;
  delete res;

  // disconnect from the database as Mickey asked
  dbserver->Close();
  delete dbserver;

  return dt.Convert();

}

void plot_deltT_vs_time(const TDatime& dt, const char *flistname = "analyzed_Laser_data2023_To_current.list")
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


  // start time (Tzero) is the timestamp from the test_ctime function
  TDatime startTime(dt);

  // calculate the time in seconds since the start time based on the timestamp from the test_ctime function
  TDatime runTime;
  startTime.Convert();

  // looping over all the root file in the list and doing the analysis  
  while (flist >> dstfname)
  {
    tfile[nruns] = new TFile(dstfname, "READ");
    title = dstfname;
    // cout << "Title = " << dstfname << endl;

    h2_mbdtt[nruns] = (TH2 *)tfile[nruns]->Get("h2_mbdtt");

    if (h2_mbdtt[nruns] != nullptr)
    {
      // get the time of the laser in the current run
      T_current = h2_mbdtt[nruns]->GetMean(1);

      // get the time of the laser in the reference run
      refFile = new TFile("LASER_UNCALMBD-00040759-0000.root", "READ");
      h2_ref = (TH2 *)refFile->Get("h2_mbdtt");
      T_ref = h2_ref->GetMean(1);

      // calculate delta-T to feed the TGraph Y-axis 
      deltaT = T_current - T_ref;

      //seting the runTime  
      runTime.Set(dt.Convert() + nruns * 86400); // add one day (86400 seconds) for each run


      //seting the run_number using the GetRunSegment() function in Fun4AllUtils class which returns a pair of integers and assigned the first elements of this pair to run_number 
      pair<int, int> runseg = Fun4AllUtils::GetRunSegment(dstfname.Data());
      int run_number = runseg.first;



      //calling the get_time function 
      runSeconds = get_time( run_number); // this function return time and pass run number


      //cout<< " runsSconds = " << runSeconds << "\t" << "dt.convert = " << dt.Convert() << "\t" << " runTime.Convert =" << runTime.Convert()<< endl;

      // skip if the time can't be found
      if ( runSeconds==0 )
      {
        cout << "ERROR, no timestamp for run " << run_number << endl;
        continue;
      }

      //fill 
      graph->SetPoint(nruns,(Double_t)runSeconds, deltaT);

      delete refFile;

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
          T_current_all[nruns][ipmt] = h_mbdt[nruns][ipmt]->GetMean(1);  //cout<< T_current_all << "\t"<< name<< " \t" << ipmt<<endl;

          Q_current[nruns][ipmt] = h_mbdq[nruns][ipmt]->GetMean(1);

          // get the time of the laser in the reference run for each ch 
          refFile_all = new TFile("LASER_UNCALMBD-00041145-0000.root", "READ");

          name = "h_mbdt"; name += ipmt;
          h_ref_all[ipmt] = (TH1*)refFile_all->Get(name);
          T_ref_all[ipmt] = h_ref_all[ipmt]->GetMean(1);


          name = "h_mbdq"; name += ipmt;
          h_ref_Q[ipmt] = (TH1*)refFile_all->Get(name);
          Q_ref[ipmt] = h_ref_Q[ipmt]->GetMean(1);



          //calculation 
          deltaT_all[nruns][ipmt] = T_current_all[nruns][ipmt] - T_ref_all[ipmt];

          Q_ratio[nruns][ipmt] = Q_current[nruns][ipmt] / Q_ref[ipmt];


          //cout<< "deltaT = " << deltaT_all[nruns][ipmt]<< " \t" <<"T_current in each ch = " << T_current_all[nruns][ipmt] << "\t" << "T_ref in each ch = "<<T_ref_all[ipmt] << " \t" << name <<"\t"<< nruns<< endl;

         // cout<< "Q_ratio = " << Q_ratio[nruns][ipmt]<< " \t" <<"Q_current in each ch = " << Q_current[nruns][ipmt] << "\t" << "Q_ref in each ch = "<<Q_ref[ipmt] << " \t" << name <<"\t"<< nruns<< endl;

          //Fill 
          gt_chBych->SetPoint(nruns*128+ipmt,(Double_t)runSeconds, deltaT_all[nruns][ipmt]);

          gq_chBych->SetPoint(nruns*128+ipmt,(Double_t)runSeconds, Q_ratio[nruns][ipmt] );

          refFile_all->Close(); // I had to add it avoiding error like too many files open 
          delete refFile_all;


        }

      }


    }
    else
    {
      cout << "Histogram 'h2_mbdtt' not found in file: " << dstfname << endl;
    }

    nruns++;
    icv++;

  }
  if(tfile[nruns] != nullptr) // to avoid such error like opening too many files 
  { 
    tfile[nruns]->Close();
    delete tfile[nruns];
  }


  // Create the TGraph
  g_Canvas = new TCanvas("deltaT_graph", "Delta-T vs Time", 1000, 500);
  g_Canvas->cd();
  g_Canvas->Update();
  graph->SetTitle("Delta-T vs Date");
  graph->GetXaxis()->SetTimeDisplay(1);
  //graph->GetXaxis()->SetLabelSize(0.02);
 // graph->GetXaxis()->SetTimeFormat("#splitline{%d/%m}{%Y}");
  graph->GetXaxis()->SetTimeFormat("%m/%d/%Y ");
 // graph->GetXaxis()->SetLabelOffset(0.15);
  graph->GetXaxis()->SetLabelSize(0.02);
  graph->GetXaxis()->SetTimeOffset(0,"gmt");
  graph->GetXaxis()->SetTitle("Date");
  graph->GetYaxis()->SetTitle("Delta-T[ns]");
  graph->SetMarkerSize(0.6);
  graph->SetMarkerStyle(20);
  graph->SetMarkerColor(2.);
  graph->Draw("AP");
  g_Canvas->SaveAs("Delta-T_vs_Time.png");
  


  gt_Canvas= new TCanvas("deltaT_graph_128ch", "Delta-T vs Date for 128 ch ", 1000, 500);
  gt_chBych->SetTitle("Delta-T vs Date");
  gt_chBych->GetXaxis()->SetTitle("Date");
  gt_chBych->GetYaxis()->SetTitle("Delta-T[ns]");
  gt_chBych->GetXaxis()->SetTimeDisplay(1);
  gt_chBych->GetXaxis()->SetTimeFormat("%m/%d/%Y ");
  gt_chBych->GetXaxis()->SetLabelSize(0.02);
  gt_chBych->SetFillColor(kBlue);
  // add space between the x-axis labels to move it vartcal
  //gt_graph->GetXaxis()->SetLabelOffset(0.08);
  gt_chBych->GetXaxis()->SetTimeOffset(0,"gmt");
  gt_chBych->SetMarkerSize(0.2);
  gt_chBych->SetMarkerStyle(5);
  gt_chBych->SetMarkerColor(2.);
  gt_chBych->Draw("AP");
  gt_Canvas->SaveAs("Delta-T_vs_Time128ch.png");
  



  gq_Canvas= new TCanvas("Q_ratio_graph_128ch", "Q_ratio vs Date for 128 ", 1000, 500);
  gq_chBych->SetTitle("Q_ratio vs Date");
  gq_chBych->GetXaxis()->SetTitle("Date");
  gq_chBych->GetYaxis()->SetTitle("Q_ratio[ADC]");
  gq_chBych->GetXaxis()->SetTimeDisplay(1);
  gq_chBych->GetXaxis()->SetTimeFormat("%Y/%m/%d ");
  gq_chBych->GetXaxis()->SetLabelSize(0.02);
  gq_chBych->GetXaxis()->SetTimeOffset(0,"gmt");
  gq_chBych->SetMarkerSize(0.2);
  gq_chBych->SetMarkerStyle(5);
  gq_chBych->SetMarkerColor(kBlue);
  gq_chBych->Draw("AP");
  gq_Canvas->SaveAs("Q_ration_vs_Time128ch.png");
  


  gPad->Modified();
  savefile->Write();
  savefile->Close();

  cout << "Processed " << nruns << " runs." << endl;
}

int plot_deltT_vs_Timestamp()
{  Double_t run_number;
  TDatime dt;
  get_time(run_number);
  plot_deltT_vs_time(dt);
  return 0;
}
