#include "VtxRegressor.h"
#include <trackbase_historic/SvtxTrack.h>
using namespace TMVA;

VtxRegressor::VtxRegressor(std::string methodname, std::string pathname){
  reader = new Reader();
  reader->AddVariable("track1_pt",&pt1);
  reader->AddVariable("track2_pt",&pt2);
  reader->AddVariable("track1_phi",&phi);
  reader->AddVariable("track1_phi-track2_phi",&dphi);
  reader->AddVariable("track1_eta",&eta);
  reader->AddVariable("track1_eta-track2_eta",&deta);
  reader->AddVariable("vtx_radius",&rin);
  reader->BookMVA(methodname.c_str(),pathname.c_str());
  _method=methodname;
}

VtxRegressor::~VtxRegressor(){
  if (reader) delete reader;
  reader=NULL;
}
float VtxRegressor::regress(SvtxTrack* reco1,SvtxTrack* reco2,genfit::GFRaveVertex* recoVertex){
  pt1=reco1->get_pt();
  pt2=reco2->get_pt();
  phi=reco1->get_phi();
  dphi=reco1->get_phi()-reco2->get_phi();
  eta=reco1->get_eta();
  deta=reco1->get_eta()-reco2->get_eta();
  rin=recoVertex->getPos().Perp();
  return reader->EvaluateRegression(_method.c_str())[0];
}
