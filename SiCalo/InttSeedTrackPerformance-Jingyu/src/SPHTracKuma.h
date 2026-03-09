/*
  Kuma track for RHIC-sPHENIX
*/
#ifndef  SPHTRACKUMA_H
#define  SPHTRACKUMA_H

#include <Rtypes.h>
class tracKuma
{
  public:
    tracKuma() :
      m_electronIs(false),
      m_charge(0),
      m_phi(0),
      m_theta(0),
      m_energy(0),
      m_p(0),
      m_pt(0),
      m_saggitaR(0),
      m_saggitaX(0),
      m_saggitaY(0),
      m_refPhi(0)
  {
    // 0: vertex, 1-3: MVTX, 4-5: INTT, 6: Cal
    for(unsigned int layId = 0; layId < 7; layId++){
      m_hitIs[layId] = false;
      m_hitR[layId] = 0.;
      m_hitZ[layId] = 0.;
      m_hitPhi[layId] = 0.;
      m_hitTheta[layId] = 0.;
    }
  };
  ~tracKuma() {};
    void setHitIs(Int_t layId, Bool_t is)
    {
        m_hitIs[layId] = is;
    }
    void setHitR(Int_t layId, Double_t r)
    {
        m_hitR[layId] = r;
    }
    void setHitZ(Int_t layId, Double_t z)
    {
        m_hitZ[layId] = z;
    }
    void setHitPhi(Int_t layId, Double_t phi)
    {
        m_hitPhi[layId] = phi;
    }
    void setHitTheta(Int_t layId, Double_t theta)
    {
        m_hitTheta[layId] = theta;
    }

    void setTrackElectronIs(bool electronIs)
    {
        m_electronIs = electronIs;
    }
    void setTrackCharge(Int_t pCharge)
    {
        m_charge = pCharge;
    }
    void setTrackPhi(Double_t phi)
    {
        m_phi = phi;
    }
    void setTrackTheta(Double_t theta)
    {
        m_theta = theta;
    }
    void setTrackE(Double_t energy)
    {
        m_energy = energy;
    }
    void setTrackP(Double_t p)
    {
        m_p = p;
    }
    void setTrackPt(Double_t pt)
    {
        m_pt = pt;
    }
    void setTrackSagR(Double_t sagittaR)
    {
        m_saggitaR = sagittaR;
    }
    void setTrackSagX(Double_t sagittaX)
    {
        m_saggitaX= sagittaX;
    }
    void setTrackSagY(Double_t sagittaY)
    {
        m_saggitaY = sagittaY;
    }
    void setRefPhi(Double_t refPhi)
    {
        m_refPhi = refPhi;
    }

    Bool_t getHitIs(Int_t layId){
        return m_hitIs[layId];
    }
    Double_t getHitR(Int_t layId){
        return m_hitR[layId];
    }
    Double_t getHitZ(Int_t layId){
        return m_hitZ[layId];
    }
    Double_t getHitPhi(Int_t layId){
        return m_hitPhi[layId];
    }
    Double_t getHitTheta(Int_t layId)
    {
        return m_hitTheta[layId];
    }
    
    bool getTrackElectronIs(){
        return m_electronIs;
    }
    Int_t getTrackCharge(){
        return m_charge;
    }
    Double_t getTrackPhi(){
        return m_phi;
    }
    Double_t getTrackTheta(){
        return m_theta;
    }
    Double_t getTrackE(){
        return m_energy;
    }
    Double_t getTrackPt(){
        return m_pt;
    }
    Double_t getTrackP(){
        return m_p;
    }
    Double_t getTrackSagR(){
        return m_saggitaR;
    }
    Double_t getTrackSagX(){
        return m_saggitaX;
    }
    Double_t getTrackSagY(){
        return m_saggitaY;
    }
    Double_t getTrackRefPhi(){
        return m_refPhi;
    }

private:
    Bool_t m_hitIs[7];
    Double_t m_hitR[7];
    Double_t m_hitZ[7];
    Double_t m_hitPhi[7];
    Double_t m_hitTheta[7];
    bool     m_electronIs;
    Int_t    m_charge;
    Double_t m_phi;
    Double_t m_theta;
    Double_t m_energy;
    Double_t m_p;
    Double_t m_pt;
    Double_t m_saggitaR;
    Double_t m_saggitaX;
    Double_t m_saggitaY;
    Double_t m_refPhi;
};	

struct hitStruct{
    Double_t r;
    Double_t z;
	Double_t phi;
    Double_t eta;
    Double_t pt;
    Double_t energy;
};

// -----------------------------------------------------------------------
// typedef std::vector<tracKuma>  KumaTracks;
// ----------------------------------------------------------------------

#endif  // SPHTRACKUMA_H

