// #include <TFile.h>
// #include <TTree.h>
// #include <TDirectory.h>

// // #include "InttEvent.cc"
// #include "ReadINTTDSTv1.C"
// #include "MBDReaderV2.C"

// #include <iostream>

// //R__LOAD_LIBRARY(libInttEvent.so)

// void sync_mbd_intt_DST()
// {
//   Int_t    evt;
//   Short_t  bns, bnn;
//   UShort_t clk, femclk;
//   Float_t  bqs, bqn, bts, btn, bz, bt0;


//   TDirectory* gDir = gDirectory;
//   ///////////
//   //

//   TFile *f_mbd  = TFile::Open("/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/centrality_run20869.root");
//   gDirectory=gDir;

//   TTree *t_mbd = (TTree*)f_mbd->Get("EventTree");
//   cout<<" "<<t_mbd<<endl;
//   if(!t_mbd) return ;

//   MBDReaderV2 mbdt(t_mbd);
//   cout<<"test1"<<endl;
//   //t_mbd->SetBranchAddress("evt", &evt);
//   //t_mbd->SetBranchAddress("bns", &bns);
//   //t_mbd->SetBranchAddress("bnn", &bnn);
//   //t_mbd->SetBranchAddress("clk", &clk);
//   //t_mbd->SetBranchAddress("femclk", &femclk);
//   //t_mbd->SetBranchAddress("bqs", &bqs);
//   //t_mbd->SetBranchAddress("bqn", &bqn);
//   //t_mbd->SetBranchAddress("bts", &bts);
//   //t_mbd->SetBranchAddress("btn", &btn);
//   //t_mbd->SetBranchAddress("bz",  &bz);
//   //t_mbd->SetBranchAddress("bt0", &bt0);

//   ///////////////////
//   TFile *f_intt = TFile::Open("/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/intt_run20869.root");
//   gDirectory=gDir;
//   TTree *t_intt = (TTree*)f_intt->Get("EventTree");
//   cout<<" "<<t_intt<<endl;
//   if(!t_intt) return ;

//   // InttEvent* inttEvt;
//   // t_intt->SetBranchAddress("event", &inttEvt);

//   ReadINTTDSTv1 * inttEvt = new ReadINTTDSTv1(t_intt);


//   cout<<t_mbd->GetEntries()<<" "<<t_intt->GetEntries()<<endl;
//   //cout<<t_mbd->GetEntries()<<endl;

//   gDirectory = gDir;

//   TH2F *h_qmbd_nintt = new TH2F("h_qmbd_nintt", "BbcQ vs Intt N", 200, 0, 20000, 200, 0, 4000);
//   TH2F *intt_mbd_bco = new TH2F("intt_mbd_bco", "INTT - MBD", 100, 0, 600000, 100, -10, 100000);
//   // TH2F *intt_mbd_bco = new TH2F("intt_mbd_bco", "INTT - MBD", 100, 0, 10000, 100, -10, 100000);
//   intt_mbd_bco->GetXaxis()->SetTitle("evt");
//   intt_mbd_bco->GetYaxis()->SetTitle("clock_diff");

//   TH1F *intt_mbd_bco_1D = new TH1F("intt_mbd_bco_1D", "INTT - MBD", 100, -10, 100000);
//   intt_mbd_bco_1D->GetXaxis()->SetTitle("clock_diff");
//   intt_mbd_bco_1D->GetYaxis()->SetTitle("entry");

//   int        prev_mbdclk = 0;
//   ULong64_t  prev_bco = 0;
  
//   bool found_firstevt=false;
//   int mbd_evt_offset = 0;
//   int intt_evt_offset = 0;
//   for(int i=0; i< t_mbd -> GetEntries(); i++){
//     mbdt.LoadTree(i+mbd_evt_offset);
//     mbdt.GetEntry(i+mbd_evt_offset);

//     int INTT_evtseq = inttEvt->event_counter;
//     int MBD_evtseq = mbdt.event;
//     // cout<<evtseq<<endl;
//     // if(evtseq==0) found_firstevt=true;
//     // if(!found_firstevt){
//     //   continue;
//     // }
//     // 

//     // int mbdevt = evtseq = mbd_evt_offset;
//     // cout<<"mbd: "<<mbdevt<<endl;
//     // mbdt.LoadTree(mbdevt);
//     // mbdt.GetEntry(mbdevt);



//     t_intt->GetEntry(i+intt_evt_offset);

//     // if((i%100)==0){
//     //   cout<<i<<" mbd: " <<mbdt.evt<<" "<<hex<<mbdt.clk<<" "<<mbdt.femclk<<dec<<" "<<mbdt.bns<<" "<<mbdt.bnn
//     //       <<" "<<mbdt.bqs<<" "<<mbdt.bqn<<" "<<mbdt.bz<<endl;
//     //   cout<<"intt: "<<inttEvt->evtSeq<<" "<<hex<<inttEvt->bco<<dec<<" "<<inttEvt->fNhits<<endl;
//     //   cout<<endl;
//     // }

//     float bbcq  = mbdt.MBD_charge_sum;
//     float nintt = inttEvt->NClus;

//     unsigned short mbdclk = mbdt.femclk;
//     ULong64_t bco         = inttEvt->INTT_BCO;
//     ULong64_t bco16       = bco&0xFFFF;

//     int       mbd_prvdif  = (mbdclk-prev_mbdclk)&0xFFFF;
//     ULong64_t intt_prvdif = bco-prev_bco;

//     if((i%1000)==0){cout<<i<<" "<<hex<<setw(6)<<mbdclk<<" "<<setw(6)<<bco16<<" (mbd-intt)"<<setw(6)<<((mbdclk-bco16)&0xFFFF)
//         <<"      (femclk-prev)"<<setw(6)<<mbd_prvdif<<" (bco-prev)"<<setw(6)<<intt_prvdif<<dec<<" INTT Evt: "<<INTT_evtseq<<" MBD Evt: "<<MBD_evtseq<<endl;}

    
//     if (mbdt.MBD_south_charge_sum > 200 && mbdt.MBD_north_charge_sum > 200)
//       h_qmbd_nintt->Fill(nintt, bbcq);
    
//     intt_mbd_bco -> Fill(i,(mbdclk-bco16)&0xFFFF);
//     intt_mbd_bco_1D -> Fill((mbdclk-bco16)&0xFFFF);

//     prev_mbdclk = mbdclk;
//     prev_bco    = bco;

//     t_intt->GetEntry(i+1+intt_evt_offset);
//     ULong64_t next_bco16       = (inttEvt->INTT_BCO)&0xFFFF;
//     mbdt.LoadTree(i+1);
//     mbdt.GetEntry(i+1);
//     unsigned short next_mbdclk = mbdt.femclk;
//     if ( ((next_mbdclk-next_bco16)&0xFFFF) != ((mbdclk-bco16)&0xFFFF)) intt_evt_offset += 1;

//     // if (i == 7012) intt_evt_offset += 1;

//     // if(i>200000) break;
//   }
  
  
//   TFile* froot = new TFile("/sphenix/user/ChengWei/DST_test_sync.root", "recreate");
//   h_qmbd_nintt->Write();
//   intt_mbd_bco->Write();
//   intt_mbd_bco_1D->Write();
//   froot->Close();
  




// /*

//   t->Draw("bz")
//   t->Draw("bz","abs(bz)<500")
//   t->Draw("bz>>h(500,-500,500)","abs(bz)<500")
//   t->Draw("bz>>h(2000,-500,500)","abs(bz)<500")
//   t->Scan("evt:femclk")
//   t->Scan("evt:femclk:clk")
//   tree->Scan("evtSeq:bco&0xFFFF")
//   .q
//   TFile *_file0 = TFile::Open("/sphenix/user/chiu/sphenix_bbc/run2023/beam_mbd-00020708-0000_mbd.root")
//   t->Print()
//   t->Draw("qs")
//   t->Draw("bqs:bqn")
//   t->Draw("bqs:bqn","","colz")
//   t->Draw("bqs:bqn>>h(200,0,2000,200,0,2000)","","colz")
//   t->Draw("bqs+bqn:evt>>h(200,0,200,100,0,2000)","evt<200")
//   tree->Scan("fNhits:evtSeq","evtSeq<100")
//   tree->Draw("fNhits:evtSeq","evtSeq<100")
//   .q
//   TFile *_file0 = TFile::Open("../../../../INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00020708-0000_event_base_ana.root")
//   tree->Draw("fNhits:evtSeq","evtSeq<100")
//   tree->SetMarkerStyle(20)
//   tree->Draw("fNhits:evtSeq","evtSeq<100")
//   t->Draw("bns+bnn:evt>>h(200,0,200,128,0,128)","evt<200")
//   t->Draw("bqs+bqn:evt>>h(200,0,200,100,0,2000)","evt<200")
//   t->Draw("bns+bnn:evt>>h(200,0,200,128,0,128)","evt<200")
//   t->Draw("bqs+bqn:evt>>h(200,0,200,100,0,2000)","evt<200")
//   .q
//   TFile *_file0 = TFile::Open("AnaTutorial.root")
//   ntp_cluspair->Draw("ang1-ang2:ang1>>h(300,-3,3,400, -0.5, 0.5)","nclus2<200","colz")
//   .q
//   TFile *_file0 = TFile::Open("AnaTutorial_run20869_newgeo.root")
//   ntp_cluspair->Draw("vy:vx>>h(200,-1,1,200, -1, 1","nclus2<200","colz")
//   ntp_cluspair->Draw("vy:vx>>h(200,-1,1,200, -1, 1","nclus2<200&&abs(ang1)<1.5","colz")
//   ntp_cluspair->Draw("vy:vx>>h1(200,-1,1,200, -1, 1","nclus2<200&&abs(ang1)>1.5","colz")
//   ntp_cluspair->Draw("vy:vx>>h(200,-1,1,200, -1, 1","nclus2<200&&abs(ang1)<1.5","colz")
//   ntp_cluspair->Draw("vy:vx>>h1(200,-1,1,200, -1, 1","nclus2<200&&abs(ang1)>1.5","colz")
//   ntp_clus->Draw("z")
//   c1->cd()
//   ntp_cluspair->Draw("vy:vx>>h(200,-1,1,200, -1, 1","nclus2<200&&abs(ang1)<1.5&&abs(ang1-ang2)<0.1","colz")
//   c1_n2->cd()
//   ntp_cluspair->Draw("vy:vx>>h(200,-1,1,200, -1, 1","nclus2<200&&abs(ang1)>1.5&&abs(ang1-ang2)<0.1","colz")
//   ntp_cluspair->Draw("vy:vx>>h1(200,-1,1,200, -1, 1","nclus2<200&&abs(ang1)>1.5&&abs(ang1-ang2)<0.1","colz")
//   c1->Cd()
//   c1->cd()
//   ntp_cluspair->Draw("vy:vx>>h(200,-1,1,200, -1, 1","nclus2<200&&abs(ang1)<1.5&&abs(ang1-ang2)<0.1","colz")
// */
// }


#include <TFile.h>
#include <TTree.h>
#include <TDirectory.h>
#include <iostream>


void sync_mbd_intt_DST()
{



  TDirectory* gDir = gDirectory;

  TFile *f_mbd  = TFile::Open("/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/centrality_run20869.root");
  gDirectory=gDir;

  TTree *t_mbd = (TTree*)f_mbd->Get("EventTree");
  cout<<" "<<t_mbd<<" Entry: "<<t_mbd -> GetEntries()<<endl;
  if(!t_mbd) return ;


  Int_t           MBD_event;
  UShort_t        femclk;
  Float_t         MBD_south_charge_sum;
  Float_t         MBD_north_charge_sum;
  
  t_mbd->SetBranchAddress("femclk", &femclk);
  t_mbd->SetBranchAddress("event", &MBD_event);
  t_mbd->SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);
  t_mbd->SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);

  TFile *f_intt = TFile::Open("/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/intt_run20869.root");
  gDirectory=gDir;
  TTree *t_intt = (TTree*)f_intt->Get("EventTree");
  cout<<" "<<t_intt<<" Entry: "<<t_intt->GetEntries()<<endl;
  if(!t_intt) return;

  Int_t           INTT_event_counter;
  ULong_t         INTT_BCO;
  Int_t           NClus;
  t_intt->SetBranchAddress("NClus", &NClus);
  t_intt->SetBranchAddress("INTT_BCO", &INTT_BCO);
  t_intt->SetBranchAddress("event_counter", &INTT_event_counter);

  gDirectory = gDir;

  TH2F *h_qmbd_nintt = new TH2F("h_qmbd_nintt", "BbcQ vs Intt N", 200, 0, 20000, 200, 0, 4000);
  TH2F *intt_mbd_bco = new TH2F("intt_mbd_bco", "INTT - MBD", 100, 0, 600000, 100, -10, 100000);
  intt_mbd_bco->GetXaxis()->SetTitle("evt");
  intt_mbd_bco->GetYaxis()->SetTitle("clock_diff");

  TH1F *intt_mbd_bco_1D = new TH1F("intt_mbd_bco_1D", "INTT - MBD", 100, -10, 100000);
  intt_mbd_bco_1D->GetXaxis()->SetTitle("clock_diff");
  intt_mbd_bco_1D->GetYaxis()->SetTitle("entry");

  int        prev_mbdclk = 0;
  ULong64_t  prev_bco = 0;
  
  bool found_firstevt=false;
  int mbd_evt_offset = 0;
  int intt_evt_offset = 0;
  for(int i=0; i< 10000; i++){
    t_mbd->GetEntry(i+mbd_evt_offset);

    int INTT_evtseq = INTT_event_counter;
    int MBD_evtseq = MBD_event;



    t_intt->GetEntry(i+intt_evt_offset);


    float bbcq  = MBD_north_charge_sum + MBD_south_charge_sum;
    float nintt = NClus;

    unsigned short mbdclk = femclk;
    ULong64_t bco         = INTT_BCO;
    ULong64_t bco16       = bco&0xFFFF;

    int       mbd_prvdif  = (mbdclk-prev_mbdclk)&0xFFFF;
    ULong64_t intt_prvdif = bco-prev_bco;

    if((i%1000)==0){cout<<i<<" "<<hex<<setw(6)<<mbdclk<<" "<<setw(6)<<bco16<<" (mbd-intt)"<<setw(6)<<((mbdclk-bco16)&0xFFFF)
        <<"      (femclk-prev)"<<setw(6)<<mbd_prvdif<<" (bco-prev)"<<setw(6)<<intt_prvdif<<dec<<" INTT Evt: "<<INTT_evtseq<<" MBD Evt: "<<MBD_evtseq<<endl;}

    
    if (MBD_south_charge_sum > 200 && MBD_north_charge_sum > 200)
      h_qmbd_nintt->Fill(nintt, bbcq);
    
    intt_mbd_bco -> Fill(i,(mbdclk-bco16)&0xFFFF);
    intt_mbd_bco_1D -> Fill((mbdclk-bco16)&0xFFFF);

    prev_mbdclk = mbdclk;
    prev_bco    = bco;

    t_intt->GetEntry(i+1+intt_evt_offset);
    ULong64_t next_bco16       = (INTT_BCO)&0xFFFF;
    t_mbd -> GetEntry(i+1);
    unsigned short next_mbdclk = femclk;
    if ( ((next_mbdclk-next_bco16)&0xFFFF) != ((mbdclk-bco16)&0xFFFF)) intt_evt_offset += 1;

  }
  
  
  TFile* froot = new TFile("/sphenix/user/ChengWei/DST_test_sync_test.root", "recreate");
  h_qmbd_nintt->Write();
  intt_mbd_bco->Write();
  intt_mbd_bco_1D->Write();
  froot->Close();
}
