// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef ANA_PPG09_MOD_H
#define ANA_PPG09_MOD_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

//#include <calotrigger/TriggerAnalyzer.h>

class PHCompositeNode;
class TH1D;
class TH2D;
class Gl1Packet;

class Ana_PPG09_Mod : public SubsysReco
{
public:

  Ana_PPG09_Mod(
		const std::string &recojetname = "Ana_PPG09_Mod",
		const std::string &outputfilename = "PlaceHolder.root");

  ~Ana_PPG09_Mod() override;

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;
  int Reset(PHCompositeNode * /*topNode*/) override;
  void setRunnumber(int run) {m_runnumber = run;}
  
private:

  void initializePrescaleInformationFromDB(int runnumber);
  
  int getEventPrescale(float leadJetPt, std::vector<int> trigs);
  bool isTrigEfficient(int trigIndex, float leadJetPt);
  
  //TriggerAnalyzer *triggeranalyzer{nullptr};
  //Gl1Packet *gl1Packet{nullptr};
  std::string m_recoJetName;
  std::string m_outputFileName;

  double NComp_Cut = 1;
  double Lead_RPt_Cut = -999;
  double All_RPt_Cut = 5;
  double ZVtx_Cut = 30;

  static const int nTrigs = 8;
  
  //Jet Info
  TH3F *h_Eta_Phi_Pt_[nTrigs];

  TH1D *h_EventCount;
  TH1F *h_theJetSpectrum;

  //Caloriemter Tower Info
  TH3F *h_EMCal_Raw_Eta_Phi_E_[nTrigs];
  TH3F *h_iHCal_Raw_Eta_Phi_E_[nTrigs];
  TH3F *h_oHCal_Raw_Eta_Phi_E_[nTrigs];
  TH3F *h_EMCal_CS_Eta_Phi_E_[nTrigs];
  TH3F *h_iHCal_CS_Eta_Phi_E_[nTrigs];
  TH3F *h_oHCal_CS_Eta_Phi_E_[nTrigs];
  TH3F *h_EMCal_C_Eta_Phi_E_[nTrigs];
  TH3F *h_EMCal_CR_Eta_Phi_E_[nTrigs];
  TH3F *h_iHCal_C_Eta_Phi_E_[nTrigs];
  TH3F *h_oHCal_C_Eta_Phi_E_[nTrigs];
  TH3F *h_EMCal_Jet_Eta_Phi_E_[nTrigs];
  TH3F *h_iHCal_Jet_Eta_Phi_E_[nTrigs];
  TH3F *h_oHCal_Jet_Eta_Phi_E_[nTrigs];

  //Event Info
  TH2D *h_nJetsAboveThresh;
  TH1D *h_ZVtx_[nTrigs];
  TH3F *h_oHCal_TE_Sub_Eta_Phi_E_[nTrigs];
  TH3F *h_iHCal_TE_Sub_Eta_Phi_E_[nTrigs];
  TH3F *h_EMCal_TE_Sub_Eta_Phi_E_[nTrigs];

  int m_event = 0;
  int m_runnumber = 0;

  std::vector<std::string> trigNames = {
    "MBD N&S >= 1",
    "MBD N&S >= 1, vtx < 10 cm",
    "Jet 8 GeV + MBD NS >= 1",
    "Jet 10 GeV + MBD NS >= 1",
    "Jet 12 GeV + MBD NS >= 1",
    "Jet 8 GeV, MBD N&S >= 1, vtx < 10 cm",
    "Jet 10 GeV, MBD N&S >= 1, vtx < 10 cm",
    "Jet 12 GeV, MBD N&S >= 1, vtx < 10 cm"};

  
  std::vector<int> trigIndices = {10,12,17,18,19,33,34,35};
  std::map <int, int> trigToVecIndex{{trigIndices[0],0}, {trigIndices[1],1}, {trigIndices[2],2}, {trigIndices[3],3}, {trigIndices[4],4}, {trigIndices[5],5}, {trigIndices[6],6},{trigIndices[7],7}};

  std::vector<int> scaleDowns {};
  std::vector<int> liveCounts {};
  float jetBins[9] = {7,10,15,20,25,30,40,60,80};
  int nJetBins = 8;
  float trigCutOffs[nTrigs] = {1, 1, 16, 18,20, 16, 18, 20};
};

//jet eta cut stuff
float radius_OH = 225.87;
 float mineta_OH = -1.1;
 float maxeta_OH = 1.1;
 float minz_OH = -301.683;
 float maxz_OH = 301.683;

 float radius_IH = 127.503;
 float mineta_IH = -1.1;
 float maxeta_IH = 1.1;
 float minz_IH = -170.299;
 float maxz_IH = 170.299;

 float radius_EM = 93.5;
 float mineta_EM = -1.13381;
 float maxeta_EM = 1.13381;
 float minz_EM = -130.23;
 float maxz_EM = 130.23;

float get_ohcal_mineta_zcorrected(float zvertex) {
  float z = minz_OH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_OH);
  return eta_zcorrected;
}

float get_ohcal_maxeta_zcorrected(float zvertex) {
  float z = maxz_OH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_OH);
  return eta_zcorrected;
}

float get_ihcal_mineta_zcorrected(float zvertex) {
  float z = minz_IH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_IH);
  return eta_zcorrected;
}

float get_ihcal_maxeta_zcorrected(float zvertex) {
  float z = maxz_IH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_IH);
  return eta_zcorrected;
}

float get_emcal_mineta_zcorrected(float zvertex) {
  float z = minz_EM - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_EM);
  return eta_zcorrected;
}

float get_emcal_maxeta_zcorrected(float zvertex) {
  float z = maxz_EM - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_EM);
  return eta_zcorrected;
}

bool check_bad_jet_eta(float jet_eta, float zertex, float jet_radius) {
  float emcal_mineta = get_emcal_mineta_zcorrected(zertex);
  float emcal_maxeta = get_emcal_maxeta_zcorrected(zertex);
  float ihcal_mineta = get_ihcal_mineta_zcorrected(zertex);
  float ihcal_maxeta = get_ihcal_maxeta_zcorrected(zertex);
  float ohcal_mineta = get_ohcal_mineta_zcorrected(zertex);
  float ohcal_maxeta = get_ohcal_maxeta_zcorrected(zertex);
  float minlimit = emcal_mineta;
  if (ihcal_mineta > minlimit) minlimit = ihcal_mineta;
  if (ohcal_mineta > minlimit) minlimit = ohcal_mineta;
  float maxlimit = emcal_maxeta;
  if (ihcal_maxeta < maxlimit) maxlimit = ihcal_maxeta;
  if (ohcal_maxeta < maxlimit) maxlimit = ohcal_maxeta;
  minlimit += jet_radius;
  maxlimit -= jet_radius;
  return jet_eta < minlimit || jet_eta > maxlimit;
}

#endif // ANA_PPG09_MOD_H
