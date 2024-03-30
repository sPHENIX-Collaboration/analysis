#ifndef __INTTEVENT_H__
#define __INTTEVENT_H__

//#include "TTree.h"
//#include "Riostream.h"
#include "TClonesArray.h"
//#include "TObject.h"

class InttHit : public TObject {
  public:
    InttHit();
    virtual ~InttHit(){};
//    InttHit(int Pid);

    void copy(InttHit& hit);
    void show(bool explanation_flag=false);

    virtual Bool_t IsEqual(const TObject *obj) const;
    virtual Bool_t IsSortable() const;
    virtual Int_t  Compare(const TObject* obj) const;

  public:
    int pid;

    int adc;
    int ampl;
    int chip_id;
    //fpga
    int module;
    
    int chan_id;
    //fem
    
    int bco;
    Long64_t bco_full;
    
    int evt;
    
    int roc;
    int barrel;
    int layer;
    int ladder;
    int arm;
    
    int full_fphx;
    int full_roc;

//  protected:
    ClassDef(InttHit, 1)
};

class InttEvent : public TObject {
  public:
    InttEvent();
    virtual ~InttEvent();

    InttHit* addHit();
    void     clear();
    void     copy(InttEvent* org);

    int      getNHits();
    InttHit* getHit(const int ihit);

    void     show();
    void     sort();

  //protected:
    int           evtSeq;
    Long64_t      bco;
    int           fNhits;
    TClonesArray* fhitArray;
 
    ClassDef(InttEvent, 2)

};

#endif
