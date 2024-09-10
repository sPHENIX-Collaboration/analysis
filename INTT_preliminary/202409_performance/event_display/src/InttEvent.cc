#include "InttEvent.h"
#include "TClonesArray.h"

#include <iostream>
#include <iomanip>

ClassImp(InttHit)
    ClassImp(InttEvent)


using namespace std;

static const int NHITS_INIT = 500;

/////////////////////
InttHit::InttHit() {
  //cout<<"ctor InttHit"<<endl;
}

void InttHit::copy(InttHit& hit){
//  //cout<<"copy: "<<hit.pid<<" : "<<endl;
  pid       = hit.pid;
//  cout<<"copy t1"<<endl;
  adc       = hit.adc;
  ampl      = hit.ampl;
  chip_id   = hit.chip_id;
  module    = hit.module;
          
//  cout<<"copy1"<<endl;
  chan_id   = hit.chan_id;
  bco       = hit.bco;
  bco_full  = hit.bco_full;

//  cout<<"copy2"<<endl;
  evt       = hit.evt;
         
  roc       = hit.roc;
  barrel    = hit.barrel;
  layer     = hit.layer;
  ladder    = hit.ladder;
  arm       = hit.arm;
        
//  cout<<"copy3"<<endl;
  full_fphx = hit.full_fphx;
  full_roc  = hit.full_roc;
//  cout<<"copy end"<<endl;
}

void InttHit::show(bool explanation_flag){
  if(explanation_flag){
    cout<<"   module chip_id chan_id adc ampl";
    cout<<endl;
  }
  cout<<"   ";
  cout<<setw(6)<<module<<" ";
  cout<<setw(6)<<chip_id<<" ";
  cout<<setw(6)<<chan_id<<" ";
  cout<<setw(3)<<adc<<" ";
  cout<<setw(4)<<ampl<<" ";
  cout<<endl;
}


Bool_t InttHit::IsEqual(const TObject *obj) const
{
    const InttHit* objcp = dynamic_cast<const InttHit*>(obj);
    bool ismodule= (module == (objcp->module));
    bool ischip  = (chip_id== (objcp->chip_id));
    bool ischan  = (chan_id== (objcp->chan_id));
    bool isadc   = (adc    == (objcp->adc));

    return ismodule&&ischip&&ischan&&isadc;
}

Bool_t InttHit::IsSortable() const { return kTRUE;}

Int_t  InttHit::Compare(const TObject* obj) const {
    const InttHit* objcp = dynamic_cast<const InttHit*>(obj);
    if(     module < objcp->module) return -1;
    else if(module > objcp->module) return  1;
    else {
      if(     chip_id < objcp->chip_id)  return -1;
      else if(chip_id > objcp->chip_id)  return  1;
      else {
        if(     chan_id < objcp->chan_id)  return -1;
        else if(chan_id > objcp->chan_id)  return  1;

        return 0;
      }
    }
}

/////////////////////

InttEvent::InttEvent(): evtSeq(0), bco(0), fNhits(0), fhitArray(NULL) {
  fhitArray = new TClonesArray("InttHit", NHITS_INIT);
  cout<<"ctor InttEvent"<<endl;
}

InttEvent::~InttEvent(){
  if(fhitArray!=NULL) delete fhitArray;
  cout<<"dtor InttEvent"<<endl;
}

InttHit* InttEvent::addHit(){
  //cout<<"nhits: "<<fNhits<<endl;
  TClonesArray& hitArray = *fhitArray;
  InttHit* hitnew = new(hitArray[fNhits++]) InttHit();

  //cout<<"nhits: before return "<<fNhits<<endl;
  return hitnew;
}

int InttEvent::getNHits(){
  return fNhits;//m_hitArray->GetEntries();
}

InttHit* InttEvent::getHit(const int ihit){
  return (ihit<getNHits()) ? (InttHit*)fhitArray->UncheckedAt(ihit) : NULL;
}

void InttEvent::clear() {
  fhitArray->Clear(); 
  fNhits=0;
  evtSeq=0;
  bco=0;
};

void InttEvent::copy(InttEvent* org) {
  if(org==NULL) return;

  clear();

  evtSeq = org->evtSeq;
  bco    = org->bco;

  for(int ihit=0; ihit<org->getNHits(); ihit++){
      InttHit* hitnew = addHit();
      InttHit* hit    = org->getHit(ihit);
      hitnew->copy(*hit);
      //cout<<"debug hit: "<<endl;
      //hitnew->show();
      //hit->show();
  }
};

void InttEvent::show() {
  cout<<"Evt : "<<evtSeq<<" 0x"<<hex<<bco<<dec<<endl;
  cout<<"  Nhits : "<<fNhits<<endl;

  for(int ihit=0; ihit<fNhits; ihit++){
    InttHit* hit = getHit(ihit);
    hit->show((ihit==0));
  }
};

void InttEvent::sort() {
  fhitArray->Sort();
};
