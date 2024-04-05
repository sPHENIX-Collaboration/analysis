
#include "InttEvent.h"
//#include "InttCluster.h"
//#include "InttEventSync.h"

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TH1.h>
#include <TDirectory.h>

using namespace std;


InttEvent* inttEvt = nullptr;

TFile* outtree_file = nullptr;

TH1F* h_adc;
TH1F* h_adc1;
TH1F* h_adc2;
TH1F* h_nclus;
TH1F* h_nbcofull;
TH1F* h_clussize;
TH1F* h_bcodiff;
TH1F* h_bcodiff2;
TNtuple* h_clustree;
TTree* h_evttree;

TTree* h_mergedtree;
InttEvent* mergedInttEvt=NULL;


// variable for evttree
int pid, evt, nclus[8][14], nhits[8][14];
uint64_t bco_full;


//////////////////////////////////////////////////
//
int init_done=0;

int InitAnalysis(const char* outRootfile)
{
  if (init_done) return 1;
  init_done = 1;

  TDirectory* gDir = gDirectory;

/*
  outtree_file = TFile::Open(outRootfile, "recreate");

  h_adc  = new TH1F("h_adc", "adc", 100, 0, 300);
  h_adc1 = new TH1F("h_adc1", "adc1", 100, 0, 300);
  h_adc2 = new TH1F("h_adc2", "adc2", 100, 0, 300);
  h_nclus = new TH1F("h_nclus", "nclus", 100, 0, 2000);
  h_nbcofull = new TH1F("h_nbcofull", "nbcofull", 100, 0, 100);
  h_clussize = new TH1F("h_clussize", "cluster size", 100, 0, 100);
  h_bcodiff  = new TH1F("h_bcodiff", "bcodiff", 1000, -10000000, 10000000);
  h_bcodiff2 = new TH1F("h_bcodiff2", "bcodiff2", 1000, -500, 500);

  h_clustree = new TNtuple("h_clustree", "cluster tree", "nclus:size:adc:module:chip_id:chan_id:bco_full:roc:barrel:layer:ladder:arm:pid");

  h_evttree = new TTree("h_evttree", "Event tree");

  h_evttree->Branch("pid",      &pid, "pid/I");
  h_evttree->Branch("bco_full", &bco_full, "bco_full/l");
  h_evttree->Branch("event",    &evt,   "event/I");
  h_evttree->Branch("nclus",     nclus, "nclus[8][14]/I");
  h_evttree->Branch("nhits",     nhits, "nhits[8][14]/I");


  mergedInttEvt = new InttEvent();

  h_mergedtree = new TTree("tree", "Merged InttEvent");
  //h_mergedtree->Branch("event", "InttEvent", &inttEvt, 8000, 99);
  h_mergedtree->Branch("event", "InttEvent", &mergedInttEvt, 8000, 99);
*/
  

  gDirectory = gDir;

  return 0;
}

int Process_event (InttEvent* inttEvent)
{

/*
  InttClusterList* inttClusterList = new InttClusterList;

  //Clustering(inttEvent, inttClusterList);

  ////////////////////////////////////
  // analysis
  //
  int nHits = inttEvent->getNHits();
  map<Long64_t, int> vbcocount;
  for(int ihit = 0; ihit<nHits; ihit++)
  {
      Long64_t bco_f = inttEvent->getHit(ihit)->bco_full;
      auto itrBC = vbcocount.find(bco_f);
      if(itrBC==vbcocount.end()){
        vbcocount.insert(make_pair(bco_f, 0));
      }
      else {
        itrBC->second++;
      }
  }
  //--cout<<"nbco : "<<vbcocount.size()<<endl;
  h_nbcofull->Fill(vbcocount.size());
  Long64_t bco_main=0;
  int ncount_max=0;
  for(auto itrBC = vbcocount.begin(); itrBC!=vbcocount.end(); ++itrBC)
  {
      if(itrBC->second >= ncount_max) {
        ncount_max=itrBC->second;
        bco_main = itrBC->first;
      }
      //--cout<<"    "<<itrBC->first<<" "<<itrBC->second<<endl;
  }
  for(auto itrBC = vbcocount.begin(); itrBC!=vbcocount.end(); ++itrBC)
  {
     Long64_t bco_diff = itrBC->first - bco_main;
     //--cout<<"    diff= "<<bco_diff<<endl;
     h_bcodiff->Fill(bco_diff, itrBC->second);
     h_bcodiff2->Fill(bco_diff, itrBC->second);
  }

  
  // hit analysis
  for(int ipid=0; ipid<8; ipid++){ 
    for(int ilad=0; ilad<14; ilad++){ nhits[ipid][ilad]=0;}
  }
  for(int ihit = 0; ihit<nHits; ihit++)
  {
      int pid    = inttEvent->getHit(ihit)->pid;
      int module = inttEvent->getHit(ihit)->module;

      if(pid<3001||3008<pid) { cout<<"invalid pid: "<<pid<<endl; continue;}
      if(module<0||14<module){ cout<<"invalid module: "<<module<<endl; continue;}
      nhits[pid-3001][module]++;
  }


  //////////////////////////////
  // cluster analysis
  int nClusters = inttClusterList->getNhits();
  h_nclus->Fill(nClusters);

  //--cout<<"Nclus: "<<nClusters<<endl;
  float buf[12];
  for(int ipid=0; ipid<8; ipid++){ 
    for(int ilad=0; ilad<14; ilad++){ nclus[ipid][ilad]=0;}
  }
  pid=0;

  for(int iclus = 0; iclus<nClusters; iclus++)
  {
      InttCluster* clus = inttClusterList->getCluster(iclus);
      if(clus==nullptr) {cout<<"Null cluster : "<<iclus<<endl; continue; }

      h_adc->Fill(clus->adc);
      if(clus->nhits==1) h_adc1->Fill(clus->adc);
      if(clus->nhits>=2) h_adc2->Fill(clus->adc);

      h_clussize->Fill(clus->nhits);

      // "nclus:size:adc:module:chip_id:chan_id:bco_full:roc:barrel:layer:ladder:arm:pid");
      buf[ 0] = nClusters;
      buf[ 1] = clus->nhits;
      buf[ 2] = clus->adc;
      buf[ 3] = clus->module;
      buf[ 4] = clus->chip_id;
      buf[ 5] = clus->ch;
      buf[ 6] = clus->bco_full;
      buf[ 7] = clus->roc;
      buf[ 8] = clus->barrel;
      buf[ 9] = clus->layer;
      buf[10] = clus->ladder;
      buf[11] = clus->arm;
      buf[12] = clus->pid;

      h_clustree->Fill(buf);

      if(iclus==0){
          pid = clus->pid;
          bco_full = clus->bco_full;
      }

      nclus[clus->pid-3001][clus->module]++;
  }
  //
  ////////////////////////////////////
  //
  h_evttree->Fill();

  mergedInttEvt->clear();
  mergedInttEvt->copy(inttEvent);
  h_mergedtree->Fill();
  

  delete inttClusterList;
*/

  return 0;
}

int RunAnalysis(const char *rootFileList)
{
  TDirectory* gDir = gDirectory;

  cout<<"file : "<<rootFileList<<endl;

  //TDirectory* gDir = gDirectory;
  TFile *f_intt = TFile::Open("/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00020708-0000_event_base_ana.root");
  gDirectory = gDir;
  TTree *t_intt = (TTree*)f_intt->Get("tree");
  cout<<" "<<t_intt<<endl;
  if(!t_intt) return -1;

  InttEvent* inttEvt;
  t_intt->SetBranchAddress("event", &inttEvt);

  ///////////

  TFile *f_mbd  = TFile::Open("/sphenix/user/chiu/sphenix_bbc/run2023/beam_mbd-00020708-0000_mbd.root");

  //gDirectory = gDir;
  TTree *t_mbd = (TTree*)f_mbd->Get("t");
  cout<<" "<<t_mbd<<endl;
  if(!t_mbd) return -1;

  Int_t    evt;
  Short_t  bns, bnn;
  UShort_t clk, femclk;
  Float_t  bqs, bqn, bts, btn, bz, bt0;

  t_mbd->SetBranchAddress("evt", &evt);
  t_mbd->SetBranchAddress("bns", &bns);
  t_mbd->SetBranchAddress("bnn", &bnn);
  t_mbd->SetBranchAddress("clk", &clk);
  t_mbd->SetBranchAddress("femclk", &femclk);
  t_mbd->SetBranchAddress("bqs", &bqs);
  t_mbd->SetBranchAddress("bqn", &bqn);
  t_mbd->SetBranchAddress("bts", &bts);
  t_mbd->SetBranchAddress("btn", &btn);
  t_mbd->SetBranchAddress("bz",  &bz);
  t_mbd->SetBranchAddress("bt0", &bt0);

  ///////////////////

  //gDirectory = gDir;
  //
  cout<<t_intt->GetEntries()<<" "<<t_mbd->GetEntries()<<endl;

  bool found_firstevt=false;
  int mbd_evt_offset = 2;
  for(int i=0; i<t_mbd->GetEntriesFast(); i++){
	  t_mbd->GetEntry(i);
	  t_intt->GetEntry(i);
	  //-- cout<<"intt: "<<inttEvt->evtSeq<<" "<<hex<<inttEvt->bco<<dec<<" "<<inttEvt->fNhits<<endl;

	  //-- int evtseq = inttEvt->evtSeq;
	  //-- cout<<evtseq<<endl;
	  //-- if(evtseq==0) found_firstevt=true;
	  //-- if(!found_firstevt){
	  //--   continue;
	  //-- }
	  //-- 

	  //-- int mbdevt = evtseq = mbd_evt_offset;
	  //-- cout<<"mbd: "<<mbdevt<<endl;
	  //t_mbd->LoadEntry(i);
	  //t_mbd->GetEntry(mbdevt);

	  cout<<i<<" mbd: " <<evt<<" "<<hex<<clk<<" "<<femclk<<dec<<" "<<bns<<" "<<bnn<<" "<<bqs<<" "<<bqn<<" "<<bz<<endl;
	  cout<<endl;

	  if(i>10) break;
  }
  

  //ifstream fin(rootFileList);
  //if(!fin) {
  //    cout<<"failed to open file : "<<rootFileList<<endl;
  //    return -1;
  //}


/*
  InttEventSync inttEvtSync;

  vector<TFile*> vTFile;
  char filename[2048];
  int ind=0;
  //while(!fin.eof())
  while(fin>>filename)
  {
      TFile* tree_file = TFile::Open(filename);
      vTFile.push_back(tree_file);
      gDirectory = gDir;

      cout<<ind<<" "<<filename<<" "<<tree_file->GetName()<<endl;

      TTree* tree = (TTree*)tree_file->Get("tree");
      inttEvtSync.addData(tree);


      ind++;
  }
  fin.close();
*/


/*

  //read the Tree
  int ievt=0;
  InttEvent* inttevt = NULL;
  while( (inttEvt = inttEvtSync.getNextEvent()) )
  {
    if((ievt%10000)==0) cout<<"Event : "<<inttEvt->evtSeq<<endl;
    //inttEvt->sort();
    //inttEvt->show();
    
    Process_event(inttEvt);
    

    delete inttEvt;
    inttEvt = NULL;
    if(ievt>=100000) {
        cout<<"Max event 100000. quit"<<endl;
        break;
    }
    ievt++;
  }
  

  //delete hfile;
  for(auto fileItr=vTFile.begin(); fileItr!=vTFile.end(); ++fileItr)
  {
      (*fileItr)->Close();
  }
  vTFile.clear();

  h_adc->Print();

  //  tree->Write();
  outtree_file->cd();
  outtree_file->Write();
  outtree_file->Close();

*/

  return 0;
}

